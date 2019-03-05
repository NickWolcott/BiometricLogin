//@@ -0,0 +1,63 @@
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
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
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
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

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  printCurrentNet();
  printWiFiData();
  
  sendWOLMP(g_TargetMacAddress);
}

void loop() {
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
