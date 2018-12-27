
//Door sensor
#define SENS  4  // Door sensorf
int sensorState = 0; // variable for reading the Door Sensor status

void setup() {
  // put your setup code here, to run once:
  // initialize Door sensor as input:
  pinMode(SENS, INPUT);
  digitalWrite(SENS, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorState = digitalRead(SENS); // read the Door sensor status
 Serial.println(sensorState);
}
