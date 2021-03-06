/*
 @cmoz
 Portable prototype October 2014
 
 ** Edit the initial tag numbers for the devices **
 Saves the scanned item in EEPROM so that when off it restores
 LEDs with RFID - RC522 (Mifare reader)
 Arduino Pro Mini 
 Piezo - Vibrating Motor
 */
 
// the sensor communicates using SPI, so include the library:
#include <SPI.h>
#include <EEPROM.h>

#define uchar unsigned char
#define uint unsigned int


#define MAX_LEN 16

/////////////////////////////////////////////////////////////////////
//set the pin
/////////////////////////////////////////////////////////////////////
const int chipSelectPin = 10;
const int NRSTPD = 3;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SET TAG NUMBERS FOR ALL DEVICES HERE
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                int blueTag = 148;
                int redTag = 247;
                int yellowTag = 34;
                int greenTag = 82;
                int pinkTag = 102; 
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//MF522
#define PCD_IDLE              0x00               //NO action;
#define PCD_AUTHENT           0x0E               
#define PCD_RECEIVE           0x08               
#define PCD_TRANSMIT          0x04               
#define PCD_TRANSCEIVE        0x0C             
#define PCD_RESETPHASE        0x0F             
#define PCD_CALCCRC           0x03               //CRC

//Mifare_One卡片命令字
#define PICC_REQIDL           0x26               
#define PICC_REQALL           0x52               
#define PICC_ANTICOLL         0x93               
#define PICC_SElECTTAG        0x93               
#define PICC_AUTHENT1A        0x60               
#define PICC_AUTHENT1B        0x61               
#define PICC_READ             0x30               
#define PICC_WRITE            0xA0               
#define PICC_DECREMENT        0xC0               
#define PICC_INCREMENT        0xC1               
#define PICC_RESTORE          0xC2               
#define PICC_TRANSFER         0xB0            
#define PICC_HALT             0x50             


//MF522
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2


//------------------MFRC522---------------
//Page 0:Command and Status
#define     Reserved00            0x00    
#define     CommandReg            0x01    
#define     CommIEnReg            0x02    
#define     DivlEnReg             0x03    
#define     CommIrqReg            0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F
//Page 1:Command     
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG    
#define     Reserved20            0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg           0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:TestRegister     
#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     Reserved31            0x3C   
#define     Reserved32            0x3D   
#define     Reserved33            0x3E   
#define     Reserved34   0x3F
//-----------------------------------------------

//4
uchar serNum[5];

