//-------------------WIFI---------------------------------
String AP = "oysterYS";       // CHANGE ME
String PASS = "52005200ys"; // CHANGE ME
String HOST = "54.180.144.77";
String PORT = "5254";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
int bar_status;
int bar_length;

//--------------------Barcode----------------------------
char user_barcode[14] ={};
char barcode[15] = {};
char barcode_rec[20][15] = {}; // 1~20번째로 저장된 상품 Barcode 정보
int bar_cnt = 0;
int bar_mode = 2; // Barcode Mode(1:회원코드가 찍혔을 때, 2: 상품바코드가 찍혔을 때)
//-------------------Weight Sensor------------------------
#include "HX711.h" //HX711로드셀 엠프 관련함수 호출
#define calibration_factor -7049.9 // 로드셀 스케일 값 선언 -7049.9
#define DOUT  23 //엠프 데이터 아웃 핀 넘버 선언
#define CLK  22  //엠프 클락 핀 넘버  
HX711 scale(DOUT, CLK); //엠프 핀 선언
int weight = 0;
int total_weight = 0;
int weight_rec[20] = {0}; // 1~20번째로 저장된 무게 정보
int bottle_cnt = 0; // 현재 보관함에 놓여있는 병의 갯수
double Slot_ratio[20] = {1}; // test // 각 슬롯별로 실험적으로 실무게와의 차이를 측정하여
                                     // 각 가중치를 아래와 같이 두어 오차를 줄이고자 하였음
//double Slot_ratio[20] = {0.89672432873,0.85834218561,0.85005544899,1.072162091,1.0066141999,
//                        0.9377254984,1.3205422357,1.1657858963,1.0072053482};
//-----------------Switch----------------------------------
boolean pin[20] ={LOW};
boolean pin_usg[20] = {LOW};
int pin_num[20] =  {24,25,26,27,28,
                    29,30,31,32,33,
                    34,35,36,37,38,
                    39,40,41,42,43};
int pin_rec[20] = {0}; // 1~20번째로 들어온 각 slot의 번째 수 
int pin_cnt =0;
//----------------ETC---------------------------------------
String order[20]= {"first","second","third","Fourth","Fifth",
                   "Sixth","Seventh","eighth","Ninth","tenth",
                   "eleventh","Twelfth","thirteenth","Fourteenth","fifteenth",
                   "Sixteenth","Seventeenth","Eighteenth", "Nineteen", "Twentieth" };
                   
char check[] = {"check"}; // Serial로 check를 입력할 경우 보관함의 현재 배출정보를 모니터로 출력


void Weight(int slot,int mode) // Weight func
{
  if(mode==1){ // 병이 배출되었을 경우
   int val = (453.592*(scale.get_units()))-total_weight; // found -> g으로 변환하기위해 453.592를 곱해줌
   Serial.print("This bottle`s weight: "); Serial.print(val); Serial.println("g"); // 58g 오차발생 // g
   total_weight += val; // 총무게에 현재병의 무게를 더해줌
   weight_rec[bottle_cnt] = val; // 현재병의 무게를 해당 변수에 기록
   bottle_cnt++; // 보관함 내의 병의 개수 +1
   pin_rec[pin_cnt] = slot; // 눌려져있는 핀의 정보를 기록
   pin_cnt++; // 눌려져있는 핀의 개수 +1 // bottle_cnt와 동일한 점 확인 // 추후 변경해야함
   
   Serial.print("Total weight: "); Serial.print(total_weight); Serial.println("g"); // 58g 오차발생 // g

  }
  else if(mode==2){ // 병의 회수되었을 경우
    Serial.print("Removed bottle`s weight: "); Serial.print(weight_rec[bottle_cnt-1]); Serial.println("g");
    total_weight -= weight_rec[bottle_cnt-1]; // 총 무게에서 회수된 병의무게를 빼줌
    Serial.print("Total Weight: "); Serial.print(total_weight); Serial.print("\n"); 
    weight_rec[bottle_cnt-1] = 0; // 이전에 기록된 병의 무게에 0을 입력
    bottle_cnt--; // 병의 개수 -1
    pin_rec[pin_cnt-1] = 0; // 바로 이전에 눌린 핀의 정보 0으로 변경 
    pin_cnt--;
    // 병을 회수하였을 때 어떤 슬롯이 눌렸는지 확인하여 핀의 정보를 변경하여야하나 바로이전에 투입된 병의 정보를 토대로 변경
    // 이에 대한 코드 수정이 필요함
  }
  else{ // 병이 투입,회수된게 아닌 다른 값으로 Mode가 입력되었을 경우 오류메시지 출력
    Serial.print("Weight Mode: ");Serial.print(mode); Serial.println();
    Serial.println("Weight Mode Error!");
  }
}


