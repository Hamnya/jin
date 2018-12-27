#include <SPI.h>
#include <WiFi101.h>
#include <WString.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "oysterYS";        // your network SSID (name)
char pass[] = "52005200ys";    // your network password (use for WPA, or use as key for WEP)
//https://arduino.stackexchange.com/questions/43644/wifi101-client-http-post-fails-after-3rd-request
int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

// server address:
char server[] = "todayrecycle.com";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 0.5L * 1000L; // delay between updates, in milliseconds

//Door sensor
#define SENS  4  // Door sensorf
int sensorState = 0; // variable for reading the Door Sensor status
// constants
const int buttonPin = 2;     // the number of the pushbutton pin
const int Solenoid =  8;      // the number of the Solenoid pin
const int TPin = 3;     // the number of the trigger pin

// variables
int buttonState = 0;         // variable for reading the pushbutton status
bool isOK = false;
String line;

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  WiFi.end();
  delay(10);
  WiFi.begin(ssid, pass);

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    //Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /todayrecycle/wifi/qrbox.jsp HTTP/1.1");
    client.println("Host: todayrecycle.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    isOK = false;
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void connectWIFI(){
    // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
    // you're connected now, so print out the status:
    printWiFiStatus();
  }
}


void setup() {
   // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the status:
  printWiFiStatus();


  // initialize Door sensor as input:
  pinMode(SENS, INPUT);
  digitalWrite(SENS, HIGH);
    // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the Solenoid pin as an output:
  pinMode(Solenoid, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
    // initialize the trigger pin as an input:
  pinMode(TPin, INPUT);
  digitalWrite(TPin, HIGH);
  Serial.begin(9600);  // opens serial port, sets data rate to 9600 bps
}

void loop() {

     while (client.available()) {
       char c = client.read();
       line = client.readStringUntil('\r');

    String a = getValue(line, '@', 1);
    if(client.available()){
      Serial.println(a);

      if(a == "1") isOK=true;
      else isOK=false;
    }


    // if ten seconds have passed since your last connection,
    // then connect again and send data:

    if (millis() - lastConnectionTime > postingInterval) httpRequest();

    sensorState = digitalRead(SENS); // read the Door sensor status
     if (sensorState == LOW) {
       // Serial.write("Closed");
        digitalWrite(LED_BUILTIN, HIGH);
        // Close Lock:
        digitalWrite(Solenoid, LOW);
     }else{
       //Serial.write("Open");
       digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
    }

    if (isOK) {
        for (int i=0; i <= 10; i++){
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(50);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(20);                       // wait for a second
        // LOCK FUNCTION
        // read the state of the pushbutton value:
        buttonState = digitalRead(buttonPin);
        Serial.println(buttonState);
        delay(500);
        if (buttonState == LOW ){
          // Open Lock:
          digitalWrite(Solenoid, HIGH);
          delay (10000);

        }else{
          // Close Lock:
          digitalWrite(Solenoid, LOW);
          digitalWrite(LED_BUILTIN, HIGH);
        }

        } //end for
      } //end isOK
    } //end while

} // end loop


