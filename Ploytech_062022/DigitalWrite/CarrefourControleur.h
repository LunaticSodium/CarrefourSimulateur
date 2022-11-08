#ifndef ARDUINOCONTROLE_H
#define ARDUINOCONTROLE_H

#include <Adafruit_NeoPixel.h>
using namespace std;

namespace CarfCont
{
    class LEDReference
    {
    private:
        int _number;
        bool _illumine;   //False=Rouge=Red=Interdit=0=0V       True=Vert=Green=Permet=1=3V
        int _color;       //0=red,1=green,2=yellow
        int _timer = 0;     //record time passed for transforming.
        bool _transforming = false;


    public:                                                             //Fonctions Public

        LEDReference(int num);
        LEDReference();
        void Redefini(int num);
        long CodeRGB();
        uint8_t GetR();
        uint8_t GetG();
        uint8_t GetB();
        int GetColor();
        void EssayTransform(bool target);
        void TrRedtoGreen();
        void TrGreentoRed();
        void Timepass(int timepassed);
    };

    class Boutons                                                                          //defini the methodes of boutons. Finish
    {
    private:
        bool* _bouton;
        int _numdebouton;

        void UpdateBoutonsValue(int i);
    public:
        Boutons();
        Boutons(int num);
        bool GetBoutonValue(int i);
    };

    class CapteurOptique                                                                      //Firmwire of Capteur Optique. Finish
    {
    private:

        int _p;
        int _value = 0;
        float _voltage = 0;
        int _stats = 0;
        int _buffer[5] = { 0 };
        bool _triggered = false;

        void UpdateValue(int p);
        void UpdateVoltage(int p);
    public:

        CapteurOptique(int pin);
        int GetPin();
        int SetPin(int p);
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
        int _lengthofstat = 0;
        int _timeofstatpassed = 0;
        bool _trigger = false;

        LEDReference* _feux;
        bool _truetable[7][16];

        int _numdeLED = 0;
        int _numdestats = 0;

        int _GetNumofMainStats();
        void _UpdateStatLength();
        bool _CheckStat();

    public:
        Loopcontroleur(LEDReference lights[], int numdeLED, int numdestats);
        void Definitruetable(bool truetable_flat[]);
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
        bool _boutontrigger[4] = { false };
        int _boutontimepass[4] = 0;//time passed since bott's triggered

        int NumTransde20a16(int i);//i de 0 a 19 ===> de 0 a 15
        void UpdatePixel();
        void PollingBoutons();
        void MacroStep();
        void MicroStep();
        int TriggerToStat();

    public:
        Syscarrefour();
        void Redifini(Loopcontroleur loop, Boutons boutons);
        void Setup();
        void Loop();
    };

}


#endif
