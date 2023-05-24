#include "SPIMemory.h"
#include <SX128XLT.h>
#include <MCP41xxx.h>
#define LED4 = A1; 
#define LED2 = A6; 
#define LED3 = A7; 
#define flashCS = A3; // Chip select for Flash
#define BUZZER = A2;
#define DIGIPOT_CS=A0
SPIFlash flash(flashCS); // Our Flash is on a different SPI bus
SX128XLT LoRa;
MCP41xxx DIGIPOT(DIGIPOT_CS);
int fail=0;
unsigned long addr = 0x01; // Random selection
byte data = 0x03; // Random data

#define NSS 10                                  //select pin on LoRa device
#define NRESET 9                                //reset pin on LoRa device
#define RFBUSY 7                                //RFBUSY pin on LoRa device
#define DIO1 5                                  //DIO1 pin on LoRa device, used for sensing RX and TX done 
#define LED1 8                                  //LED used to indicate transmission

#define LORA_DEVICE DEVICE_SX1280               //this is the device we are using
 
//*******  Setup LoRa Test Parameters Here ! ***************

//const uint32_t Frequency = 2445000000;                     //frequency of transmissions
const uint32_t Offset = 0;                                 //offset frequency for calibration purposes

//*******  Setup LoRa modem parameters here ! ***************
const uint8_t Bandwidth = LORA_BW_1600;                    //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF5;                  //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;                      //LoRa coding rate

const uint8_t BytesToPrint = 16;                           //number of bytes of packet to print



void setup()
{
   pinMode(LED4,OUTPUT);
   pinMode(LED2,OUTPUT);
   pinMode(LED3,OUTPUT);
   pinMode(LED4,OUTPUT);

   digitalWrite(LED4,HIGH);
   digitalWrite(LED2,HIGH);
   digitalWrite(LED3,HIGH);
   digitalWrite(LED4,HIGH);
   delay(500);
   digitalWrite(LED4,LOW);
   digitalWrite(LED2,LOW);
   digitalWrite(LED3,LOW);
   digitalWrite(LED4,LOW);
   delay(500);
   digitalWrite(LED4,HIGH);
   digitalWrite(LED2,HIGH);
   digitalWrite(LED3,HIGH);
   digitalWrite(LED4,HIGH);
   delay(500);
   digitalWrite(LED4,LOW);
   digitalWrite(LED2,LOW);
   digitalWrite(LED3,LOW);
   digitalWrite(LED4,LOW);
   delay(500);
   tone(BUZZER,500);
   delay(2000);
   noTone(BUZZER);

   
   SPI.begin(); // Don't forget to begin SPI! 
   flash.begin();
 

    
    if(flash.eraseBlock32K(addr))
    { 
        Serial.println("Block erased.");
    }
    else
    {
        Serial.println("Flash Memory Error.");
        fail=1;
    }

    if(flash.writeByte(addr, data, true))
    {
    }
    else
    {
        fail=1;
    }
    
    if (LoRa.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
    {
      
    }
    else
    {
      fail=1;
    }
}

void loop()
{
    if(fail)
    {
      digitalWrite(LED4,HIGH);
    }
    else
    {
      digitalWrite(LED4, HIGH);
      delay(500);
      digitalWrite(LED4, LOW);
      delay(500);
    }

}
