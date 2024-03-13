#include <SimpleCLI.h>
#include <Encoder.h>    // Verwendung der Bibliothek https://github.com/PaulStoffregen/Encoder

struct Motor {
  int stepPin; 
  int dirPin;
  float speed;
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

    Serial.println("+ Pong!");
    Serial.println("$ OK");
}

// ENCODER COMMAND
Command encoderCmd;
void encoderCmdCallback(cmd* c) {
  Command cmd(c);
  Argument sideArgument      = cmd.getArgument("side");
  String side = sideArgument.getValue();

  if(side.equals("left")) {
      Serial.println("+ Encoder "+side+" side is at: "+leftMotor.encoder);
      Serial.println("$ OK " + leftMotor.encoder);
  } else 
  if(side.equals("right")) {
      Serial.println("+ Encoder "+side+" side is at: "+rightMotor.encoder);
      Serial.println("$ OK " + rightMotor.encoder);
  } else {
      Serial.println("+ Unkown side: "+side);
      Serial.println("$ ERROR");
  }
}

// STOP COMMAND
Command stopCmd;
void stopCmdCallback(cmd* c) {
  Command cmd(c);
  Argument sideArgument      = cmd.getArgument("side");
  String side = sideArgument.getValue();

  if(side.equals("left")) {
      leftMotor.speed = 0;
      leftMotor.lastActivation = millis();
  } else 
  if(side.equals("right")) {
      rightMotor.speed = 0;
      rightMotor.lastActivation = millis();
  } else {
      Serial.println("+ Unkown side: "+side);
      Serial.println("$ ERROR");
      return;
  }

  Serial.println("+ Stoped "+side+" side!");
  Serial.println("$ OK");
}

// SPEED COMMAND
Command speedCmd;
void speedCmdCallback(cmd* c) {
  Command cmd(c);
  Argument sideArgument      = cmd.getArgument("side");
  Argument speedArgument      = cmd.getArgument("speed");
  String side = sideArgument.getValue();
  float speed = speedArgument.getValue().toFloat();

  if(side.equals("right")) {
      rightMotor.speed = speed;
      rightMotor.lastActivation = millis();
  } else 
  if(side.equals("left")) {
      leftMotor.speed = speed;
      leftMotor.lastActivation = millis();
  } else 
  {
      Serial.println("+ Unkown side: "+side);
      Serial.println("$ ERROR");
      return;
  }

  Serial.println("+ Set speed of "+side+" side to "+speed+" !");
  Serial.println("$ OK");
}

// Callback in case of an error
void errorCallback(cmd_error* e) {
    CommandError cmdError(e); // Create wrapper object

    Serial.println("$ ERROR "+cmdError.toString());

    if (cmdError.hasCommand()) {
        Serial.println("+ Did you mean \""+cmdError.getCommand().toString()+"\"?");
    }
}

void initMotor(Motor motor);
Motor executeMotor(Motor motor);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Initializing...");

  ping = cli.addCmd("ping", pingCallback);
  
  stopCmd = cli.addCmd("stop", stopCmdCallback);
  stopCmd.addArgument("side");
  
  speedCmd = cli.addCmd("speed", speedCmdCallback);
  speedCmd.addArgument("side");
  speedCmd.addArgument("speed");
  
  encoderCmd = cli.addCmd("encoder", encoderCmdCallback);
  encoderCmd.addArgument("side");
  
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
        Serial.println("# "+input);

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
     
    Serial.println("& Emergency "+motor.name+" motor stop!");
  }

  if(motor.encoderOld != motor.encoder) {
    motor.encoderOld = motor.encoder;
    Serial.println("& Encoder "+motor.name+" "+motor.encoder);
  }

  float speed = motor.speed;
  if(speed < 0) {
    digitalWrite(motor.dirPin, true);
    speed *= -1;
  } else {
    digitalWrite(motor.dirPin, false);
  }

  int analogSpeed = (int)(speed * 255.0);
  analogWrite(motor.stepPin, analogSpeed);

  return motor;
}