#include <cstring>
#include <string>

#include <Adafruit_NeoPixel.h>
#include "CarrefourControleur.h"


using namespace std;
using namespace CarfCont;

#define DELAYDEJAUNE 3000 //ms        delay of the yellow light
#define DELAYDEMAINSR 50000         //delay of the main ssrrrr. Sub R also content.
#define DELAYDEMAINLR 25000         //delay of the main llrrrr, etc
#define DELAYDEMAINSP 20000         //rely on Pmain(note: m=n+s but Pm=Wp+Ep)
#define DELAYDESUBSP 15000          //rely on Subs et Subp
#define DELAYDESUBLR 20000          //rely on Subl
#define DELAYDEALLP 25000           //only when 4/3 bottons trigger

#define DELAYDEPIETON 4000          //delay of the eteindre between light of passingers and of vehicles

#define SEUILLEFTDECAPTEUR 1.6      //voltage threshold between null/idle, left, stright and right.
#define SEUILSTRDECAPTEUR 2.2
#define SEUILRIGHTDECAPTEUR 2.8

#define STEP 1000                   //step of control and 'clock' onboard.
          
#define BOUTONDELAY 2               //mecanism of voting of bottons, not forced and not useful.

#define PINCAPTEUR1 2               //pin analog of capteur optique. btw, pin of bottons is 2345 and of led is 6 while in digital.
#define PINCAPTEUR2 3
#define FIRSTBOUTON 2     

LEDReference::LEDReference(int num)
{
    _number = num;
    _illumine = false;
}

LEDReference::LEDReference()
{
    _number = 0;
    _illumine = false;
}

void LEDReference::Redefini(int num)
{
    _number = num;
}

long LEDReference::CodeRGB()
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

uint8_t LEDReference::GetR()
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
      
uint8_t LEDReference::GetG()
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

uint8_t LEDReference::GetB()
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

int LEDReference::GetColor()
{
    return _color;
}

void LEDReference::EssayTransform(bool target)
{
    if(_illumine==target) return;
    else if(_illumine==false&&target==true) TrRedtoGreen();
    else if(_illumine==true&&target==false) TrGreentoRed();
    return;
}

void LEDReference::TrRedtoGreen()
{
    if((_color==0||_color==2)&&_illumine==false)
    {
        _color=1;
        _illumine=true;
    }
    return;
}

void LEDReference::TrGreentoRed()
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

void LEDReference::Timepass(int timepassed)
{
    if(_transforming==true)_timer+=timepassed;
    return;
}

                                                                                //fin of class LED

void Boutons::UpdateBoutonsValue(int i)                                                                    //realise by firmware
{
    if(digitalRead(i+FIRSTBOUTON)>=1) _bouton[i]=true;
    else _bouton[i]=false;
    return;
}

Boutons::Boutons()
{
    Boutons(4);
}

Boutons::Boutons(int num)
{
    bool _bouton[num];
    for(int i=0;i<num;i++) _bouton[i]=false;
    _numdebouton = num;
}

bool Boutons::GetBoutonValue(int i)//i=0123
{
    UpdateBoutonsValue(i);
    return _bouton[i];
}
};                                                                                          

void CapteurOptique::UpdateValue(int p)//(0-1023,10bit)
{
    _value=analogRead(p);
    return;
}

void CapteurOptique::UpdateVoltage(int p) //(0V-5V,V)
{
    UpdateValue(p);
    _voltage = 5 * (_value + 0.5) / 1024;
    return;
}

CapteurOptique::CapteurOptique(int pin)
{
    _p = pin;
}

int CapteurOptique::GetPin()
{
    return _p;
}

int CapteurOptique::SetPin(int p)
{
    _p = p;
}

int CapteurOptique::GetStats()//0:null/idle 1:Left 2:Str 3:Right
{
    return _stats;
}

bool CapteurOptique::UpdateStats(int p)//return ture if triggered
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

bool CapteurOptique::Clear()//clear triggered
{
    bool temp = _triggered;
    _triggered = false;
    return temp;
}


int Loopcontroleur::_GetNumofMainStats()
{
    if(_stats>0) return _stats-1;
    else return 0;
}

