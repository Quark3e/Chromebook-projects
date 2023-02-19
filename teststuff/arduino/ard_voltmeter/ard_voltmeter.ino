
int V_src = 5.1;
int V_read = 0;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
}

void loop() {
  V_read = V_src * analogRead(A5) / 1024;
  Serial.println(V_read);
  delay(10);
}
