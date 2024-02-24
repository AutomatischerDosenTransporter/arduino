/**
Speed 1 = Every Programm Zyklen
Speed 0.5 = Alle zwei Program Zyklen
*/
#include <SimpleCLI.h>

const int relayPinsSize = 8;
const int relayPins[] ={2,3,4,5,6,7,8,9};

// Create CLI Object
SimpleCLI cli;

// PING COMMAND
Command ping;
void pingCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    Serial.println("Pong!");
}

// STOP COMMAND
Command offCmd;
void offCmdCallback(cmd* c) {
  Command cmd(c);
  Argument numberArgument = cmd.getArgument("number");
  int number = (int) numberArgument.getValue().toInt();
  if(number > relayPinsSize - 1 || number < 0) {
    
    Serial.print("+ Unknown Relay ");
    Serial.print(number);
    Serial.println("!");
    Serial.println("$ ERROR");
    return;
  }

  digitalWrite(relayPins[number], false);

  Serial.print("+ Switch Relay ");
  Serial.print(number);
  Serial.println(" off!");
  Serial.println("$ OK");
}

// STOP COMMAND
Command onCmd;
void onCmdCallback(cmd* c) {
  Command cmd(c);
  Argument numberArgument = cmd.getArgument("number");
  int number = (int) numberArgument.getValue().toInt();
  if(number > relayPinsSize - 1 || number < 0) {
    
    Serial.print("+ Unknown Relay ");
    Serial.print(number);
    Serial.println("!");
    Serial.println("$ ERROR");
    return;
  }

  digitalWrite(relayPins[number], true);

  Serial.print("+ Switch Relay ");
  Serial.print(number);
  Serial.println(" on!");
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

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  ping = cli.addCmd("ping", pingCallback);
  
  onCmd = cli.addCmd("on", onCmdCallback);
  onCmd.addArgument("number");
  
  offCmd = cli.addCmd("off", offCmdCallback);
  offCmd.addArgument("number");
  
  cli.setOnError(errorCallback);

  for(int i = 0; i < relayPinsSize; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], false);
  }

 Serial.println("Initialized");
}

void loop() {
  checkSerial();
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