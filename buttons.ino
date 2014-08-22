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
const int pins[pinc] = {5,6,7,8}; //physical to logical pin
const int eeprom[pinc] = {0,1,2,3}; //pin address in eeprom
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
   // initialize the digital pin as an output.
   //pinMode(led, OUTPUT);

   // initialise to internal 20Mom rezister
   for (int i = 0; i < pinc; i++)
      {
   //
      pinMode(pins[i], INPUT); 
      digitalWrite(pins[i], HIGH);
      }

   //
   Timer1.initialize(2000); // 500Hz // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
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
   //int ledstate = 0;
   for (int i = 0; i < pinc; i++)
      {
   //
      //if (pinsoftstate[i]==1) ledstate = 1;
      //анализ pinsoftstate? без pinreport? где сообщать клиенту?
      if (pinhardstate[i] == 1 && pinsoftstate[i]==1 && pinreport[i]==0)
         {
      //
         if (pinreport[i]==0)
            {
         //
            //Serial.println();
            //Serial.print("pin down["); Serial.print(i, DEC); Serial.print("]");
            server.println();
            server.print("pin down["); server.print(i, DEC); server.print("]");
            
            pinreport[i] = 1;
            }
         }
      else
         if (pinhardstate[i] == 0 && pinsoftstate[i]==1 && pinreport[i]==1)
            {
         //
            //Serial.println();
            //Serial.print("pintime["); Serial.print(i, DEC); Serial.print("]=");
            //Serial.println(pintime[i], DEC);
            server.println();
            server.print("pintime["); server.print(i, DEC); server.print("]="); server.print(pintime[i], DEC);
            
            pinsoftstate[i] = 0;
            pinreport[i] = 0;
            //digitalWrite(led, HIGH);   delay(200);    digitalWrite(led, LOW);   delay(200);
            }
      }
}
