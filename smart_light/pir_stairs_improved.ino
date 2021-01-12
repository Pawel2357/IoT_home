/*
 * PIR sensor tester
 */
 
int inputPin = D1;               // choose the input pin (for PIR sensor)
int lampPin = D2;
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
 
void setup() {
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(lampPin, OUTPUT);
  digitalWrite(lampPin, HIGH);
  Serial.begin(9600);
}
 
void loop(){
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(lampPin, LOW);
    Serial.println("Lamp turn on!");
    delay(1000 * 45);
    digitalWrite(lampPin, HIGH);
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH){
      digitalWrite(lampPin, HIGH);
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}
