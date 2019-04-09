#include <Keyboard.h>
#include <Wire.h>
#define button 2

int lastButtonState;
int buttonState;
int seqState;
int lastSeqState;

int waitLen = 10;

char tabKey = KEY_TAB;
char entKey = KEY_RETURN;



char abuffer[99];
boolean receiveFlag = false;
String userbuffer[5];

char aUsername[99];
char aPassword[99];

char oldUser[99];
char oldPass[99];

int i = 0;
int a = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  startReceive();

  seqState = 0;         // Sequence starts on the OFF position
  lastButtonState = 0;  // Initialize to NOT pressed

  pinMode(4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  /*if (a == 2) {
    digitalWrite(4, HIGH);
  }

  lastSeqState = seqState;

  buttonInput();*/


}

void startReceive() {
  Serial.println("Receiving:");
  Wire.begin(1); // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent);
}

void receiveEvent(int howMany)
{
  memset(abuffer, 0, sizeof(abuffer));

  while (Wire.available())
  {
    Wire.readBytes(abuffer, howMany);
    receiveFlag = true;
  }

  userbuffer[i] = abuffer;

  Serial.println(userbuffer[i]);

  i++;
  a++;

  if (a == 2) {
    strcpy(aUsername, userbuffer[0].c_str());
    strcpy(aPassword, userbuffer[1].c_str());

    Serial.print("Received username: ");
    Serial.println(aUsername);
    Serial.println(strlen(aUsername));
    Serial.print("Received password: ");
    Serial.println(aPassword);

    Serial.println(strlen(aUsername));
    Serial.println(strlen(aPassword));
  delay(1000);
  
  if((strcmp(aUsername, oldUser) !=0) || (strcmp(aPassword, oldPass) !=0)){
    Serial.println("Pasting Credential[s]");
for(int k = 0; k<=strlen(aUsername); k++)
{
  oldUser[k] = aUsername[k];
}
for(int k = 0; k<=sizeof(aPassword); k++)
{
  oldPass[k] = aPassword[k];
}
  kbOut();

    memset(aUsername, 0, sizeof(aUsername));
    memset(aPassword, 0, sizeof(aPassword));
    memset(oldUser, 0, sizeof(oldUser));
    memset(oldPass, 0, sizeof(oldPass));
    a = 0;
    i=0;
  }
}
}

/*void buttonInput()
{
  long int buttonMillis = millis();
  long int buttonWait = 1;

  while ((millis() - buttonMillis) <= buttonWait)
  {
    buttonState = digitalRead(button);

    if ((buttonState == HIGH) && (lastButtonState == LOW)) // Check to see if button is down
    {
      seqState = !seqState; // Update sequence running state
    }

    lastButtonState = buttonState;                         // remember last buttonState
  }
}*/

void kbOut() {
  /*if (seqState == !lastSeqState)
  {*/
    if (strlen(aUsername) <= 2) {
      Keyboard.begin();

      Keyboard.print(aPassword);

      Keyboard.press(entKey);
      Keyboard.release(entKey);

      Keyboard.releaseAll();

      Keyboard.end();
    }

    else {
      Keyboard.begin();

      Keyboard.print(aUsername);

      Keyboard.press(tabKey);
      Keyboard.release(tabKey);

      Keyboard.print(aPassword);

      Keyboard.press(entKey);
      Keyboard.release(entKey);

      Keyboard.releaseAll();

      Keyboard.end();
    }
 // }
}
