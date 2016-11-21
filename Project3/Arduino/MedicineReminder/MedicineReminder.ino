/*
  Button

 Turns on and off a light emitting diode(LED) connected to digital
 pin 3, when clicking on web page


 The circuit:
 * LED attached from resistor to ground
 * Web Controlled using PHP 
 * 1K resistor attached to pin 3 to LED
 */

/* 
      RFID RC522 connections
      SPI coonections with Arduino YUN 
      Digital PINS have not been connected to SPI pin 
      
      On Arduino YUN, the SPI pins are located as follows:
      
      pin 11: MOSI,
      pin 12: MISO,
      pin 13: SCK
      
           1 2
      MISO - . VTG
      SCK  . . MOSI
      RST  . . GND
           
      SPI: on the ICSP header. 
      These pins support SPI communication using the SPI library.
      Note that the SPI pins are not connected to any of the digital I/O pins as they are on the Uno, 
      They are only available on the ICSP connector. This means that if you have a shield that uses SPI, 
      but does NOT have a 6-pin ICSP connector that connects to the Yún's 6-pin ICSP header, the shield will not work.
      
*/

//For RFID RC522
#include <SPI.h>
#include <MFRC522.h>
//For Connection betwwen Arduino and Linino(WwebServer and Python)
#include <Bridge.h>
//Library for Server
#include <YunServer.h>
//Library for Client
#include <YunClient.h>
//Librbay for IO operations on Files
//#include <FileIO.h>
// constants won't change. They're used here to
// set pin numbers:


//Setting for SPI communication between RFID and 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
byte cardPresent;
byte RFIDValue[4];
//String uID;


const int GreenLed =  5;      // the number of the LED pin
const int RedLed =  4;      // the number of the LED pin
const int DoubleLed =  7;      // the number of the LED pin
// variables will change:

//tempLength=5;
char LEDCommand[20];
char userCommand[10];

//String LocationMedCheck="/mnt/sda1/scripts/Medicine.py";
String LocationMedCheck="/mnt/sda1/scripts/Medicine.py";
//digitalWrite(ledPin2, HIGH);
YunServer server;

//String userid;
String command;

void BlinkLed(int Ledpin,int times,int delaytime) {
  //1
      for (int j=0;j<times;j++){    
          digitalWrite(Ledpin, HIGH);   
          delay(delaytime);              
          digitalWrite(Ledpin, LOW);
          delay(delaytime);              
      }
}

//Function to read the RFID tag/card value from the reader RF RC522
String ReadRFID (){
  String uID="";
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return "0";
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return "0";
  }

// Dump debug info about the card; PICC_HaltA() is automatically called
// mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
            
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      //Serial.print(mfrc522.uid.uidByte[i], HEX);
      RFIDValue[i] = mfrc522.uid.uidByte[i] ; 
    } 
    //Serial.println();
    //converting array of inteer values of UID to Hex and storing as single Hex string 
    String val[4];
    for (int j=0 ; j <4 ;j++ ){
        val[j]=String(mfrc522.uid.uidByte[j],HEX);
        uID=uID + val[j];
    }
    //Serial.print(uID);      
    delay(1000);
    mfrc522.PICC_HaltA(); // Stop reading
    return uID; 
}

String CaptureRFID(){
      String uID=ReadRFID ();
      //Serial.println("Caputring RFID Value");
      
      //client.println("Led Status is: <br> ");
      if(uID != "0"){
          Serial.println("Sending the UID value");
          Bridge.put("arduinoCommand",uID);
          Serial.println(uID);   
      }
      //Not working .. as always staying on None
       /* else
        {
        Serial.println("Sending Blank  value");   
        Bridge.put("arduinoCommand","");
        }
       */ 
       delay(100) ;
   return uID;    
}

String  SplitCommand(String command)
{
        int slashIndex;
        int textIndex;
        slashIndex = command.indexOf('/');        
        command = command.substring(0,slashIndex); //Substring from 0 to till Slash
        // Serial.println("Inside split command");

        return command;
 }





void setup() {
   
   pinMode(13,OUTPUT);
   BlinkLed(13,5,500);  
   //setting up the connection between Controller and Linux Server  
   digitalWrite(13,LOW);
   Bridge.begin();
   digitalWrite(13,HIGH);
  
   //starting the Linux Server
   server.listenOnLocalhost();
   server.begin(); //start server
   delay(100);
   Serial.println("Starting Linux Server");
   
   //serial for Debuging
   Serial.begin(9600);
   Serial.println("Starting up");
   // initialize the LED pin as an output:
   pinMode(GreenLed,OUTPUT);
   pinMode(RedLed,OUTPUT);
   pinMode(DoubleLed,OUTPUT);
   BlinkLed(RedLed,5,100);
   BlinkLed(GreenLed,5,100);
   BlinkLed(DoubleLed,5,100);   
   //Initializing SPI  
    SPI.begin();      // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
}


//Python Command to Check if the Medicine has been taken or not 
String CheckMedicine (String command,String Location)
 {  
    String WebMessage;
    //Create a process
    Process p;
    //p.begin("/mnt/sda1/arduino/www/PasswordWebSaltPanel/PasswordWebSaltPanel.py");
    p.begin(Location);
    p.addParameter(command);
    p.run();
    //Read results ,append to string ,return
    while(p.available()>0)
    { 
      char c = p.read();
      WebMessage += c ;
    }
    //Serial.println("Inside python ");
    //Serial.println(WebMessage);
    return WebMessage;
 }

//int CheckRFID()

void loop() 
{
  //Acquring value from Bridge for lightcommand   
  Bridge.get("lightCommand",LEDCommand,20);
  String command =String(LEDCommand);
  command.trim();  
  Serial.println(command);    
  String MedCommand = SplitCommand(command);
     if(MedCommand == "CheckRFID")
    {
      String CheckUID=ReadRFID();
      command = command + CheckUID;  
      //Serial.println(command); 
      if (CheckUID != "0"){      
            Serial.println(command); 
            String WebMessage=CheckMedicine(command,LocationMedCheck);
            Serial.println(WebMessage);
            WebMessage.trim();
            if (WebMessage == "2"){
                BlinkLed(DoubleLed,5,100);
            }
            else if (WebMessage == "1"){
                BlinkLed(GreenLed,7,100);
            }
            else if (WebMessage == "0"){
                BlinkLed(RedLed,5,100);
              }
            else{
                Serial.println("No message");
                //Serial.println(WebMessage);
              }  
            }
      }
    else if(MedCommand == "Capture"){
      CaptureRFID();
      } 

    else if  (MedCommand == "TakeMed"){
          String CheckUID=ReadRFID();
          command = command + CheckUID;  
          //Serial.println(command); 
          if (CheckUID != "0"){      
                Serial.println(command); 
                String WebMessage=CheckMedicine(command,LocationMedCheck);
                Serial.println(WebMessage);
                WebMessage.trim();
            if (WebMessage == "2"){
                BlinkLed(DoubleLed,5,100);
            }
            else if (WebMessage == "1"){
                BlinkLed(GreenLed,7,100);
            }
            else if (WebMessage == "0"){
                BlinkLed(RedLed,5,100);
              }
           }
            else{
                Serial.println("No message");
                //Serial.println(WebMessage);
              }  
    }
       
     
     
     
   delay(1000);   
  //client.stop(); //stopping the client
}
