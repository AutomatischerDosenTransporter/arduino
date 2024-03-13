/**
Speed 1 = Every Programm Zyklen
Speed 0.5 = Alle zwei Program Zyklen
*/
#include <SimpleCLI.h>
#include <time.h>

struct Motor {
  int stepPin; 
  int dirPin;
  int limitPin;
  float speed; // in Steps per Second
  int encoder;
  float counter;
  String axis;
  long lastActivation;
  bool limitActivated;
};

Motor xMotor = {2, 5,  9, 0.0, 0, 0.0, "x", 0, false};
Motor yMotor = {3, 6, 10, 0.0, 0, 0.0, "y", 0, false};
Motor zMotor = {4, 7, 11, 0.0, 0, 0.0, "z", 0, false};


// Create CLI Object
SimpleCLI cli;

// PING COMMAND
Command ping;
void pingCallback(cmd* c) {
  Command cmd(c); // Create wrapper object

  Serial.println("+ Pong!");
  Serial.println("$ OK");
}

// ENCODER COMMAND
Command encoderCmd;
void encoderCmdCallback(cmd* c) {
  Command cmd(c);
  Argument axisArgument      = cmd.getArgument("axis");
  String axis = axisArgument.getValue();

  

  if(axis.equals("x") || axis.equals("X")) {
    Serial.println("+ Encoder "+axis+" axis is at " + String(xMotor.encoder));
  } else 
  if(axis.equals("y") || axis.equals("Y")) {
    Serial.println("+ Encoder "+axis+" axis is at " + String(yMotor.encoder));
  } else 
  if(axis.equals("z") || axis.equals("z")) {
    Serial.println("+ Encoder "+axis+" axis is at " + String(zMotor.encoder));
  } else {
    Serial.println("+ Unkown axis "+axis+"!");
    Serial.println("$ ERROR");
    return;
  }

  Serial.println("$ OK");
}

// STOP COMMAND
Command stopCmd;
void stopCmdCallback(cmd* c) {
  Command cmd(c);
  Argument axisArgument      = cmd.getArgument("axis");
  String axis = axisArgument.getValue();

  if(axis.equals("x") || axis.equals("X")) {
      xMotor.speed = 0;
      xMotor.lastActivation = millis();
  } else 
  if(axis.equals("y") || axis.equals("Y")) {
      yMotor.speed = 0;
      yMotor.lastActivation = millis();
  } else 
  if(axis.equals("z") || axis.equals("z")) {
      zMotor.speed = 0;
      zMotor.lastActivation = millis();
  } else {
      Serial.println("+ Unkown axis "+axis+"!");
      Serial.println("$ ERROR");
      return;
  }

  Serial.println("+ Stopped "+ axis+" axis!");
  Serial.println("$ OK");
}

// SPEED COMMAND
Command speedCmd;
void speedCmdCallback(cmd* c) {
  Command cmd(c);
  Argument axisArgument      = cmd.getArgument("axis");
  Argument speedArgument      = cmd.getArgument("speed");
  String axis = axisArgument.getValue();
  float speed = speedArgument.getValue().toFloat();
  Motor motor;

  if(axis.equals("x") || axis.equals("X")) {
      xMotor.speed = speed;
      xMotor.lastActivation = millis();
      motor = xMotor;
  } else 
  if(axis.equals("y") || axis.equals("Y")) {
      yMotor.speed = speed;
      yMotor.lastActivation = millis();
      motor = yMotor;
  } else 
  if(axis.equals("z") || axis.equals("z")) {
      zMotor.speed = speed;
      zMotor.lastActivation = millis();
      motor = zMotor;
  } else 
  {
      Serial.println("+ Unkown axis "+axis+"!");
      Serial.println("$ ERROR");
      return;
  }

  Serial.println("+ Set speed of "+axis+" axis to "+String(motor.speed)+" !");
  Serial.println("$ OK");
}

// Callback in case of an error
void errorCallback(cmd_error* e) {
    CommandError cmdError(e); // Create wrapper object

    Serial.println("+ "+cmdError.toString());
    if (cmdError.hasCommand()) {
        Serial.println("+ Did you mean \""+cmdError.getCommand().toString()+"\"?");
    }
    Serial.println("$ ERROR");
}

void initMotor(Motor motor);
Motor executeMotor(Motor motor);

void setup() {
  Serial.begin(115200);
  Serial.println("& Initializing...");

  ping = cli.addCmd("ping", pingCallback);
  
  stopCmd = cli.addCmd("stop", stopCmdCallback);
  stopCmd.addArgument("axis");
  
  speedCmd = cli.addCmd("speed", speedCmdCallback);
  speedCmd.addArgument("axis");
  speedCmd.addArgument("speed");
  
  encoderCmd = cli.addCmd("encoder", encoderCmdCallback);
  encoderCmd.addArgument("axis");
  
  cli.setOnError(errorCallback);

  initMotor(xMotor);
  initMotor(yMotor);
  initMotor(zMotor);


 Serial.println("& Initialized");
}


unsigned long motorTime = 0;

void loop() {
  checkSerial();

  unsigned long currentTime = micros();
  if((currentTime-motorTime) > 500) {
    motorTime = currentTime;


    xMotor = executeMotor(xMotor);
    yMotor = executeMotor(yMotor);
    zMotor = executeMotor(zMotor);
    delayMicroseconds(10);
    digitalWrite(xMotor.stepPin, false);
    digitalWrite(yMotor.stepPin, false);
    digitalWrite(zMotor.stepPin, false);
  }
}


void checkSerial() {
  if (Serial.available()) {
        // Read out string from the serial monitor
        String input = Serial.readStringUntil('\n');

        // Echo the user input
        Serial.println("# "+input);

        // Parse the user input into the CLI
        cli.parse(input);
    }
}


void initMotor(Motor motor) {
 pinMode(motor.stepPin, OUTPUT);
 pinMode(motor.dirPin, OUTPUT);
 pinMode(motor.limitPin, INPUT_PULLUP);
}


Motor executeMotor(Motor motor) {
  if(motor.lastActivation == 0) motor.lastActivation = millis();
  if(millis() - motor.lastActivation > 1000 && motor.speed != 0) {
    motor.speed = 0;
     
    Serial.println("& Emergency "+motor.axis+" motor stop becouse of timeout!");
  }

  if(digitalRead(motor.limitPin) && !motor.limitActivated) {
    motor.speed = 0;
    motor.limitActivated = true;
    Serial.println("& Emergency "+motor.axis+" motor stop becouse of limit switch!");
  } else if(!digitalRead(motor.limitPin) && motor.limitActivated) {
    motor.limitActivated = false;
    Serial.println("+ Limit switch of "+motor.axis+" motor reset!");
  } else {
    motor.speed = 0;
  }

  motor.counter += abs(motor.speed);
  if(motor.counter < 100.0) return motor;
  motor.counter -= 100.0;

  digitalWrite(motor.dirPin, motor.speed < 0);
  digitalWrite(motor.stepPin, true);

  return motor;
}