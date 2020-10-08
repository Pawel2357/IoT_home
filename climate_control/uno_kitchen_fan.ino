const byte OC1A_PIN = 9;
const byte OC1B_PIN = 10;

const word PWM_FREQ_HZ = 25000; //Adjust this value to adjust the frequency
const word TCNT1_TOP = 16000000/(2*PWM_FREQ_HZ);

String a;
int pwd_speed;

void setup() {
  Serial.begin(9600);
  pinMode(OC1A_PIN, OUTPUT);

  // Clear Timer1 control and count registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  
  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << CS10);
  ICR1 = TCNT1_TOP;

  setPwmDuty(10);
}

void loop() {
    if (Serial.available() > 0) {
      // read the incoming byte:
      a = Serial.readString();
      Serial.print(a);
      pwd_speed = a.toInt() * 11;
      Serial.print(pwd_speed);
      setPwmDuty(pwd_speed);
    }
}

void setPwmDuty(byte duty) {
  OCR1A = (word) (duty*TCNT1_TOP)/100;
}
