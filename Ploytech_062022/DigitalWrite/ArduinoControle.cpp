#include <iostream>
#include <cstring>
#include <string>

#include <Adafruit_NeoPixel.h>
#include "ArduinoControle.h"


using namespace std;

#define DELAYDEJAUNE 3000 //ms        delay of the yellow light
#define DELAYDEMAINSR 50000         //delay of the main ssrrrr. Sub R also content.
#define DELAYDEMAINLR 25000         //delay of the main llrrrr, etc
#define DELAYDEMAINSP 20000         //rely on Pmain(note: m=n+s but Pm=Wp+Ep)
#define DELAYDESUBSP 15000          //rely on Subs et Subp
#define DELAYDESUBLR 20000          //rely on Subl
#define DELAYDEALLP 25000           //only when 4/3 bottons trigger

#define DELAYDEPIETON 4000          //delay of the eteindre between light of passingers and of vehicles
//#define DELAYDEBOTTON_MAX 60000     //delay maximum between the passingers push the botton and their light come to green 
//#define DELAYDECONTINUER 6000       //delay of how long the stats "no vehicle detected" continue would cause their round fini. 

#define SEUILLEFTDECAPTEUR 1.6      //voltage threshold between null/idle, left, stright and right.
#define SEUILSTRDECAPTEUR 2.2
#define SEUILRIGHTDECAPTEUR 2.8

#define STEP 1000                   //step of control and 'clock' onboard.
          
#define BOUTONDELAY 2               //mecanism of voting of bottons, not forced and not useful.

#define PINCAPTEUR1 2               //pin analog of capteur optique. btw, pin of bottons is 2345 and of led is 6 while in digital.
#define PINCAPTEUR2 3
#define FIRSTBOUTON 2     
//botton:long side shortconnect, keep pressing for trigger.



class LEDReference                                                            //For build and control the model of LED. Finish
{
    private:

    string _strName;
    int _number;
    bool _illumine;   //False=Rouge=Red=Interdit=0=0V       True=Vert=Green=Permet=1=3V
    int _color;       //0=red,1=green,2=yellow
    int _timer=0;     //record time passed for transforming.
    bool _transforming=false;
      
      
    public:                                                             //Fonctions Public

    LEDReference(string name, int num, bool illum)
    {
        _strName = name;
        _number = num;
        _illumine = illum;
    }

    LEDReference(string name, int num)
    {
        _strName = name;
        _number = num;
        _illumine = false;
    }

    LEDReference()
    {
        _strName = "defaute";
        _number = 0;
        _illumine = false;
    }




    //Redefinir
      
  
    void Redefini(string name, int num, bool illum)
    {
        _strName = name;
        _number = num;
        _illumine = illum;
    }

    void Redefini(string name, int num)
    {
        _strName = name;
        _number = num;
        _illumine = false;
    }

    //Provide these sub Values

    long CodeRGB()
    {
        if (_color == 0)//red
        {
            return 0x880000;
        }
        else if (_color == 1)//green
        {
            return 0x008800;
        }
        else if (_color == 2)//soft yellow
        {
            return 0x444400;
        }
          
        return 0x000044; //error code - light blue
    }

    uint8_t GetR()
    {
        if (_color == 0)//red
        {
            return 0x10;
        }
        else if (_color == 1)//green
        {
            return 0x00;
        }
        else if (_color == 2)//soft yellow
        {
            return 0x05;
        }
          
        return 0x00; //error code - light blue
    }
      
    uint8_t GetG()
    {
        if (_color == 0)//red
        {
            return 0x00;
        }
        else if (_color == 1)//green
        {
            return 0x10;
        }
        else if (_color == 2)//soft yellow
        {
            return 0x05;
        }
          
        return 0x00; //error code - light blue
    }

    uint8_t GetB()
    {
        if (_color == 0)//red
        {
            return 0x00;
        }
        else if (_color == 1)//green
        {
            return 0x00;
        }
        else if (_color == 2)//soft yellow
        {
            return 0x00;
        }
          
        return 0x44; //error code - light blue
    }

    int GetColor()
    {
        return _color;
    }



    //Transforme color

