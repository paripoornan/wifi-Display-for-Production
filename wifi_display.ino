#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP_EEPROM.h>
#include "SevenSegmentTM1637.h"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager_custom.h> 

#define TRIGGER_PIN D8
void config_loop(void);    //Function for DemandAP & Config
const byte PIN_CLK = D2;   // define CLK pin (any digital pin)
const byte PIN_DIO = D3;   // define DIO pin (any digital pin)
SevenSegmentTM1637    display(PIN_CLK, PIN_DIO);


String line,shift;
void setup() 
{
  pinMode(TRIGGER_PIN, INPUT);
  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %
  display.print("INIT");      // display INIT on the display
  delay(1000);                // wait 1000 ms

  Serial.begin(115200);

  
  

//  WiFi.begin(ssid, password);

   //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    display.print("Conn........");
    //if you get here you have connected to the WiFi
    Serial.println("connected.....:)");

  
//String line1="a0402";
//String shift1="dd";
  // put some data into eeprom
  //EEPROM.put(0,line1 );  // ( address is '0')
  //EEPROM.put(50,shift1);  // ( address is '50')
   // boolean ok1 = EEPROM.commit();
  //Serial.println((ok1) ? "First commit OK" : "Commit failed");
  
  EEPROM.begin(100);
  EEPROM.get(0, line);
  EEPROM.get(50, shift);
  display.clear();                      // clear the display

  display.print("Line_"+line+"  "+"Shift_"+shift);
  delay(2000);
  display.clear();                      // clear the display
}

void loop() 
{
   config_loop();
    if (WiFi.status() == WL_CONNECTED) 
  {

    String Base_url="http://202.179.77.229:8080/pct/dashboard/get_mes_data_esp.php";
    //String Base_url="http://10.10.100.13:8080/pct/dashboard/get_mes_data_esp.php";
    HTTPClient http; //Object of class HTTPClient
    String URL=Base_url+"?"+"line="+line+"&shift="+shift;
    Serial.println(URL);
    http.begin(URL);
    int httpCode = http.GET();

    if (httpCode > 0) 
    {
  
  
  
  
      const size_t bufferSize = 100;
      //DynamicJsonDocument jsonBuffer(bufferSize);
DynamicJsonDocument root(bufferSize);
//JsonObject& root = doc.parseObject(http.getString());
String json_string=http.getString().substring(101);  //Offset Text 
//Serial.println(json_string);
DeserializationError error = deserializeJson(root, json_string);
      //JsonObject& root = jsonBuffer.parseObject(http.getString());
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  else{
     const char* input = root["input"]; 
     const char* output = root["output"]; 
     
     
     
     
      Serial.print("Input:");
      Serial.println(input);
      Serial.print("Output:");
      Serial.println(output);
      display.print("..in..");
      display.clear();                      // clear the display
      display.print(input);// print COUNTING SOME DIGITS
  }

     

      
    }
    http.end(); //Close connection
  }

 else
 {
  Serial.println("Network disconnected.... Reset the device to Reconnect");
  display.print("Network disconnected.... Reset the device to Reconnect");
 }    
delay(3000);
}

void config_loop()
{
  Serial.println("");
  Serial.println("Waiting For Config button Status...");
  //display.print("Wait");
if ( digitalRead(TRIGGER_PIN) == HIGH ) {
    display.print("Conf");
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //reset settings - for testing
    //wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    //wifiManager.setTimeout(120);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
    //WiFi.mode(WIFI_STA);
    
    if (!wifiManager.startConfigPortal("OnDemandAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
   }  

//Serial.println("Button status False.Back to Main loop");
//display.print("Main loop");
//display.clear();

}