void Loopcontroleur::_UpdateStatLength()
{
    switch(_stats)
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

bool Loopcontroleur::_CheckStat()
{
    bool trans = false;
    if(_timeofstatpassed>=_lengthofstat) 
    {
        _timeofstatpassed=0;
        if(_stattogo==0||_stattogo==1)
        {
            switch(_stats)
            {
                case 1:_stats=0;
                    break;
                default: _stats=1;
                    break;
            }
        }
        else if(_stattogo<=6)
        {
            _stats=_stattogo;
            _stattogo=0;
            trans = true;
        }
        for(int i=0;i<_numdeLED;i++)
        {
            _feux[i].EssayTransform(_truetable[_stats][i]);
        }
        _UpdateStatLength();
    }
    return trans;
}

Loopcontroleur::Loopcontroleur(LEDReference lights[],int numdeLED,int numdestats)
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
    _lengthofstat = 25000;
}

void Loopcontroleur::Definitruetable(bool truetable_flat[])
{
    int num=0;
    bool _truetable[_numdestats][_numdeLED];
    for(int i=0;i<_numdestats;i++) for(int j=0;j<_numdeLED;j++) _truetable[i][j]=truetable_flat[num++];
}

void Loopcontroleur::Looper(int jumpto)
{
    if(jumpto>=0&&jumpto<_numdestats)
    {
        _stats=jumpto;
        _timeofstatpassed=0;
        _UpdateStatLength();
    }
    Looper();
    return;
}

bool Loopcontroleur::Looper()
{
    bool trans = false;
    trans = _CheckStat();
    _timeofstatpassed+=STEP;                                    //Time passed
    for (int i = 0; i < _numdeLED; i++)
    {
        _feux[i].Timepass(STEP);
    }
    return trans;
}

void Loopcontroleur::StatsPietonTemporal()
{
    _stats=6;
    _timeofstatpassed=0;
    _UpdateStatLength();
}

int Loopcontroleur::GetStattogo()
{
    return _stattogo;
}

void Loopcontroleur::SetStattogo(int stattogo)
{
    _stattogo=stattogo;
    return;
}

int Loopcontroleur::GetnumdeLED() {return _numdeLED;}
int Loopcontroleur::GetnumdeStats() {return _numdestats;}
int Loopcontroleur::GetColor(int numerodeLED) {return _feux[numerodeLED].GetColor();}
int Loopcontroleur::GetR(int numerodeLED) {return _feux[numerodeLED].GetR();}
int Loopcontroleur::GetG(int numerodeLED) {return _feux[numerodeLED].GetG();}
int Loopcontroleur::GetB(int numerodeLED) {return _feux[numerodeLED].GetB();}

int Syscarrefour::NumTransde20a16(int i)//i de 0 a 19 ===> de 0 a 15
{
    if((i+1)%5==0) return i-4-i/5;
    else return i-i/5;
}

void Syscarrefour::UpdatePixel()
{
    for(int i=0;i<20;i++)
    {
        pixels.setPixelColor(i, pixels.Color(_loop.GetR(NumTransde20a16(i)),_loop.GetG(NumTransde20a16(i)),_loop.GetB(NumTransde20a16(i))));
    }
    return;
}

void Syscarrefour::PollingBoutons()
{
    for(int i=0;i<4;i++) if(_boutons.GetBoutonValue(i)&&_boutontimepass==0) _boutontrigger[i]=true;
    return;
}

void Syscarrefour::MacroStep()//1000ms=20x50ms STEP = p x microstep
{
    int p = STEP / 50;
    for (int i = 0; i < p; i++) MicroStep();
    return;
}

void Syscarrefour::MicroStep()//50ms
{
    PollingBoutons();
    if (_capteur1.UpdateStats(_capteur1.GetPin()) || _capteur2.UpdateStats(_capteur2.GetPin())) TriggerToStat();
    for(int i=0;i<4;i++) if(_boutontrigger[i]==true) _boutontimepass+=50;
    delay(50);
    return;
}

int Syscarrefour::TriggerToStat()//return 0 only when no trigger detected and return stattogo normally.
{   //captuer judge without duree, so the trigger for captuer could be only used when the real signal of capteur is determined, like continuing for 250ms(5 Microstep).
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

Syscarrefour::Syscarrefour()
{
    _capteur1.SetPin(PINCAPTEUR1);
    _capteur2.SetPin(PINCAPTEUR2);
}

void Syscarrefour::Redifini(Loopcontroleur loop, Boutons boutons)
{
    _loop = loop;
    _boutons = boutons;
}

void Syscarrefour::Setup()
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

void Syscarrefour::Loop()
{
    pixels.clear();
    UpdatePixel();         
}