    void EssayTransform(bool target)
    {
        if(_illumine==target) return;
        else if(_illumine==false&&target==true) TrRedtoGreen();
        else if(_illumine==true&&target==false) TrGreentoRed();
        return;
    }

    void TrRedtoGreen()
    {
        if((_color==0||_color==2)&&_illumine==false)
        {
            _color=1;
            _illumine=true;
        }
        return;
    }

    void TrGreentoRed()
    {
        if(_timer>=DELAYDEJAUNE)//enough time passed
        {
            _illumine=true;
            _color=0;
            _timer=0;
            _transforming=false;
        }
        else if(_color==1&&_illumine==true&&_timer==0)//green now. transform begin
        {
            _transforming=true;
            _color=2;
        }
        return;
    }

      void Timepass(int timepassed)
    {
        if(_transforming==true)_timer+=timepassed;
        return;
    }



};                                                                                     //fin of class LED


class Boutons                                                                          //defini the methodes of boutons. Finish
{
    private:
    bool* _bouton;
    int _numdebouton;
      

    void UpdateBoutonsValue(int i)                                                                    //realise by firmware
    {
        if(digitalRead(i+FIRSTBOUTON)>=1) _bouton[i]=true;
        else _bouton[i]=false;
        return;
    }

    public:

    Boutons()
    {
        Boutons(4);
    }

    Boutons(int num)
    {
        bool _bouton[num];
        for(int i=0;i<num;i++) _bouton[i]=false;
        _numdebouton = num;
    }

    bool GetBoutonValue(int i)//i=0123
    {
        UpdateBoutonsValue(i);
        return _bouton[i];
    }
};                                                                                              //fin of class bouton.

class CapteurOptique                                                                            //Firmwire of Capteur Optique. Finish
{
private:

    int _p;
    int _value = 0;
    float _voltage = 0;
    int _stats = 0;
    int _buffer[5] = { 0 };
    bool _triggered = false;

    void UpdateValue(int p)//(0-1023,10bit)
    {
        _value=analogRead(p);
        return;
    }

    void UpdateVoltage(int p) //(0V-5V,V)
    {
        UpdateValue(p);
        _voltage = 5 * (_value + 0.5) / 1024;
        return;
    }


    public:

    CapteurOptique(int pin)
    {
        _p = pin;
    }

    int GetPin()
    {
        return _p;
    }

    int SetPin(int p)
    {
        _p = p;
    }

    int GetStats()//0:null/idle 1:Left 2:Str 3:Right
    {
        return _stat;
    }

    bool UpdateStats(int p)//return ture if triggered
    {
        int buf = 0;
        bool same = true;
        UpdateVoltage(p);
        if (_voltage < SEUILLEFTDECAPTEUR) buf = 0;
        else if (_voltage < SEUILSTRDECAPTEUR) buf = 1;
        else if (_voltage < SEUILRIGHTDECAPTEUR) buf = 2;
        else buf = 3;
        for (int i = 0; i < 5; i++) if (buf != _buffer[i]) same = false;
        if (same == true)
        {
            _stats = buf;
            _triggered = true;
            return true;
        }
        else {
            for (int i = 0; i < 4; i++) _buffer[i] = _buffer[i + 1];
            _buffer[4] = buf;
        }
        return false;
    }

    bool Clear()//clear triggered
    {
        bool temp = _triggered;
        _triggered = false;
        return temp;
    }
}                                                                                              //fin of class capteur.

class Loopcontroleur                   //For control leds by truetable and timing, without bouton or capteur. Finish
{
    private:
    int _ddj=DELAYDEJAUNE;
    //int _ddml=DELAYDEMAINLOOP;
    //int _ddsl=DELAYDESUBLOOP;
    //int _ddb=DELAYDEBOTTON_MAX;
    //int _ddc=DELAYDECONTINUER;
    int _ddp=DELAYDEPIETON;

    int _stat=0;
    int _stattogo=0;
    int _lengthofstat=25000;
    int _timeofstatpassed=0;
    bool _trigger=false;



    LEDReference *_feux;
    bool **_truetable;
    
    int _numdeLED=0;
    int _numdestats=0;

