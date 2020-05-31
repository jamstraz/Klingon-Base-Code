//Version 3.00
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <PalatisSoftPWM.h>

SoftwareSerial mySerial(4, 5); // RX, TX
DFPlayerMini_Fast myMP3;
//-------------------------------  PWM CODE for Palatis
SOFTPWM_DEFINE_PIN13_CHANNEL(0);
SOFTPWM_DEFINE_OBJECT(1);
//-------------------------------
#define PUSHED                   LOW

//intensiy
#define randomLowLevel             0
#define randomHighLevel            100

//PWM
#define randomLowInterval          20
#define randomHighInterval         100

const byte buttonPin1            = A0;    //connected to GND & D2
const byte buttonPin2            = A1;
const byte buttonPin3            = A2;

const byte led1                  = 3;    //PWM pin the LED is attached to
const byte led2                  = 10;   //PWM pin the LED is attached to
const byte led3                  = 11;   //PWM pin the LED is attached to
const byte led4                  = 6;

const byte threshold             = 255;

const unsigned long intervalSlow = 250;
const unsigned long intervalFast = 750;
unsigned long interval           = intervalSlow;

unsigned long switchMillis;
unsigned long currentMillis;
unsigned long previousMillis;
unsigned long fadeMillis;
unsigned long mp3Millis;

unsigned long led2Millis;
unsigned long random2Interval;

unsigned long led3Millis;
unsigned long random3Interval;

unsigned long led4Millis;
unsigned long random4Interval;

unsigned long Palatis0Millis;
unsigned long random5Interval;

bool led4Flag = true;
byte buttonState1;
byte lastButtonState1;

byte buttonState2;
byte lastButtonState2;

byte buttonState3;
byte lastButtonState3;

unsigned int fadeCounter;
unsigned int button2counter;
unsigned int CloakCounterDown;
unsigned int CloakThreshold     = 0;
unsigned int fadeThreshold1     = 100;
unsigned int fadeThreshold      = 255;
unsigned int fadeSpeed          = 10;
unsigned int ledState;
unsigned int counter;
unsigned int cycle;
unsigned int period1 = 5000;

//****************************************
//define our states in the State Machines

enum STATESTORP {TORPSEQUENCE, WARMUP, FLASHON, ENDSTATE};
STATESTORP mState1 = TORPSEQUENCE;

enum STATES {STARTUP, STATE1, STATE2, STATE3, DECLOAK1, DECLOAK2, DECLOAK3};
STATES mState2 = STARTUP;

enum RAMDOM {rSTARTUP, rSTATE1, rSTATE2, rSTATE3, rSTATE4, rSTATE5};
RAMDOM mState3 = rSTARTUP;

//****************************************************************
void setup()
{
  Serial.begin(9600);
  //Serial.begin(115200);
  mySerial.begin(9600);
  myMP3.begin(mySerial);

  Serial.println("Setting volume to max");
  myMP3.volume(25);
  delay(20);
//----------------
PalatisSoftPWM.begin(60);
//----------------

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  analogWrite (led2, 255);
  analogWrite (led3, 255);

} //END of setup()
//****************************************************************
void loop()
{
  //save the current Arduino time
  currentMillis = millis();

  //****************************************
  checkSwitches();

  //****************************************
  stateMachineStartup();

  //****************************************
  stateMachineTorp();

  //****************************************
  stateMachineRandom();

  //****************************************
  //toggle led4 stuff
  if (led4Flag == true)
  {
    if (currentMillis - previousMillis >= interval)
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW)
      {
        ledState = HIGH;
      }
      else
      {
        ledState = LOW;
        counter ++;
      }
      digitalWrite(led4, ledState);
    }

    if (counter == 2)
    {
      //reset counter and swap interval
      counter = 0;
      if ( interval == intervalSlow)
      {
        interval = intervalFast;
      }

      else
      {
        interval = intervalSlow;
      }
    }

  } //END of   if (led4Flag == true)


  //****************************************
  // Other non blocking code goes here
  //****************************************


} //END of loop()


//                s t a t e M a c h i n e T o r p ( )
//****************************************************************
void stateMachineTorp()
{
  switch (mState1)
  {
    //**************
    case TORPSEQUENCE:
      {
        //do nothing
      }
      break;

    //**************
    case WARMUP:
      {
        //time to fade LED1 ?
        if (currentMillis - fadeMillis >= fadeSpeed )
        {
          //restart the TIMER
          fadeMillis = currentMillis;


          if (fadeCounter <= fadeThreshold1)
          {
            //update led with new analog value (brightness)
            analogWrite(led1, fadeCounter);
            fadeCounter++;
          }

          else
          {
            mState1 = FLASHON;

            //restart the TIMER
            fadeMillis = currentMillis;
          }

        }
      }
      break;

    //**************
    case FLASHON:
      {
        //wait here for 1 seconds
        if (currentMillis - fadeMillis >= 1000ul)
        {

          mState1 = ENDSTATE;
          analogWrite(led1, 255);
          fadeCounter = 0;

          //restart the TIMER
          fadeMillis = currentMillis;
        }
      }
      break;

    //**************
    case ENDSTATE:
      {
        //wait here for 1 seconds
        if (currentMillis - fadeMillis >= 100ul)
        {

          //analogWrite(led1, 0);

          myMP3.play(1);
          //if (currentMillis-mp3Millis >=1000ul){
          if (currentMillis >= mp3Millis + period1) {
            //mp3Millis += period1;

            mp3Millis = currentMillis;
            Serial.println (mp3Millis);
            //myMP3.sleep();
          }
          analogWrite(led1, 0);
          //we ar now finished with this animation
          mState1 = TORPSEQUENCE;
        }
      }

      break;

      //**************


  } //END of switch/case

} //END of stateMachineTorp()


