#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "inetGSM.h"

InetGSM inet;

RF24 radio(8, 7);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)

struct n_message {                  //n_message structure
  const uint16_t sensor_id;             //add more variables or change the datatype to whatever suits your project
  float value;
};

bool n011 = false;
bool n02 = false;
boolean started = false;
char smsbuffer[160];
char n[20];
byte valor;
float temp[2];
uint16_t IDs[2];

void setup() {
  SPI.begin();
  Serial.begin(9600);
  powerUpOrDown();
  Serial.println(F("Testando GSM Shield SIM900"));
  if (gsm.begin(2400))  {
    Serial.println(F("nstatus=READY"));
    started = true;
  }else 
    Serial.println(F("nstatus=IDLE"));
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
}

void loop() {
  network.update();
  while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    n_message incomingData = {-1,-1};
    network.read(header, &incomingData, sizeof(incomingData)); // Read the incoming data
    if(incomingData.sensor_id == (uint16_t)012 && !n011){
      Serial.println("RECEBIDO!  N011");
      temp[0] = incomingData.value;
      IDs[0] = incomingData.sensor_id;
      n011 = true;
    }
    if(incomingData.sensor_id == (uint16_t)02 && !n02){
      Serial.println("RECEBIDO!  N02");
      temp[1] = incomingData.value;
      IDs[1] = incomingData.sensor_id;
      n02 = true;
    }
    if(n011 && n02){
      Serial.println("Pacotes de todos os filhos recebidos");
      Serial.println("Tentando conexão HTTP...");
      for(int i = 0; i<2;i++){
        envia_GSM(temp[i], IDs[i]);
      }
    }
  }
}

void powerUpOrDown(){
      Serial.print(F("Liga GSM..."));
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
      delay(1000);
      digitalWrite(6, HIGH);
      delay(1000);
      Serial.println(F("OK!"));
      digitalWrite(6, LOW);
      delay(500);
}
void envia_GSM(float temp, uint16_t ID){  
  char temp_string[55];
  char msg[10];
  int numdata;
  if (inet.attachGPRS("zap.vivo.com.br", "vivo", "vivo"))
    Serial.println(F("status=Conectado..."));
  else Serial.println(F("status=Nao conectado !!"));  
  
  delay(100); 
  String valor = "{\"sensor_id\":"+String(ID)+", \"value\":"+String(temp)+"}";
 
  valor.toCharArray(temp_string, 55);
  numdata = inet.httpPOST("18.229.124.23", 3000, "/create-measurement", temp_string, msg,50);
  Serial.println(numdata);
  delay(5000);
  n011 = false;
  n02 = false;
  delay(5000);
}
