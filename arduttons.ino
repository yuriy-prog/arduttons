#include <EEPROM.h>
#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <util.h>
#include <TimerOne.h>

//
const int pinc = 4;
const int pins[pinc] = {5,6,7,8}; //physical to logical pin address
const int eeprom[pinc] = {0,1,2,3}; //pin address in eeprom

volatile int pineepromstate[pinc] = {0,0,0,0};
volatile int pinhardstate[pinc] = {0,0,0,0};
volatile int pinsoftstate[pinc] = {0,0,0,0};
volatile unsigned long pintime[pinc] = {0,0,0,0};
int pinreport[pinc] = {0,0,0,0};

byte mac[] = {0x02, 0x80, 0xC2, 0x00, 0x00, 0x01};
byte ip[] = {192, 168, 12, 80};
byte netdns[] = {192, 168, 12, 1};
byte gateway[] = {192, 168, 12, 1};
byte mask[] = {255, 255, 255, 0};

EthernetServer server(2323);

//
void timerIsr()
{
//
   int state;
   for (int i = 0; i < pinc; i++) 
      {
   //
      state = digitalRead(pins[i]);
      if (state==LOW && pinhardstate[i]==0 && pinsoftstate[i]==0) // button down...
         {
      //
         pinhardstate[i] = 1;
         pinsoftstate[i] = 1;
         pineepromstate[i] = 1;
         EEPROM.write(eeprom[i], 1);
         pintime[i] = millis();
         }
      else 
         if (state==HIGH && pinhardstate[i]==1) // ...button up
            {
         //
            pinhardstate[i] = 0;
            pintime[i] = millis() - pintime[i];
            }
      }
}


// the setup routine runs once when you press reset:
void setup() 
{                
   // initialise to internal 20Mom resistor
   for (int i = 0; i < pinc; i++)
      {
   //
      pinMode(pins[i], INPUT); 
      digitalWrite(pins[i], HIGH);
      }

   //eeprom read
   for (int i = 0; i < pinc; i++)
      {
   //
      pineepromstate[i] = EEPROM.read(eeprom[i]);
      }

   //
   Timer1.initialize(100000); // 10Hz // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz)
   Timer1.attachInterrupt( timerIsr );

   //Start up the serial port
   //Serial.begin(9600);
   
   //
   Ethernet.begin(mac, ip, netdns, gateway, mask);
   server.begin();
}

// the loop routine runs over and over again forever:
void loop() 
{
//   
   for (int i = 0; i < pinc; i++)
      {
   //
      //анализ pinsoftstate? без pinreport? где сообщать клиенту?
      if (pinhardstate[i] == 1 && pinsoftstate[i]==1 && pinreport[i]==0)
         {
      //
         server.println();
         server.print("pin down["); server.print(i, DEC); server.print("]");
            
         pinreport[i] = 1;
         }
      else
         if (pinhardstate[i] == 0 && pinsoftstate[i]==1 && pinreport[i]==1)
            {
         //
            server.println();
            server.print("pintime["); server.print(i, DEC); server.print("]="); server.print(pintime[i], DEC);
            
            pinsoftstate[i] = 0;
            pinreport[i] = 0;
            }
      }
}
