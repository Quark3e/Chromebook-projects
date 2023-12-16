
float V_src = 5;
float V_read = 0;

float R2 = 100000;
float R1 = 0;

int readSign = 0;

int analPin = A3;

void setup() {
    Serial.begin(9600);
    //analogReference(INTERNAL);
}

void loop() {
    readSign = analogRead(analPin);
    V_read = (V_src*(readSign/float(1023)));
    R1 = (R2*V_src)/V_read-R2;

    Serial.print(readSign);
    Serial.print(": ");
    Serial.print(V_read);
    Serial.print(": ");
    Serial.print(R1);
    Serial.println("");
    delay(1000);
}