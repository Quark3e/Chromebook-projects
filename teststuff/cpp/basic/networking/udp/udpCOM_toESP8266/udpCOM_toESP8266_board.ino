
//WORKS

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Telia-47118D";
const char* password = "0C94C28B5D";

WiFiUDP Udp;
unsigned int localUdpPort = 53; // local port to listen on
char incomingPacket[255]; // buffer for incoming packets
char replyPacket[] = "Hi there! Got the message :-)"; // a reply string to send back

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.printf("Connecting to %s ", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");
    Udp.begin(localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
    pinMode(0, INPUT_PULLUP);
}

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize) { // receive incoming UDP packets
        //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        int len = Udp.read(incomingPacket, 255);
        if (len > 0) { incomingPacket[len] = 0; }
        //Serial.printf("UDP packet contents: %s\n", incomingPacket); // send back a reply, to the IP address and port we got the packet from
        String resultStr = "pin: "+String(digitalRead(0));
        int newPackLen = resultStr.length()+1;
        char newReplyPack[newPackLen];
        resultStr.toCharArray(newReplyPack, newPackLen);
        Udp.beginPacket(Udp.remoteIP(),
        Udp.remotePort()); Udp.write(newReplyPack);
        Udp.endPacket();
        //Serial.print("Sent packet: \"");
        //Serial.print(newReplyPack);
        //Serial.println("\"");
    }
}