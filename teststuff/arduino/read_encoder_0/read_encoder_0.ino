// Arduino Rotary Encoder from Mouse Scroll Wheel
// and SparkFun 4 Digit Serial 7-Segment Display
// by Steve Kamerman 12/18/2010
// https://www.stevekamerman.com/2010/12/understanding-a-mouse-scroll-wheel/

// These MUST be on interrupt pins!
//#define encoderPinA 2
//#define encoderPinB 3

//#define displayTXPin 10
#define outputA 2
#define outputB 3

int counter = 0;
int aState;
int aLastState;

void setup() {
  pinMode (outputA, INPUT);
  pinMode (outputB, INPUT);

  Serial.begin (9600);
  // Reads the initial state of the outputA
  aLastState = digitalRead(outputA);
}

void loop() {
  aState = digitalRead(outputA); // Reads the "current" state of the outputA
  // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState) {
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(outputB) != aState) {
      counter ++;
    } else {
      counter --;
    }
    Serial.print("Position: ");
    Serial.println(counter);
  }
  aLastState = aState; // Updates the previous state of the outputA with the current state
}
