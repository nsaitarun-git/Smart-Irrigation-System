//Smart Irrigation System by Saitarun Nadipineni 
#include <Adafruit_GFX.h>
#include <Arduino_ST7789_Fast.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#define Type DHT11

int DC=9;
int RST=8;
int SCRH=240;
int SCRW=240;

int TX=2;
int RX=3;

int HTPin=A0;
float temp;
float humidity;

int soilSensorPin=A5;
int soilState;
int soilMoistureMax=565;
int soilMoistureMin=280;
int soilMoisturePer;

int waterLevelSensor=A4;
int waterLevel;
int waterLevelMax=636;
int waterLevelMin=30;
int waterLevelPer;

int relay=7;
int potPin=A3;
int potVal;
int buzzer=6;
int CMLevel;
int flag=0;

Arduino_ST7789 lcd=Arduino_ST7789(DC,RST);
SoftwareSerial gsm(TX,RX);
DHT HT(HTPin,Type);

void setup() {
  Serial.begin(115200);
  digitalWrite(relay,HIGH);
  HT.begin();
  gsm.begin(115200);
  lcd.init(SCRH,SCRW);
  pinMode(soilSensorPin,INPUT);
  pinMode(waterLevelSensor,INPUT);
  pinMode(relay,OUTPUT);
  pinMode(potPin,INPUT);
  pinMode(buzzer,OUTPUT);
  delay(1000);
  StartScreen();
  SystemReady();
  lcd.fillScreen(BLACK);
  lcd.drawRect(0,0,240,240,YELLOW);

  waterLevel=analogRead(waterLevelSensor);
  waterLevelPer=map(waterLevel,waterLevelMin,waterLevelMax,0,100);
  if(waterLevelPer<40){
    SendMessage2();
   }
  }

void loop() {

  soilState=analogRead(soilSensorPin);
  soilMoisturePer=map(soilState,soilMoistureMax,soilMoistureMin,0,100);

  waterLevel=analogRead(waterLevelSensor);
  waterLevelPer=map(waterLevel,waterLevelMin,waterLevelMax,0,100);

  potVal=analogRead(potPin);
  CMLevel=map(potVal,0,1023,0,100);
 
  PrintData();
  ReadHT();
  
if(soilMoisturePer<CMLevel){
    while(flag==0){
      soilState=analogRead(soilSensorPin);
      soilMoisturePer=map(soilState,soilMoistureMax,soilMoistureMin,0,100);
      
      waterLevel=analogRead(waterLevelSensor);
      waterLevelPer=map(waterLevel,waterLevelMin,waterLevelMax,0,100);

      potVal=analogRead(potPin);
      CMLevel=map(potVal,0,1023,0,100);
      
      digitalWrite(relay,LOW);
      tone(buzzer,2000,500);
      
      PrintData();
      ReadHT();
      
      if(soilMoisturePer>=CMLevel){
        digitalWrite(relay,HIGH);
        break;
       }
      }
    SendMessage();
    if(waterLevelPer<40){
    SendMessage2();
     }
    }else{
      digitalWrite(relay,HIGH);
    }
}

void StartScreen(){
  lcd.fillScreen(WHITE);
  tone(buzzer,1000,500);
  delay(500);
  noTone(buzzer);
  lcd.fillScreen(BLACK);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(2);
  lcd.setCursor(45,100);
  lcd.println("Tarun's Smart");
  lcd.setCursor(20,120);
  lcd.println("Irrigation System");
  lcd.drawRect(0,0,240,240,RED);
  delay(2000);
  lcd.fillRect(20,100,210,100,BLACK);
  lcd.drawRect(0,0,240,240,BLUE);
  lcd.setCursor(45,110);
  lcd.println("Please Wait");
  delay(500);
  lcd.fillRect(200,110,30,10,BLACK);
  lcd.setCursor(180,110);
  lcd.println(".");
  delay(500);
  lcd.fillRect(200,110,30,10,BLACK);
  lcd.setCursor(180,110);
  lcd.println("..");
  delay(500);
  lcd.fillRect(200,110,30,10,BLACK);
  lcd.setCursor(180,110);
  lcd.println("...");
  delay(2000);
  }

 void SystemReady(){
  tone(buzzer,1500,500);
  delay(50);
  noTone(buzzer);
  delay(50);
  tone(buzzer,1500,500);
  lcd.setTextColor(WHITE,BLACK);
  lcd.drawRect(0,0,240,240,GREEN);
  lcd.setCursor(45,110);
  lcd.println("System Ready! ");
  delay(2000);
 }