uchar  writeData[16]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100};  // 100
uchar  moneyConsume = 18 ;  //18
uchar  moneyAdd = 10 ;  //10
//A 6Byte
 uchar sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             //{0x19, 0x84, 0x07, 0x15, 0x76, 0x14},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                            };
 uchar sectorNewKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x84,0x07,0x15,0x76,0x14},
                                 //you can set another ket , such as  " 0x19, 0x84, 0x07, 0x15, 0x76, 0x14 "
                                 //{0x19, 0x84, 0x07, 0x15, 0x76, 0x14, 0xff,0x07,0x80,0x69, 0x19,0x84,0x07,0x15,0x76,0x14},
                                 // but when loop, please set the  sectorKeyA, the same key, so that RFID module can read the card
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0x19,0x33,0x07,0x15,0x34,0x14},
                               };
                               
   int timer = 5000;           // The higher the number, the slower the timing.                           
   int ledPins[] = { 
          4, 5, 6, 7, 8, 9 };  // an array of pin numbers to which LEDs are attached   
 
   int pinCount = 6;           // the number of pins (i.e. the length of the array) 

   int otherPins[] = {
        A1,A0 }; // A1 / [0] =Vibe;  Pin A0 [1] Piezo
 
   int pinOtherCount = 2; 
 
            boolean tag1Card; // green
            boolean tag2Card; // yellow
            boolean tag3Card; // red
            boolean tag4Card; // blue
            boolean tag5Card; // pink
  
  //constants used to identify EEPROM addresses
  const int MY_BOOL1 = 0; 
  const int MY_BOOL2 = 1;
  const int MY_BOOL3 = 2;
  const int MY_BOOL4 = 3;
  const int MY_BOOL5 = 4;  

                               
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ SETUP +++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup() {                
   Serial.begin(19200); // RFID reader SOUT pin connected to Serial RX pin at 2400bps 
 
   // the array elements are numbered from 0 to (pinCount - 1).
  // use a for loop to initialize each pin as an output:
  for (int thisPin = 0; thisPin < pinCount; thisPin++)  {
    pinMode(ledPins[thisPin], OUTPUT);   
  }
    
    pinMode(otherPins[0], OUTPUT); 
    pinMode(otherPins[1], OUTPUT); 
  
 // start the SPI library:
  SPI.begin();
  
  pinMode(chipSelectPin,OUTPUT);             // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin 
    digitalWrite(chipSelectPin, LOW);          // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);               // Set digital pin 10 , Not Reset and Power-down
    digitalWrite(NRSTPD, HIGH);

  MFRC522_Init();  
  
    // turn the LEDS off
    digitalWrite(ledPins[0], LOW); 
    digitalWrite(ledPins[1], LOW);
    digitalWrite(ledPins[2], LOW);
    digitalWrite(ledPins[3], LOW);
    digitalWrite(ledPins[4], LOW);
    delay(100);   
    
      Serial.println("set up ready");  
      setLEDsToLow();
   
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ LOOP ++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void loop()
{
  if ((tag1Card == true) && (tag2Card == true) && (tag3Card == true) && (tag4Card == true) && (tag5Card == true)) {
            Serial.println("all tags scanned");   
                
  }
  
   uchar i,tmp;
   uchar status;
        uchar str[MAX_LEN];
        uchar RC_size;
        uchar blockAddr; //0～63
        String mynum = "";
 
 status = MFRC522_Request(PICC_REQIDL, str); 
 if (status == MI_OK)
 {
   Serial.println(" ");
   Serial.println("Card detected");
     //Serial.print(str[0],BIN);
     //Serial.print(" , ");
 }

 // 4
 status = MFRC522_Anticoll(str);
 memcpy(serNum, str, 5);
 if (status == MI_OK)
 {

                        Serial.println("The card's number is  : ");
 Serial.print(serNum[0]);
                        Serial.print(" , ");
 Serial.print(serNum[1]);
                        Serial.print(" , ");
 Serial.print(serNum[2]);
                        Serial.print(" , ");
 Serial.print(serNum[3]);
                        Serial.print(" , ");
 Serial.print(serNum[4]);
                        Serial.println(" ");
                        
                        // Should really check all pairs, but for now we'll just use the first
                       // beep when tag placed over area
  //tone(otherPins[1],800,100); 
  digitalWrite(ledPins[5], LOW);
  digitalWrite(otherPins[0], HIGH);
  delay(250);
  //tone(otherPins[1],1300,100); //piezo
  digitalWrite(ledPins[5], HIGH);
  digitalWrite(otherPins[0], LOW); // vibe motor
  
                        if((serNum[0] == 454)&&(serNum[1] == 445)) {
                          Serial.println("Hello Grant");
                        } 
                        
                        if(serNum[0] == blueTag) {
                          Serial.println("Hello Mr. BLue");
                          tag4Card ^= true;
                          }
                          
                        if(serNum[0] == redTag) {
                          Serial.println("Hello Mr Red");
                          tag3Card ^= true;
                          } 
                          
                         if(serNum[0] == yellowTag) {
                          Serial.println("Hello Yellow");
                          tag2Card ^= true;
                          }
                         
                         if(serNum[0] == greenTag) {
                          Serial.println("Hello Mr. Green");
                          tag1Card ^= true;
                          } 
                          
                          if(serNum[0] == pinkTag) {
                          Serial.println("Hello Mr. Pink");
                          tag5Card ^= true;
                        }
                        
                        delay(1000);
 }
              setLEDsToLow();

 MFRC522_Halt();              
          
}

   // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void setLEDsToLow(){ 

                if (tag1Card == true) {
                digitalWrite(ledPins[1], LOW);  
                //EEPROM.write(MY_BOOL1, 1);
                delay(10);   
                } else {
                  digitalWrite(ledPins[1], HIGH);
                  //EEPROM.write(MY_BOOL1, 0);
                }
                                          
                if (tag2Card == true) {
                digitalWrite(ledPins[2], LOW);
               // EEPROM.write(MY_BOOL2, 1);   
                delay(10);               
                } else {
                  digitalWrite(ledPins[2], HIGH);
                 // EEPROM.write(MY_BOOL2, 0);
                }
            
                if (tag3Card == true) {
                digitalWrite(ledPins[3], LOW); 
               // EEPROM.write(MY_BOOL3, 1); 
                delay(10);               
                } else {
                  digitalWrite(ledPins[3], HIGH);
                //  EEPROM.write(MY_BOOL3, 0);
                }
        
                if (tag4Card == true) {
                digitalWrite(ledPins[4], LOW); 
               // EEPROM.write(MY_BOOL4, 1);
                delay(10);                
                } else {
                  digitalWrite(ledPins[4], HIGH);
               //   EEPROM.write(MY_BOOL4, 0);
                }
            
                if (tag5Card == true) {
                digitalWrite(ledPins[0], LOW);  
               // EEPROM.write(MY_BOOL5, 1);
                delay(10);               
                } else {
                  digitalWrite(ledPins[0], HIGH);
                 // EEPROM.write(MY_BOOL5, 0);
                }
  

    }
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/*
 * Write_MFRC5200
 * MFRC522
 * addr--；val--
 * 
 */
