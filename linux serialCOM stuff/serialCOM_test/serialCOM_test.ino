

int LED = 12;

 
int input,num;
String inp;
int buff[5];
int j=-1;

void setup()
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
}
 
int calc()
{
    int num=0,x=0;
 
    for(x;x<=j;x++)
          num=num+(buff[x]-48)*pow(10,j-x);
 
    return num;
}

void loop()
{
    if(Serial.available()>0) {
        Serial.println("starting...");
        delay(3000);
        //input=Serial.read();
        inp = Serial.readStringUntil('\n');
        
        if(inp.toInt() == 69) {
          Serial.println("nice");
          delay(2000);
        }
        else {
          Serial.println("not 69");
        }
        //if(input==',') {
        //  num=calc();
        //  j=-1;
        //  Serial.println(num);
        //  digitalWrite(LED, HIGH);
        //  delay(1000);
        //  digitalWrite(LED, LOW);
        //}
        //else {
        //  j++;
        //  buff[j]=input;
        //}
        while(Serial.available()>0) {
          Serial.read();
        }
    }
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
}
