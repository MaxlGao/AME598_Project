//min and max servo angle for each servo
//Servo  | Min | Max
//pitch  | 0   | 180
//yaw    | 0   | 180


//#include <Servo.h>
#include <ESP32Servo.h>
#include <math.h>
String input;  // stores string from serial input


//Storing user chosen servo
String stringSerX; //Stores string for user chosen servo
char charSerX;  //Stores user chosen servo as a char. This is needed for switch statements

//Stores current positions of servo angle
int Ys;
int Ps;

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

// create servo object to control a servo
Servo serY; //Yaw servo
Servo serP; //Pitch servo

void setup() {
  Serial.begin(9600);
  Serial.println("servo-test-MeArm"); // keeps track of when the program is started

  // attach servos to pins Servo#.attach(Pin#);
  serY.attach(27);  // attaches the yaw servo to pin XX
  serP.attach(25);  // attaches the pitch servo to pin XX

  //Set starting positions
  Ys = 90;
  Ps = 90;
  
  //define servo angle limits
  //Yaw Servo
  limMinYs = 0; //min limit
  limMaxYs = 180; //max limit

  //Pitch Servo
  limMinPs = 0; //min limit
  limMaxPs = 180; //max limit

  serY.write(Ys);
  // moveServo(serY, limMinYs, limMaxYs, nYs, Ys);
  delay(500);

  // serP.write(Ps);
  moveServo(serP, limMinPs, limMaxPs, nPs, Ps);
  delay(500);

}



void loop() {

 
  //prompt user to choose what they want to perform
  Serial.println("Specify pitch up, pitch down, yaw left, yaw right");
  Serial.println("0x00 = up, 0x01 = down, 0x02 = left, 0x03 = right");

  //pauses code until user input is detected
  while (Serial.available() == 0) {
    //Wait for user input
  }

  
  //DOSENT Get and store user input into a string
  //DOES take the value of incomingByte 
//  input = GetBLE();
  input = getString();
  //move servo depending on the input

  if (input.length() > 0) {
    // Pitch up
    if (strcmp(input.c_str(), "0") == 0) {
      Serial.println("Pitching Up");
      nPs = Ps + 10; // calculates next desired servo position
      moveServo(serP, limMinPs, limMaxPs, nPs, Ps); // calls this fuction to move the servo
      Ps = max(limMinPs, min(limMaxPs, nPs)); //stores the current angle of the servo, resetting it to within bounds if needed.
      Serial.print("Pitch is ");
      Serial.println(Ps);
      input = ""; //empty for next input
    }

    // Pitch Down
    if (strcmp(input.c_str(), "1") == 0) {
      Serial.println("Pitching Down");
      nPs = Ps - 10; // calculates next desired servo position
      moveServo(serP, limMinPs, limMaxPs, nPs, Ps); // calls this fuction to move the servo
      Ps = max(limMinPs, min(limMaxPs, nPs)); //stores the current angle of the servo, resetting it to within bounds if needed.
      Serial.print("Pitch is ");
      Serial.println(Ps);
      input = ""; //empty for next input    
    }

    // Yaw Left
    if (strcmp(input.c_str(), "2") == 0) {
      Serial.println("Turning Left");
      nYs = Ys + 10; // calculates next desired servo position
      moveServo(serY, limMinYs, limMaxYs, nYs, Ys); // calls this fuction to move the servo
      Ys = max(limMinYs, min(limMaxYs, nYs)); //stores the current angle of the servo, resetting it to within bounds if needed.
      Serial.print("Yaw is ");
      Serial.println(Ys);
      input = ""; //empty for next input
    }

    // Yaw Right
    if (strcmp(input.c_str(), "3") == 0) {
      Serial.println("Turning Right");
      nYs = Ys - 10; // calculates next desired servo position
      moveServo(serY, limMinYs, limMaxYs, nYs, Ys); // calls this fuction to move the servo
      Ys = max(limMinYs, min(limMaxYs, nYs)); //stores the current angle of the servo, resetting it to within bounds if needed.
      Serial.print("Yaw is ");
      Serial.println(Ys);
      input = ""; //empty for next input  
    }
  }
}

//Function to Get and store string
String getString() {
  String i = ""; //clear input
  while (Serial.available()) {
    i = Serial.readStringUntil('\n'); //stores the user input as a string. Any spaces, lines, and tabs are removed
    i.trim();
  }
  return i; //function returns the string input
}


//BLE Data Function
String GetBLE(){
  int incomingByte = 0; // for incoming serial data from the BLE module
  // send data only when you receive data:
  //if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();


  //}//==========================================
  String BLESTR = String(incomingByte);
  if(strcmp(BLESTR.c_str(),"-1")==0){
    BLESTR="";
  }
  else{
    // say what you got:
    
    Serial.print("BLE Byte: ");
    Serial.println(BLESTR);
  return BLESTR;
  }

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
