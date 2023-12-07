#define LILYGO_WATCH_2019_WITH_TOUCH
#include <LilyGoWatch.h>
TTGOClass *ttgo;

#include <SimpleDHT.h>

#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "ds-2109";
const char* password = "Kristicti";

//Your Domain name with URL path or IP address with path
const char* serverName1 = "http://54.210.207.43:8080/setValue";
const char* serverName2 = "http://107.21.26.55:8080/setValue";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
unsigned long timerDelay = 300;
unsigned long timestart = 0;

String response1;
String response2;
int runtime = 0;
// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 21 or 25
int pinDHT11 = 25;
SimpleDHT11 dht11(pinDHT11);

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  http.setTimeout(2000);

  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}

void setup() {
  timestart = millis();
  Serial.begin(115200);
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
  ttgo->tft->setTextFont(4);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  if (millis() > timestart + 5000 - runtime) {
    // start counting
    int stopwatchStart = millis();
    Serial.println("=================================");
    Serial.println("Sample DHT11...");
    // read without samples.
    byte temperature = 0;
    byte humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
      return;
    }
    Serial.print("Sample OK: ");
    Serial.print(String((float)temperature) + "* C, ");
    Serial.println(String((float)humidity) + "% H");
  
    ttgo->tft->drawString(String((int)temperature*1.8 + 32) + " *F",  5, 10);
    ttgo->tft->drawString(String(humidity) + " % H",  5, 40);
  
    int t = (int)temperature;
    int h = (int)humidity;
    String attachment = String("?t=") + t + "&h=" + h + "&t=" + millis();
    String url1 = String(serverName1) + attachment;
    String url2 = String(serverName2) + attachment;
    Serial.println(url1);
    Serial.println("and");
    Serial.println(url2);
    Serial.println("with response: ");
    response1 = httpGETRequest(url1.c_str());
//    response2 = httpGETRequest(url2.c_str());
    response2 = "None";
    Serial.println(response1);
    Serial.println("and");
    Serial.println(response2);
    timestart = millis();
    runtime = millis() - stopwatchStart;
    Serial.println("runtime" + String(runtime));
  }
}
