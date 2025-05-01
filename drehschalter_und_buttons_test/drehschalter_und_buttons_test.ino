//drehschalter
int DT = 6;
int CLK = 5;
int SW = 7;
int Position = 0;
int Letzte_Position = LOW;
int n = LOW;
int Taster = LOW;
int Letzte_Taster = LOW;

//4 buttons
int b_oben_rechts = 8;
int b_oben_rechts_old_state = LOW;
int b_unten_rechts = 9;
int b_unten_rechts_old_state = LOW;
int b_links_oben = 10;
int b_links_oben_old_state = LOW;
int b_links_unten = 11;
int b_links_unten_old_state = LOW;
void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(b_oben_rechts, INPUT);
  pinMode(b_oben_rechts, INPUT);
  pinMode(b_oben_rechts, INPUT);
  pinMode(b_oben_rechts, INPUT);
  //Drehschalter
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(115200);

  //set first values :)

  b_oben_rechts_old_state = digitalRead(b_oben_rechts);
  b_unten_rechts_old_state =digitalRead(b_unten_rechts);

  b_links_oben_old_state = digitalRead(b_links_oben);
  b_links_unten_old_state =digitalRead(b_links_unten);
}
void loop() {
  n = digitalRead(CLK);
  Taster = !digitalRead(SW);
  if (Taster != Letzte_Taster) {
    Serial.print(Position);
    Serial.print("|");
    Serial.println(Taster);
    delay(10);
    Letzte_Taster = Taster;
  }
  if ((Letzte_Position == LOW) && (n == HIGH)) {
    if (digitalRead(DT) == LOW) {
      Position++;
    }
    else {
      Position--;
    }
    Serial.print(Position);
    Serial.print("|");
    Serial.println(Taster);
  }
  Letzte_Position = n;

  /* finally check if a button changed
   *  
   */
   if(b_oben_rechts_old_state != digitalRead(b_oben_rechts))
   {
      b_oben_rechts_old_state = digitalRead(b_oben_rechts);
    Serial.print("b_oben_rechts|");
    Serial.println(b_oben_rechts_old_state);
   }

      if(b_unten_rechts_old_state != digitalRead(b_unten_rechts))
   {
      b_unten_rechts_old_state = digitalRead(b_unten_rechts);
    Serial.print("b_unten_rechts|");
    Serial.println(b_unten_rechts_old_state);
   }

         if(b_links_oben_old_state != digitalRead(b_links_oben))
   {
      b_links_oben_old_state = digitalRead(b_links_oben);
    Serial.print("b_links_oben|");
    Serial.println(b_links_oben_old_state);
   }

            if(b_links_unten_old_state != digitalRead(b_links_unten))
   {
      b_links_unten_old_state = digitalRead(b_links_unten);
    Serial.print("b_links_unten|");
    Serial.println(b_links_unten_old_state);
   }
   
   }
