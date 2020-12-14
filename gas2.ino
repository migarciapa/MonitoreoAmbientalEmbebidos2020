int redLed = 12;
int greenLed = 13;
int buzzer = 25;
int smokeA0 = 14;
int sluz = 4;
// Your threshold value
int sensorThres = 400;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  pinMode(sluz, INPUT);
  Serial.begin(115200);
}

void loop() {
  int analogSensor = analogRead(smokeA0);
  int luzV = analogRead(sluz);
  Serial.print("Gas: ");
  Serial.println(analogSensor);
  Serial.print("Luz: ");
  Serial.println(luzV);

  // Checks if it has reached the threshold value
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