    int _GetNumofMainStats()
    {
        if(_stat>0) return _stat-1;
        else return 0;
    }

    void _UpdateStatLength()
    {
        switch(_stat)
        {
        case 0:_lengthofstat=DELAYDEMAINLR; //Mllrrrr
            break;
        case 1:_lengthofstat=DELAYDEMAINSR; //Mssrrrr
            break;
        case 2:_lengthofstat=DELAYDEMAINSP; //Msspp
            break;
        case 3:_lengthofstat=DELAYDESUBLR; //Sllrrrr
            break;
        case 4:_lengthofstat=0; //Sssrrrr (n'existe)
            break;
        case 5:_lengthofstat=DELAYDESUBSP; //Ssspp
            break;
        case 6:_lengthofstat=DELAYDEALLP; //pppp
            break;
        default:_lengthofstat=0;
            break;
        }
        return;
    }

    bool _CheckStat()
    {
        bool trans = false;
        if(_timeofstatpassed>=_lengthofstat) 
        {
            _timeofstatpassed=0;
            if(_stattogo==0||_stattogo==1)
            {
                switch(_stat)
                {
                    case 1:_stat=0;
                        break;
                    default: _stat=1;
                        break;
                }
            }
            else if(_stattogo<=6)
            {
                _stat=_stattogo;
                _stattogo=0;
                trans = true;
            }
            for(int i=0;i<_numdeLED;i++)
            {
                _feux[i].EssayTransform(_truetable[_stat][i]);
            }
            _UpdateStatLength();
        }
        return trans;
        
    }

    public:


    Loopcontroleur(LEDReference lights[],int numdeLED,int numdestats)
    {
        int num=sizeof(lights)/sizeof(LEDReference);
        LEDReference _feux[num];
        for(int i=0;i<num;i++)
        {
            _feux[i]=lights[i];
        } 
        //_feux[num]=0;
        int _numdeLED=numdeLED;
        int _numdestats=numdestats;
    }

    void Definitruetable(bool truetable_flat[])
    {
        int num=0;
        bool _truetable[_numdestats][_numdeLED];
        for(int i=0;i<_numdestats;i++) for(int j=0;j<_numdeLED;j++) _truetable[i][j]=truetable_flat[num++];
    }

    void Looper(int jumpto)
    {
        if(jumpto>=0&&jumpto<_numdestats)
        {
            _stat=jumpto;
            _timeofstatpassed=0;
            _UpdateStatLength();
        }
        Looper();
        return;
    }

    bool Looper()
    {
        bool trans = false;
        trans = _CheckStat();
        _timeofstatpassed+=STEP;                                    //Time passed
        for (int i = 0; i < _numdeLED; i++)
        {
            _feux[i].Timepass(STEP);
        }
        returntrans;
    }

    void StatsPietonTemporal()
    {
        _stat=6;
        _timeofstatpassed=0;
        _UpdateStatLength();
    }

    int GetStattogo()
    {
        return _stattogo;
    }

    void SetStattogo(int stattogo)
    {
        _stattogo=stattogo;
        return;
    }

    int GetnumdeLED() {return _numdeLED;}
    int GetnumdeStats() {return _numdestats;}
    int GetColor(int numerodeLED) {return _feux[numerodeLED].GetColor();}
    int GetR(int numerodeLED) {return _feux[numerodeLED].GetR();}
    int GetG(int numerodeLED) {return _feux[numerodeLED].GetG();}
    int GetB(int numerodeLED) {return _feux[numerodeLED].GetB();}

};                                                                                                        //fin of class looper

class Syscarrefour                                                                         //For integre looper, capteur and bouton by arduino. Unfinish.
{
    private:
    Loopcontroleur _loop;
    Boutons _boutons;
    CapteurOptique _capteur1;
    CapteurOptique _capteur2;
    //int _foisdeVote=0;
    //bool _boutondetect=true;
    //int _timer=0;
    int _timerdebouton=0;
    bool _boutontrigger[4]={false,false,false,false};
    int _boutontimepass[4]=0;//time passed since bott's triggered
    
