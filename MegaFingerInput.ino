#include <Wire.h>
#include <EEPROM.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

FPS_GT511C3 fps(10, 11); // (Arduino SS_RX = pin 10, Arduino SS_TX = pin 11)

bool toggleEnroll = false;

String userUser = "";
String userPass = "";
String wolStr = "";

char changeCred[1];
char addCred = "";

char abuffer[99];
boolean receiveFlag = false;
String userbuffer[5];

char aUsername[40];
char aPassword[40];
char aWol[10];
char testUsername[40];
char testPassword[40];
char testWol[10];

const int outPin = 4;

int credAdd = 0;

long int currmil = 0;

void setup() {


  pinMode(outPin, OUTPUT);


  Serial.begin(9600);

  delay(500);
  fps.Open();         //send serial command to initialize fps

  currmil = millis();

  startConfig(currmil);
}

void loop() {

  testPrintCred();
}


void serialInputUser() {
  Serial.println("Enroll");
  while (toggleEnroll == false) {
    testPrint();          //begin enrolling fingerprint
    delay(1000);
  }

  Serial.flush() ;
  Serial.readString();

  Serial.print("Please enter your Username: ");

  //while (userUser == "") {
  //if (Serial.available() > 0) { //recieves data only if it can be recieved

  userUser = Serial.readString(); //recieves user input
  strcpy(aUsername, userUser.c_str());
  Serial.println(userUser);
  delay(500);
  Serial.flush() ;
  //}
  // }

  Serial.print("Please enter your Password: ");

  //  while (userPass == "") {
  // if (Serial.available() > 0) { //recieves data only if it can be recieved

  userPass = Serial.readString(); //recieves user input
  strcpy(aPassword, userPass.c_str());
  Serial.println(userPass);
  delay(500);
  Serial.flush() ;
  // }
  // }

  //while (wolStr == "") {
  // if (Serial.available() > 0) { //recieves data only if it can be recieved

  Serial.print("Is this a windows login credetial (Y/N)? ");
  wolStr = Serial.readString(); //recieves user input
  strcpy(aWol, wolStr.c_str());
  Serial.println(wolStr);
  delay(500);
  Serial.flush() ;
  //}
  //}

  Serial.println(credAdd);
  Serial.println(sizeof(aUsername));
  EEPROM.put(credAdd, aUsername);
  credAdd += (sizeof(aUsername) + 1);

  Serial.println(credAdd);
  Serial.println(sizeof(aPassword));
  EEPROM.put(credAdd, aPassword);
  credAdd += (sizeof(aPassword) + 1);

  Serial.println(credAdd);
  EEPROM.put(credAdd, aWol);

  Serial.print("Your username is: ");
  Serial.println(aUsername);
  Serial.print("Your password is: ");
  Serial.println(aPassword);
  Serial.print("Is a windows credential: ");
  Serial.println(aWol);

  currmil = millis();

  startConfig(currmil);
}


void startConfig(long int currmil) {
  changeCred[0] = 'n';

  Serial.println(String(changeCred));
  userUser = "";
  userPass = "";

  while (millis() - currmil < 5000) {
    if (Serial) {


      Serial.print("Change credentials (Y/N)?");
      Serial.setTimeout(5000);

      Serial.readBytes(changeCred, 1);

      Serial.println(changeCred[0]);

      Serial.println(String(changeCred[0]));
      if (String(changeCred[0]).equalsIgnoreCase("Y") == true)
      {
        Serial.println("ChangeCred");
        toggleEnroll = false;

        // while (userPass == "")
        if ((userUser == "") || (userPass == ""))
        {
          Serial.println("Goto Enroll");
          serialInputUser();
        }
      }
      else {
        break;
      }
    }
  }
}


