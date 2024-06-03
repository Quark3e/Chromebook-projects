/* PING -> ESP TO PY
 * Written by Junicchi
 * https://github.com/Kebablord 
 *
 * Reads the value of Ping Sensor and returns it to Python
 * This example is written for Nodemcu Cards 
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

// PORT
WiFiServer server(80);
WiFiClient client;
String rule;

void start(String ssid, String pass) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(),pass.c_str());

    Serial.println("");
    
    // Wait for connection
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Setting up mDNS responder
    if (!MDNS.begin("esp8266")) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {delay(1000);}
    }
    Serial.println("mDNS responder started");
    
    // Start TCP (HTTP) server
    server.begin();
    Serial.println("TCP server started");
    
    // Add service to MDNS-SD
    MDNS.addService("http", "udp", 80);
}

bool isReqCame = false;

void CheckNewReq() {
    client = server.available();
    if (!client) {return;}

    Serial.println("");
    Serial.println("NEW REQUEST");
    
    // Waiting client to connect
    while (client.connected() && !client.available()) {delay(1);}
    
    // Read the first line of HTTP request
    String req = client.readStringUntil('\r');
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1) {
        Serial.print("Invalid request: ");
        Serial.println(req);
        return;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print("Requested Path: ");
    Serial.println(req);

    rule = req;
    isReqCame = true;
    client.flush();
}
void waitUntilNewReq() {
    do {CheckNewReq();} while (!isReqCame);
    isReqCame = false;
}

void returnThisStr(String final_data) { 
    String s;
    //HTTP Protocol code.
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s += final_data; //Our final raw data to return
    client.print(s);
    Serial.println("Returned to client.");
}
void returnThisInt(int final_data) {
    returnThisStr(String(final_data));
}

String getPath() {return rule;}


#define TRIG_PIN 4
#define ECHO_PIN 5
#define INP_PIN 0
long duration;
int distance;

void setup(){
    Serial.begin(9600);
    start("Telia-47118D","0C94C28B5D"); // Wifi details connect to

    // pinMode(TRIG_PIN, OUTPUT);
    // pinMode(ECHO_PIN, INPUT);
    pinMode(0, INPUT_PULLUP);
}

void loop(){
    waitUntilNewReq();  //Waits until a new request from python come

    // digitalWrite(TRIG_PIN, LOW);
    // delayMicroseconds(2);
    // digitalWrite(TRIG_PIN, HIGH);
    // delayMicroseconds(10);
    // digitalWrite(TRIG_PIN, LOW);

    // // Read the returned wave
    // duration = pulseIn(ECHO_PIN, HIGH);
    // // Calculate the delay into cm
    // distance = duration*0.034/2;
    distance = digitalRead(0);

    returnThisInt(distance); //Returns the data to python
}
