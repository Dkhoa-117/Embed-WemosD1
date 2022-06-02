/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID           "TMPLW8ZEbeny"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "d1nx1aMy2FbZWBQX-bnRVmgQZnONljVa"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
#include "DHT.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <arduino_homekit_server.h>
#include <BlynkSimpleEsp8266.h>
#include <Firebase_ESP_Client.h>
#define PL(s) Serial.println(s)
#define P(s) Serial.print(s)
#define DHTTYPE DHT11

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "MangDayKTX G4-903";
//char pass[] = "ag490311";
//#define WIFI_SSID "MangDayKTX G4-903"
//#define WIFI_PASSWORD "ag490311"


char ssid[] = "iPhone của Đăng";
char pass[] = "0cho0cho";
#define WIFI_SSID "iPhone của Đăng"
#define WIFI_PASSWORD "0cho0cho"
char auth[] = BLYNK_AUTH_TOKEN;
long timer_end;
bool timer_set = false;
BlynkTimer timer;
#define DATABASE_URL "henhung-74f0f-default-rtdb.asia-southeast1.firebasedatabase.app"
/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth firebaseauth;
/* Define the FirebaseConfig data for config data */
FirebaseConfig firebaseconfig;

int count = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
#define M 100000
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define SENSOR_D6 12 
#define SENSOR_D7 13
#define D7 13
DHT dht(SENSOR_D7, DHTTYPE);

float Pin_Status;
bool Light_State = HIGH;
bool isread_temp = true;
// This function is called every time the Virtual Pin 0 state changes
void WifiConnector()
{
    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.disconnect(false);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

void FirebaseConnector()
{
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the database URL(required) */
    firebaseconfig.database_url = DATABASE_URL;

    firebaseconfig.signer.test_mode = true;

    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&firebaseconfig, &firebaseauth);
}
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int light_state = param.asInt();

  // Update state
  digitalWrite(D4,light_state); // hardware control
}
BLYNK_WRITE(V5){
  int val = param.asInt();
  if(val > 0){
    if(!timer_set){
      Blynk.virtualWrite(V0, 1);  
      timer_end = millis()/1000 + val;
      timer_set = true;
    }
  }else{
    timer_set = false;
  }
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
    Blynk.syncAll();
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000); // uptime value
  BLYNK_WRITE(V0); // receive data from V0 - light switch
  BLYNK_WRITE(V5); // light time setting check
  led_manager();
    Pin_Status = digitalRead(SENSOR_D6);
    timeClient.update();
    String formattedDate = timeClient.getFormattedTime();
    if (Pin_Status == 1)
    {
        Serial.print("[1]");
        Serial.print("Light: ");
        Serial.println(Pin_Status);
        digitalWrite(D2, HIGH);
        digitalWrite(D3, HIGH);
        Serial.printf("Set light... %s\n", Firebase.RTDB.setInt(&fbdo, "/test/light", 1) && Firebase.RTDB.setString(&fbdo, "/test/time", formattedDate) ? "ok" : fbdo.errorReason().c_str());
    }
    else 
    {
        Serial.print("[0]");
        Serial.print("No Light: ");
        Serial.println(Pin_Status);
        digitalWrite(D2, LOW);
        digitalWrite(D3, LOW);
        Serial.printf("Set light... %s\n", Firebase.RTDB.setInt(&fbdo, "/test/light", 0) && Firebase.RTDB.setString(&fbdo, "/test/time", formattedDate) ? "ok" : fbdo.errorReason().c_str());
    }
    if (isread_temp){
        isread_temp = false;
        float h = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float t = dht.readTemperature();
        if (isnan(h) || isnan(t)) {
            Serial.println("Failed to read from DHT sensor!");
        }
        else{
            Serial.printf("Set temperature... %s\n", Firebase.RTDB.setFloat(&fbdo, "/test/temperature", t) ? "ok" : fbdo.errorReason().c_str());
            Serial.printf("Set humidity... %s\n", Firebase.RTDB.setFloat(&fbdo, "/test/humility", h) ? "ok" : fbdo.errorReason().c_str());
        }
    }
    else{
        isread_temp = true;
    }

}
void led_manager(){
  if(timer_set){
    int temp = (timer_end - millis()/1000); //on time
    if(temp > 0){
      Blynk.notify("led ACTIVADO");
      digitalWrite(D4, HIGH);
      Blynk.virtualWrite(V5, temp); // status of pin
    }else{ // out of time
      timer_set = false;
      digitalWrite(D4, LOW);
      Blynk.virtualWrite(V0, 0); // status of pin
      Blynk.virtualWrite(V5, 0); // status of pin
    }
  }else{
    
  }
}
void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.setRxBufferSize(32);
  Serial.setDebugOutput(false);
  pinMode(D4, OUTPUT); //blynk
  pinMode(D2, OUTPUT); //firebase + sensor
  pinMode(D3, OUTPUT); //relay
  pinMode(D5, OUTPUT); //Homekit
  pinMode(SENSOR_D6, INPUT);//light sensor
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timeClient.begin();
  timeClient.setTimeOffset(25200); // Thailan+7
  WifiConnector();
  FirebaseConnector();
  // Setup a function to be called every second
  DEBUG_HEAP();
  homekit_setup();
  DEBUG_HEAP();
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  homekit_loop();
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

//==============================
// Homekit setup and loop
//==============================

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t name;
extern "C" void led_toggle();
extern "C" void accessory_init();

uint32_t next_heap_millis = 0;

void homekit_setup() {
  accessory_init();
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  int name_len = snprintf(NULL, 0, "%s_%02X%02X%02X", name.value.string_value, mac[3], mac[4], mac[5]);
  char *name_value = (char*)malloc(name_len + 1);
  snprintf(name_value, name_len + 1, "%s_%02X%02X%02X", name.value.string_value, mac[3], mac[4], mac[5]);
  name.value = HOMEKIT_STRING_CPP(name_value);

  arduino_homekit_setup(&config);
 
}

void homekit_loop() {
  arduino_homekit_loop();
  uint32_t time = millis();
  if (time > next_heap_millis) {
    INFO("heap: %d, sockets: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
    next_heap_millis = time + 5000;
  }
}
