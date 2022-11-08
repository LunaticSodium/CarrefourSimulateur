// JavaScript source code




#define delaydejaune 2000




  int delayDeJaune = delaydejaune; //ms

  struct LEDReference(int num)
{
     
    int numero = num;
    bool illumine = false;
}

  struct LEDReference(int num, bool illum)
{
    
    int numero = num;
    bool illumine = illum;
}


void TrRougetoVert(int num)
{

}

void TrVerttoRouge(int num)
{

}


void setup() {
  // put your setup code here, to run once:

  int numdeLED = 0;
  
  LEDReference Np, Nl, Ns, Nr, Ep, El, Es, Er, Sp, Sl, Ss, Sr, Wp, Wl, Ws, Wr = new LEDReference(numdeLED++) //False=Rouge=Interdit=0=0V       True=Vert=Permet=1=5V

  int DefLED(int num)
    {

    }

}

void loop() {
    // put your main code here, to run repeatedly:



}

