
/*Communicates to a 7 segment display through an 74hc595 shift register
    B 0  0  0  0  0  0  0  0
      a  b  c  d  e  f  g  [not used]
      P7 P6 P5 P4 P3 P2 P1 P0[not used]
*/

int clockPin = 7;
int dataPin = 6;
int latchPin = 5;

int frameDelay = 500;
int iterDelay = 20; //specifically for mode 1
int mainDelay = 0;

int mode = 1; /*
mode = 0: writes to all 8 Parallel-Out (PO) pins at once
mode = 1: writes only to a single PO pin at once, iteration through the bits of the given byte

*/

int totIterTime = 0;

byte numbers[10]= {
  B11111100,
  B01100000,
  B11011010,
  B11110010,
  B01100110,
  B10110110,
  B10111110,
  B11100000,
  B11111110,
  B11110110
};


void setup() {
    Serial.begin(9600);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
}

void loop() {
    for(int i=0; i<10; i++) {
        if(mode==0) {
            digitalWrite(latchPin, LOW);
            shiftOut(dataPin, clockPin, MSBFIRST, numbers[i]);
            digitalWrite(latchPin, HIGH);
            delay(frameDelay);
        }
        else if(mode==1) {
            byte tempRow = B00000000;
            
            while(true) {
                for(int n=7; n>0; n--) {
                    if(bitRead(numbers[i], n)) tempRow = 1 << n;
                    else tempRow = 0;
                    digitalWrite(latchPin, LOW);
                    shiftOut(dataPin, clockPin, MSBFIRST, tempRow);
                    digitalWrite(latchPin, HIGH);
                    delay(iterDelay);
                    totIterTime+=iterDelay;
                }
                if(totIterTime>=frameDelay) {
                    totIterTime=0;
                    break;
                }
            }
        }
        delay(mainDelay);
        Serial.println(i);
    }
}