    int NumTransde20a16(int i)//i de 0 a 19 ===> de 0 a 15
    {
        if((i+1)%5==0) return i-4-i/5;
        else return i-i/5;
    }

    void UpdatePixel()
    {
        for(int i=0;i<20;i++)
        {
            pixels.setPixelColor(i, pixels.Color(_loop.GetR(NumTransde20a16(i)),_loop.GetG(NumTransde20a16(i))_loop.GetB(NumTransde20a16(i))));
        }
        return;
    }

    void PollingBoutons()
    {
        for(int i=0;i<4;i++) if(_boutons.GetBoutonValue(i)&&_boutontimepass==0) _boutontrigger[i]=true;
        return;
    }

    void MacroStep()//1000ms=20x50ms STEP = p x microstep
    {
        int p = STEP / 50;
        for (int i = 0; i < p; i++) MicroStep();
        return;
    }

    void MicroStep()//50ms
    {
        PollingBoutons();
        if (_capteur1.UpdateStats(_capteur1.GetPin()) || _capteur2.UpdateStats(_capteur2.GetPin())) TriggerToStat();
        for(int i=0;i<4;i++) if(_boutontrigger[i]==true) _boutontimepass+=50;
        delay(50);
        return;
    }

    int TriggerToStat()//return 0 only when no trigger detected and return stattogo normally.
    {                  //captuer judge without duree, so the trigger for captuer could be only used when the real signal of capteur is determined, like continuing for 250ms(5 Microstep).
        int vote = 0;
        bool botton[4] = { false };
        for (int i = 0; i < 4; i++)
        {
            botton[i] = _boutons.GetBoutonValue(i);
            if (botton[i] == true) vote++;
        }
        if (vote >= 3)//b:3/4 ==> 6 Pall
        {
            _loop.SetStattogo(6);
            return 6;
        }
        else if (botton[0] == true || botton[2] == true || _capteur1.GetStats() == 2 || _capteur2.GetStats() == 2)//Bn or Bs or Cs ==> Pmain(Pw and Pe) ==> 5. 
        {
            _loop.SetStattogo(5);
            return 2;
        }
        else if (_capteur1.GetStats() == 1 || _capteur2.GetStats() == 1)//Cl ==> Lsub ==> 3
        {
            _loop.SetStattogo(3);
            return 3;
        }
        else if (botton[1] == true || botton[3] == true)//Bw or Be ==> Psub(Pn and Ps) ==> 2
        {
            _loop.SetStattogo(2);
            return 5;
        }
        return 0;
    }

    public:

    Syscarrefour(Loopcontroleur loop, Boutons boutons)
    {
        _loop=loop;
        _boutons=boutons;
        _capteur1.SetPin(PINCAPTEUR1);
        _capteur2.SetPin(PINCAPTEUR2);
    }

    void Setup()
    {  
        for(int i=2;i<6;i++)
        {
            pinMode(i, INPUT);//bouton:2345,digital
        }
        pinMode(6,OUTPUT);//LED:6,digital
        //Detector:23,analog input. use int analogread(i); 0v-5v(for Uno) ===> (int) 0-1023 (10bit).
        pixels.clear();
        pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    }

    void Loop()
    {
        /*
        if(_boutondetect)
        {
            _boutons.UpdateBoutonsValue();
            if(_boutons.VotedeBoutons()) _foisdeVote++;
            if(_foisdeVote>BOUTONDELAY) 
            {
                _loop.Looper(true);
                _boutondetect=false;
                _foisdeVote=0;
            }
            else _loop.Looper(false);
        }
        else 
        {
            _loop.Looper(); 
            _timerdebouton+=STEP;                       //if the note in looper() has been deleted correctly, the delay(1000) isn' needy here.
        }
        if(_timerdebouton>4*DELAYDESUBLOOP)
        {
            _timerdebouton=0;
            _boutondetect=true;
        }
        */



        pixels.clear();
        UpdatePixel();
        

          
    }

        
};

/*

void ArduinoSetup()
{
  for(int i=2;i<6;i++)
  {
    pinMode(i, INPUT);
  }
  pinMode(6,OUTPUT);
  pixels.clear();
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

*/