void Barcode() // Barcode func
{
      char a = (char)Serial3.read(); // WIFI RX,TX를 Arduino Mega의 RX,TX핀에 연결하여 인식
      if(bar_cnt==0){//bar_cnt는 데이터가 보드로 넘어온 후 첫번째로 입력되는 문자
        if(a == 'A'){//회원코드는 'A'로 시작하는 QR코드
          bar_mode = 1; // bar_mode:1 -> 회원바코드
        }
        else{
          bar_mode = 2; // bar_mode:2 -> 상품바코드
        }
      }
      
      if(a !='\n'){ // 데이터가 보드로 넘어와 모든 데이터를 넘겨주었을 때 출력하는 마지막 문자인 '\n'이 아닌 값이 들어오면
          if(bar_mode==1){
              user_barcode[bar_cnt] = a; 
          }
          else if(bar_mode==2){
            barcode[bar_cnt] = a;
          }
          else{ // bar_mode가 1,2가 아닌 값이 들어올 경우 오류메시지 출력
          Serial.println("Barcode mode Error!_1");
          }
          bar_cnt++;
       }
      else{ // 마지막 문자를 입력받았을 경우
        
        if(bar_mode==1){
          if(bar_cnt!=13){
            for(int i=bar_cnt; i<13;i++){
              user_barcode[i] = 'A'; // 13자리보다 작은 값으로 회원코드가 입력되었을 경우 뒤에 'A'를 덭붙여써줌
            }
          }
          for(int i=0;i<14;i++){
            Serial.print(user_barcode[i]);
          }
        }
        else if(bar_mode ==2){
            if(bar_cnt!=14){
            for(int i=bar_cnt; i<14;i++){
              barcode[i] = 'A'; // 14자리보다 작은 값으로 상품바코드가 입력되었을 경우 뒤에 'A'를 덭붙여써줌
            }
          }
          for(int i=0;i<15;i++){
            Serial.print(barcode[i]);
          }
        } 
        
        bar_cnt=0; // 마지막으로 수신된 데이터를 확인할 경우 다시 bar_cnt를 0으로 변경
      }

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
    Serial.println("OYI");
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
void send_data(int mode)
{      
       String _user_barcode;
       String _barcode;
       
       for(int i=0;i<14;i++){
       _user_barcode += user_barcode[i];
       }
       
       if(mode == 1){ // 병이 투입되었을 경우
         for(int i=0;i<15;i++){
         _barcode += barcode[i];
         }
       }
       else if(mode==2){ // 병이 회수되었을 경우
         for(int i=0;i<15;i++){
         _barcode += barcode_rec[pin_rec[pin_cnt-1]][i];
         }
       }
       else{ // mode가 1,2가 아닌 값이 들어올 경우
        Serial.println("Server to data mode Error!");
       }
       String getData; // Server에 보낼 문자열을 저장할 변수 선언
       getData += _user_barcode; // 회원바코드
       getData += ","; // 각 데이터를 ,로 구분
       getData += _barcode; // 상품바코드
       getData += ",";
       getData += (String)pin_rec[pin_cnt-1]; // 놓여진 슬롯정보
       getData += ",";
       getData += (String)weight_rec[bottle_cnt-1]; // 해당하는 병의 무게
       getData += ",";
       getData += (String)total_weight; // 총 무게
       getData += ",";
       getData += (String)mode; // 투입:1, 회수:2
       
       sendCommand("AT+CIPMUX=1",5,"OK");
       sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
       sendCommand("AT+CIPSEND=0," +String(getData.length()),4,">");
       Serial.println(getData);
       delay(100);
       Serial2.println(getData);
       
       delay(1500);countTrueCommand++;
       sendCommand("AT+CIPCLOSE=0",5,"OK");
       getData = "";
}
void Pin(int slot,int mode)
{
  if(mode==1){ // 투입되었을 경우
      delay(1500); // 1.5초 대기
      if(digitalRead(pin_num[slot])==HIGH){ // 다시 해당하는 슬롯이 눌려있는지 확인 
        pin_usg[slot]=HIGH;
        int mode = 1;
        Serial.print("The slot used by the user is ");Serial.println(order[slot]);
        Weight(slot,mode);
        send_data(mode);
        for(int i=0;i<15;i++){
        barcode_rec[pin_cnt-1][i] = barcode[i];
        }
      }
  }
  else if(mode==2){
      delay(500);
      if(digitalRead(pin_num[slot])==LOW){
       pin_usg[slot]=LOW;
       Serial.print("User took the "); Serial.print(order[slot]); Serial.print(" empty bottle.\n");
       int mode = 2;
       send_data(mode);
       Weight(slot,mode);
       for(int i=0;i<15;i++){
          barcode_rec[pin_cnt-1][i] = "";
       }
      }
  }
  else{
    Serial.println("Pin mode Error!");
  }
}
void Check()
{
    int check_cnt=0;
    String inString = Serial.readStringUntil('\n');
    for(int i=0;i<5;i++){
      if(inString[i]==check[i]){
        check_cnt++;
      }
    }
    if(check_cnt==5){
      for(int j=0;j<pin_cnt;j++){
      Serial.print(order[pin_rec[j]]); Serial.print(" slot`s weight is "); Serial.print(weight_rec[j]); Serial.print("g\n");
      }
    }
}

void setup() {
  Serial.begin(9600);  // 시리얼 통신 개방
  Serial.println("Serial begin OK");
  Serial2.begin(9600);
  Serial.println("WIFI begin OK");
  Serial3.begin(9600);
  Serial.println("Barcode begin OK");
  delay(500);

  scale.set_scale(calibration_factor);  //스케일 지정 
  scale.tare();  //스케일 설정
  for(int i=24;i<44;i++){
    pinMode(i, OUTPUT);
  }
  
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  
  Serial.println("IOT empty bottle collector TEST");
  
}
void loop() {
  for(int i=0;i<20;i++){
    pin[i] = digitalRead(pin_num[i]);
    delay(1);
    if(pin[i]==HIGH) {
      if(pin_usg[i]==LOW){
      Pin(i,1);
      }
    }
    else{
      if(pin_usg[i]==HIGH){
        Pin(i,2);
        
      }
    }
   }
  if(Serial3.available()) {
    Barcode();
   }
  if(Serial.available()){
    Check();
  }
}
