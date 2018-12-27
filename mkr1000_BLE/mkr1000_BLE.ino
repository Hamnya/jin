
//Serial1 == BLUETOOTH. HM-10's (TX,RX) > MKR's (RX,TX);
String a = "";
void setup()  
{
  Serial.println("HELLO IM MRK1000 WITH HM-10");
  Serial.begin(9600);
  Serial1.begin(9600);
  
}

void loop()
{
    a = "";
    while(Serial1.available()){
        a += (char)Serial1.read();
    }
    
    while(Serial.available()){
      Serial1.print((char)Serial.read());
    }
    
    if(a.length() > 0){
      if(a == "OK+CONN"){
        Serial.println("connected");
      }
      if(a == "OK+LOST"){
        Serial.println("disconnected");
      }
    }
    delay(1000);
}
