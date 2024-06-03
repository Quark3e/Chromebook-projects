
float V_src = 5;
float V_read = 0;

float R1 = 113, R2 = 33.12;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL);
}

void loop() {
  while (!Serial.available());
  V_read = (1.1 * analogRead(A5) * (R1+R2)) / (1024 * R2);
  Serial.println(V_read);
}
