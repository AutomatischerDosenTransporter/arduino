/**
Speed 1 = Every Programm Zyklen
Speed 0.5 = Alle zwei Program Zyklen
*/
#include <SimpleCLI.h>

struct Motor {
  int stepPin; 
  int dirPin;
  float speed; // in Steps per Second
  int encoder;
};

Motor xMotor = {3, 2, 0, 0};
Motor yMotor = {5, 4, 0, 0};
Motor zMotor = {7, 6, 0, 0};


// Create CLI Object
SimpleCLI cli;

// PING COMMAND
Command ping;
void pingCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    Serial.println("Pong!");
}

// ENCODER COMMAND
Command encoderCmd;
void encoderCmdCallback(cmd* c) {
  Command cmd(c);
  Argument axisArgument      = cmd.getArgument("axis");
  String axis = axisArgument.getValue();

  
  Serial.print("+ Encoder ");
  Serial.print(axis);
  Serial.print(" axis is at: ");

  if(axis.equals("x") || axis.equals("X")) {
      Serial.println(xMotor.encoder);
  } else 
  if(axis.equals("y") || axis.equals("Y")) {
      Serial.println(yMotor.encoder);
  } else 
  if(axis.equals("z") || axis.equals("z")) {
      Serial.println(zMotor.encoder);
  } else {
      Serial.println(0);
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
  } else 
  if(axis.equals("y") || axis.equals("Y")) {
      yMotor.speed = 0;
  } else 
  if(axis.equals("z") || axis.equals("z")) {
      zMotor.speed = 0;
  } else {
      Serial.print("+ Unkown axis: ");
      Serial.println(axis);
      Serial.println("$ ERROR");
      return;
  }

  Serial.print("+ Stoped ");
  Serial.print(axis);
  Serial.println(" axis!");
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
      motor = xMotor;
  } else 
  if(axis.equals("y") || axis.equals("Y")) {
      yMotor.speed = speed;
      motor = yMotor;
  } else 
  if(axis.equals("z") || axis.equals("z")) {
      zMotor.speed = speed;
      motor = zMotor;
  } else 
  {
      Serial.print("+ Unkown axis: ");
      Serial.println(axis);
      Serial.println("$ ERROR");
      return;
  }

  Serial.print("+ Set speed of ");
  Serial.print(axis);
  Serial.print(" axis to ");
  Serial.print(motor.speed);
  Serial.println(" !");
  Serial.println("$ OK");
}

// Callback in case of an error
void errorCallback(cmd_error* e) {
    CommandError cmdError(e); // Create wrapper object

    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    if (cmdError.hasCommand()) {
        Serial.print("Did you mean \"");
        Serial.print(cmdError.getCommand().toString());
        Serial.println("\"?");
    }
}

void initMotor(Motor motor);
Motor executeMotor(Motor motor);

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("Initializing...");

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


 Serial.println("Initialized");
}

void loop() {
  checkSerial();

  xMotor = executeMotor(xMotor);
  yMotor = executeMotor(yMotor);
  zMotor = executeMotor(zMotor);
}


void checkSerial() {
  if (Serial.available()) {
        // Read out string from the serial monitor
        String input = Serial.readStringUntil('\n');

        // Echo the user input
        Serial.print("# ");
        Serial.println(input);

        // Parse the user input into the CLI
        cli.parse(input);
    }
}


void initMotor(Motor motor) {
 pinMode(motor.stepPin, OUTPUT);
 pinMode(motor.dirPin, OUTPUT);
}


Motor executeMotor(Motor motor) {
  float speed = motor.speed;
  if(speed < 0) {
    digitalWrite(motor.dirPin, true);
    speed *= -1;
  } else {
    digitalWrite(motor.dirPin, false);
  }

  float digitalSpeed = motor.speed;
  int analogSpeed = (int)(digitalSpeed * 255.0);
  analogWrite(motor.stepPin, analogSpeed);

  return motor;
}