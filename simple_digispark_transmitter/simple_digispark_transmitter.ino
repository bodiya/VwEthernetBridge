/**************************************************************************
This transmitter sends the status of a button to the receiver every
half second.
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
#include <VirtualWire.h>

int transfer_pin = 0;
int button = 5;
uint8_t node_id = 5;

void setup()
{
  pinMode(button,INPUT);

  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_tx_pin(transfer_pin);
  vw_setup(2000);	 // Bits per sec
}

void loop()
{
  uint8_t package[2];
  package[0] = node_id;
  package[1] = (digitalRead(button) == HIGH? (uint8_t)'0' : (uint8_t)'1');
    
  digitalWrite(1, HIGH); // Flash a light to show transmitting
  vw_send(package, 2);
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(1, LOW);
  delay(500);
}
