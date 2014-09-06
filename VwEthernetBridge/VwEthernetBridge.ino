/**************************************************************************
This receiver collects sensor data from a transmitter and forwards a UDP
packet with this data to a pre-defined IP address. The port can vary based
on sender ID, so that multiple sensors can be used.
Copyright (C) 2014 Brian Bodiya

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

For questions, comments, or suggestions about this software,
you can contact Brian Bodiya at: brian@bodiya.com
*************************************************************************/
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <VirtualWire.h>

#include <stdlib.h>

/************ BEGIN ETHERNET SETUP ************************************/
// Enter a MAC address and IP address for your Uno here
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
unsigned int local_port = 8888;      // local port to listen on

// This is the address and the base port to forward RF messages to
IPAddress remoteIp(192, 168, 1, 100);
unsigned int remote_port = 6780;         // base port to send to

EthernetUDP Udp;
/************ END ETHERNET SETUP **************************************/

/************ BEGIN VIRTUALWIRE SETUP *********************************/
// Pin we'll use to receive messages from remote arduinos
int vwReceivePin = 2;
// Pins that don't conflict with any timers.
//   Note: if these are used, set their pinmode after calling vw_setup()
int vwGarbageTransferPin = 7;
int vwGarbagePttPin = 8;

// Buffer to hold incoming VW messages, and the length associated with
// the current message in the buffer
//Note: +1 to avoid out of bounds when adding \n for debug
uint8_t buf[VW_MAX_MESSAGE_LEN + 1];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
/************ END VIRTUALWIRE SETUP ***********************************/

int led = 8;
int state = LOW;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);
  Udp.begin(local_port);
  
  // set the rx, tx, and ptt pins.
  vw_set_rx_pin(vwReceivePin);
  vw_set_tx_pin(vwGarbageTransferPin);
  vw_set_ptt_pin(vwGarbagePttPin);
  
  vw_setup(2000);      // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
  
  //set any other inputs/outputs
  pinMode(led,OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Setup Complete.");
}

void loop() {
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    //make sure the message at least contains a sender ID, and a body
    if(buflen < 2)
      return;
      
    //for demo purposes set LED
    state = ((char)buf[1] == '0'? LOW : HIGH);
    digitalWrite(led,state);

    // Message with a good checksum received, parse message (get sender id, body)
    unsigned short sender = buf[0];
    //Serial.print("Received from: ");Serial.println(sender);
    //Serial.print("Sending to: ");Serial.println(remoteIp);
    //Serial.print("At port: ");Serial.println(remote_port + sender);
    double *temp_f = (double*)&buf[1];
    char temp_string[5] = {'\0'};
    dtostrf(*temp_f,5,2,temp_string);
    
    Serial.println(*temp_f);
    
    // send a reply, to the IP address and port that sent us the packet we received
    int success = Udp.beginPacket(remoteIp, remote_port + sender);
    //Serial.print("Connection status: ");Serial.println(success);
    Udp.write((uint8_t*)temp_string,5);
    Udp.endPacket();
    //Serial.println("Done sending");
  }
}