//             s t a t e M a c h i n d e S t a r t u p ( )
//****************************************************************
void stateMachineStartup()
{
  switch (mState2)
  {
    //**************
    case STARTUP:
      {

        //do nothing
      }
      break;

    //**************
    case STATE1:

      {

        //time to fade LED2 ?
        if (currentMillis - fadeMillis >= fadeSpeed )
        {
          //restart the TIMER
          fadeMillis = currentMillis;


          if (CloakCounterDown > CloakThreshold)
          {
            //update led with new analog value (brightness)
            //Serial.println (CloakCounterDown);
            analogWrite(led2, CloakCounterDown);
            //Serial.println (fadeCounter);
            CloakCounterDown--;
            led4Flag = false;
            analogWrite(led4, 0);
           


          }

          else
          {
            mState2 = STATE2;
            myMP3.play(2);
            //if (currentMillis-mp3Millis >=1000ul){
            if (currentMillis >= mp3Millis + period1) {
              //mp3Millis += period1;

              mp3Millis = currentMillis;
              // Serial.println (mp3Millis);
              //myMP3.sleep();
            }

            //restart the TIMER
            fadeMillis = currentMillis;
          }

        }
      }
      break;

    //**************
    case STATE2:
      {
        //wait here for 2 seconds
        //if (currentMillis - fadeMillis >= 2000ul)
        {
          mState2 = STATE3;
          CloakCounterDown = 0;
          fadeCounter = 255;

          //restart the TIMER
          fadeMillis = currentMillis;
          analogWrite(led2, 0);
        }
      }
      break;

    //**************
    case STATE3:
      {
        //time to fade LED3 ?
        if (currentMillis - fadeMillis >= fadeSpeed )
        {
          //restart the TIMER
          fadeMillis = currentMillis;

          if (fadeCounter <= fadeThreshold)
          {
            //update led with new analog value (brightness)
            analogWrite(led3, fadeCounter);
            fadeCounter--;

          }
          else {
            mState2 = STARTUP;
          }
        }
      }
      break;

    //**************

    case DECLOAK1:
      {
        Serial.println("Decloak 1");
        //time to fade LED3 ?
        if (currentMillis - fadeMillis >= fadeSpeed )
        {
          //restart the TIMER
          fadeMillis = currentMillis;


          if (fadeCounter <= fadeThreshold)
          {
            //update led with new analog value (brightness)
            analogWrite(led3, fadeCounter);
            fadeCounter++;



          }

          else
          {
            mState2 = DECLOAK2;
            myMP3.play(3);
            //if (currentMillis-mp3Millis >=1000ul){
            if (currentMillis >= mp3Millis + period1) {
              //mp3Millis += period1;

              mp3Millis = currentMillis;
              //myMP3.sleep();
            }

            //restart the TIMER
            fadeMillis = currentMillis;
          }

        }
      }
      break;
    //**************
    case DECLOAK2:
      {
        Serial.println("Decloak 2");
        //wait here for 2 seconds
        if (currentMillis - fadeMillis >= 2000ul)
        {
          mState2 = DECLOAK3;

          fadeCounter = 0;

          //restart the TIMER
          fadeMillis = currentMillis;
        }
      }
      break;

    //**************

    case DECLOAK3:
      {
        Serial.println("Decloak 3");
        //time to fade LED3 ?
        if (currentMillis - fadeMillis >= fadeSpeed )
        {
          //restart the TIMER
          fadeMillis = currentMillis;

          if (fadeCounter <= fadeThreshold)
          {
            //update led with new analog value (brightness)
            analogWrite(led2, fadeCounter);
            fadeCounter++;
            led4Flag = true;


          }
          else {

            mState2 = STARTUP;
          }
        }
      }
      break;

      //**************


  } //END of switch/case

} //END of stateMachineStartup()


