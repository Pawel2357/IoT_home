String a; //Initialized variable to store recieved data
const int pin = 10;

void setup() {
  //Serial Begin at 9600 Baud
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pin, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    a = Serial.readString();

    // TODO: iterate over elements in this string or look at nodemcu

    // say what you got:
    if (a == "1"){
      digitalWrite(pin, LOW);
      //Serial.print(a);
    }if (a == "0"){
      digitalWrite(pin, HIGH);
    }
  }
}
