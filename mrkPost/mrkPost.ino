#include <ArduinoHttpClient.h>
#include <WiFi101.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// Wifi Settings ///////
char ssid[] = "oysterYS";
char pass[] = "52005200ys";


char serverAddress[] = "todayrecycle.com";  // server address
int port = 80;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;
String response;
int statusCode = 0;


#define SENS  4  // Door sensor
// constants 
const int buttonPin = 2;     // the number of the pushbutton pin
const int Solenoid =  8;      // the number of the Solenoid pin
const int TPin = 3;     // the number of the trigger pin

// variables 
int buttonState = 0;         // variable for reading the pushbutton status
int sensorState = 0; // variable for reading the Door Sensor status

int cnt =0;


void setup() {
  Serial.begin(9600);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  
    // initialize HW setting
  pinMode(SENS, INPUT);
  digitalWrite(SENS, HIGH); 
  pinMode(LED_BUILTIN, OUTPUT); //Button led
  pinMode(Solenoid, OUTPUT); //lock 
  pinMode(buttonPin, INPUT); //push btn
  digitalWrite(buttonPin, HIGH); //push led btn
  pinMode(TPin, INPUT); //door 
  digitalWrite(TPin, HIGH);

  
}

void loop() {
  status = WiFi.begin(ssid, pass);
  Serial.println("making POST request");
  String contentType = "application/x-www-form-urlencoded";
  String postData = "name=Alice&age=12";
  client.post("/todayrecycle/wifi/qrbox.jsp", contentType, postData);
  // read the status code and body of the response
  statusCode = client.responseStatusCode();
  response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  Serial.println("Wait five seconds");
  delay(1000);
  if(statusCode == 200){
      cnt ++;
    if(response == "1"){
      ledOn();
    }else{
      ledOff();
    }
  }
}

void ledBlink(){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(100);                       // wait for a second
}
void ledOn(){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.println("led ON");
}
void ledOff(){
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
}
void lockClose(){
 digitalWrite(Solenoid, LOW);
}
void lockOpen(){
 digitalWrite(Solenoid, HIGH);
 delay(2500);
}

