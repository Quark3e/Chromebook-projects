
int a = 13; //Arduino pins connected with 7 segment pins
int b = 12;
int c = 11;
int d = 10;
int e = 9;
int f = 8;
int g = 7;

                 //A   B   C   D   E  F  G
int seg_pins[7] = {13, 12, 11, 10, 9, 8, 7};

int numbers[10][7] = {
  {1,1,1,1,1,1,0},
  {0,1,1,0,0,0,0},
  {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}
};

void setup() {
  //Declaring all the pins as output
  for(int i=0; i<7; i++) {
    pinMode(seg_pins[i], OUTPUT);
    digitalWrite(seg_pins[i], LOW);
  }
}

// the loop routine runs over and over again forever:
void loop() {
  for(int i=0; i<10; i++) {
    for(int n=0; n<7; n++) {
      digitalWrite(seg_pins[n], numbers[i][n]);
    }
    delay(500);
  }
}
