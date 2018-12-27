String serialResponse = "";
char sz[] = "Here; is some; sample;100;data;1.414;1020";

void setup()
{
  
 Serial.begin(9600);
 Serial.println("im test");
 String a = getValue("@1",'@',0);
 Serial.println(a);
}

void loop()
{
 
}

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

