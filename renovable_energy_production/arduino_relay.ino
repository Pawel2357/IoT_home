String a; //Initialized variable to store recieved data
const int pin_1 = 1;
const int pin_2 = 2;
const int pin_3 = 3;
const int pin_4 = 4;
const int pin_5 = 5;
const int pin_6 = 6;
const int pin_7 = 7;
const int pin_8 = 8;

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
    if (a == "1"){
      digitalWrite(pin_1, LOW);
      //Serial.print(a);
    }if (a == "9"){
      digitalWrite(pin_1, HIGH);
    }if (a == "2"){
      digitalWrite(pin_1, LOW);
    }if (a == "a"){
      digitalWrite(pin_1, HIGH);
    }if (a == "3"){
      digitalWrite(pin_1, LOW);
    }if (a == "b"){
      digitalWrite(pin_1, HIGH);
    }if (a == "4"){
      digitalWrite(pin_1, LOW);
    }if (a == "c"){
      digitalWrite(pin_1, HIGH);
    }if (a == "5"){
      digitalWrite(pin_1, LOW);
    }if (a == "d"){
      digitalWrite(pin_1, HIGH);
    }if (a == "6"){
      digitalWrite(pin_1, LOW);
    }if (a == "e"){
      digitalWrite(pin_1, HIGH);
    }if (a == "7"){
      digitalWrite(pin_1, LOW);
    }if (a == "f"){
      digitalWrite(pin_1, HIGH);
    }if (a == "8"){
      digitalWrite(pin_1, LOW);
    }if (a == "g"){
      digitalWrite(pin_1, HIGH);
    }
  }
}