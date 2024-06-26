#include <SoftwareSerial.h>

struct Point {
  float x, y;
};

struct Line {
  Point p1, p2;
};

String self = "000000000200"; //This is where our nanoTron ID goes

float distance_master = 1000;
float radius = 10.0;

String tagIDs[1][2] = {{"000000000201","0"}}; //element 0 is id, element 1 is shock status (0 for not shocked, 1 for shocked)
int tagIDsLength = 1; // (sizeof(tagIDs) / sizeof(int))/2;
int tagItter = 0;

int blinkRate = 1000;

//SoftwareSerial S3(15,14);

//Features to be added:
//Make the nanoRead function (and its implementation) more robust. 
// -- We should only get the first line of any Message and discard any subsequent characters
//Split function
//Automatic tag discovery
//Communication with other beacons
//--Workload distribution
//--Tag (and Beacon) Triangulation
//
//Look into other nanoTron communication commands (EIDN, EDAN, SDAT, and GDAT)
//
//CDMA for tag communication
//
//As a proof of concept/final assembly create a permeable gate setup

void setup() {
  Serial.begin(115200);
  Serial.println(tagIDsLength);
  //SoftwareSerial Serial2(15,14);
  Serial2.begin(115200);
  Serial2.print("SFAC\r\n"); //reset nanotron
  Serial2.println("SNID "+ self); //set nanotron ID
  Serial2.print("GNID\r\n");
  //Serial2.print("EBID 1\r\n"); //enable blink
  Serial2.print("BRAR 1\r\n"); //disable ranging broadcast
  Serial2.print("EBID 0\r\n");
  Serial2.print("NCFG 8\r\n");  // Set *RRN response to include RSSI data
  //Serial2.print("SBIV "+String(blinkRate)+"\r\n");  //Set host to blink every 1 second
  //Serial2.print("EDNI 1\r\n"); // Activate *DNI to be able to read messagaes
  Serial2.print("FNIN 01 00\r\n"); //clear the DNI buffer
  
  int timeout=0;
  while(!Serial2.available()&&timeout<10)  {
    timeout++;
    delay(1);
  }
  if (Serial2.available()){       
    while(Serial2.available()){
      Serial.write(Serial2.read());
      delay(1);  //a small delay helps the entire message come through before moving on. Otherwise messages got cut in half
    }
    Serial.println();
  }
 /*
  for(int i=0; i<tagIDsLength; i++){
    int checks = 0;
    bool checked = false;
    String c1 = "FNIN 0A 10";
    String target = sFill(String(tagIDs[i][0]), 12, '0');
    String message = "000001";
    Serial2.print(c1+target+message+"\r\n");
    Serial.print(c1+target+message+"\r\n");
    while(checked == false){
      //Serial2.print("RATO 1 000000000201 1000\r\n");
      String inMessage = readNanoTron();
      if(inMessage != ""){
        Serial.println(inMessage);
      }
      inMessage = getMessage(getDNI(inMessage));
      if(inMessage == "11"+target+"000011\r"){
        checked = true;
        Serial.println("Successfully contacted tag " + target);
      }
      else if ( checks == 10){
        checks = 0;
        Serial.println("Unable to contact tag " + target);
        checked = true;
      }
      else{
        checks++;
      }  
      //delay(blinkRate); //this guarantees that we have blinked atleast once so we don't 
      //potentially overwrite our message before it gets read
    }
  }
  */

}

void loop() {
  //check distance
  // The exact mechanisms for this may be subject ot change. It's possible that, as the nanoTron
  //allows for this, we could check all tags simultaneously. As it stands now we'll check whichever
  //tag we are concerned with this cycle and discard the ranging data for the rest.
  bool ranged = false;
  float distance = 0;
  int timeout=0;
  //while(ranged == false){  
  Serial2.print("RATO 0 "+tagIDs[tagItter][0]+"\r\n");
  Serial.print("RATO 0 "+tagIDs[tagItter][0]+"\r\n");
  String message = readNanoTron();
  delay(1000);
    if(message != ""){
      Serial.println("-----------------------------");
      Serial.println(message);
      Serial.println(getRRN(message).substring(0,12));
      Serial.println(tagIDs[tagItter][0]);
      Serial.println("-----------------------------");
    }
    
    if(getRRN(message).substring(0,12)==tagIDs[tagItter][0]){
      ranged = true;
      distance = float(getDistance(getRRN(message)).toInt());
      Serial.println(distance);
    }
  //}
  //send command
  if(distance < distance_master && distance != 0){
    String c1 = "FNIN 0A 10";
    String target = tagIDs[tagItter][0];
    String message = "111111";
    Serial2.print(c1+target+message+"\r\n");  
    Serial.print(c1+target+message+"\r\n");  

    tagIDs[tagItter][1] = "1";   
    
  }
  else if(distance >= distance_master && tagIDs[tagItter][1] == "1"){
    
    String c1 = "FNIN 0A 10";
    String target = tagIDs[tagItter][0];
    String message = "000000";
    Serial2.print(c1+target+message+"\r\n");  
    Serial.print(c1+target+message+"\r\n");  

    tagIDs[tagItter][1] = "0"; 
  }
  
  delay(blinkRate); //this guarantees that we have blinked atleast once so we don't 
  //potentially overwrite our message before it gets read
  //wait for response -- This adds an enourmous ammount of latency to the system may 
  //simply not do this
  if(tagItter == tagIDsLength-1){
    tagItter = 0;
  }
  else{
    tagItter++;
  }

}

