// 3200motor
//PID
float kp = 1;
float ki = 0.003;
float kd = 0.0;
// Pin
const byte interruptPinA = 2;
const byte interruptPinB = 3;
volatile unsigned long EncoderCount = 0;
const byte PWMPin = 6;
const byte DirPin1 = 7;
// Time
unsigned long t;
unsigned long t_prev = 0;
volatile unsigned long count = 0;
unsigned long count_prev = 0;
// PID coef
float Theta, RPM, RPM_d;
float Theta_prev = 0;
int dt;
float RPM_max = 60;
// Low pass filter
float v1Filt = 0;
float v1Prev = 0;
// voltage coef
#define pi 3.1416
float Vmax = 100;
float Vmin = -100;
float V = 0.1;
float e, e_prev = 0, inte, inte_prev = 0;
//**********FUNCTIONS******************
//     Void ISR_EncoderA
//     Void ISR_EncoderB
//     Void Motor Driver Write
//     Timer Interrupt
//***********************************
void ISR_EncoderA() {
  bool B = digitalRead(interruptPinB);
  bool A = digitalRead(interruptPinA);
  bool atemp,btemp;
  bool check=1;
  
  if (B == LOW) {
    if (A == HIGH) {
      for(int i=0;i++;i<2000){ 
        atemp=digitalRead(interruptPinA);
        btemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount--;}
    }
    else {
      for(int i=0;i++;i<2000){ 
        atemp=digitalRead(interruptPinA);
        btemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount++;}
    }
  }

  else {
    if (A == HIGH) {
      for(int i=0;i++;i<2000){ 
        atemp=digitalRead(interruptPinA);
        btemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount++;}
    }
    else {
      for(int i=0;i++;i<2000){ 
        atemp=digitalRead(interruptPinA);
        btemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount--;}
    }
  }
}
void ISR_EncoderB() {
  bool B = digitalRead(interruptPinA);
  bool A = digitalRead(interruptPinB);
  bool atemp,btemp;
  bool check=1;
  if (A == LOW) {
    if (B == HIGH) {
      for(int i=0;i++;i<2000){ 
        btemp=digitalRead(interruptPinA);
        atemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount++;}
    }
    else {
      for(int i=0;i++;i<2000){ 
        btemp=digitalRead(interruptPinA);
        atemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount--;}
      }
    }
  
  else {
    if (B == HIGH) {
      for(int i=0;i++;i<2000){ 
        btemp=digitalRead(interruptPinA);
        atemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount--;}
    }
    else {
      for(int i=0;i++;i<2000){ 
        btemp=digitalRead(interruptPinA);
        atemp=digitalRead(interruptPinB);
        if(btemp!=B || atemp!=A){check=0;}  
      }
      if(check==1){
      EncoderCount++;}
    }
  }
}
float sign(float x) {
  if (x > 0) {
    return 1;
  } else if (x < 0) {
    return -1;
  } else {
    return 0;
  }
}
//***Motor Driver Functions*****
void WriteDriverVoltage(float V, float Vmax) {
  int PWMval = int(255 * abs(V) / Vmax);
  if (PWMval > 255) {   //頓點
    PWMval = 255;
  }
  if (V < 0) {
    digitalWrite(DirPin1, HIGH);
  }
  else if (V > 0) {
    digitalWrite(DirPin1, LOW);
  }
  else {
    analogWrite(PWMPin, 0);
  }
  analogWrite(PWMPin, PWMval);
}

void setup() {
  Serial.begin(9600);
  pinMode(interruptPinA, INPUT_PULLUP);
  pinMode(interruptPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPinA), ISR_EncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPinB), ISR_EncoderB, CHANGE);
  pinMode(DirPin1, OUTPUT);
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A =12499; //Prescaler = 64  (12499)
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11 | 1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();
  
}

void loop() {
  if (count > count_prev) {
    t = millis();    
    Theta = EncoderCount / 3200.0;
    dt = (t - t_prev);
    RPM = (Theta - Theta_prev) / (dt / 1000.0) * 60 ;
    //RPM_d = RPM_max *(sin(2 * pi * 0.05 * t / 1000.0));
    RPM_d =RPM_max;
    
    e = RPM_d - v1Filt;
    inte = inte_prev + (dt * (e + e_prev) / 2);
    V = kp * e + ki * inte + (kd * (e - e_prev) / dt) ;
    if (V > Vmax) {
      V = Vmax;
      inte = inte_prev;
    }
    if (V < Vmin) {
      V = Vmin;
      inte = inte_prev;
    }

    WriteDriverVoltage(V, Vmax);

    // Low-pass filter (25 Hz cutoff)
    v1Filt = 0.854*v1Filt + 0.0728*RPM + 0.0728*v1Prev;
    v1Prev = RPM;
    Serial.print(RPM_d); Serial.print(" \t");
    Serial.print(v1Filt); Serial.print(" \t ");
    //Serial.print(digitalRead(interruptPinA)); Serial.print(" \n");
    //Serial.print(digitalRead(interruptPinB)); Serial.print(" \n ");
    //Serial.print(EncoderCount); Serial.print(" \t ");
    //Serial.print(V); Serial.print("\t  ");
    Serial.print(e); Serial.println("  ");
    
    Theta_prev = Theta;
    count_prev = count;
    t_prev = t;
    inte_prev = inte;
    e_prev = e;

  }
}


ISR(TIMER1_COMPA_vect) {
  count++;
  Serial.print(count * 0.05); Serial.print(" \t");
}

