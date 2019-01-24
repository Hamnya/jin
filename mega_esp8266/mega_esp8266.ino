String AP = "oysterYS";       // CHANGE ME : WIFI_NAME
String PASS = "52005200ys"; // CHANGE ME : WIFI_PWD
String API = "JY3ND9YVXCJNHHU4";   // CHANGE ME : THINKSPEAK
String HOST = "184.106.153.149"; // 
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false;  
int valSensor = 1;

  
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}
void loop() {
 valSensor = getSensorData();
 //HTTP 통신 
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 Serial2.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}
//테스트를 위한 센서 데이터 
int getSensorData(){
  return random(1000); // Replace with 
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    Serial2.println(command);//at+cipsend
    if(Serial2.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("SUCCESS");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
