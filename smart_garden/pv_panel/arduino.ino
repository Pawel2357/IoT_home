int sensor_pin = A0;
const int relay_pin = 13;
String a; //Initialized variable to store recieved data

int output_value ;

void setup() {
  Serial.begin(9600);
  Serial.println("Reading From the Sensor ...");
  delay(2000);
}

void loop() {
  output_value = analogRead(sensor_pin);
  output_value = map(output_value, 550, 0, 0, 100);
  Serial.print("Mositure : ");
  Serial.print(output_value);
  Serial.println("%");
  delay(1000);
}


int measure_moisture(){
  output_value = analogRead(sensor_pin);
  output_value = map(output_value, 550, 0, 0, 100);
  return output_value;
}

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
    }if (a == "1"){
      digitalWrite(relay_pin, HIGH);
    }if (a == "m"){
      Serial.print(measure_air_quality());
    }
  }
}