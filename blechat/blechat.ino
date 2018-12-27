#include <SoftwareSerial.h>

SoftwareSerial BTSerial(6, 7); //Connect HC-06. Use your (TX, RX) settings


void setup()
{
  Serial.begin(9600);
  BTSerial.begin(9600);  // set the data rate for the BT port
}

void loop()
{
  // BT –> Data –> Serial
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
  // Serial –> Data –> BT
  if (Serial.available()) {

    char ch = (char)Serial.read();
    if (ch == 'u')
    {
      BTSerial.write("STOP");
      Serial.write("DISCONNECT");
    }
    else if (ch == 'r')
    {
      //    BTSerial.write("RECYCLE");
        Serial.write("RECYCLE");
    }
  }

}

