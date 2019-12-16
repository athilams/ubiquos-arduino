#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <DHT.h>
 
#define DHTPIN A2
#define DHTTYPE DHT11
// DHT 11
DHT dht(DHTPIN,DHTTYPE);

RF24 radio(8, 7);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 012;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t gsm_node = 00;

void setup() {
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  dht.begin();
}

struct n_message {                  //n_message structure
  const uint16_t sensor_id;             //add more variables or change the datatype to whatever suits your project
  float value;
};

void loop() {
  network.update();
  n_message sendData = {01,dht.readTemperature()};
  RF24NetworkHeader header(gsm_node);     // (Address where the data is going)
  bool ok = network.write(header, &sendData, sizeof(sendData)); // Send the data
}
