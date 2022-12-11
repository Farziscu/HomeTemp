
// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#include "lib/WifiData.h"
#include "lib/Temp.h"
#include "lib/html.h"
#include "lib/button.h"

#define VER           "01.00.00"
#define LED_WIFISTAT  D8   //LED at GPIO4 D8

/*  DISPLAY */
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool          lightON             = false;
unsigned long previousMillisLight = 0;
const long    intervalLight       = 4000;  //4 seconds
/*  DISPLAY - END */



/*  TEMP */
// current temperature & humidity, updated in loop()
float         temp  = 0.0;
float         hum   = 0.0;
unsigned long previousMillisTemp  = 0;    // will store last time DHT was updated
// Updates DHT readings every 10 seconds
const long intervalTemp           = 10000;  
/*  TEMP - END */


/* NTP */
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THUR", "FRI", "SAT"};

// Define NTP Client to get time
WiFiUDP   ntpUDP;
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

  //LED_WIFISTAT - Switched on when ESP is not connected to the WiFi
  pinMode(LED_WIFISTAT, OUTPUT);
  digitalWrite(LED_WIFISTAT, HIGH);

  //Button
  //pinMode(buttonPin, INPUT);
  pinMode(button.PIN, INPUT_PULLUP);
  attachInterrupt(button.PIN, isr, FALLING);

  // Serial port for debugging purposes
  Serial.begin(115200);  
    
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  //LED_WIFISTAT - Switched off when ESP is connected to the WiFi
  digitalWrite(LED_WIFISTAT, LOW);
  isConnected = true;

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

  // Time with NTP server
  timeClient.begin();

  Serial.println("dht.begin()...");
  dht.begin();

  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(16,2);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();

  //print "HELLO WORLD"
  lcd.setCursor(5, 0);  
  lcd.print("HELLO");
  lcd.setCursor(5, 1);      
  lcd.print("WORLD");

  delay(3000);

  lcd.noBacklight();  
  lcd.clear();
}

void printOnScreen()
{  
  timeClient.update();
  
  Serial.print(timeClient.getFormattedTime()); Serial.print(" ");
  Serial.println(daysOfTheWeek[timeClient.getDay()]);

  lcd.clear();
  lcd.setCursor(0, 0);  /*time formatted like `hh:mm:ss` + day*/
  lcd.print(timeClient.getFormattedTime());
  lcd.print("    ");  lcd.print(daysOfTheWeek[timeClient.getDay()]);

  lcd.setCursor(0, 1);  /*temperature and humidity*/
  lcd.print("T: "); lcd.print(temp, 1);   /* T: tt.t */
  lcd.setCursor(9, 1);      
  lcd.print("H: "); lcd.print(hum, 1);    /* H: hh.h */
}
 

void loop(){  
  unsigned long currentMillis = millis();

  //Check button - turn backlight display on if button has been pressed
  if (button.pressed)
  {
    previousMillisLight = millis();
    button.pressed = false;
    lcd.backlight();    
    lightON = true;    
  }

  //Check display backlight - turn it off after intervalLight seconds 
  if ( (currentMillis - previousMillisLight >= intervalLight) && (lightON) )
  {
    lightON = false;    
    lcd.noBacklight();
  }

  //Check Wifi connection
  if (currentMillis - previousMillisWifiCheck >= intervalWifiCheck)
  {
    previousMillisWifiCheck = millis();

    if (isConnected == true)
    {
      if (!WiFi.isConnected())
      {
        digitalWrite(LED_WIFISTAT, HIGH);        
        isConnected = false;
        Serial.println("Conn LOST ");
      }
    }
    else
    {
      Serial.println("Conn DOWN... ");
      if (WiFi.isConnected()) {
        digitalWrite(LED_WIFISTAT, LOW);
        isConnected = true;
        Serial.println("Conn OK ");
      }
    }
  }

  //Retrieve temperature and humidity after previousMillisTemp seconds
  if (currentMillis - previousMillisTemp >= intervalTemp) {    
    // save the last time you updated the DHT values
    previousMillisTemp = currentMillis;
    
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();    
    if (isnan(newT)) 
    {
      // read failed, don't change t value
      Serial.println("Failed to read from DHT sensor! newT");
    }
    else {
      temp = newT;
      Serial.println(temp);
    }

    // Read Humidity
    float newH = dht.readHumidity();    
    if (isnan(newH)) 
    {
      // read failed, don't change h value 
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
      button.pressed = true;
      last_button_time = button_time;
    }
}