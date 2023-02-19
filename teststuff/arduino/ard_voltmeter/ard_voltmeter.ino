
float V_src = 5;
float V_read = 0;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
}

void loop() {
  //Serial.print(analogRead(A5));
  V_read = V_src * analogRead(A5) / 1024;
  Serial.print(0);
  Serial.print(" ");
  Serial.print(5);
  Serial.print(" ");
  Serial.println(V_read);
  delay(10);
}