void PrintData(){
  lcd.setCursor(10,10);
  lcd.println("Soil Moisture:");
  if(soilMoisturePer<100 && soilMoisturePer>0){
  lcd.setCursor(175,10);
  lcd.setTextColor(WHITE,BLACK);
  lcd.println(soilMoisturePer);
  lcd.setCursor(199,10);
  lcd.println("% ");
  }else if(soilMoisturePer>=100){
  lcd.setTextColor(WHITE,BLACK);
  lcd.setCursor(175,10);
  lcd.println("100 % ");
  }else if(soilMoisturePer<=0){
  lcd.setTextColor(WHITE,BLACK);
  lcd.setCursor(175,10);
  lcd.println("0 %  ");
  }
  
  lcd.setCursor(10,35);
  lcd.println("GSM Status:Ready");
  
  if(soilMoisturePer<=CMLevel){
  lcd.setTextColor(WHITE,BLACK);
  lcd.setCursor(10,60);
  lcd.println("Pump State:");
  lcd.setCursor(140,60);
  lcd.println("ON ");
  }else{
  lcd.setTextColor(WHITE,BLACK);
  lcd.setCursor(10,60);
  lcd.println("Pump State:");
  lcd.setCursor(140,60);
  lcd.println("OFF");
  }

  lcd.setTextColor(WHITE,BLACK);
  lcd.setCursor(10,85);
  lcd.println("Temperature:");
  lcd.setCursor(152,85);
  lcd.println(temp);
  lcd.setCursor(225,85);
  lcd.println("C");
  lcd.setCursor(10,110);
  lcd.println("Humidity:");
  lcd.setCursor(120,110);
  lcd.println(humidity);
  lcd.setCursor(190,110);
  lcd.println("%");

  lcd.setCursor(10,135);
  lcd.println("Water Level:");
  if(waterLevelPer>0 && waterLevelPer<100){
  lcd.setTextColor(WHITE,BLACK);
  lcd.setCursor(165,135);
  lcd.print(" ");
  lcd.setCursor(155,135);
  lcd.println(waterLevelPer);
  lcd.setCursor(179,135);
  lcd.println("% ");
  }else if(waterLevelPer>=100){
    lcd.setCursor(155,135);
    lcd.println("100%  ");
  }else if(waterLevelPer<=0){
    lcd.setCursor(155,135);
    lcd.println("0 %  ");
  }
  
  lcd.setTextColor(WHITE,BLACK);
  lcd.setCursor(10,160);
  lcd.println("Set ML:");
  if(CMLevel>0 && CMLevel<100){
  lcd.setCursor(105,160);
  lcd.setCursor(95,160);
  lcd.println(CMLevel);
  lcd.setCursor(118,160);
  lcd.println("%  ");
  }
  
  if(CMLevel==0){
  lcd.setCursor(95,160);
  lcd.println("0 %   ");
  }else if(CMLevel==100){
  lcd.setCursor(95,160);
  lcd.println("100%  ");
  }
}

void ReadHT(){
   temp=HT.readTemperature();
   humidity=HT.readHumidity();
   delay(1000);
  }

void SendMessage(){
  if(gsm.available()){
    Serial.write(gsm.read());
}

Serial.println("Sending Message...");

tone(buzzer,1500,500);
lcd.setTextColor(WHITE,BLACK);
lcd.fillScreen(BLACK);
lcd.drawRect(0,0,240,240,CYAN);
lcd.setCursor(18,110);
lcd.println("Sending Message...");
delay(3000);
soilState=analogRead(soilSensorPin);
soilMoisturePer=map(soilState,soilMoistureMax,soilMoistureMin,0,100);

gsm.print("AT+CMGF=1\r");
delay(100);
gsm.print("AT+CMGS=\"07459489629\"\r");
gsm.print("Notificatiom from your Smart Irrigation System!");
gsm.println();
gsm.println();
gsm.print("Your plant has just been watered.");
gsm.println();
gsm.println();
gsm.print("Information:");
gsm.println();
gsm.println();
gsm.print("Soil Moisture Level:");
if(soilMoisturePer>0 && soilMoisturePer<100){
gsm.print(soilMoisturePer);
}
if(soilMoisturePer>=100){
gsm.print("100");
}
if(soilMoisturePer<=0){
gsm.print("0");
}
gsm.print("%");
gsm.println();
gsm.print("Temperature:");
gsm.print(temp);
gsm.print("C");
gsm.println();
gsm.print("Humidity:");
gsm.print(humidity);
gsm.print(" %");
gsm.println();
gsm.print("Container Water Level:");
if(waterLevelPer>0 && waterLevelPer<100){
gsm.print(waterLevelPer);
}
if(waterLevelPer>=100){
gsm.print("100");
}
if(waterLevelPer<=0){
gsm.print("0");
}
gsm.print(" %");
gsm.println();
gsm.print("\r");
delay(200);
gsm.print(char(26));
delay(100);
gsm.println();
Serial.println("Sent Message");

tone(buzzer,1500,500);
delay(50);
noTone(buzzer);
delay(50);
tone(buzzer,1500,500);
lcd.setCursor(18,110);
lcd.println("  Message Sent!   ");
delay(3000);
lcd.fillScreen(BLACK);
lcd.drawRect(0,0,240,240,YELLOW);
}


void SendMessage2(){
  if(gsm.available()){
    Serial.write(gsm.read());
}

tone(buzzer,1500,500);
lcd.setTextColor(WHITE,BLACK);
lcd.fillScreen(BLACK);
lcd.drawRect(0,0,240,240,MAGENTA);
lcd.setCursor(18,110);
lcd.print("Water Level below");
lcd.setCursor(110,135);
lcd.print("40%");
delay(3000);
lcd.drawRect(0,0,240,240,CYAN);
lcd.setCursor(18,110);
lcd.println("Sending Message...");
lcd.setCursor(110,135);
lcd.print("   ");
delay(3000);
Serial.println("Sending Message...");

gsm.print("AT+CMGF=1\r");
delay(100);
gsm.print("AT+CMGS=\"07459489629\"\r");
gsm.print("Alert from your Smart Irrigation System!");
gsm.println();
gsm.println();
gsm.print("Water level of your water container has fallen below 40%!");
gsm.println();
gsm.print("Water container refill recommended ASAP.");
gsm.println();
gsm.print("\r");
delay(200);
gsm.print(char(26));
delay(100);
gsm.println();
Serial.println("Sent Message");

tone(buzzer,1500,500);
delay(50);
noTone(buzzer);
delay(50);
tone(buzzer,1500,500);
lcd.setCursor(18,110);
lcd.println("  Message Sent!   ");
delay(3000);
lcd.fillScreen(BLACK);
lcd.drawRect(0,0,240,240,YELLOW);
}
