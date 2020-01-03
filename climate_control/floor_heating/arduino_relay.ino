const int pin_0 = 10;
const int pin_1 = 11;
const int pin_2 = 12;
const int pin_3 = 13;
const int pin_in_0 = A0;
const int pin_in_1 = A1;
const int pin_in_2 = A2;
const int pin_in_3 = A3;
const int NUM_SAMPLES = 15;
const float VOLTAGE_THR = 2.5;

void setup() {
  //Serial Begin at 9600 Baud
  pinMode(pin_0, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pin_0, HIGH);
  pinMode(pin_1, OUTPUT);
  digitalWrite(pin_1, HIGH);
  pinMode(pin_2, OUTPUT);
  digitalWrite(pin_2, HIGH);
  pinMode(pin_3, OUTPUT);
  digitalWrite(pin_3, HIGH);
}


float measure_voltage(int pin){
    // take a number of analog samples and add them up
    int sample_count = 0;
    int sum = 0;
    while (sample_count < NUM_SAMPLES) {
        sum += analogRead(pin);
        sample_count++;
        delay(10);
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.015V is the calibrated reference voltage
    float voltage = ((float)sum / (float)NUM_SAMPLES * 5) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value
    return voltage;
}

void set_pin_value(int pin_in, int pin){
  float voltage = measure_voltage(pin_in);
  if(voltage > VOLTAGE_THR){
    digitalWrite(pin, LOW); // turn on
  }else{
    digitalWrite(pin, HIGH); // turn off
  }
}

void loop() {
  //digitalWrite(pin_1, HIGH); // turn off
  //delay(1000);
  //digitalWrite(pin_2, LOW); // high speed
  set_pin_value(int pin_in_0, int pin_0);
  set_pin_value(int pin_in_1, int pin_1);
  set_pin_value(int pin_in_2, int pin_2);
  set_pin_value(int pin_in_3, int pin_3);
}
