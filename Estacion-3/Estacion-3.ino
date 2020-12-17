#include <Arduino_FreeRTOS.h>
#include<LiquidCrystal.h>

LiquidCrystal lcd(7,6,5,4,3,2);

#define VIN 5
#define R 10000

void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

int mq5 = A0;
int ldr = A5;
int led1 = 10;
int led2 = 9;
int led3 = 8;
int buzzer = 11;

int nivelGas = 0;
int nivelLuz = 0;

void setup(){

  lcd.begin(16,2);
  lcd.clear();
  lcd.home();
  lcd.display();
  lcd.setCursor(0,0);
  lcd.print("Cargando...");
  
  Serial.begin(115200);
  
  while (!Serial) {
    ; 
  }
    
  Serial.setTimeout(50);
  
  
  xTaskCreate(
    TaskSensores
    ,  "Sensores"
    ,  138
    ,  NULL
    ,  3
    ,  NULL );

  xTaskCreate(
    TaskAlarmas
    ,  "Alarmas"
    ,  138
    ,  NULL
    ,  2
    ,  NULL );

  xTaskCreate(
    TaskDisplay
    ,  "Display"
    ,  170
    ,  NULL
    ,  1
    ,  NULL );

}

void loop()
{
  
}

void TaskSensores(void *pvParameters)
{
  (void) pvParameters;

  pinMode(mq5,INPUT);
  pinMode(ldr,INPUT);

  int promLuz[5] = {0,0,0,0,0};
  int promGas[5] = {0,0,0,0,0};
  int p = 0;
  
  for (;;)
  {
    double lectura = analogRead(mq5);
    promGas[p] = lectura;
    lectura = 0;
    for(int i=0;i<5;i++)
      lectura = lectura + promGas[i];
    String mostrar = "101" + String(lectura/5);
    nivelGas = lectura/5;
    Serial.println(mostrar);
    
    vTaskDelay( 100 / portTICK_PERIOD_MS );

    lectura = analogRead(ldr);
    promLuz[p] = lectura;
    p++;
    if(p==5)
      p = 0;
    lectura = 0;
    for(int i=0;i<5;i++)
      lectura = lectura + promLuz[i];
    lectura = lectura/5;
    float x = float(lectura)*(VIN/float(1023));
    x = (R*(VIN-x))/x;
    x = 500/(x/1000);
    lectura = x;
    mostrar = "102" + String(lectura);
    nivelLuz = lectura;
    Serial.println(mostrar);
    
    vTaskDelay( 100 / portTICK_PERIOD_MS );
  }
}

void TaskAlarmas(void *pvParameters)
{
  (void) pvParameters;

  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(buzzer,OUTPUT);
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
  digitalWrite(buzzer,LOW);

  bool onoff = 0;

  for (;;)
  {
    if(nivelGas<100){
      digitalWrite(led1,HIGH);
      digitalWrite(led2,LOW);
      digitalWrite(led3,LOW);
      digitalWrite(buzzer,LOW);
    } else if((nivelGas>=100)&&(nivelGas<400)){
      digitalWrite(led1,LOW);
      digitalWrite(led2,HIGH);
      digitalWrite(led3,LOW);
      digitalWrite(buzzer,LOW);
    } else {
      digitalWrite(led1,LOW);
      digitalWrite(led2,LOW);
      digitalWrite(led3,HIGH);
      digitalWrite(buzzer,onoff);
      onoff = !onoff;
    }
    
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
    
  }
}

void TaskDisplay(void *pvParameters)
{
  (void) pvParameters;

  lcd.clear();
  lcd.home();
  lcd.display();
  lcd.setCursor(0,0);
  lcd.print("PPM: ");
  lcd.setCursor(0,1);
  lcd.print("Luxes: ");

  for (;;)
  {
    lcd.setCursor(5,0);
    lcd.print(String(nivelGas));
    lcd.setCursor(7,1);
    lcd.print(nivelLuz);

    vTaskDelay( 200 / portTICK_PERIOD_MS );
  }

}
