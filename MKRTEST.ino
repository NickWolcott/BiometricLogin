#include <Keyboard.h>
#include <Wire.h>
#define button 2

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include "arduino_secrets.h"

int lastButtonState;
int buttonState;
int seqState;
int lastSeqState;

int waitLen = 10;

char tabKey = KEY_TAB;
char entKey = KEY_RETURN;
char bckKey = KEY_BACKSPACE;



char abuffer[99];
boolean receiveFlag = false;
String userbuffer[5];

char aUsername[99];
char aPassword[99];

char oldUser[99];
char oldPass[99];

int i = 0;
int a = 0;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
byte g_TargetMacAddress[] = MAC_ADDRESS; // the destination PC to be woken up

const int nMagicPacketLength = 102;
byte abyMagicPacket[nMagicPacketLength] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
byte abyTargetIPAddress[] = {255, 255, 255, 255};
const int nWOLPort = 9;

int status = WL_IDLE_STATUS;     // the WiFi radio's status

WiFiUDP Udp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  startReceive();

  seqState = 0;         // Sequence starts on the OFF position
  lastButtonState = 0;  // Initialize to NOT pressed

  pinMode(4, OUTPUT);


  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }
  Serial.println("You're connected to the network");
  printCurrentNet();
  printWiFiData();
  
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
    sendWOLMP(g_TargetMacAddress);
delay(8000);
    if (strlen(aUsername) <= 2) {
      Keyboard.begin();
      Keyboard.print(bckKey);
     // Keyboard.press(bckKey);
     // Keyboard.release(bckKey);
      //Keyboard.press(bckKey);
      //Keyboard.release(bckKey);
      Keyboard.print(bckKey);
      delay(3000);
      Keyboard.print(bckKey);
      Keyboard.print(aPassword);
      delay(1000);
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

void sendWOLMP(byte * pMacAddress){
Serial.println("Sending Wake-On-Lan Magic Packet");

for (int ix = 6; ix < nMagicPacketLength; ix++){
  abyMagicPacket[ix] = pMacAddress[ix%6];
}

Udp.begin(nWOLPort);
if(Udp.beginPacket(abyTargetIPAddress, nWOLPort)){
  Udp.write(abyMagicPacket, nMagicPacketLength);
  Udp.endPacket();
}

Serial.println("Wake-On-Lan Magic Packet sent");
}

void printWiFiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address : ");
  Serial.println(ip);

  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP : ");
  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type: ");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
