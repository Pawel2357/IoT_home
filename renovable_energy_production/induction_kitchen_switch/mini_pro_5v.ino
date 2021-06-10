int analogPin_0 = A0; // potentiometer wiper (middle terminal) connected to analog pin 3
int analogPin_1 = A1; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V
int val0;  // variable to store the value read
int val1;  // variable to store the value read

void setup() {
  Serial.begin(9600);           //  setup serial
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(10, LOW); // sets the digital pin 13 on
  digitalWrite(11, LOW); // sets the digital pin 13 on
}

void loop() {
  val0 = analogRead(analogPin_0);  // read the input pin
  val1 = analogRead(analogPin_1);  // read the input pin
  Serial.println(val0);
  if(val0 > 500){
    digitalWrite(10, HIGH); // sets the digital pin 13 on
    Serial.println("high");
  }else{
    digitalWrite(10, LOW); // sets the digital pin 13 on
    Serial.println("low");
  }

  if(val1 > 500){
    digitalWrite(11, HIGH); // sets the digital pin 13 on
  }else{
    digitalWrite(11, LOW); // sets the digital pin 13 on
  }

  delay(1000);
}
