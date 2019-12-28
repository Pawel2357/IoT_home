String a; //Initialized variable to store recieved data
const int pin_1 = 10;
const int pin_2 = 11;

void setup() {
  //Serial Begin at 9600 Baud
  pinMode(pin_1, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pin_1, HIGH);
  pinMode(pin_2, OUTPUT);
  digitalWrite(pin_2, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    a = Serial.readString();

    // TODO: iterate over elements in this string or look at nodemcu

    // say what you got:
    if (a == "1"){
      digitalWrite(pin_1, LOW); // turn on
    }if (a == "0"){
      digitalWrite(pin_1, HIGH); // turn off
    }if (a == "3"){
      digitalWrite(pin_2, LOW); // high speed
    }if (a == "2"){
      digitalWrite(pin_2, HIGH); // low speed
    }
  }
}
