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
bool globalBreak = false;



// ssid/password вафли и порт сервера
extern const char* ssid;
extern const char* password;
ESP8266WebServer server(4599);

/*                          Главная страница, отображается при переходе по IP                             */

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <body>
    <form action="/action_page">
    Type:
    <br>
      <select size="1" name="str_type">
        <option>zerolight</option>
        <option>Static</option>
        <option>WeelColour</option>
        <option>WeelColourReverse</option>
        <option>Blackout</option>
        <option>Comet</option>
        <option>Random</option>
        <option>Fill</option>
        <option>Bryak</option>
      <select>
    <br>
      Red colour:
    <br>
      <input type="text" name="str_R" value="0..255">
    <br>
      Green colour:
    <br>
      <input type="text" name="str_G" value="0..255">
    <br>
      Blue colour:
    <br>
      <input type="text" name="str_B" value="0..255">
    <br>
      Speed:
    <br>
      <input type="text" name="Speed" value="40">
    <br>
      Brightness:
    <br>
      <input type="text" name="Brightness" value="100">
    <br>
      Break:
    <br>
      <input type="text" name="Break" value="0">
    <br>
    <br>
      <input type="submit" value="Submit">
  </body>
</html>
)=====";



//the Wemos WS2812B RGB shield has 1 LED connected to pin 2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2*pix_length, PIN, NEO_GRB + NEO_KHZ800);
int pix_count = 2 * pix_length;

void staticColour(int int_R, int int_G, int int_B, int ledSpeed, int ledBright) 
{
  //Serial.println ("staticColour enter");
  pixels.setPixelColor(ledPosition, pixels.Color(int_R * ledBright / 100, int_G * ledBright / 100, int_B * ledBright / 100)); 
  pixels.show();
  delay(ledSpeed);
  ledPosition += 1;
  if (ledPosition >= pix_count)
    ledPosition = 0;
  if (ledPosition == tempLedPosition)
      workIsFinished = true;
}


void WeelColour (int ledSpeed, int ledBright)
{
  //Serial.println ("WeelColour enter");
  int _r, _g, _b = 0;
  if (colour <= 255) // красный макс, зелёный растёт
  {
    _r = 255;
    _g = colour;
    _b = 0;
  }
  if ((colour > 255) && (colour <= 510)) // зелёный макс, падает красный
  {
    _r = 510 - colour;
    _g = 255;
    _b = 0;
  }
  if ((colour > 510) && (colour <= 765)) // зелёный макс, растёт синий
  {
    _r = 0;
    _g = 255;
    _b = colour - 510;
  }
  if ((colour > 765) && (colour <= 1020)) // синий макс, падает зелёный
  {
    _r = 0;
    _g = 1020 - colour;
    _b = 255;
  }
  if ((colour > 1020) && (colour <= 1275)) // синий макс, растёт красный
  {
    _r = colour - 1020;
    _g = 0;
    _b = 255;
  }
  if ((colour > 1275) && (colour <= 1530)) // красный макс, падает синий
  {
    _r = 255;
    _g = 0;
    _b = 1530 - colour;
  }

  _r = _r * ledBright / 100;
  _g = _g * ledBright / 100;
  _b = _b * ledBright / 100; 
  
  pixels.setPixelColor(ledPosition, pixels.Color(_r, _g, _b)); 
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

void handleForm() 
{
  String str_type = server.arg("str_type");
  int_R = (server.arg("str_R")).toInt();;
  int_G = (server.arg("str_G")).toInt();
  int_B = (server.arg("str_B")).toInt();
  ledSpeed = (server.arg("Speed")).toInt();
  ledBright = (server.arg("Brightness")).toInt();
  globalBreak = (server.arg("Break")).toInt();
  Serial.println (ledPosition);
  server.send(200, "text/html", MAIN_page, sizeof(MAIN_page));
  if (globalBreak == 1) {
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
}

// Ошибка в адресе
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  message += server.args();
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "json/plain", message);
}

void handleRoot() 
{
  Serial.println("Main IP");
  server.send(200, "text/html", MAIN_page, sizeof(MAIN_page)); //Отправка страницы, sizeof для исключения переполнения буфера
}

void setup(void) {
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
  server.onNotFound(handleNotFound); // NotFound адрес 
  Serial.println("HTTP сервер запущен");
  server.begin();
}

void loop(void) { 
  server.handleClient();

  if ( workIsFinished == false){
    switch (lightningType) {
        case 1:
          staticColour(int_R, int_G, int_B, ledSpeed, ledBright);
          break;;
        case 2:
          WeelColour(ledSpeed, ledBright);
          break;
        break;;
    }  
    uint32_t free = system_get_free_heap_size();
    Serial.print("free mem after effect: ");
    Serial.println(free);
  }
}
