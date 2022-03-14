
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

#include <Servo.h>

// This is for each variable to use it's real size when stored
#pragma pack(push, 1)

//Constants
#define LED D4
#define SERVO_DOWNLOAD D6
#define SERVO_UPLOAD D7

Servo servo_upload;
Servo servo_download;

int increment = 0;
int angulo = 0;

// prototipes
void read_values();
void blinkLed();
void connectIfNeeded();

const char* ssid = "Starlink";
const char* password = "82111847";
const char* baseURL = "http://192.168.1.151:8889";

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
  { true, 5*1000, 0, &read_values, "read_values" },    
  { true, 5*1000, 0, &connectIfNeeded, "connectIfNeeded" },  
};

void setup() {
  pinMode(LED, OUTPUT);
  
  Serial.begin(115200);

  servo_upload.attach(SERVO_UPLOAD);
  servo_upload.write(0);

  servo_download.attach(SERVO_DOWNLOAD);
  servo_download.write(180);

  delay(5000);
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


void read_values(){
  read_download();
  read_upload();
}

void read_download(){

  static long count=0;

  char buffer[100];
  WiFiClient client;
  HTTPClient http;

  sprintf(buffer, "%s/download", baseURL);
  
  http.begin(client, buffer);
  
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200){

        String payload = http.getString();

        // Something went wrong on the servers side
        if (payload == "-1") return;
            
        const char *info = payload.c_str();
        int speed = atoi(info);
        int angle = (int)(180*speed)/350;
        
        //sprintf(buffer, "%04d [%d] %d", count, httpResponseCode, angle);
        //Serial.println(buffer);
        sprintf(buffer, "[%d] Download: moving to %d", count, angle);
        Serial.println(buffer);
        
        move_download(angle);
        
        count ++;
      }
      else {
        sprintf(buffer, "[send] error code: %d", httpResponseCode);
        Serial.println(buffer);
      }
      // Free resources
      http.end();                                              

}

void read_upload(){

  static long count=0;

  char buffer[100];
  WiFiClient client;
  HTTPClient http;

  sprintf(buffer, "%s/upload", baseURL);
  
  http.begin(client, buffer);
  
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200){
    
        String payload = http.getString();

        // Something went wrong on the servers side
        if (payload == "-1") return;
            
        const char *info = payload.c_str();
        int speed = atoi(info);
        int angle = (int)(180*speed)/60;
        
        //sprintf(buffer, "%04d [%d] %d", count, httpResponseCode, angle);
        //Serial.println(buffer);
        sprintf(buffer, "[%d] Upload: moving to %d", count, angle);
        Serial.println(buffer);

        move_upload(angle);
        
        count ++;
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

void move_upload(int angle){
  servo_upload.write(angle);
}

void move_download(int angle){
  servo_download.write(180 - angle);
}

void test_servo_upload(){
  for (int angle=0; angle<=180; angle+=90){
    servo_upload.write(angle);
    delay(1000);
  }
  
  for (int angle=180; angle>=0; angle-=90){
    servo_upload.write(angle);
    delay(1000);
  }  
}

void test_servo_download(){
  for (int angle=0; angle<=180; angle+=90){
    servo_download.write(180 - angle);
    delay(1000);
  }
  
  for (int angle=180; angle>=0; angle-=90){
    servo_download.write(180 - angle);
    delay(1000);
  }  
}

void fake_readings(){
  int download = random(0, 350);
  int upload = random(0, 60);

  int download_angle = (int)(180*download)/350;
  int upload_angle = (int)(180*download)/60;

  move_download(download_angle);
  move_upload(upload_angle);  
}

void loop() {
  
  //test_servo_upload();
  //test_servo_download();
  
  attendTimers();
  delay(20);
}
