/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "stdlib_noniso.h"

#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#include "lib/WifiData.h"
#include "lib/Temp.h"
#include "lib/html.h"
#include "lib/button.h"

#define VER   "01.00.00"

/*  DISPLAY */
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool lightON = false;
unsigned long previousMillisLight = 0;
const long intervalLight = 4000;  //4 seconds
/*  DISPLAY - END */

/*  TEMP */
// current temperature & humidity, updated in loop()
float temp = 0.0;
float hum = 0.0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis      = 0;    // will store last time DHT was updated
// Updates DHT readings every 10 seconds
const long interval      = 10000;  
/*  TEMP - END */



/* NTP */
const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THUR", "FRI", "SAT"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

/* NTP - END */


// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temp);
  }
  else if(var == "HUMIDITY"){
    return String(hum);
  }
  return String();
}



void setup(){

  //Button
  //pinMode(buttonPin, INPUT);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);


  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Start --> ");
  
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
 

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temp).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(hum).c_str());
  });

  // Start server
  server.begin();

  timeClient.begin();

  Serial.println("dht.begin()...");
  dht.begin();

  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(16,2);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();

  // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd.setCursor(5, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd.print("HELLO");

  // Move the cursor to the next line and print
  // WORLD.
  lcd.setCursor(5, 1);      
  lcd.print("WORLD");

  delay(3000);

  lcd.noBacklight();  
  lcd.clear();
}

void printOnScreen()
{  
  timeClient.update();

  int hour = timeClient.getHours();
  int min  = timeClient.getMinutes();
  int sec  = timeClient.getSeconds();
  int day  = timeClient.getDay();

  Serial.print(hour);  Serial.print(":");  
  Serial.print(min);  Serial.print(":");  
  Serial.println(sec); 
  Serial.print(daysOfTheWeek[day]);

  lcd.clear();

  lcd.setCursor(0, 0);   /*HH:MM:SS    DDD */
  lcd.print(hour); lcd.print(":"); lcd.print(min); lcd.print(":"); lcd.print(sec);
  lcd.print("    ");  lcd.print(daysOfTheWeek[day]);

  lcd.setCursor(0, 1);
  lcd.print("T: "); lcd.print(temp, 1);   /* T: tt.t */
  lcd.setCursor(9, 1);      
  lcd.print("H: "); lcd.print(hum, 1);    /* H: hh.h */
}
 

void loop(){  
  unsigned long currentMillis = millis();

  if (button1.pressed)
  {
    lcd.backlight();
    previousMillisLight = millis();
    lightON = true;
    button1.pressed = false;
  }

  if ( (lightON) && (currentMillis - previousMillisLight >= intervalLight) )
  {
    lightON = false;    
    lcd.noBacklight();
  }

  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor! newT");
    }
    else {
      temp = newT;
      Serial.println(temp);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      hum = newH;
      Serial.println(hum);
    }
    printOnScreen();
    
  }
}





void ICACHE_RAM_ATTR isr() {
    button_time = millis();
    if (button_time - last_button_time > 250)
    {
      button1.numberKeyPresses++;
      button1.pressed = true;
      last_button_time = button_time;
    }
}