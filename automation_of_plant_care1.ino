// include the servo library
#include <Servo.h>

// define the number of servos and their pins
#define NUM_SERVOS 4
#define SWITCH_PIN 7

class PotSettings {
  public:
  
  // arduino pin for the potentiometer
  int pin;
  // variable to store the reading from the analog input
  int reading;
  // variable for the maximum value of potentiometer
  static const int maxPotValue = 1023;
  
  PotSettings(int pin) {
    this->pin = pin;
  }
};

class ServoSettings {
  public:
  
  // change the pulses to turn servos a full 0 – 180 degrees
  static const int minPulse = 480;
  static const int maxPulse = 2500;
  // variable for the maximum value of the positional servo
  static const int maxServoDegree = 180;
  
  // arduino pin for the micro servo
  int pin;
  // initial position of servo motor
  int position;
  // degrees step of servo moving 
  int step;
  // interval between the steps
  int interval;
  // last time servo moved
  unsigned long lastTime;
  // the direction of servo
  bool reverse;
  // the servo itself
  Servo motor;
  
  ServoSettings(int pin, int position, int step, int interval) {
    this->pin = pin;
    this->position = position;
    this->step = step;
    this->interval = interval;
  }
  
};

PotSettings pots[NUM_SERVOS] = {
  PotSettings(A0),
  PotSettings(A1),
  PotSettings(A2),
  PotSettings(A3)
};

ServoSettings servos[NUM_SERVOS] = {
  ServoSettings(5,  0,  60, 2000),
  ServoSettings(4,  40, 30, 1500),
  ServoSettings(3,  90, 20, 1000),
  ServoSettings(2, 120, 10,  500)
};

int mode = 1;

void setup() {
  pinMode(SWITCH_PIN, INPUT);
  initPots();
  initServos();
  attachServos();
  resetServos();
  Serial.begin(9600);
}

void loop() {
  int readMode = digitalRead(SWITCH_PIN);
  
  if(mode != readMode) {
    resetServos();
    mode = readMode;
  }
  
  if(mode == 1)     {
  	manualMode();
  } else {
    autoMode();
  }
}

void initServos() {
  for(int i = 0; i < NUM_SERVOS; i++) {
    pinMode(servos[i].pin, OUTPUT);
  }
}

void initPots(){
  for(int i = 0; i < NUM_SERVOS; i++) {
    pinMode(pots[i].pin, INPUT);
  }
}

void resetServos(){
  for(int i = 0; i < NUM_SERVOS; i++) {
    servos[i].motor.write(servos[i].position);
  }
}

void attachServos(){
  // attach the Servo variables to pins
  for(int i = 0; i < NUM_SERVOS; i++) {
    // won't turn full 0 – 180 degrees
    // servos[i].motor.attach(servos[i].pin);
    
    // will turn full 0 – 180 degrees
    servos[i].motor.attach(servos[i].pin, 
                          ServoSettings::minPulse, 
                          ServoSettings::maxPulse); 
  }
}

void manualMode(){
   for(int i = 0; i < NUM_SERVOS; i++) {
    // read the analog input (the value of the potentiometer)
  	pots[i].reading = analogRead(pots[i].pin); 
  	// map the input to a value between 0 and 180 degrees
  	servos[i].position = map(pots[i].reading, 
                             0, PotSettings::maxPotValue, 
                             0, ServoSettings::maxServoDegree);
  	// tell the servo to go to the set position
  	servos[i].motor.write(servos[i].position); 
  	// wait 15 ms for the servo to reach the position
  	delay(15); 
  }
}

void autoMode() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    // check if it's time to move the servo
    if (millis() - servos[i].lastTime >= servos[i].interval) {
      // update the last time the servo was moved
      servos[i].lastTime = millis();
      
      // write the new position to the servo
      servos[i].motor.write(servos[i].position);
     
      // move the servo to its degrees depending on direction
      servos[i].position += servos[i].reverse ?
        servos[i].step*(-1) : servos[i].step;
    
      // define the direction of rotation
      if(servos[i].position > 180) {
        servos[i].reverse = true;
      } else if (servos[i].position < 0) {
        servos[i].reverse = false;
      }
    }
  }
  
}