void Write_MFRC522(uchar addr, uchar val)
{
 digitalWrite(chipSelectPin, LOW);

 //：0XXXXXX0
 SPI.transfer((addr<<1)&0x7E); 
 SPI.transfer(val);
 
 digitalWrite(chipSelectPin, HIGH);
}


/*
Read_MFRC522
 MFRC522
 addr--

 */
uchar Read_MFRC522(uchar addr)
{
 uchar val;

 digitalWrite(chipSelectPin, LOW);

 //：1XXXXXX0
 SPI.transfer(((addr<<1)&0x7E) | 0x80); 
 val =SPI.transfer(0x00);
 
 digitalWrite(chipSelectPin, HIGH);
 
 return val; 
}

/*
SetBitMask
 RC522
：reg--;mask--

 */
void SetBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}


/*
ClearBitMask
RC522
reg--;mask--

 */
void ClearBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
} 


/*
AntennaOn
1ms

 */
void AntennaOn(void)
{
 uchar temp;

 temp = Read_MFRC522(TxControlReg);
 if (!(temp & 0x03))
 {
 SetBitMask(TxControlReg, 0x03);
 }
}


/*
AntennaOff
1ms

 */
void AntennaOff(void)
{
 ClearBitMask(TxControlReg, 0x03);
}


/*
ResetMFRC522
RC522

 */
void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}


/*
InitMFRC522
RC522

 */
void MFRC522_Init(void)
{
 digitalWrite(NRSTPD,HIGH);

 MFRC522_Reset();
   
 //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    Write_MFRC522(TModeReg, 0x8D); //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E); //TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);           
    Write_MFRC522(TReloadRegH, 0);
 
 Write_MFRC522(TxAutoReg, 0x40); //100%ASK
 Write_MFRC522(ModeReg, 0x3D); //CRC0x6363 ???

 //ClearBitMask(Status2Reg, 0x08); //MFCrypto1On=0
 //Write_MFRC522(RxSelReg, 0x86); //RxWait = RxSelReg[5..0]
 //Write_MFRC522(RFCfgReg, 0x7F);    //RxGain = 48dB

 AntennaOn(); 
}


/*
 MFRC522_Request

 reqMode--
 *  TagType--
 *   0x4400 = Mifare_UltraLight
 * 0x0400 = Mifare_One(S50)
 * 0x0200 = Mifare_One(S70)
 * 0x0800 = Mifare_Pro(X)
 * 0x4403 = Mifare_DESFire
MI_OK
 */
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
 uchar status;  
 uint backBits; 

 Write_MFRC522(BitFramingReg, 0x07); //TxLastBists = BitFramingReg[2..0] ???
 
 TagType[0] = reqMode;
 status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

 if ((status != MI_OK) || (backBits != 0x10))
 {    
 status = MI_ERR;
 }
   
 return status;
}


