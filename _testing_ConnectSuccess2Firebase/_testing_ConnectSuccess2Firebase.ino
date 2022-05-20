#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Firebase_ESP_Client.h>
// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
//#define WIFI_SSID "SIX Cafe"
//#define WIFI_PASSWORD "chaokhub"
#define WIFI_SSID "MangDayKTX G4-903"
#define WIFI_PASSWORD "ag490311"
//#define WIFI_SSID "Coffe Dreamy"
//#define WIFI_PASSWORD "DT686868"
/* 2. Define the RTDB URL */
#define DATABASE_URL "henhung-74f0f-default-rtdb.asia-southeast1.firebasedatabase.app"

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

int count = 0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
#define M 100000
#define SENSOR 14 // light sensor - pin D5
float Pin_Status;
bool Light_State = HIGH;
// cảm biến nhiệt độ và độ ẩm.
//  missing location and time for node.
//  need to add a break each time data send
//  relay and light to add more function.
//  Google Assistance? Maybe
//
void WifiConnector()
{
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
    config.database_url = DATABASE_URL;

    config.signer.test_mode = true;

    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);
}
void setup()
{
    Serial.begin(115200);
    delay(2000);
    pinMode(LED_BUILTIN, OUTPUT); // pin D4
    pinMode(SENSOR, INPUT);
    timeClient.begin();
    timeClient.setTimeOffset(25200); // Thailan+7
    WifiConnector();
    FirebaseConnector();
}

void loop()
{
    Pin_Status = digitalRead(SENSOR);
    timeClient.update();
    String formattedDate = timeClient.getFormattedTime();
    //    if (isnan(t) || isnan(h))
    //    {
    //      Serial.println("Failed to read from DHT sensor!");
    //      if (isnan(t))
    //        t = M;
    //      if (isnan(h))
    //        h = M;
    //    }else{
    //        Serial.printf("Set temp... %s\n", Firebase.RTDB.setInt(&fbdo, "/test/temp", t) ? "ok" : fbdo.errorReason().c_str());
    //        Serial.printf("Set humidity... %s\n", Firebase.RTDB.setInt(&fbdo, "/test/humidity", h) ? "ok" : fbdo.errorReason().c_str());
    //    }
    if (Pin_Status == 1)
    {
        Serial.print("[1]");
        Serial.print("Light: "); // khong co den nen can den
        Serial.println(Pin_Status);
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.printf("Set light... %s\n", Firebase.RTDB.setInt(&fbdo, "/test/light", 1) && Firebase.RTDB.setString(&fbdo, "/test/time", formattedDate) ? "ok" : fbdo.errorReason().c_str());
    }
    else 
    {
        Serial.print("[0]");
        Serial.print("No Light: ");
        Serial.println(Pin_Status);
        digitalWrite(LED_BUILTIN, LOW);
        Serial.printf("Set light... %s\n", Firebase.RTDB.setInt(&fbdo, "/test/light", 0) && Firebase.RTDB.setString(&fbdo, "/test/time", formattedDate) ? "ok" : fbdo.errorReason().c_str());
    }
    delay(1000);
}
