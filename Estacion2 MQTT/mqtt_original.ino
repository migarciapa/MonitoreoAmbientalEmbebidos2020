#include "EspMQTTClient.h"

EspMQTTClient client(
  "2940794",  //userInternet
  "ventusrdif4",  //claveInternet
  "monitorse.duckdns.org",  // MQTT Broker server ip
  "admin",   // Can be omitted if not needed
  "monitorEmbebidos2020",   // Can be omitted if not needed
  "Estacion2",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

int redLed = 12;
int greenLed = 13;
int buzzer = 25;
int smokeA0 = 35;
int sluz = 32;
// Limite de riesgo marcado por el sensor
int sensorThres = 400;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  pinMode(sluz, INPUT);
  Serial.begin(115200);

  // Optionnal
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  }

void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("Rest2/gas", [](const String & payload) {
  Serial.println(payload);
  
    });
client.subscribe("Rest2/luz", [](const String & payload) {
  Serial.println(payload);
  
  
    });
}

void loop() {
  client.loop();
  int analogSensor = analogRead(smokeA0);
  Serial.println(analogSensor);
  String chainSensor = String(analogSensor);
  client.publish("Rest2/gas", chainSensor);
  int luzV = analogRead(sluz);
  Serial.print("Luz: ");
  Serial.println(luzV);
  String chainLuz = String(luzV);
  client.publish("Rest2/luz", chainLuz);
  
  
//programa en sitio
  Serial.print("Gas: ");
  Serial.println(analogSensor);
 
  //Revisa el limite del sensor
  if (analogSensor > sensorThres)
  {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(buzzer, HIGH);
  }
  else
  {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(buzzer,LOW);
  } 
  delay(1000);
  
}
