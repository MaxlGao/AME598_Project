//EGR 455 FINAL PROJECT MEARM 4   -   BLE EDITION
//Benjamin Yee and Max Gao
//12-06-21
//Note all user input is actual angles of the robot arm (theta). All unser input is then converted into servo angles in the code.



//min and max actual angles for each joint
//Servo     | Min   | Max
//left arm  | Rs-254| Rs-159
//right arm | 149   | 4
//base      | -78   | 72
//claw      | 0     | 10

//min and max servo angle for each servo
//Servo     | Min | Max
//left arm  | 45  | 140
//right arm | 35  | 180
//base      | 10  | 160
//claw      | 0   | 40


//#include <Servo.h>
#include <ESP32Servo.h>
#include <math.h>
String input;  // stores string from serial input

//integers used to simplify equations in ik calculations
float xp;
float zp;
float C;

//Storing user chosen servo
String stringSerX; //Stores string for user chosen servo
char charSerX;  //Stores user chosen servo as a char. This is needed for switch statements

//Stors user chosen x, y, and z cordinates
String cord;
float x;
float y;
float z;

//Storing user defined servo angles
String stringTheta;  //Stores user defined servo angle as a string
int theta; //Stores user defined servo angle as an int

//Stores previous position of right servo angle
int pRs;

//Stores current positions of servo angle
int Ls;
int Rs;
int Bs;
int Cs;

//Stores next desired positions of servo angle
int nLs;
int nRs;
int nBs;
int nCs=0;

//Stores current positions of actual angle (theta)
float Lt; //in ik theta L will have 2 values
float Lt1;
float Lt2;

float Rt; //in ik theta R will have 2 values
float Rt1;
float Rt2;

int Bt;
int Ct;

//Servo speed control
int msec = 10; //Time (msec) between each degree of movement

//set limits for servo angles
//Left servo
int limMinLs; //min limit
int limMaxLs; //max limit

//Right Servo
int limMinRs; //min limit
int limMaxRs; //max limit

//Base Servo
int limMinBs; //min limit
int limMaxBs; //max limit

//Claw Servo
int limMinCs; //min limit
int limMaxCs; //max limit

//variables for theta limits
int limMinLt; //min limit
int limMaxLt; //max limit

//Right Servo
int limMinRt; //min limit
int limMaxRt; //max limit

//Base Servo
int limMinBt; //min limit
int limMaxBt; //max limit



// create servo object to control a servo
Servo serL; //left arm servo
Servo serR; //right arm servo
Servo serB; //base servo
Servo serC; //claw servo



void setup() {
  Serial.begin(9600);
  Serial.println("servo-test-MeArm"); // keeps track of when the program is started

  // attach servos to pins Servo#.attach(Pin#);
  serL.attach(27);  // attaches the left arm servo to pin 4
  serR.attach(25);  // attaches the right arm servo to pin 3
  serB.attach(12);  // attaches the base servo to pin 2
  serC.attach(33);  // attaches the claw servo to pin 5

  //Set starting position theta
  Bt = 0;
  Rt = 90;
  Lt = -90;
  Cs = 32;

  //convert to servo position angles
  Bs = servoAngle('B', Bt);
  Rs = servoAngle('R', Rt);
  Ls = servoAngle('L', Lt);
//  Cs = servoAngle('C', Ct);
  
  //define servo angle limits
  //Left limits are variable and are define in the main for loop

  //Base Servo
  limMinBs = 10; //min limit
  limMaxBs = 160; //max limit

  //Right Servo
  limMinRs = 35; //min limit
  limMaxRs = 180; //max limit

  //Claw Servo
  limMinCs = 0; //min limit
  limMaxCs = 40; //max limit

  //define theta limits
  //base angle limits
  limMinBt = servoTheta('B', limMinBs);
  limMaxBt = servoTheta('B', limMaxBs);

  //Right theta limits
  limMinRt = servoTheta('R', limMaxRs);
  limMaxRt = servoTheta('R', limMinRs);

  serL.write(Ls);
  delay(500);

  serR.write(Rs);
  delay(500);

  serB.write(Bs);
  delay(500);

  serC.write(Cs);
  delay(500);
}



