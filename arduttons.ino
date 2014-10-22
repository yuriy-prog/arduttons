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

volatile int pinhardstate[pinc] = {0,0,0,0};
volatile int pinsoftstate[pinc] = {0,0,0,0};
int pineepromstate[pinc] = {0,0,0,0};
int pinreport[pinc] = {0,0,0,0};

byte mac[] = {0x02, 0x80, 0xC2, 0x00, 0x00, 0x01};
byte ip[] = {192, 168, 12, 80};
byte netdns[] = {192, 168, 12, 1};
byte gateway[] = {192, 168, 12, 1};
byte mask[] = {255, 255, 255, 0};

//
const int BUTTON1 = 1;
const int BUTTON2 = 2;
const int BUTTON3 = 4;
const int BUTTON4 = 8;
const int TSTMODE = 16;
const int DORESET = 32;
const int GETSTAT = 64;

const unsigned long MAX_UNSIGNED_LONG = 4294967295;
const unsigned long TSTLEN = 5*60*1000;
unsigned long tstbgn = 0;
boolean iststmode = false;

//
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
         }
      else 
         if (state==HIGH && pinhardstate[i]==1) // ...button up
            {
         //
            pinhardstate[i] = 0;
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
   Timer1.initialize(10000); // 100Hz
   Timer1.attachInterrupt( timerIsr );

   //Start up the serial port
   //Serial.begin(9600);
   
   //
   Ethernet.begin(mac, ip, netdns, gateway, mask);
   server.begin();
}

//
char getstatus(char cmd)
{
//
   char result = cmd;
   for (int i = 0; i < pinc; i++)
      if (pineepromstate[i] == 1) result |= 1 << i;
   return result;
}

// the loop routine runs over and over again forever:
void loop() 
{
//
   //process up-down events
   for (int i = 0; i < pinc; i++)
      if (pinsoftstate[i]==1)
         {
      //
         EEPROM.write(eeprom[i], 1);
         pineepromstate[i] = 1;
         pinsoftstate[i] = 0;
         pinreport[i] = 0;
         }

   //down always send to client
   for (int i = 0; i < pinc; i++)
      {
   //
       if (pinreport[i] == 0 && pineepromstate[i]==1)
         {
      //
         server.write( getstatus( iststmode ? GETSTAT|TSTMODE : GETSTAT ) );
         pinreport[i] = 1;
         }
      }

   //process command from station
   while (EthernetClient client = server.available())
      while (client.available())
         {
      //
         char cmd = client.read();
         if (cmd & TSTMODE)
            {
         //
            cmd |= GETSTAT;
            tstbgn = millis();
            iststmode = true;
            }
         //else
         if (cmd & DORESET)
            {
         //
            iststmode = false;
            for (int i = 0; i < pinc; i++)
               {
            //
               EEPROM.write(eeprom[i], 0);
               pineepromstate[i] = 0;
               }
            }
         //else
         if (cmd & GETSTAT)
            {
         //
            server.write(getstatus( iststmode ? GETSTAT|TSTMODE : GETSTAT ));
            }
         }

   //drop TSTMODE
   if (iststmode)
      {
   //
      unsigned long currt = millis();
      unsigned long duration = 0;
      if (currt < tstbgn) duration = (MAX_UNSIGNED_LONG - tstbgn) + currt;
      else                duration = currt - tstbgn;

      if (duration > TSTLEN) iststmode = false;
      }
}
