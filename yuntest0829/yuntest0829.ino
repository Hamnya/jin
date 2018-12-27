#include <Process.h>
#include <SoftwareSerial.h>

//BLUETOOTH NAME is QUAN.using BLE CHAT APP. send Data 0 or 1(open)
SoftwareSerial BTSerial(6, 7); //Connect HM-10 Use your (TX, RX) settings
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
String QID = "Q00002";//Q00001 IT, Q00002 HNString line;
const unsigned long postingInterval = 1L * 1000L; // delay between updates, in milliseconds


void setup() {
  /*
     set YUN
  */
  // Initialize Bridge
  Bridge.begin();
  // Initialize Serial
  Serial.begin(9600);
 // set the data rate for the BT port
  BTSerial.begin(9600);  
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

  /*
   * YUN WIFI FUNCTION
   */
    // res = connectServer();
     // Serial.println("3. WIFI - result = " + res);


  /*
   * HM-10 BLUETOOTH FUCTION
   */
  // Serial –> Data –> BT
  
  if (BTSerial.available()) {
    char ch = (char)BTSerial.read();
    Serial.write(BTSerial.read());
  }
  //Serial.println("3-1. BT-result = " + line);
  if (line == "1"){
     res = "@1";
  }else if (line == "0"){
     res = "@0";
  }
    

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
  p.addParameter("http://todayrecycle.com/todayrecycle/wifi/log.jsp?data=" + QID + "&dstat=" + strDoorStat); // Add the URL parameter to "curl"
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