String sFill(String s, int len, char fill){
  String filled = s;
  for(int i=0; i<len-s.length(); i++){
    filled = fill+filled;
  }
  return filled;
}
// These functions need to just read the first message recieved. It seems that while *DNI is
// activated *RRN will send an empty message. We need to discard this. ReadNanoTron2 would
// be preferable as it is simpler 
String readNanoTron2(){
  int timeout = 0;
  String nanoRead = "";
  while(!Serial2.available()&&timeout<10)  {
    timeout++;
    delay(1);
  }
  if (Serial2.available()){       
    while(Serial2.available()){
      Serial.write(Serial2.read());
      nanoRead = nanoRead+char(Serial2.read());
      delay(1);  //a small delay helps the entire message come through before moving on. Otherwise messages got cut in half
    }
    Serial.println();
  }
  return nanoRead;
}

String readNanoTron(){
  String nanoRead = "";
  while (Serial2.available()) {
    int bad=0;
    char c = Serial2.read();
    if (c == '*') {
      while (!Serial2.available()) {
         //Wait for a Response
      }
      char c1 = Serial2.read();
      nanoRead = nanoRead+ String(c)+String(c1);
      for(int i=1; i<80; i++){
        while (!Serial2.available()) {
         //Wait for a Response
        }
        c = Serial2.read();               //Keep Reading in Node ID 
        //Serial.print(c);
        if (c=='*'){
          bad = 1;  //variable to check for overlapping *RRN commands
          break;
        } 
        
          //stop filling out IDBlink at new line - not needed
        if (c=='\n') {
          nanoRead = nanoRead+ String(c);
          int track=i;
            //Serial.println(track);  //print length of serial command
          break;                          
        }
        nanoRead = nanoRead+ String(c);
      }
      if (bad==1) {    //Exit while loop if overlapping *RRN commands
        nanoRead = "overlap";
      }
    }
  }
  return nanoRead;
}

//A split function sure would be nice. Oh well...
//Also since we don't have both *RRNs and *DNIs at the same time we can generalize these 
//functions.
String getDNI(String readIn){
  String DNI = "";
  int readInSize = readIn.length();
  int DNIstart = -1;

  for(int i=0; i<readInSize; i++){
    //Serial.print(readIn.charAt(i));
    if(readIn.charAt(i) == ':'){
      String check = readIn.substring(i-3, i);
      if(check == "DNI"){
        DNIstart = i+1;
      }
    }
    if(readIn.charAt(i) == '*' && DNIstart != -1){
      DNI = readIn.substring(DNIstart, i-1);
    }
    
  }
  return DNI;
}

String getRRN(String readIn){
  String RRN = "";
  int readInSize = readIn.length();
  int RRNstart = -1;

  for(int i=0; i<readInSize; i++){
    //Serial.print(readIn.charAt(i));
    if(readIn.charAt(i) == ':'){
      String check = readIn.substring(i-3, i);
      if(check == "RRN"){
        RRNstart = i+1;
      }
    }
    if(readIn.charAt(i) == '\n' && RRNstart != -1){
      RRN = readIn.substring(RRNstart, i-1);
    }
    
  }
  return RRN;
}

String getMessage(String readIn){
  String message = "";
  int readInSize = readIn.length();
  int messageStart = -1;

  for(int i=readInSize; i>0; i--){
    //Serial.print(readIn.charAt(i));
    if(readIn.charAt(i) == ','){
      message = readIn.substring(i+1, readInSize);
      break;
    }
    
  }
  return message;
}
String getDistance(String readIn){
  String distance = "";
  int readInSize = readIn.length();
  int cutStart, cutEnd;
  int commaCounter = 0;

  for(int i=0; i<readInSize; i++){
    if (readIn.charAt(i) ==','){
      commaCounter++;
      if(commaCounter == 3){
        cutStart = i+1;
      }
      if(commaCounter == 4){
        cutEnd = i;
      }
    }
  }

  distance = readIn.substring(cutStart, cutEnd);
  Serial.println(readIn);
  Serial.println(distance);
  return distance;
  
}