//                s t a t e M a c h i n e R a n d o m ( )
//****************************************************************
void stateMachineRandom()
{
  switch (mState3)
  {
    //**************
    case rSTARTUP:
      {
        //do nothing
      }
      break;

    //**************
    case rSTATE1:
      {
        //initialize things
        cycle = 0;

        analogWrite(led2, random(randomLowLevel, randomHighLevel));
        random2Interval = random(randomLowInterval, randomHighInterval);
        led2Millis = currentMillis;

        analogWrite(led3, random(randomLowLevel, randomHighLevel));
        random3Interval = random(randomLowInterval, randomHighInterval);
        led3Millis = currentMillis;

        analogWrite(led4, random(randomLowLevel, randomHighLevel));
        random4Interval = random(randomLowInterval, randomHighInterval);
        led4Millis = currentMillis;

        PalatisSoftPWM.set(0, random(randomLowLevel, randomHighLevel));
        random5Interval = random(randomLowInterval, randomHighInterval);
        Palatis0Millis = currentMillis;

        mState3 = rSTATE2;
      }
      break;

    //**************
    case rSTATE2:
      {
        if (currentMillis - led2Millis >= random2Interval)
        {
          analogWrite(led2, random(randomLowLevel, randomHighLevel));
          random2Interval = random(randomLowInterval, randomHighInterval);
          led2Millis = currentMillis;
        }

        mState3 = rSTATE3;
      }
      break;

    //**************
    case rSTATE3:
      {
        if (currentMillis - led3Millis >= random3Interval)
        {
          analogWrite(led3, random(randomLowLevel, randomHighLevel));
          random3Interval = random(randomLowInterval, randomHighInterval);
          led3Millis = currentMillis;
        }

        mState3 = rSTATE4;
      }
      break;
   //***************
     case rSTATE4:
      {
        if (currentMillis - led4Millis >= random3Interval)
        {
          analogWrite(led4, random(randomLowLevel, randomHighLevel));
          random3Interval = random(randomLowInterval, randomHighInterval);
          led4Millis = currentMillis;
        }

        mState3 = rSTATE5;
      }
      break;

    //**************
    case rSTATE5:
      {
        if (currentMillis - Palatis0Millis >= random4Interval)
        {
          PalatisSoftPWM.set(0, random(randomLowLevel, randomHighLevel));
          random4Interval = random(randomLowInterval, randomHighInterval);
          Palatis0Millis = currentMillis;

          //start the next cycle
          cycle++;

          //are we finished?
          if (cycle > 250)
          {
            //we are now finish with the damage animation
            mState3 = rSTARTUP;

            //enable normal flashing on led4
            led4Flag = true;

            analogWrite(led2, 255);
            analogWrite(led3, 255);
            PalatisSoftPWM.set(0,255);

            break;
          }
        }

        //back to led2
        mState3 = rSTATE2;
      }
      break;

  } //END of switch/case

} //END of stateMachineRandom()


//                    c h e c k S w i t c h e s ( )
//****************************************************************
void checkSwitches()
{
  //is it time to look at the switches ?
  if (currentMillis - switchMillis < 50)
  {
    //not time yet
    return;
  }

  //restart the TIMER
  switchMillis = currentMillis;

  //*******************************                  b u t t o n P i n 1
  buttonState1 = digitalRead(buttonPin1);
  //switch changed state ?
  if (lastButtonState1 != buttonState1)
  {
    //update to the new state
    lastButtonState1 = buttonState1;

    if (buttonState1 == PUSHED)
    {
      Serial.println("pressed 1");

      mState1 = ENDSTATE;
      //Enter Warmup
      mState1 = WARMUP;

      //start 'led' at intensity of 0
      fadeCounter = 0;

      //restart the TIMER
      fadeMillis = currentMillis;
    }

    else
    {
      //do nothing
    }

  } //END of this switch

  //*******************************                  b u t t o n P i n 2
  buttonState2 = digitalRead(buttonPin2);

  //switch changed state ?
  if (lastButtonState2 != buttonState2)
  {
    //update to the new state
    lastButtonState2 = buttonState2;

    if (buttonState2 == PUSHED)
    {
      Serial.println("pressed 2");
      Serial.println (button2counter);
      // analogWrite (led2,0); removed 5-25
      //analogWrite(led3, 0); removed 5-35
      //enter STATE1
      button2counter ++;
      mState2 = STATE1;

      //start 'led' at intensity of 0
      fadeCounter = 0;
      CloakCounterDown = 255;

      //restart the TIMER
      fadeMillis = currentMillis;
    }

    else if (button2counter == 2)
    {
      mState2 = DECLOAK1;

      Serial.println ("Decloaking");
      fadeCounter = 0;
      led4Flag == true;
      analogWrite(led2, 0);
      analogWrite(led3, 0);
      button2counter = 0;
      // CloakCounterDown = 255;
      fadeMillis = currentMillis;
      //turn off leds when the 'buttonPin2' is released

    }
  }
  //END of this StateMachine


  //*******************************                  b u t t o n P i n 3
  buttonState3 = digitalRead(buttonPin3);

  //switch changed state ?
  if (lastButtonState3 != buttonState3)
  {
    //update to the new state
    lastButtonState3 = buttonState3;

    if (buttonState3 == PUSHED)
    {
      Serial.println("pressed 3");

      //disable normal flashing on led4
      led4Flag = false;

      //start the damage flashing sequence
      mState3 = rSTATE1;

      //randomize random
      randomSeed(analogRead(0)); //A0 is left floating
    }

    else
    {
      //do nothing

    }

  } //END of this switch


} //END of checkSwitches()

//****************************************************************
