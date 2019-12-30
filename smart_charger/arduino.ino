const int pin_1 = 10;
const int pin_2 = 11;
const int NUM_SAMPLES = 15;

void setup() {
  //Serial Begin at 9600 Baud
  pinMode(pin_1, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pin_1, HIGH);
  pinMode(pin_2, OUTPUT);
  digitalWrite(pin_2, HIGH);
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

void loop() {
  //digitalWrite(pin_1, HIGH); // turn off
  //delay(1000);
  //digitalWrite(pin_2, LOW); // high speed

  float voltage_A0 = measure_voltage(A0);

  if(voltage_A0 > 2.5){
    digitalWrite(pin_1, LOW); // turn on
  }else{
    digitalWrite(pin_1, HIGH); // turn off
  }
}
