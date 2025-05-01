int DT;
int CLK;
int SW;
int Position;
int Letzte_Position;
int n;
int Taster;
int Letzte_Taster;

int b_oben_rechts = 8;

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(b_oben_rechts, INPUT);
  
DT = 6;
CLK = 5;
SW = 7;
Position = 0;
Letzte_Position = LOW;
n = LOW;
Taster = LOW;
Letzte_Taster = LOW;
pinMode (CLK, INPUT_PULLUP);
pinMode (DT, INPUT_PULLUP);
pinMode (SW, INPUT_PULLUP);
Serial.begin (115200);
}
void loop() {
n = digitalRead(CLK);
Taster = !digitalRead(SW);
if (Taster != Letzte_Taster){
Serial.print (Position);
Serial.print ("|");
Serial.println (Taster);
delay(10);
Letzte_Taster = Taster;
}
if ((Letzte_Position == LOW) && (n == HIGH)) {
if (digitalRead(DT) == LOW) {
Position++;
} else {
Position--;
}
Serial.print (Position);
Serial.print ("|");
Serial.println (Taster);
}
Letzte_Position = n;
}
