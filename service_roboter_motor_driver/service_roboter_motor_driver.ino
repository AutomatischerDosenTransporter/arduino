/**
Speed 1 = Every Programm Zyklen
Speed 0.5 = Alle zwei Program Zyklen
*/
#include <SimpleCLI.h>
#include <Encoder.h>    // Verwendung der  Bibliothek 

struct Motor {
  int stepPin; 
  int dirPin;
  float speed; // in Steps per Second
  long encoder;
  long encoderOld;
  String name;
  long lastActivation;
};

Motor leftMotor  = {3, 2, 0, 0, 0, "left", 0};
Motor rightMotor = {5, 4, 0, 0, 0, "right", 0};

const int leftEncoderDT = 8;
const int leftEncoderCLK = 9;
Encoder leftEncoder(leftEncoderDT, leftEncoderCLK);

const int rightEncoderDT = 10;
const int rightEncoderCLK = 11;
Encoder rightEncoder(rightEncoderDT, rightEncoderCLK);


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

  if(axis.equals("left")) {
      Serial.println(leftMotor.encoder);
  } else 
  if(axis.equals("right")) {
      Serial.println(rightMotor.encoder);
  } else {
      Serial.print("+ Unkown axis: ");
      Serial.println(axis);
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

  if(axis.equals("left")) {
      leftMotor.speed = 0;
      leftMotor.lastActivation = millis();
  } else 
  if(axis.equals("right")) {
      rightMotor.speed = 0;
      rightMotor.lastActivation = millis();
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

  if(axis.equals("right")) {
      rightMotor.speed = speed;
      rightMotor.lastActivation = millis();
  } else 
  if(axis.equals("left")) {
      leftMotor.speed = speed;
      leftMotor.lastActivation = millis();
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
  Serial.print(speed);
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

  initMotor(leftMotor);
  initMotor(rightMotor);

  pinMode(leftEncoderDT, INPUT);
  pinMode(leftEncoderCLK, INPUT);
  pinMode(rightEncoderDT, INPUT);
  pinMode(rightEncoderCLK, INPUT);


  Serial.println("Initialized");
}

void loop() {
  checkSerial();

  leftMotor = executeMotor(leftMotor);
  rightMotor = executeMotor(rightMotor);

  leftMotor.encoder = leftEncoder.read();
  rightMotor.encoder = rightEncoder.read();

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
  if(motor.lastActivation == 0) motor.lastActivation = millis();
  if(millis() - motor.lastActivation > 1000 && motor.speed != 0) {
    motor.speed = 0;
     
    Serial.print("& Emergency ");
    Serial.print(motor.name);
    Serial.print(" motor stop!");
  }

  if(motor.encoderOld != motor.encoder) {
    motor.encoderOld = motor.encoder;
    Serial.print("& Encoder ");
    Serial.print(motor.name);
    Serial.print(" ");
    Serial.println(motor.encoder);
  }

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