void loop() {

 
  //prompt user to choose what they want to perform
  Serial.println("Specify fk, ik, current angles, or current location");
  Serial.println("0x00 = fk, 0x01 = ik, 0x02 = Current Angle, 0x03 = Current Location");

  //pauses code until user input is detected
  while (Serial.available() == 0) {
    //Wait for user input
  }

  
  //DOSENT Get and store user input into a string
  //DOES take the value of incomingByte 
//  input = GetBLE();
  input = getString();
  //basically replacing GetString with GetBLE
  //go to fk, ik, or current depending on the input



  
  if (input.length() > 0) {
    //perform fk
    if (strcmp(input.c_str(), "0") == 0) {
      Serial.println("Performing Forward Kinematics");
      Serial.println("Which servo do you want to move?");
      Serial.println("Base is 0x00, Right is 0x01, Left is 0x02, Claw is 0x03");

      //pauses code until user input is detected
      while (Serial.available() == 0) {
        //Wait for user input
      }

      //Get and store user input into the string stringSerX
//      stringSerX = GetBLE();
      stringSerX = getString();

      //convert "servoX" string into "serX" character so it can be used in a switch statement
      //left-L, right-R, base-B, claw-C
      if (stringSerX.length() > 0) {
        Serial.println("you selected: " + stringSerX);

        if (strcmp(stringSerX.c_str(), "2") == 0) {
          charSerX = 'L'; //asign the appropriate character
          stringSerX = ""; //empty for next input
        }
        else if (strcmp(stringSerX.c_str(), "1") == 0) {
          charSerX = 'R'; //asign the appropriate character
          stringSerX = ""; //empty for next input
        }
        else if (strcmp(stringSerX.c_str(), "0") == 0) {
          charSerX = 'B'; //asign the appropriate character
          stringSerX = ""; //empty for next input
        }
        else if (strcmp(stringSerX.c_str(), "3") == 0) {
          Serial.println("type a value between 0-40 for the angle. 0 is closed, 40 is open");
          charSerX = 'C'; //asign the appropriate character
          stringSerX = ""; //empty for next input
        }
        // If the user types an invalid statement, an error message will appear and the code will loop back to the top
        else {
          Serial.println("servo name was typed incorrectly");
          Serial.println();
          charSerX = 'X'; //char x prevents the code from continuing
          stringSerX = ""; //empty for next input
        }

        //Move the selected servo
        if (charSerX != 'X') {
          Serial.println();
          Serial.println("How many degrees of rotation?");
          stringTheta = ""; //empty for next input

          //Pauses code until user has put in an input
          while (Serial.available() == 0) {
            //Wait for user input
          }

//          stringTheta = GetBLE();   //Get and store user defined angle as a string
          stringTheta = getString();
          Serial.print("you typed in: ");
          theta = stringTheta.toInt();  //convert user defined angle string into an int
          Serial.println(theta);

          //Switch to the appropriate case depending on the user chosen servo
          switch (charSerX) {
            //Left servo
            case 'L':
              Lt = theta; //stores user input into the corisponding servo theta variable
              nLs = servoAngle('L', Lt); // converts theta to servo angle
              Llimits(Rs);//calculate L servo limits based on R servo position
              moveServo(serL, limMinLs, limMaxLs, nLs, Ls); // calls this fuction to move the servo
              Ls = nLs; //stores the current angle of the servo
              break;
            //Right servo
            case 'R':
              Rt = theta; //stores user input into the corisponding servo theta variable
              pRs = Rs;
              nRs = servoAngle('R', Rt); // converts theta to servo angle
              moveServo(serR, limMinRs, limMaxRs, nRs, Rs); // calls this fuction to move the servo
              Rs = nRs; //stores the current angle of the servo

              //move left servo
              nLs = -Rs + Ls + pRs; //Special formula used to move left servo when right servo is moved
              Lt = servoTheta('L', nLs); //converts servo angle to theta and stores this value
              Llimits(Rs);//calculate L servo limits based on R servo position
              moveServo(serL, limMinLs, limMaxLs, nLs, Ls); // calls this fuction to move the servo
              Ls = nLs; //stores the current angle of the servo
              break;
            //base servo
            case 'B':
              Bs = theta;
              nBs = servoAngle('B', Bs);
              moveServo(serB, limMinBs, limMaxBs, nBs, Bs); // calls this fuction to move the servo
              Bs = nBs; //stores the current angle of the servo
              break;
            //Claw servo
            case 'C':
              nCs = theta;
              //nCs = servoAngle('C', Cs);
              moveServo(serC, limMinCs, limMaxCs, nCs, Cs); // calls this fuction to move the servo
              Cs = nCs; //stores the current angle of the servo
              break;
          }
        }
      }
      input = ""; //empty for next input
    }

    //performs inverse kinematics
    if (strcmp(input.c_str(), "1") == 0) {
      int d; //used as acounter to determine which path to choose
      int e; //used as acounter to determine which path to choose
      Serial.println("Performing Inverse Kinematics");

      //promt for x,y,z cordinates and store them
      Serial.println("Type in the x, y, and z coordinates in mm");

      //promt for x
      Serial.print("x: ");

      //pauses code until user input is detected
      String GetB = "";
      String CorrectBLE="";
      int i = 1;
      while (i != 0) {
        //Wait for user input
//        GetB = GetBLE();
        GetB = getString();
        if(GetB.length()){i=0;CorrectBLE=GetB;}
      }

      //Get and store user input into the string cord
      cord = CorrectBLE;
//      Serial.print(cord);//EDIT==============================================================
      //convert string cordinate to an int
      x = cord.toInt();
      Serial.println(x);
      cord = ""; //clear cord string

      //promt for y
      Serial.print("y: ");

      //pauses code until user input is detected
      GetB = "";
      i = 1;
      while (i != 0) {
        //Wait for user input
//        GetB = GetBLE();
        GetB = getString();
        if(GetB.length()){i=0;CorrectBLE=GetB;}
      }
      
      //Get and store user input into the string cord
      cord = CorrectBLE;
      //convert string cordinate to an int
      y = cord.toInt();
      Serial.println(y);
      cord = ""; //clear cord string

      //promt for z
      Serial.print("z: ");

      //pauses code until user input is detected
      GetB = "";
      i = 1;
      while (i != 0) {
        //Wait for user input
//        GetB = GetBLE();
        GetB = getString();
        if(GetB.length()>0){i=0;CorrectBLE=GetB;}
      }
      
      //Get and store user input into the string cord
      cord = CorrectBLE;
      //convert string cordinate to an int
      z = cord.toInt();
      Serial.println(z);
      cord = ""; //clear cord string

      //integers to simplify ik calcualtions not linked to any servos
      xp = sqrt(pow((y + 48), 2) + pow(x, 2)) - 53;
      zp = z - 55;
      C  = (320 + pow(xp, 2) + pow(zp, 2)) / 162;

      //Calculate theta values for B, R, and L servos
      Bt = (atan(x / (y + 48))*-1)*(180/PI);
      Serial.print("Bt = ");
      Serial.println(Bt);
      Rt1 = 2 * atan((zp + sqrt(pow(zp, 2) - pow(C, 2) + pow(xp, 2))) / (xp + C))*(180/PI);
      Rt2 = 2 * atan((zp - sqrt(pow(zp, 2) - pow(C, 2) + pow(xp, 2))) / (xp + C))*(180/PI);
      Lt1 = (asin((zp - 81 * sin(Rt1*PI/180)) / 79)*(180/PI)) - Rt1;
      Lt2 = (asin((zp - 81 * sin(Rt2*PI/180)) / 79)*(180/PI)) - Rt2;


      //Compare thetaR values to their limits and determine which one will physically work. If both work default to thetaR1
      //evaluates if theatR1 will work
      if (Rt1 > limMinRt && Rt1 < limMaxRt) {
        d = 1;
        Serial.print("Rt1 = ");
        Serial.println(Rt1);
        //Compare thetaL values to their limits and determine which one will physically work. If both work default to thetaL1
        Llimits(servoAngle('R', Rt1));//calculate L servo limits based on R servo position
      }
      //evaluates if theatR2 will work
      else if (Rt2 > limMinRt && Rt2 < limMaxRt) {
        d = 2;
        Serial.print("Rt2 = ");
        Serial.println(Rt2);
        //Compare thetaL values to their limits and determine which one will physically work. If both work default to thetaL1
        Llimits(servoAngle('R',Rt2));//calculate L servo limits based on R servo position
      }
      //error message if none work
      else {
        Serial.println("your cordinates are out of range");
        Serial.print(limMinRt);
        Serial.print(" < R theta range < ");
        Serial.println(limMaxRt);
        Serial.print("thetaR calculated values: ");
        Serial.print(Rt1);
        Serial.print(" and ");
        Serial.println(Rt2);
        d = 0;
      }

      //evaluates if theatL1 will work
      if (Lt1 > limMinLt && Lt1 < limMaxLt) {
        e = 1;
        Serial.print("Lt1 = ");
        Serial.println(Lt1);
      }
      //evaluates if theatL2 will work
      else if (Lt2 > limMinLt && Lt2 < limMaxLt) {
        e = 2;
        Serial.print("Lt2 = ");
        Serial.println(Lt2);
      }
      //error message if none work
      else {
        Serial.println("your cordinates are out of range");
        Serial.print(limMinLt);
        Serial.print(" < L theta range < ");
        Serial.println(limMaxLt);
        Serial.print("thetaL calculated values: ");
        Serial.print(Lt1);
        Serial.print(" and ");
        Serial.println(Lt2);
        e = 0;
      }
      //Determine the best path to take and move the servos
      if (d == 1 && e == 1) {
        Serial.println("path option 1 will be taken");
        //base servo
        nBs = servoAngle('B', Bt); //convert theta to servo angles
        moveServo(serB, limMinBs, limMaxBs, nBs, Bs); // move servo
        Bs = nBs; //store current position
        //Right servo
        nRs = servoAngle('R', Rt1); //convert theta to servo angles
        moveServo(serR, limMinRs, limMaxRs, nRs, Rs); // move servo
        Rs = nRs; //store current position
        //Left servo
        Llimits(servoAngle('R',Rt1));//calculate L servo limits based on R servo position
        nLs = servoAngle('L', Lt1); //convert theta to servo angles
        moveServo(serL, limMinLs, limMaxLs, nLs, Ls); // move servo
        Ls = nLs; //store current position
        
      }
      else if (d == 2 && e == 2) {
        Serial.println("path option 2 will be taken");
        //base servo
        nBs = servoAngle('B', Bt); //convert theta to servo angles
        moveServo(serB, limMinBs, limMaxBs, nBs, Bs); // move servo
        Bs = nBs; //store current position
        //Right servo
        nRs = servoAngle('R', Rt2); //convert theta to servo angles
        moveServo(serR, limMinRs, limMaxRs, nRs, Rs); // move servo
        Rs = nRs; //store current position
        //Left servo
        Llimits(servoAngle('R',Rt2));//calculate L servo limits based on R servo position
        nLs = servoAngle('L', Lt2); //convert theta to servo angles
        moveServo(serL, limMinLs, limMaxLs, nLs, Ls); // move servo
        Ls = nLs; //store current position
       
      }
      else {
        Serial.println("the cordinates are out of range, please try again");
      }
      
      moveServo(serC,limMinCs,limMaxCs,nCs,Cs);


      input = ""; //empty for next input
    }

    //Shows current angle of servos and arms
    if (strcmp(input.c_str(), "2") == 0) {
      Serial.println("showing current angles");
      Serial.print("Left theta = ");
      Serial.println(Lt);
      Serial.print("Left servo angle = ");
      Serial.println(Ls);
      Serial.print("Right theta = ");
      Serial.println(Rt);
      Serial.print("Right servo angle = ");
      Serial.println(Rs);
      Serial.print("Base theta = ");
      Serial.println(Bt);
      Serial.print("Base servo angle = ");
      Serial.println(Bs);
      Serial.print("Claw theta = ");
      Serial.println(Ct);
      Serial.print("Claw servo angle = ");
      Serial.println(Cs);
      Serial.println();
      delay(100);
      input = ""; //empty for next input
    }

    //shows current location of end effector
    if (strcmp(input.c_str(), "3") == 0) {
      Serial.println("showing current location");
      int x = -sin(Bt)*(79*cos(Lt+Rt) + 81*cos(Rt) + 53);
      int y = cos(Bt)*(79*cos(Lt+Rt) + 81*cos(Rt) + 53) - 48;
      int z = 79*sin(Lt+Rt) + 81*sin(Rt) + 55;
      Serial.print("X position = ");
      Serial.println(x);
      Serial.print("Y position = ");
      Serial.println(y);
      Serial.print("Z position = ");
      Serial.println(z);
      
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

//Function is to be called everytime posR value is changed to adjust L servo limits
//R is interms of servo angles (degrees)
void Llimits(int R) {
  R = servoTheta('R', R);
  //define left servo theta limits depending on right servo position
  //if theta R < 57
  if (servoTheta('R', R) < 57) {
    limMinLt = -88 - R; //min limit
    limMaxLt = -20; //max limit
  }
  //if theta R > 57
  else {
    limMinLt = -145; //min limit
    limMaxLt = 37 - R; //max limit
  }
  //set left theta limits
  limMinLs = servoAngle('L', limMinLt);
  limMaxLs = servoAngle('L', limMaxLt);
}

//function converts actual angles on arm (theta) to servo angles
//servoAngle(desired servo, theta)
int servoAngle(char s, int th) {
  int a;
  switch (s) {
    case 'B':
      a = th + 88;
      break;
    case 'R':
      a = 184 - th;
      break;
    case 'L':
      a = 299 - Rs + th;
      break;
    //case 'C':
      //a = th * 4;
      //break;
  }
  return a;
}

//function converts servo angles to theta
//servoTheta(desired servo, servo angle)
int servoTheta(char se, int sa) {
  int t;
  switch (se) {
    case 'B':
      t = sa - 88;
      break;
    case 'R':
      t = 184 - sa;
      break;
    case 'L':
      t = -299 + sa + Rs;
      break;
     //case 'C':
      //t = sa / 4;
      //break;
  }
  return t;
}
