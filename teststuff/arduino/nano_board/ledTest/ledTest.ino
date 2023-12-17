
//the nano board uses ATmega328P (Old Bootloader)

int led_pin = 3;

void setup() {
    Serial.begin(115200);
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, LOW);
}

void loop() {
    for(int i=0; i<=1023; i+=2) {
        analogWrite(led_pin, i);
        Serial.println(i);
        delay(100);
    }
    for(int i=1023; i>=0; i-=2) {
        analogWrite(led_pin, i);
        Serial.println(i);
        delay(100);
    }
}