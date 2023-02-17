#include <Adafruit_NeoPixel.h> //библиотека подсветки
#include <ESP8266WiFi.h> //библиотека wi-fi
#include <ESP8266WebServer.h> //библиотека server
#include <Esp.h>
#include "wifiAuth.h"

#define PIN D8
//#define LED1 D4 // встроенная светилка ESP-12E
#define pix_length 60 // половина длины ленты

int colour = 0;
int lightningType = 0;
int ledPosition = 0;
int tempLedPosition = 0;
int ledBright = 0;
int ledSpeed = 0;
int int_R = 0;
int int_G = 0;
int int_B = 0;
bool workIsFinished = true;
String mainPage = "";


// ssid/password вафли и порт сервера
extern const char* ssid;
extern const char* password;
ESP8266WebServer server(4599);

//the Wemos WS2812B RGB shield has 1 LED connected to pin 2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2*pix_length, PIN, NEO_GRB + NEO_KHZ800);
int pix_count = 2 * pix_length;

void staticColour(){
  pixels.setPixelColor(ledPosition, pixels.Color(int_R * ledBright / 100, int_G * ledBright / 100, int_B * ledBright / 100)); 
  pixels.show();
  delay(ledSpeed);
  ledPosition += 1;
  if (ledPosition >= pix_count)
    ledPosition = 0;
  if (ledPosition == tempLedPosition)
      workIsFinished = true;
}

void WeelColour (){
  Serial.println("WeelColour enter");
  if (colour <= 255) // красный макс, зелёный растёт
  { 
    Serial.println("if 1 enter");
    int_R = 255;
    int_G = colour;
    int_B = 0;
    Serial.println("if 1 exit");
  }
  if ((colour > 255) && (colour <= 510)) // зелёный макс, падает красный
  {
    Serial.println("if 2 enter");
    int_R = 510 - colour;
    int_G = 255;
    int_B = 0;
    Serial.println("if 2 exit");
  }
  if ((colour > 510) && (colour <= 765)) // зелёный макс, растёт синий
  {
    Serial.println("if 3 enter");
    int_R = 0;
    int_G = 255;
    int_B = colour - 510;
    Serial.println("if 3 exit");
  }
  if ((colour > 765) && (colour <= 1020)) // синий макс, падает зелёный
  {
    Serial.println("if 4 enter");
    int_R = 0;
    int_G = 1020 - colour;
    int_B = 255;
    Serial.println("if 4 exit");
  }
  if ((colour > 1020) && (colour <= 1275)) // синий макс, растёт красный
  {
    Serial.println("if 5 enter");
    int_R = colour - 1020;
    int_G = 0;
    int_B = 255;
    Serial.println("if 5 exit");
  }
  if ((colour > 1275) && (colour <= 1530)) // красный макс, падает синий
  {
    Serial.println("if 6 enter");
    int_R = 255;
    int_G = 0;
    int_B = 1530 - colour;
    Serial.println("if 6 exit");
  }
  Serial.println("рассчёт яркости 1");
  int_R = int_R * ledBright / 100;
  Serial.println("рассчёт яркости 2");
  int_G = int_G * ledBright / 100;
  Serial.println("рассчёт яркости 3");
  int_B = int_B * ledBright / 100; 
  Serial.println("рассчёт яркости окончен");

  Serial.println("setPixelColor"+String(ledPosition)+" "+String(int_R)+" "+String(int_G)+" "+String(int_B)+" "+String(colour));
  pixels.setPixelColor(ledPosition, pixels.Color(int_R, int_G, int_B));
  Serial.println("show"); 
  pixels.show();
  yield();
  Serial.println("delay"); 
  delay(ledSpeed);


  Serial.println("if colour");
  if (colour + 3 > 1530)
    {
      colour = 0;
      Serial.println("if colour true");
    }
  else
  {
    colour += 3;
    Serial.println("if colour false");
  }
  Serial.println("ledPosition");
  ledPosition += 1;
  Serial.println("if ledPosition");
  if (ledPosition >= pix_count)
    {
      Serial.println("if ledPosition true");
      ledPosition = 0;
    }
  Serial.println("WeelColour exit");
}

void WeelColourReverse (){
  if (colour <= 255) // красный макс, зелёный растёт //красный макс, синий растёт 
  {  
    int_R = 255;
    int_G = 0;
    int_B = colour;
  }
  else if ((colour > 255) && (colour <= 510)) // синий макс, падает красный // синий макс, падает красный
  {
    int_R = 510 - colour;
    int_G = 0;
    int_B = 255;
  }
  else if ((colour > 510) && (colour <= 765)) // зелёный макс, растёт синий // синий макс, растёт зелёный
  {
    int_R = 0;
    int_G = colour - 510;
    int_B = 255;
  }
  else if ((colour > 765) && (colour <= 1020)) // синий макс, падает зелёный // зелёный макс, падает синий
  {
    int_R = 0;
    int_G = 255;
    int_B = 1020 - colour;
  }
  else if ((colour > 1020) && (colour <= 1275)) // синий макс, растёт красный // синий макс, растёт красный
  {
    int_R = colour - 1020;
    int_G = 255;
    int_B = 0;
  }
  else if ((colour > 1275) && (colour <= 1530)) // красный макс, падает синий // красный макс, зелёный падает 
  {
    int_R = 255;
    int_G = 1530 - colour;
    int_B = 0;
  }

  int_R = int_R * ledBright / 100;
  int_G = int_G * ledBright / 100;
  int_B = int_B * ledBright / 100; 
  
  pixels.setPixelColor(ledPosition, pixels.Color(int_R, int_G, int_B)); 
  pixels.show();    
  delay(ledSpeed);
  
  if (colour + 3 > 1530)
    colour = 0;
  else
    colour += 3;
  
  ledPosition += 1;
  if (ledPosition >= pix_count)
    ledPosition = 0;
}

