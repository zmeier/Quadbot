#include <Servo.h>

//#define SHOULD_CALIBRATE
//#define AUTO_PILOT
#define ESC_HIGH_DEFAULT 200
#define ESC_LOW_DEFAULT 20
#define NUM_MOTORS 4
#define SERIAL_RATE 9600
#define SPEED_STEP_VALUE 10

typedef struct {
  Servo   Motor;
  int     Pin;   // Indicates the Pin this motor is connected to
} MotorDef;

MotorDef Motors[NUM_MOTORS];
int currentSpeed = ESC_LOW_DEFAULT;

void setup() {
  Serial.begin(SERIAL_RATE);
  Serial.println("Running with a serial value: [" + String(SERIAL_RATE) + "]");

  Motors[0].Pin =  13;
  Motors[1].Pin =  12;
  Motors[2].Pin =  11;
  Motors[3].Pin =  10;

  for (int i = 0; i < NUM_MOTORS; i++) {
    int pin = Motors[i].Pin;
    Motors[i].Motor.attach(pin);
  }

#ifdef SHOULD_CALIBRATE
  calibrateThrottle();
#else
  for (int i = 0; i < NUM_MOTORS; i++) {
    Motors[i].Motor.write(ESC_LOW_DEFAULT);
  }
  Serial.println("Warming up the engines");
  delay(3000);
  waitForUserCharacterInput();
#endif
}

void loop() {
#ifdef AUTO_PILOT
  autoPilot();
#else
  flyBySerial();
#endif
}

void calibrateThrottle() {
  Serial.println("Calibrating throttle range");

  for (int i = 0; i < NUM_MOTORS; i++) {
    Motors[i].Motor.write(ESC_HIGH_DEFAULT);
  }
  Serial.println("Connect the ESC now. After connecting the ESC, you should hear the ESC startup tones.");
  Serial.println("Shortly afterwards, you should hear two beeps indicating that the ESC has registered the high throttle value.");
  Serial.println("Immediately after hearing the two beeps, push any key. If you don't do so in 5 sec, the ESC will go into program mode");
  waitForUserCharacterInput();

  Serial.println("\nSetting the low throttle setting. If this happens successfully, you should hear several beeps indicating");
  Serial.println("the input voltage supplied to the ESC followed by a long beep indicating that the low throttle has been set.");
  Serial.println("After this point, push any key to proceed, your ESC is ready to be used");

  for (int i = 0; i < NUM_MOTORS; i++) {
    Motors[i].Motor.write(ESC_LOW_DEFAULT);
  }
  waitForUserCharacterInput();
}

void autoPilot() {
  // Send a low signal initially for normal mode
  for (int i = 0; i < NUM_MOTORS; i++) {
    Motors[i].Motor.write(ESC_LOW_DEFAULT);
  }

  Serial.println("Running in autopilot mode");
  for(int i = 0; i < 4; i++) {
   adjustSpeed(SPEED_STEP_VALUE);
   delay(1000);
  }
  for(int i = 0; i < 4; i++) {
   adjustSpeed(-1 * SPEED_STEP_VALUE);
   delay(1000);
  }
  stopMotors();
  delay(5000);
}

void flyBySerial() {
  // Send a low signal initially for normal mode
  for (int i = 0; i < NUM_MOTORS; i++) {
    Motors[i].Motor.write(ESC_LOW_DEFAULT);
  }

  Serial.println("Running ESC");
  Serial.println("\nPress 'w' to increase speed, 's' to reduce speed");

  while (1) {
    char currentChar = waitForUserCharacterInput();
    if (currentChar == 'w') {
      Serial.println("\nIncreasing motor speed by step");
      adjustSpeed(SPEED_STEP_VALUE);
    } else if (currentChar == 's') {
      Serial.println("\nDecreasing motor speed by step\n");
      adjustSpeed(-1 * SPEED_STEP_VALUE);
    } else if (currentChar == 'e' || currentChar == 'q') {
      stopMotors();
    }
  }
}

void adjustSpeed(int speedChange) {
  int newSpeed = currentSpeed + speedChange;
  if (newSpeed > ESC_HIGH_DEFAULT) {
    newSpeed = ESC_HIGH_DEFAULT;
  } else if (newSpeed < ESC_LOW_DEFAULT) {
    newSpeed = ESC_LOW_DEFAULT;
  }
  Serial.print("Speed changed from: [");
  Serial.print(currentSpeed);
  Serial.print("] to: [");
  Serial.print(newSpeed);
  Serial.println("]");
  currentSpeed = newSpeed;
  updateMotorSpeeds(currentSpeed);
}

void stopMotors() {
  Serial.println("Stopping Motors");
  currentSpeed = ESC_LOW_DEFAULT;
  updateMotorSpeeds(currentSpeed);
}

void updateMotorSpeeds(int motorSpeed) {
  Serial.print("Setting motor speeds of: [");
  Serial.print(motorSpeed);
  Serial.println("]");
  for (int i = 0; i < NUM_MOTORS; i++) {
    Motors[i].Motor.write(motorSpeed);
  }
}

char waitForUserCharacterInput() {
  while (!Serial.available()) {}
  return Serial.read();
}

