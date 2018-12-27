#include <Process.h>

//Door sensor
#define SENS  4  // Door sensorf
int sensorState = 0; // variable for reading the Door Sensor status
// constants// constants
const int buttonPin = 2;     // the number of the pushbutton pin
const int Solenoid =  8;      // the number of the Solenoid pin
// variables
String line;
String res;
String strDoorStat = "N";
String QID = "Q00001";//Q00001 IT, Q00002 HNString line;
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds

void setup() {
  /*
     set YUN
  */
  // Initialize Bridge
  Bridge.begin();
  // Initialize Serial
  Serial.begin(9600);
  // Wait until a Serial Monitor is connected.
  while (!Serial);
  // run various example processes
  connectServer();
  Serial.println("start Yun 0830");

  /*
     set PIN
  */
  pinMode(SENS, INPUT);
  digitalWrite(SENS, HIGH);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the Solenoid pin as an output:
  pinMode(Solenoid, OUTPUT);
  // initialize the trigger pin as an input:
  digitalWrite(LED_BUILTIN, HIGH); //first start turn on led

}

void loop() {
  res = connectServer();
  Serial.println("3. result = " + res);

  if (res == "@1") {
    if (strDoorStat == "N") {
      openLock();
    }
    digitalWrite(Solenoid, LOW);
  } else if (res == "@0") {
    // Close Lock
    digitalWrite(LED_BUILTIN, HIGH);
  }

  sensorState = digitalRead(SENS); // read the Door sensor status
  if (sensorState == LOW) {
    // Serial.write("Closed");
    strDoorStat = "N";
  } else {
    //Serial.write("Open");
    strDoorStat = "Y";
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  }

  delay(postingInterval);
}
/*
   blink led, lock open
*/
void openLock() {
  for (int i = 0; i <= 10; i++) {
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(300);                       // wait for a second
    digitalWrite(Solenoid, HIGH); //open lock
  }
}
/*
   server connect
*/
String connectServer() {
  Serial.println();
  Serial.println("1. start to connecting server");
  // Launch "curl" command and get Arduino ascii art logo from the network
  // curl is command line program for transferring data using different internet protocols
  Process p;        // Create a process and call it "p"
  p.begin("curl");  // Process that launch the "curl" command
  p.addParameter("http://todayrecycle.com/todayrecycle/wifi/qrbox.jsp?qid=" + QID + "&dstat=" + strDoorStat); // Add the URL parameter to "curl"
  p.run();      // Run the process and wait for its termination

  // Print arduino logo over the Serial
  // A process output can be read with the stream methods
  while (p.available() > 0) {
    line = p.readStringUntil('\r');
  }
  line.trim();
  Serial.flush();
  Serial.println("2. connect server OK, update doorState= " + strDoorStat);
  return line;
}
