String a; //Initialized variable to store recieved data
const int ledPin = 13;

void setup() {
  //Serial Begin at 9600 Baud 
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(ledPin, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    a = Serial.readString();

    // say what you got:
    if (a == "0"){
      digitalWrite(ledPin, LOW); 
    }if (a == "1"){
      digitalWrite(ledPin, HIGH);
    }
    Serial.write("bla bla");
  }
}