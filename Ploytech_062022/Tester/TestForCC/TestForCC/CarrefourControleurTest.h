#ifndef ARDUINOCONTROLE_H
#define ARDUINOCONTROLE_H
using namespace std;
#include <stdint.h>
#include <stdio.h>

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
#define NUMOFBOUTONS 4

namespace CarfContTest
{
  
    
    class LEDReference
    {
    private:
        int _number = 0;
        bool _illumine = false;   //False=Rouge=Red=Interdit=0=0V       True=Vert=Green=Permet=1=3V
        int _color = 0;      //0=red,1=green,2=yellow
        int _timer = 0;     //record time passed for transforming.
        bool _transforming = false;
        //CHANGE HERE


    public:                                                             //Fonctions Public

        LEDReference(int num);
        LEDReference();
        void Redefini(int num);
        long CodeRGB();
        int GetR();
        int GetG();
        int GetB();
        int GetColor();
        void EssayTransform(bool target);
        void TrRedtoGreen();
        void TrGreentoRed();
        void Timepass(int timepassed);
    };

    class Boutons                                                                          //defini the methodes of boutons. Finish
    {
    private:
        bool _bouton[4];
        //CHANGE HERE
        int _numdebouton;

        void UpdateBoutonsValue(int i);
    public:
        Boutons();
        bool GetBoutonValue(int i);
    };

    class CapteurOptique                                                                      //Firmwire of Capteur Optique. Finish
    {
    private:

        int _p = PINCAPTEUR1;
        int _value = 0;
        float _voltage = 0;
        int _stats = 0;
        int _buffer[5] = { 0 };
        bool _triggered = false;

        void UpdateValue(int p);
        void UpdateVoltage(int p);
    public:
        CapteurOptique();
        CapteurOptique(int pin);
        int GetPin();
        void SetPin(int p);
        int GetStats();
        bool UpdateStats(int p);
        bool Clear();
    };

    class Loopcontroleur                               //For control leds by truetable and timing, without bouton or capteur. Finish
    {
    private:
        int _ddj = DELAYDEJAUNE;
        int _ddp = DELAYDEPIETON;

        int _stats = 0;
        int _stattogo = 0;
        int _lengthofstat = 25000;
        int _timeofstatpassed = 0;
        bool _trigger = false;

        LEDReference _feux[16];
        bool _truetable[7][16] = { false };

        int _numdeLED = 16;
        int _numdestats = 7;

        int _GetNumofMainStats();
        void _UpdateStatLength();
        bool _CheckStat();

    public:
        Loopcontroleur();
        //void Definitruetable(bool truetable_flat[]);
        void Looper(int jumpto);
        bool Looper();
        void StatsPietonTemporal();
        int GetStattogo();
        void SetStattogo(int stattogo);
        int GetnumdeLED();
        int GetnumdeStats();
        int GetColor(int numerodeLED);
        int GetR(int numerodeLED);
        int GetG(int numerodeLED);
        int GetB(int numerodeLED);
    };

    class Syscarrefour                                                                         //For integre looper, capteur and bouton by arduino. Unfinish.
    {
    private:
        Loopcontroleur _loop;
        Boutons _boutons;
        CapteurOptique _capteur1;
        CapteurOptique _capteur2;
        int _timerdebouton = 0;
        bool _boutontrigger[4] = { false,false,false,false };
        int _boutontimepass[4] = {0};//time passed since bott's triggered

        int NumTransde20a16(int i);//i de 0 a 19 ===> de 0 a 15
        void UpdatePixel();
        void PollingBoutons();
        void MacroStep();
        void MicroStep();
        int TriggerToStat();

    public:
        Syscarrefour();
        void Setup();
        void Loop();
    };

}


#endif
