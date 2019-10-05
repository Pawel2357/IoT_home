String a; //Initialized variable to store recieved data
const int pin_2 = 2;

void setup() {
  //Serial Begin at 9600 Baud
  pinMode(pin_2, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pin_2, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    a = Serial.readString();

    // TODO: iterate over elements in this string or look at nodemcu

    // say what you got:
    if (a == "1"){
      digitalWrite(pin_2, LOW);
      //Serial.print(a);
    }if (a == "0"){
      digitalWrite(pin_2, HIGH);
    }
  }
}
