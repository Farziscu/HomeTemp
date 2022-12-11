
#ifndef WiFi_Data
#define WiFi_Data

const char* ssid = "***";
const char* password = "***";

/* WIFI connection */
unsigned long previousMillisWifiCheck = 0;
const long    intervalWifiCheck = 1000;  //1 seconds
bool          isConnected = false;
/* WIFI connection */

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

#endif