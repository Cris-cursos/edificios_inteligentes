#include <WiFi.h>
#include <PubSubClient.h>
#include <TaskScheduler.h>
#include <SPI.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_CCS811.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>
#include "GLOBALS.h"
#include "WIFI.hpp"
#include "MQTT.hpp"
#include "RFID.hpp"

void read1sec();                                    //Declaro funciones externas que están abajo
void TextDisplay();                                 //Declaro funciones externas que están abajo
float getCorriente();                               //Declaro funciones externas que están abajo
Task TareaRead1Sec(5000, TASK_FOREVER, &read1sec);  //Genero tarea para que se ejecute cada 60 segundos

void setup() {
  /******** GENERAL CONFIG ******/
  delay(5000);
  Serial.begin(9600); //init SERIAL COMMS
  SPI.begin();        // init SPI bus
  Wire.begin();       // init I2C bus
  Serial.println("\n.........INICIANDO SISTEMA.............");
  ConnectWiFi_STA(true); //init WIFI COMMS
  InitMqtt();            //init MQTT PROTOCOL 
  /******** RFID SENSOR ******/
  rfid.PCD_Init();  // init MFRC522
  /******** LIGHT SENSOR ******/
  lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE);
  lightMeter.begin();
  /******** POWER SENSOR ******/
  ads.setGain(GAIN_TWO);  //2.048 V 1 bit = 0.0625mV
  ads.begin();
  /******** CO2 SENSOR ******/  
  if(!ccs.begin()){
    Serial.println("Error Inicializando sensor! Revisar cableado.");
    while(1);
  }
  while(!ccs.available());
  /******** OLED SCREEN ******/
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
  } else {
    display.clearDisplay();
    TextDisplay();
    display.display();
  }
  /******** PIR SENSOR ******/
  pinMode(pirPin, INPUT);
  /******** MOTION SENSOR ******/
  pinMode(detectPin, INPUT);
  /******** NOISE SENSOR ******/
  pinMode(microPin, INPUT);
  /******** RELAY OUTPUT ******/
  pinMode(doorOpen, OUTPUT);
  /******** TASK CONFIG ******/
  runner.addTask(TareaRead1Sec);
  TareaRead1Sec.enable();
}

void loop() {
  //********* WIFI CONNECTION **********//
  if (WiFi.status() != WL_CONNECTED) { ConnectWiFi_STA(true); }
  //********* MQTT CONNECTION **********//
  HandleMqtt();
  //************* RFID SENSOR **************//
  uid = chechRFID();
  if (uid.compareTo("") != 0) {
    PublisMqtt(TOPICS_PUBLICATIONS[0], uid);
    uid = "";
  }
  //************* PIR SENSOR ************//
  bool pirdetect = digitalRead(pirPin);
  if ((pirdetect == HIGH) && (pirMotion == false)) {
    pirMotion = true;
    Serial.println("PIR Movement detected");
    PublisMqtt(TOPICS_PUBLICATIONS[4], String(pirdetect));
  } else {
    if (pirdetect == LOW) pirMotion = false;
  }
  //************* MOTION SENSOR ************//
  bool detect = digitalRead(detectPin);
  if ((detect == HIGH) && (detectMotion == false)) {
    detectMotion = true;
    Serial.println("Movement detected");
    PublisMqtt(TOPICS_PUBLICATIONS[3], String(detect));
  } else {
    if (detect == LOW) detectMotion = false;
  }
  //************ RELAY OUTPUT **************//
  if (accessGranted == 0) {
    accessGranted = 3;
    digitalWrite(doorOpen, LOW);
    PublisMqtt(TOPICS_PUBLICATIONS[7], "0");
  } else if (accessGranted == 1) {
    accessGranted = 3;
    digitalWrite(doorOpen, HIGH);
    PublisMqtt(TOPICS_PUBLICATIONS[7], "1");
  }
  //***********TASK CALL *****************//
  runner.execute();
}

/******************************************
**************** SUBRUTINAS ***************
******************************************/
void read1sec() {

  //************* LIGHT SENSOR ************//
  float lux = lightMeter.readLightLevel();
  Serial.println("Light: " + String(lux) + " lx");
  PublisMqtt(TOPICS_PUBLICATIONS[2], String(lux));
  //************* POWER SENSOR ************//
  float power = voltage * getCorriente();
  Serial.println("Power: " + String(power) + " W");
  PublisMqtt(TOPICS_PUBLICATIONS[5], String(power));
  //************* CO2 SENSOR ************//
  if(ccs.available()){
    if(!ccs.readData()){
      float co2 = ccs.geteCO2();
      Serial.println("CO2: " + String(co2)+" ppm");
      PublisMqtt(TOPICS_PUBLICATIONS[6], String(co2));
    }
    else{
      Serial.println("ERROR en la lectura CO2");
    }
  }
  //************* NOISE SENSOR **************//
  int noise = analogRead(microPin);
  Serial.println("Noise: " + String(noise));
  PublisMqtt(TOPICS_PUBLICATIONS[1], String(noise));
}

void TextDisplay() {
  //************* OLED SCREEN ************//
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 28);
  display.println("Jesuitas");
  display.display();
  delay(3000);
}

float getCorriente() {
  //************* POWER SENSOR ************//
  float voltageSCT = 0;
  float corriente = 0;
  float sum = 0;
  long tiempo = millis();
  int counter = 0;
  while (millis() - tiempo < 1000) {
    voltageSCT = ads.readADC_Differential_0_1() * multiplier;
    corriente = voltageSCT * FACTOR;
    corriente = corriente / 1000;
    sum = sum + sq(corriente);
    counter = counter + 1;
  }
  corriente = sqrt(sum / counter);
  return (corriente);
}