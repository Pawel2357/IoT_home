String a; //Initialized variable to store recieved data
const int relay_pin = 13;

void setup() {
  //Serial Begin at 9600 Baud 
  pinMode(relay_pin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(relay_pin, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    a = Serial.readString();

    // TODO: iterate over elements in this string or look at nodemcu

    // say what you got:
    if (a == "0"){
      digitalWrite(relay_pin, LOW);
      //Serial.print(a); 
    }if (a == "1"){
      digitalWrite(relay_pin, HIGH);
    }
  }
}
