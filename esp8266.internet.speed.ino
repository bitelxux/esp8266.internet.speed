
/* Testest with
 -  Arduino IDE 1.8.15
    - Arduino AVR board 1.8.3
    - ESP8266 2.7.0

https://arduino.esp8266.com/stable/package_esp8266com_index.json
board: NodeMCU1.0 (ESP-12E Module)    
*/

//Libraries
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>;

// This is for each variable to use it's real size when stored
#pragma pack(push, 1)

//Constants
#define LED D4

// prototipes
void read_value();
void blinkLed();
void connectIfNeeded();

const char* ssid = "Starlink";
const char* password = "82111847";
const char* baseURL = "http://94.177.253.187:8889";

unsigned int tConnect = millis();
unsigned long tLastConnectionAttempt = 0;

// Timers
#define NUM_TIMERS 3 // As I can't find an easy way to get the number
                     // of elements in an array

struct
{
    boolean enabled;
    unsigned long timer;
    unsigned long lastRun;
    void (*function)();
    char* functionName;
} TIMERS[] = {
  { true, 1*1000, 0, &blinkLed, "blinkLed" },
  { true, 2*1000, 0, &read_value, "read_value" },    
  { true, 5*1000, 0, &connectIfNeeded, "connectIfNeeded" },  
};

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200); 
}


void attendTimers(){
    
  for (int i=0; i<NUM_TIMERS; i++){
    if (TIMERS[i].enabled && millis() - TIMERS[i].lastRun >= TIMERS[i].timer) {
      TIMERS[i].function();
      TIMERS[i].lastRun = millis();
    }
  }
  
}

void connect(){
  Serial.println("");
  Serial.println("Connecting");

  WiFi.begin(ssid, password);

  tLastConnectionAttempt = millis();
  tConnect =  millis();
  while(WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, !digitalRead(LED));
    delay(100);
    if ((millis() - tConnect) > 500){
      Serial.print(".");
      tConnect = millis();
    }

    // If it doesn't connect, let the thing continue
    // in the case that in a previous connection epochTime was
    // initizalized, it will store readings for future send
    if (millis() - tLastConnectionAttempt >= 30000L){      
      break;
    }    
  }

  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) { 
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());  
  }
  else
  {
    Serial.println("Failed to connect");
  }

  tLastConnectionAttempt = millis();
  
}

void blinkLed(){
  digitalWrite(LED, !digitalRead(LED));
}

void read_value(){

  static int count=0;

  char buffer[100];
  WiFiClient client;
  HTTPClient http;

  sprintf(buffer, "%s/last", baseURL);
  
  http.begin(client, buffer);
  
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200){
        String payload = http.getString();
        sprintf(buffer, "%04d [%d] %s", ++count, httpResponseCode, payload.c_str());
        Serial.println(buffer);
      }
      else {
        sprintf(buffer, "[send] error code: %d", httpResponseCode);
        Serial.println(buffer);
      }
      // Free resources
      http.end();    

}


void connectIfNeeded(){
  // If millis() < 30000L is the first boot so it will try to connect
  // for further attempts it will try with spaces of 60 seconds

  if (WiFi.status() != WL_CONNECTED && (millis() < 30000L || millis() - tLastConnectionAttempt > 60000L)){
    Serial.println("Trying to connect");
    connect();
  }  

}

void loop() {

  attendTimers();
  delay(20);

  //digitalWrite(LED, !digitalRead(LED));
  //delay(100);

}