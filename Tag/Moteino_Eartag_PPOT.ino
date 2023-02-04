//Dev Shrestha
//Date Modified 8/1/2022
//Used to shock cow
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa/blob/master/src/LoRa.h
//https://github.com/sandeepmistry/arduino-LoRa/blob/master/API.md
#define SERIAL0_BAUD      115200
#define Radiopin 10  //Moteino Mega 4, Moteino 10
#define Shockpin 7
#define Noisepin 8  //I don't actually know what pin the noisemaker is on :C

//Variable Resistor X9313 https://www.renesas.com/us/en/document/dst/x9313-datasheet pins
#define INC 4  //Connected to X9313 pin 1
#define UD  5  //Connected to X9313 pin 2
#define CS  6  //Connected to X9313 pin 7

////////////VARIABLES DECLARIOS///////////
int MyID = 3;  //THis is my ID
String MsgIn = "";
String MsgOut = "";

int Duration = 100; //milli seconds
int Intensity = 0;
int NewIntensity = 31;
int MaxIntensity = 5; //arbitraty for now. I don't actually know the diode tollerance.
int CowID = 1; //Temporary value to hold incoming cow id
int Strength = 0;

bool loopStart = false;
int iter = 3;

//SETUP///////////////////////////////////
void setup() {

  pinMode(Shockpin, OUTPUT);
  digitalWrite(Shockpin, LOW); //First thing to do is disble shock pin
  pinMode(Radiopin, OUTPUT);
  pinMode(Noisepin, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(UD, OUTPUT);
  pinMode(INC, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  SetPot(31); //Resistance to 31 = Max between pin 5&6, 0 = Min between pin 5&6
  
  Serial.begin(SERIAL0_BAUD);
  while (!Serial);
  
  
  SetLora();
  digitalWrite(LED_BUILTIN, LOW); 
  MsgOut = "Current Intensity: " + String(Intensity);
  Broadcast();
 
 
}
//LOOP/////////////////////////////////////////
void loop() {
  /**
   * If valid message sound the beeper and turn on treatment loop, send message content into loop
   * loop untill the treatment number is reached or untill the interupt signal is recieved
   **/
  if(ValidMessage() and loopStart == false){
    Serial.println(MsgIn);
    loopStart = true;
    //--Sound Noise--
    digitalWrite(Noisepin, HIGH);   // turn the NOISE on
    delay(Duration);                // wait Duration  
    digitalWrite(Noisepin, LOW);
    //MsgOut = "Requested intensity: " + String(NewIntensity);
    //Broadcast();
    //delay(200);
    SetPot(NewIntensity);
    //MsgOut = "Current intensity: " + String(Intensity);
    //Broadcast();
    //delay(200);
    //Shock();
  
    }

  if(loopStart == true and iter > 0){
    if(Interupt()){
      loopStart = false;
      goto breakout;
    }
    NewIntensity = NewIntensity - 5;
    SetPot(NewIntensity);
    Shock();
    iter--;
  }

  breakout:
    __asm__("nop\n\t");//nop
}

//HELPER FUNCTIONS
bool Interupt(){
  bool valid = false;
  int packetSize = LoRa.parsePacket();
   
  if (packetSize) { //Message Received
      //  Sine pacet size is > 0, Read packet
    
    MsgIn = "";
    while (LoRa.available()) {
      MsgIn += (char)LoRa.read();
    }
    // Message received.Done reading 

    if (ValidFormat()){
      MsgIn = MsgIn.substring(1,MsgIn.length()-1);
      if (MsgIn == "INTERUPT"){
        valid = true;
      }
    }
  }
  return valid;
}

bool ValidMessage(){ // Check if there is a message rurrounded by <>
 //Read message is stored in global variable MsgIn 
 
  bool b = false;
  int packetSize = LoRa.parsePacket();
   
  if (packetSize) { //Message Received
      //  Sine pacet size is > 0, Read packet
    
    MsgIn = "";
    while (LoRa.available()) {
      MsgIn += (char)LoRa.read();
    }
    // Message received.Done reading 

    if (ValidFormat()){
      MsgIn = MsgIn.substring(1,MsgIn.length()-1); // Discard <> Sunstring(From Character, to Character+1); Substring counts from 0 and returns character up to position of to character-1. 
      int c1 = MsgIn.indexOf(','); //Finit first comma position
      int c2 = MsgIn.indexOf(',', c1+1); //Find Second Comma position
      
      Duration = MsgIn.substring(0, c1).toInt(); //Substring counts from 0. NOTE: Strangely returns character up to position of c1-1. C1 is not included
      Strength = MsgIn.substring(c1+1, c2).toInt();
      NewIntensity = 31-Strength*3;
      CowID = MsgIn.substring(c2+1).toInt();
      // Now check for limits 
      
      if (Duration >= 1 and Duration <= 2000 and NewIntensity >=0 and NewIntensity <=31 and CowID == MyID ){
 
         b = true;
         Blink(100,1); //Confim with blink for a valid message
        }
     
    }
   }
  return b; 
 
}
///////////////////////////////////
void Broadcast(){
  LoRa.beginPacket();  LoRa.print(MsgOut); LoRa.endPacket();
  //Serial.println(MsgOut);
  //Blink(100,2);
}
 /////////////////////////
bool ValidFormat(){
//MsgIn, a module level varaible
// A valid message has structure of <Durarion,Checksum>  
//Checksum is the char of checksum
//Returns True is Checksum matches
if (MsgIn.charAt(0) != '<' || MsgIn.charAt(MsgIn.length()-1) != '>' ){return false;} //VAlid message is enclossed in pair of bracket
//else if (Checksum(MsgIn.substring(0,MsgIn.length()-2)) != MsgIn.charAt(MsgIn.length()-2)){return false;} //charAt(MsgIn.length()-2) is Chcksum char
else{return true;}//If avobe two conditions fails to meet, the message must be valid
}
 
void Shock(){
  digitalWrite(Shockpin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(Duration);                // wait Duration
  if (Duration !=999){            //999 is a special case to turn the shock on continuously  
  digitalWrite(Shockpin, LOW);    // turn the LED off by making the voltage LOW
  MsgOut = "Shock Delivered, Duration: " + String(Duration) + " ms." + ", Stength: " + String(Strength) +" to cow ID: " + String(CowID);
  }
  else{MsgOut = "Shock is continuously on";}
  Broadcast();
 }  


///////////////////////////////////////

void SetLora(){
 
  digitalWrite(Radiopin, LOW);// Turn on radio
   LoRa.setPins(Radiopin);
   delay(100);
   LoRa.setSpreadingFactor(7);
   LoRa.setTxPower(10); //in dB
   if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  


}  
/////////////////////////////////

void Change(bool UpTrue, int Steps){
  
  digitalWrite(INC, HIGH);
  digitalWrite(UD, UpTrue);
  digitalWrite(CS, LOW);
  delay(10);
  
 
   for(int i = 0; i<Steps; i++){
    
    if ((UpTrue and Intensity >= 31) or (!UpTrue and Intensity <= 0)) {
      //digitalWrite(CS, HIGH); //The value is stored if INC in high when CS goes from low to high
      return;} 
    else {
      digitalWrite(INC,HIGH); //When parked keep INC high
      digitalWrite(CS, LOW);  //EnableChip
      delay(1);
      digitalWrite(INC,LOW);  // Increment/Decrement
      digitalWrite(CS, HIGH); // Disable Chip
      digitalWrite(INC,HIGH); //When parked keep INC high
      delay(1);
      UpTrue? Intensity++ : Intensity --;
    }
   }
}
void SetPot(int Wiper){
  if (Wiper >= MaxIntensity){
    Wiper = MaxIntensity;
  }
  if (Wiper == Intensity){}
  else if (Wiper > Intensity){
    Change(true, Wiper - Intensity);
   }
  else if (Wiper < Intensity){
    Change(false, Intensity-Wiper);
    }
   }

///////////////////////////////

char Checksum(String s){
//Returns Checksum (XOR of string S) as a char
  int  sum = 0;
    printf ("   ");
    for (int n = 0; n < s.length(); n++)  {
         sum = sum ^ s[n];
    }
    return (char)sum;
}
void Blink(int DELAY_MS, byte loops)
{
  while (loops--)
  {
    digitalWrite(LED_BUILTIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(LED_BUILTIN,LOW);
    delay(DELAY_MS);  
  }
}
