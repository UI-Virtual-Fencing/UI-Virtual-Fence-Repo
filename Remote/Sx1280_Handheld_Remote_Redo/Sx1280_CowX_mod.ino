//Dev Shrestha, Andrew Carefoot
//Date Modified 7/11/2023
// This is a working Code
//Change MyID to match cow tag before downloading
// Set debug to 0
/*This code has broken down the beep function into multiple functions.
This was done by request from the animal team to allow for a pause in the beep.
By breaking the different beeps into parts and checking for a cancel signal in between.*/
int MyID = 1;     //This is COW ID. Match before download
bool debug = 0;  //Turn = 1 for verbose output



/////////////////////////////
#include "CowX.h"
#define SERIAL0_BAUD      9600


void setup() {
  PinInitialize();
  
  Serial.begin(SERIAL0_BAUD);
  while (!Serial);//Wait until serial port is ready
  
  if (debug){Serial.print("Pin Initialized");}
  
  SPI.begin(); // No checking necessary
  
  
  while (! LT.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))  {
    Serial.println(F("Sx1280 not found. Check Connections"));
 
    delay(1000);
    }
  if (debug){Serial.println(F("Sx1280 found"));}


  LT.setupLoRa(2445000000, 0, LORA_SF7, LORA_BW_0400, LORA_CR_4_5);      //configure frequency and LoRa settings
  if (debug){Serial.println(F("Radio initialized at: 2.445 GHz, SF07"));}
  
  MsgOut = "C" + String(MyID)+ "," + String(MaxBeeps+2)+"C";
  SendConfirmation(MsgOut);
   
}

void loop() {
 if (ParseMessage() or !Cancelled){  //If there is a new message or not in Cancelled state. Cancelled is also updated in Parsemessage.
    if (Cancelled){
      if (debug){Serial.println(F("Shock cancelled! "));}
      MsgOut = "C" + String(MyID) + ",0C";
      SendConfirmation(MsgOut);
      Beeped = 0;
      RecordEvent();
      }
    else if(Paused){
      ParseMessage();
    }
    else{
      if(ShockCounter <= MaxShocks && TimerShock <= millis()){
        if (Beeped <= MaxBeeps){  //Need to use <= because beep is reset to 0 and increments only after beeping
          Beep();
        }
      }
       else if(Timer <= millis()){
           if(debug){Serial.println("Shocking!");}
           Shock();
        }
      }
   }
}



//LOOP/////////////////////////////////////////

    
 
