#include <HCSR04.h>

HCSR04 hc(33, new int[14]{23,25,27,29,31,   35,37,39,41,43,   47,49,51,53}, 14); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)

void setup()
{
   Serial.begin(115200);
}

void loop()
{
  String text = "";
    text += "& USS [";
    for (int i = 0; i < 14; i++ ) {
        text +=  hc.dist(i) ;
        // Serial.print(i);
        if(i != 13) text += ", ";
    }
    text += "]";

    Serial.println(text);

    delay(60);                        // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
}