/*
MFRC522_ToCard
RC522 ISO14443
command--MF522
 *  sendData-- RC522
 *  sendLen--  
 *  backData--
 *  backLen--
 * MI_OK
 */
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;
    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT: 
 {
 irqEn = 0x12;
 waitIRq = 0x10;
 break;
 }
 case PCD_TRANSCEIVE: //FIFO
 {
 irqEn = 0x77;
 waitIRq = 0x30;
 break;
 }
 default:
 break;
    }
   
    Write_MFRC522(CommIEnReg, irqEn|0x80); 
    ClearBitMask(CommIrqReg, 0x80); 
    SetBitMask(FIFOLevelReg, 0x80); //FlushBuffer=1, FIFO初始化
    
 Write_MFRC522(CommandReg, PCD_IDLE); //NO action; ???

 //FIFO
    for (i=0; i<sendLen; i++)
    {   
 Write_MFRC522(FIFODataReg, sendData[i]);    
 }

 
 Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
 SetBitMask(BitFramingReg, 0x80); //StartSend=1,transmission of data starts  
 }   
    
 
 i = 2000; //M125ms ???
    do 
    {
 //CommIrqReg[7..0]
 //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80); //StartSend=0
 
    if (i != 0)
    {    
        if(!(Read_MFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   
 status = MI_NOTAGERR; //??   
 }

            if (command == PCD_TRANSCEIVE)
            {
                n = Read_MFRC522(FIFOLevelReg);
               lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {   
 *backLen = (n-1)*8 + lastBits;   
 }
                else
                {   
 *backLen = n*8;   
 }

                if (n == 0)
                {   
 n = 1;    
 }
                if (n > MAX_LEN)
                {   
 n = MAX_LEN;   
 }
 
 //FIFO
                for (i=0; i<n; i++)
                {   
 backData[i] = Read_MFRC522(FIFODataReg);    
 }
            }
        }
        else
        {   
 status = MI_ERR;  
 }
        
    }
 
    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE); 

    return status;
}


/*
MFRC522_Anticoll

serNum--45
MI_OK
 */
uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
 uchar serNumCheck=0;
    uint unLen;
    

    //ClearBitMask(Status2Reg, 0x08); //TempSensclear
    //ClearBitMask(CollReg,0x80); //ValuesAfterColl
 Write_MFRC522(BitFramingReg, 0x00); //TxLastBists = BitFramingReg[2..0]
 
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
 {
 
 for (i=0; i<4; i++)
 {   
   serNumCheck ^= serNum[i];
 }
 if (serNumCheck != serNum[i])
 {   
 status = MI_ERR;    
 }
    }

    //SetBitMask(CollReg, 0x80); //ValuesAfterColl=1

    return status;
} 


/*
CalulateCRC
 MF522 CRC
pIndata--CRC，len--，pOutData--CRC

 */
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    ClearBitMask(DivIrqReg, 0x04); //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80); //FIFO
    //Write_MFRC522(CommandReg, PCD_IDLE);

 //FIFO
    for (i=0; i<len; i++)
    {   
 Write_MFRC522(FIFODataReg, *(pIndata+i));   
 }
    Write_MFRC522(CommandReg, PCD_CALCCRC);

 //CRC
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04)); //CRCIrq = 1

 //CRC
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}


/*
MFRC522_SelectTag

serNum--

 */
uchar MFRC522_SelectTag(uchar *serNum)
{
    uchar i;
 uchar status;
 uchar size;
    uint recvBits;
    uchar buffer[9]; 

 //ClearBitMask(Status2Reg, 0x08); //MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
     buffer[i+2] = *(serNum+i);
    }
 CalulateCRC(buffer, 7, &buffer[7]); //??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    
    if ((status == MI_OK) && (recvBits == 0x18))
    {   
 size = buffer[0]; 
 }
    else
    {   
 size = 0;    
 }

    return size;
}


/*
MFRC522_Auth

authMode--
                 0x60 = A
                 0x61 = B 
             BlockAddr--
             Sectorkey--
             serNum--4
MI_OK
 */
uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
    uchar status;
    uint recvBits;
    uchar i;
 uchar buff[12]; 

 
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {    
 buff[i+2] = *(Sectorkey+i);   
 }
    for (i=0; i<4; i++)
    {    
 buff[i+8] = *(serNum+i);   
 }
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {   
 status = MI_ERR;   
 }
    
    return status;
}


/*
MFRC522_Read

blockAddr--;recvData--
MI_OK
 */
uchar MFRC522_Read(uchar blockAddr, uchar *recvData)
{
    uchar status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }
    
    return status;
}


/*
MFRC522_Write

blockAddr--;writeData--16
MI_OK
 */
uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
 uchar buff[18]; 
    
    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {   
 status = MI_ERR;   
 }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++) //FIFO 16Byte
        {    
         buff[i] = *(writeData+i);   
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        
 if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {   
 status = MI_ERR;   
 }
    }
    
    return status;
}


/*
MFRC522_Halt

 */
void MFRC522_Halt(void)
{
 uchar status;
    uint unLen;
    uchar buff[4]; 

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}

