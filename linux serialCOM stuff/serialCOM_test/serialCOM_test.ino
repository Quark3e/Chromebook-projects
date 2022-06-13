

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
        
        //inp = Serial.readStringUntil('\n');
        
        //if(inp.toInt() == 69) {
        //  Serial.println("nice");
        //  delay(2000);
        //}
        //else {
        //  Serial.println("not 69");
        //}
        input=Serial.read();
        if(input==',') {
          num=calc();
          j=-1;
          Serial.println(num, DEC);
          if(num = 69) {
            digitalWrite(LED, HIGH);
            delay(1000);
            digitalWrite(LED, LOW);
            delay(1000);
          }
        }
        else {
          j++;
          buff[j]=input;
        }
        //while(Serial.available()>0) {
        //  Serial.read();
        //}
    }
}
