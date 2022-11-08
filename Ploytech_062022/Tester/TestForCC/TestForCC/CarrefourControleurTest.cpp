#include "CarrefourControleurTest.h"

using namespace std;
using namespace CarfContTest;


LEDReference::LEDReference(int num)
{
    _number = num;
}

LEDReference::LEDReference()
{
    _number = 0;
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

int LEDReference::GetR()
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
      
int LEDReference::GetG()
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

int LEDReference::GetB()
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
    int j = 0;
    if (_illumine) j = 1;
    int p = 0;
    if (target) p = 1;
    if (_illumine == target) return;
    printf("%d try to transform to %d, status %d;\n", _number, p, j);
    if(_illumine==false&&target==true) TrRedtoGreen();
    if(_illumine==true&&target==false) TrGreentoRed();
    return;
}

void LEDReference::TrRedtoGreen()
{
    _transforming = true;
    return;
}

void LEDReference::TrGreentoRed()
{
    _transforming=true;
    _color=2;
    return;
}
void LEDReference::Timepass(int timepassed)
{
    if (_transforming == true)
    {
        _timer += timepassed;
        if (_timer >= DELAYDEJAUNE && _color==2) //enough time passed 
        {
            _illumine = false;//CHANGE HERE !!!
            _color = 0;
            _timer = 0;
            _transforming = false;
        }
        else if (_timer >= DELAYDEJAUNE + 2 * STEP)
        {
            _illumine = true;
            _color = 1;
            _timer = 0;
            _transforming = false;
        }
    }
    return;
}

                                                                                //fin of class LED

void Boutons::UpdateBoutonsValue(int i)                                                                    //realise by firmware
{
    return;
}

Boutons::Boutons()
{
    //CHANGE HERE
    for(int i=0;i< NUMOFBOUTONS;i++) _bouton[i]=false;
    _numdebouton = NUMOFBOUTONS;
}

bool Boutons::GetBoutonValue(int i)//i=0123
{
    UpdateBoutonsValue(i);
    //return _bouton[i];
    return false;
}                                                                                     

void CapteurOptique::UpdateValue(int p)//(0-1023,10bit)
{
    return;
}

void CapteurOptique::UpdateVoltage(int p) //(0V-5V,V)
{
    UpdateValue(p);
    _voltage = 5 * (_value + 0.5f) / 1024;
    return;
}

CapteurOptique::CapteurOptique(int pin)
{
    _p = pin;
}

CapteurOptique::CapteurOptique()
{
    _p = PINCAPTEUR1;
}

int CapteurOptique::GetPin()
{
    return _p;
}

void CapteurOptique::SetPin(int p)
{
    _p = p;
    return;
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
        printf("transform on, formal lts = %d \n", _lengthofstat);
        if(_stattogo==0||_stattogo==1)
        {
            switch(_stats)
            {
                case 1:_stats=0;
                    printf("transform to s0\n");
                    break;
                default: _stats=1;
                    printf("transform to s1\n");
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
        printf("transform off, new lts = %d \n", _lengthofstat);
    }
    return trans;
}

Loopcontroleur::Loopcontroleur()
{
    //LEDReference _feux[16];
    bool truetable[7][16] = {
        {false,false,false,true,false,true,false,true,false,false,false,true,false,true,false,true},    //0b0001010100010101 rrllrr
        {false,false,false,true,false,false,true,true,false,false,false,true,false,false,true,true},    //0b0001001100010011 rrssrr
        {true,false,false,false,false,true,false,true,false,false,false,false,false,false,true,false},  //0b1000001010000010 ppss
        {false,true,false,true,false,false,false,true,false,true,false,true,false,false,false,true},    //0b0101000101010001 llrrrr
        {false,false,true,true,false,false,false,true,false,false,true,true,false,false,false,true},    //0b0011000100110001 ssrrrr
        {false,false,true,false,true,false,false,false,false,false,true,false,true,false,false,false},  //0b0010100000101000 sspp
        {true,false,false,false,true,false,false,false,true,false,false,false,true,false,false,false},  //0b1000100010001000 pppp
    };
    for (int i = 0; i < 7; i++) for (int j = 0; j < 16; j++)
    {
        _truetable[i][j] = truetable[i][j];
    }
    for (int i = 0; i < 16; i++)
    {
        _feux[i].Redefini(i);
        if(_truetable[0][i]) _feux[i].TrRedtoGreen();
    }

}
/*
void Loopcontroleur::Definitruetable(bool truetable_flat[])
{
    int num=0;
    //bool _truetable[_numdestats][_numdeLED];
    for(int i=0;i<_numdestats;i++) for(int j=0;j<_numdeLED;j++) _truetable[i][j]=truetable_flat[num++];
}
*/
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
    int color;
    for (int i = 0; i < 20; i++)
    {
        color = _loop.GetColor(NumTransde20a16(i));
        printf("%d,%d,%d; \n", i, NumTransde20a16(i), color);
    }
    printf("\n");
    return;
}

void Syscarrefour::PollingBoutons()
{
    for(int i=0;i<4;i++) if(_boutons.GetBoutonValue(i)&&_boutontimepass==0) _boutontrigger[i]=true;
    return;
}

void Syscarrefour::MacroStep()//1000ms=20x50ms STEP = p x microstep
{
    for (int i = 0; i < STEP / 50; i++) MicroStep();
    bool trans = _loop.Looper();
    if (trans)
    {
        for (int i = 0; i < 4; i++)
        {
            _boutontrigger[i]=false;
        }
    }
    return;
}

void Syscarrefour::MicroStep()//50ms
{
    PollingBoutons();
    if (_capteur1.UpdateStats(_capteur1.GetPin()) || _capteur2.UpdateStats(_capteur2.GetPin())) TriggerToStat();
    for(int i=0;i<4;i++) if(_boutontrigger[i]==true) _boutontimepass[i]+=50;
    //delay(50);
    return;
}

int Syscarrefour::TriggerToStat()//return 0 only when no trigger detected and return stattogo normally.
{   //captuer judge without duree, so the trigger for captuer could be only used when the real signal of capteur is determined, like continuing for 250ms(5 Microstep).
    int vote = 0;
    bool botton[4] = { false };
    for (int i = 0; i < 4; i++)
    {
        botton[i] = _boutontrigger[i];
        if (botton[i] == true) vote++;
    }
    if (vote >= 3)//b:3/4 ==> 6 Pall
    {
        _loop.SetStattogo(6);
        for (int i = 0; i < 4; i++)
        {
            _boutontimepass[i] = 0;
        }
        return 6;
    }
    else if (botton[0] == true || botton[2] == true || _capteur1.GetStats() == 2 || _capteur2.GetStats() == 2)//Bn or Bs or Cs ==> Pmain(Pw and Pe) ==> 5. 
    {
        _loop.SetStattogo(5);
        _boutontimepass[0] = 0;
        _boutontimepass[2] = 0;
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
        _boutontimepass[1] = 0;
        _boutontimepass[3] = 0;
        return 5;
    }
    return 0;
}

Syscarrefour::Syscarrefour()
{
    _capteur1.SetPin(PINCAPTEUR1);
    _capteur2.SetPin(PINCAPTEUR2);
}

void Syscarrefour::Setup()
{  

}

void Syscarrefour::Loop()
{
    MacroStep();
    UpdatePixel();
    getchar();
}