void Enroll()
{
  // Enroll test

  // find open enroll id
  fps.SetLED(true);   //turn on LED so fps can see fingerprint

  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid == true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  // enroll
  Serial.print("Press finger to Enroll #");
  Serial.println(enrollid);
  while (fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    Serial.println("Remove finger");
    fps.Enroll1();
    while (fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    while (fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      fps.Enroll2();
      while (fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      while (fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successful");
          fps.SetLED(false);   //turn on LED so fps can see fingerprint

          toggleEnroll = true;

          credAdd = 0;

          credAdd = (enrollid + 1) * 100;
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
        }
      }
      else Serial.println("Failed to capture third finger");
    }
    else Serial.println("Failed to capture second finger");
  }
  else Serial.println("Failed to capture first finger");
}


void startSend() {
  Serial.println("Sending credentials");
  Wire.begin(); // join i2c bus (address optional for master)
  sendEvent();
}

void sendEvent() {

  if(String(testWol[0]).equalsIgnoreCase("Y") == true ){
  digitalWrite(outPin, HIGH);
  }
  else{
    digitalWrite(outPin, LOW);
  }

  Wire.beginTransmission(1);
  Serial.print("Sending username: ");
  Serial.println(testUsername);
  Wire.write(testUsername);
  Wire.endTransmission();

  Wire.beginTransmission(1);
  Serial.print("Sending password: ");
  Serial.println(testPassword);
  Wire.write(testPassword);
  Wire.endTransmission();

}

void getStoredCred() {

  Serial.println(credAdd);
  EEPROM.get(credAdd, testUsername);
  Serial.print("Retreived username: ");
  Serial.println(testUsername);
  credAdd += (sizeof(testUsername) + 1);
  Serial.println(sizeof(testUsername));
  Serial.println(credAdd);
  EEPROM.get(credAdd, testPassword);
  Serial.print("Retreived password: ");
  Serial.println(testPassword);

  credAdd += (sizeof(testPassword) + 1);
  Serial.println(sizeof(testPassword));
  Serial.println(credAdd);
  EEPROM.get(credAdd, testWol);
  Serial.print("Is a Windows logon: ");
  Serial.println(testWol);
}

void testPrint()
{
  fps.SetLED(true);   //turn on LED so fps can see fingerprint

  // Identify fingerprint test
  if (fps.IsPressFinger())
  {
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();

    /*Note:  GT-521F52 can hold 3000 fingerprint templates
             GT-521F32 can hold 200 fingerprint templates
              GT-511C3 can hold 200 fingerprint templates.
             GT-511C1R can hold 20 fingerprint templates.
      Make sure to change the id depending on what
      model you are using */
    if (id < 200) //<- change id value depending model you are using
    { //if the fingerprint matches, provide the matching template ID
      fps.SetLED(false);   //turn on LED so fps can see fingerprint

      Serial.print("Verified ID:");
      Serial.println(id);
      credAdd = 0;
      toggleEnroll = true;
      credAdd = (id) * 100;

    }
    else
    { //if unable to recognize
      Serial.println("Finger not found");

      Enroll();
    }
  }
  else
  {
    Serial.println("Please press finger");
  }
  delay(100);
}


void testPrintCred()
{
  fps.SetLED(true);   //turn on LED so fps can see fingerprint

  // Identify fingerprint test
  if (fps.IsPressFinger())
  {
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();

    /*Note:  GT-521F52 can hold 3000 fingerprint templates
             GT-521F32 can hold 200 fingerprint templates
              GT-511C3 can hold 200 fingerprint templates.
             GT-511C1R can hold 20 fingerprint templates.
      Make sure to change the id depending on what
      model you are using */
    if (id < 200) //<- change id value depending model you are using
    { //if the fingerprint matches, provide the matching template ID
      fps.SetLED(false);   //turn on LED so fps can see fingerprint

      Serial.print("Verified ID:");
      Serial.println(id);

      credAdd = 0;

      credAdd = (id) * 100;

      getStoredCred();

      startSend();
    }
    /*else
      { //if unable to recognize
      Serial.println("Finger not found");
      }
      }
      /*else
      {
      Serial.println("Please press finger");
      }*/
    delay(100);
  }
}
