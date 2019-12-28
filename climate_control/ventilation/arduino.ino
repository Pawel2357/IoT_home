String a; //Initialized variable to store recieved data
const int pin_power = 10;
const int pin_speed = 11;

void setup() {
  //Serial Begin at 9600 Baud
  pinMode(pin_power, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pin_power, HIGH);
  pinMode(pin_speed, OUTPUT);
  digitalWrite(pin_speed, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    a = Serial.readString();

    // TODO: iterate over elements in this string or look at nodemcu

    // say what you got:
    if (a == "1"){
      digitalWrite(pin_power, LOW); // turn on
    }if (a == "0"){
      digitalWrite(pin_power, HIGH); // turn off
    }if (a == "3"){
      digitalWrite(pin_speed, LOW); // high speed
    }if (a == "2"){
      digitalWrite(pin_speed, HIGH); // low speed
    }
  }
}