void Randomizer (){
  pixels.setPixelColor(ledPosition, pixels.Color((random (0,255) * ledBright / 100), (random (0,255) * ledBright / 100), (random (0,255) * ledBright / 100))); 
  pixels.show();    
  delay(ledSpeed);
  ledPosition += 1;
  if (ledPosition >= pix_count)
    ledPosition = 0;
}

void handleForm(){
  String str_type = server.arg("str_type");
  int_R = (server.arg("str_R")).toInt();;
  int_G = (server.arg("str_G")).toInt();
  int_B = (server.arg("str_B")).toInt();
  ledSpeed = (server.arg("Speed")).toInt();
  ledBright = (server.arg("Brightness")).toInt();
  workIsFinished = (server.arg("Break")).toInt();
  server.send(200, "text/html", mainPage);
  mainPageupdater();
  if (workIsFinished == 1) {
    lightningType = 0;
    return;
  } 
  if (str_type == "Static"){
    workIsFinished = false;
    lightningType = 1; 
    tempLedPosition = ledPosition;
    return;
  }
  if (str_type == "WeelColour"){
    workIsFinished = false;
    lightningType = 2;
    return;
  }
  if (str_type == "WeelColourReverse"){
    workIsFinished = false;
    lightningType = 3;
    return;
  }
  if (str_type == "Random"){
    workIsFinished = false;
    lightningType = 4;
    return;
  }
}

void handleRoot(){
  Serial.println("Main IP");
  //mainPageupdater();
  server.send(200, "text/html", mainPage);
}

void mainPageupdater() {
  mainPage = "<!DOCTYPE html><html><body><form action=\"/action_page\">Type:";
  mainPage += "<br><select size=\"1\" name=\"str_type\"><option>none</option><option>Static</option><option>WeelColour</option><option>WeelColourReverse</option><option>Random</option><option>Fill</option><select>";
  mainPage += "<br>Red colour:<br><input type=\"text\" name=\"str_R\" value=" + String(int_R) + ">";
  mainPage += "<br>Green colour:<br><input type=\"text\" name=\"str_G\" value=" + String(int_G) + ">";
  mainPage += "<br>Blue colour:<br><input type=\"text\" name=\"str_B\" value=" + String(int_B) + ">";
  mainPage += "<br>Speed:<br><input type=\"text\" name=\"Speed\" value=" + String(ledSpeed) + ">";
  mainPage += "<br>Brightness:<br><input type=\"text\" name=\"Brightness\" value=" + String(ledBright) + ">";
  mainPage += "<br>Break:<br><input type=\"text\" name=\"Break\" value=" + String(workIsFinished)+ "><br>";
  mainPage += "<br><input type=\"submit\" value=\"Submit\"></body></html>"; 
}

void setup(void){
  delay(10);
  mainPageupdater();
  pixels.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Подключение к Wi-Fi
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Ожидание подключения");
  }
  Serial.println("");
  Serial.print("Подключено к ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/check", handleRoot);      //Which routine to handle at root location
  server.on("/action_page", handleForm); //form action is handled here
  Serial.println("HTTP сервер запущен");
  server.begin();
}

void loop(void){
  if (workIsFinished == false)
      Serial.println("Вход Луп");
  //yield();
  if (workIsFinished == false)
    Serial.println("Перед принятием клиента");
  server.handleClient();
  if (workIsFinished == false)
    Serial.println("После принятия клиента");
  delay(10);
  if (workIsFinished == false)
      Serial.println("После delay");
  if (workIsFinished == false){
    Serial.println("внутри if");
    switch (lightningType) {
        case 0:
        Serial.println("case 0 enter");
          workIsFinished = true;
        Serial.println("case 0 exit");
          break;;
        case 1:
        Serial.println("case 1 enter");
          staticColour();
        Serial.println("case 1 exit");
          break;;
        case 2:
        Serial.println("case 2 enter");
          WeelColour();
        Serial.println("case 2 exit");
          break;;
        case 3:
        Serial.println("case 3 enter");
          WeelColourReverse();
        Serial.println("case 3 exit");
          break;;
        case 4:
        Serial.println("case 4 enter");
          Randomizer();
        Serial.println("case 4 exit");
          break;;
    }
    Serial.println("после if");
    //mem info
    uint32_t free = system_get_free_heap_size();
    if (workIsFinished == false)
      Serial.println("первый mem");
    Serial.print("free mem after effect: ");
    if (workIsFinished == false)
      Serial.println("второй mem");
    Serial.println(free);
    if (workIsFinished == false)
      Serial.println("тертий mem");
  }
}
