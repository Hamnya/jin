#include <SPI.h>
#include <WiFi101.h>
#include <WString.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "oysterYS";        // your network SSID (name)
char pass[] = "52005200ys";    // your network password (use for WPA, or use as key for WEP)
String QID = "Q00002"; //Q00001 IT, Q00002 HN
//https://arduino.stackexchange.com/questions/43644/wifi101-client-http-post-fails-after-3rd-request
int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

// server address:
char server[] = "todayrecycle.com";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 3L * 1000L; // delay between updates, in milliseconds

//Door sensor
#define SENS  4  // Door sensorf
int sensorState = 0; // variable for reading the Door Sensor status
// constants
const int buttonPin = 6;     // the number of the pushbutton pin
const int Solenoid =  8;      // the number of the Solenoid pin
// variables
String line;
String res;
String strDoorStat="N"; 
char btChar;
/*
 * split result from server split result from server 
 */
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


/* 
 *  this method makes a HTTP connection to the server:this method makes a HTTP connection to the server:
 */
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield.
  Serial.println();
  Serial.println("1. start to connecting server");
  client.stop();
  WiFi.end();
  delay(10);
  WiFi.begin(ssid, pass);

  // if there's a successful connection:
  if (client.connect(server, 80)) {

    Serial.println("2. connect server OK, door sate= "+strDoorStat);
    //Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /todayrecycle/wifi/qrbox.jsp?qid="+QID+"&dstat="+strDoorStat+" HTTP/1.1");
    client.println("Host: todayrecycle.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
 
}

/* 
 *  print wifi state
 */
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

/* 
 *  connect wifi
 */
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
  connectWIFI();
  // initialize Door sensor as input:
  pinMode(SENS, INPUT);
  digitalWrite(SENS, HIGH);
    // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the Solenoid pin as an output:
  pinMode(Solenoid, OUTPUT);
    
  Serial.begin(9600);  // opens serial port, sets data rate to 9600 bps
  Serial1.begin(9600); // HM-10 Bluetooth communication
  
  digitalWrite(LED_BUILTIN, HIGH); //first start turn on led
  Serial.println("0. my delay time to server is 3000ms(var-postingInterval)");
}

/*
 * blink led, lock open
*/
void open(){
   for (int i=0; i <= 10; i++){
          digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
          delay(300);
          digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(300);                       // wait for a second
          digitalWrite(Solenoid, HIGH); //open lock
          
    }
}

void loop() {
  
  //1. get door state
    sensorState = digitalRead(SENS); // read the Door sensor status
     if (sensorState == LOW) {
       // Serial.write("Closed");
       strDoorStat= "N";
       digitalWrite(LED_BUILTIN, HIGH); 
     }else{
       //Serial.write("Open");
         strDoorStat= "Y";
      //no door
       digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
    }
    //Serial.println("0. door = "+strDoorStat); 
//    if(strDoorStat == "Y"){
//      //if door is open at least once, no more open function. = isDONE 
//      isDone = true;
//    }

    //BT

    //PHONE > ARDUINO
    while(Serial1.available()){
       btChar = (char)Serial1.read();
    }

//    //ARDUINO > PHONE
//    while(Serial.available()){
//      Serial1.print((char)Serial.read());
//    }  

    if(sizeof(btChar)>1){
        Serial.println("BT START");
    }
    
 /***************bluetoothconnection***********************/
       switch(btChar){
         case '1':
         if(strDoorStat == "N"){open();}
          Serial1.print("1");
          digitalWrite(Solenoid, LOW);
         // btLog = "open lock fuction";
          break;
        case '2':
          digitalWrite(Solenoid, HIGH);
          //btLog = "lock on";
          break;  
        case '3':
          digitalWrite(Solenoid, LOW);
          //btLog = "lock off";
          break;
       }
 /***************wifi connection**************************/
//        //2.enter server
//          line = "";
//           while (client.available()) {
//             line = client.readStringUntil('\r');
//           }
//      
//        //3.get result
//          res = getValue(line, 'v@', 1);
//          if(res.length() >= 1){
//              res.trim();
//            if(res == "1"){
//        //4. only open! when door is closed. 
//               if(strDoorStat == "N"){
//                  //if(!isDone){ // no opened door state
//                    open();
//                  //}
//               }
//                //digitalWrite(Solenoid, LOW);
//            }else if(res == "0"){
//                // Close Lock 
//                  digitalWrite(LED_BUILTIN, HIGH);
//                  digitalWrite(Solenoid, LOW);
//                 // isDone = false;
//            }
//            //debug
//             Serial.println("3. result = "+res);
//          }
//          
//          // if N seconds have passed since your last connection,
//          // then connect again and send data:
//          if (millis() - lastConnectionTime > postingInterval){
//              httpRequest();
//          }
}


