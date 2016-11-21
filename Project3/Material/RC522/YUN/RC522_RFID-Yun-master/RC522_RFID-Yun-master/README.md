RC522wYun
=========

Yun board with RFID RC522 board

Initial Master is a working RC522 code with an UNO and LEDs for notification. This needs to be modified for a Yun board and then will eventually send the data over WiFi and possibly GSM with the addition of a GSM shield. 

Ports for the RC522 on Uno:
3.3V
MISO: 12
MOSI: 11
SCK: 13
SDA(SS): 10

=========

The Yun can not have the digital pins used, it must be mapped with the ICSP which requires a 6-pin header 

it is mapped: http://www.arduinopassion.com/wp-content/uploads/2013/12/yun-pinout.pdf 


    13 Reset      9 SCK         11 MISO
       0            0               0
       0            0               0
      GND       10 MOSI            VCC
      
      
This is the ICSP mapping on the Yun board same as other ICSP and shown in the .pdf above


    
