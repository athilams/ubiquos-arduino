#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

RF24 radio(8, 7);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 01;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t gsm_node = 00;

void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
}

void loop() {
  network.update();
}
