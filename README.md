"# HomeTemp" 

#first commit - Base for the whole project
 - ESP8266
 - Display LiquidCrystal with I2C module
 - DHT11 temperature and humidity sensor
 - Button

 The module connects to the home WiFi network using ssid and password into WifiData.h
 The temperature and sensore are displayed on the display. Values are refreshed with a period of 
 10 seconds, defined into const interval variable.
 The user can reach the temperature and data values using the IP address shown at the startup.
 If the button is pressed the display switch the light on for 4 seconds (defined by const long intervalLight).

#second commit
 - Added release tag: 01.00.00
 - Added time with NTP server

#third commit
 - Added LED to show wifi connection
 - Refactory