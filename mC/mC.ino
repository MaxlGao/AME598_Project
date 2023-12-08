//min and max servo angle for each servo
//Servo  | Min | Max
//pitch  | 0   | 180
//yaw    | 0   | 180


//#include <Servo.h>
#include <ESP32Servo.h>
#include <math.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "[SSID]";
const char* password = "[PASSWORD]";
//Your Domain name with URL path or IP address with path
const char* serverUrl = "http://34.207.112.40:8080/getValue";

// create servo object to control a servo
Servo serY; //Yaw servo
Servo serP; //Pitch servo

int Ys = 90;  // Current yaw position
int Ps = 90;  // Current pitch position
int servoStep = 10;  // Amount to move the servo on each command

//Stores next desired positions of servo angle
int nYs;
int nPs;

//Servo speed control
int msec = 10; //Time (msec) between each degree of movement

//set limits for servo angles
//Yaw servo
int limMinYs; //min limit
int limMaxYs; //max limit

//Pitch Servo
int limMinPs; //min limit
int limMaxPs; //max limit



void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Attach servos to pins
  serY.attach(27);  // Yaw servo pin
  serP.attach(25);  // Pitch servo pin

  // Initialize servo positions to defaults
  serY.write(Ys);
  delay(500);
  serP.write(Ps);
  delay(500);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Received: " + payload);

      // Process yaw command
      if (payload.indexOf("right") != -1) {
        Serial.println("Turning Right");
        Ys = min(Ys + servoStep, 180);
        serY.write(Ys);
        Serial.print("Yaw is ");
        Serial.println(Ys);
      } else if (payload.indexOf("left") != -1) {
        Serial.println("Turning Left");
        Ys = max(Ys - servoStep, 0);
        serY.write(Ys);
        Serial.print("Yaw is ");
        Serial.println(Ys);
      }

      // Process pitch command
      if (payload.indexOf("up") != -1) {
        Serial.println("Pitching Up");
        Ps = min(Ps + servoStep, 180);
        serP.write(Ps);
        Serial.print("Pitch is ");
        Serial.println(Ps);
      } else if (payload.indexOf("down") != -1) {
        Serial.println("Pitching Down");
        Ps = max(Ps - servoStep, 0);
        serP.write(Ps);
        Serial.print("Pitch is ");
        Serial.println(Ps);
      }
    } else {
      Serial.println("Error in HTTP request");
    }
    http.end();
  }
  delay(1000); // Polling interval
}

//Function to move servo
//parameters moveServo(servo name, min limit of that servo, max limit of that servo, desired servo angle, current servo angle )
void moveServo(Servo serX, int limMinX, int limMaxX, int desPos, int curPos) {
  //Display error messages if value is out of range
  if (desPos > limMaxX || desPos < limMinX)
  {
    Serial.println("The value is out of range, try again.");
  }
  //moves servo if value is with in the range
  else
  {
    Serial.print("writing servo Angle: ");
    Serial.println(desPos);
    Serial.println("start moving");
    //while loops are used to control the speed of the servos.
    //Delay time between each deg of movement can be adjusted at the top
    while ( desPos < curPos) { //decreases angle
      --curPos;
      serX.write(curPos);
      delay(msec);
      //Serial.println(curPos);
    }
    while ( desPos > curPos) { //increases angle
      ++curPos;
      serX.write(curPos);
      delay(msec);
      //Serial.println(curPos);
    }
    Serial.println("done moving");
  }
  Serial.println();
}
