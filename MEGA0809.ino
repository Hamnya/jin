#include <hidboot.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <HX711.h>
#include <MsTimer2.h>
#include <SoftwareSerial.h>
#include <Adafruit_PWMServoDriver.h>
//#include <Stepper.h>
#include <EEPROM.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

SoftwareSerial mySerial(4, 5);  //rx,tx
HX711 scale(3,2); //dout,sck
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
//Stepper step1(200, 6, 8, 7, 9);   //a,b,a-b-

unsigned char SysCnt=0,BottleIDCpr=0;
unsigned int SysTimerCnt=0;
//char uuiddg[] = "FFFFFFFF";
char uuiddf[] = "11111111";
char uuid1[] = "11111111";
char uuid2[] = "11111111";
char uuid3[] = "11111111";
char uuid4[] = "11111111";

byte Timer1mSecEvt=0;
unsigned int TimerCnt=0;
char Timer1SecCnt=0;

unsigned char BottlePosition=0,BottlePositionBu=0;
unsigned char BottleCpr=0;   //'1'=맥주 큰거 , '2'=맥주 작은거, '3'=소주  - 로드셀 측정
unsigned char BottleID=0;    //'1'=맥주 큰거 , '2'=맥주 작은거, '3'=소주  - 바코드 측정
unsigned char BottleIdBu=0;
char BottleBf[15]={'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'};
unsigned char BottleCnt=0;
char Bottle[21][15] ={
  //맥주 대
  "88005317AAAAA",  //오비맥주 카스후레쉬-1
  "8801119762108",  //하이트맥주 드라이피니시D
  "88008615AAAAA",  //하이트맥주 하이트프라임
  "88008806AAAAA",  //오비맥주 오비골든라거
  "8801119831101",  //하이트맥주 하이트
  "8801858044688",  //카스후레쉬-6

  //맥주 소
  "88005324AAAAA",  //오비맥주 카스후레쉬 -7
  "88005331AAAAA",  //오비맥주 카스후레쉬
  "8801119262202",  //하이트맥주 프라임맥스
  "8801119762207",  //하이트맥주 드라이피니시D 
  "8801030999096",  //롯데칠성음료 클라우드
  "8801858044701",  //롯데칠성음료 클라우드-12

  //소주
  "8801048311002",  //하이트진로 이슬톡톡복숭아 -13
  "8801048941001",  //하이트진로(주) 참이슬클래식
  "8801152134795",  //롯데주류 처음처럼
  "8801100129729",  //무학 좋은데이
  "8801048921829",  //하이트진로 자몽에이슬 
  "8801048951000",  //하이트진로 자몽에이슬 -18  
  "8801048951017",  //참이슬 
  "8801152135235",  //처음처럼
  "8801048921010",  //자몽의이슬 -21
};

//--------------------------------------------------------------------------------------------------------------------------------------
class KbdRptParser : public KeyboardReportParser
{
    void PrintKey(uint8_t mod, uint8_t key);
  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);
    void OnKeyDown	(uint8_t mod, uint8_t key);
    void OnKeyUp	(uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  MODIFIERKEYS mod;
  *((uint8_t*)&mod) = m;
  //Serial.print((mod.bmLeftCtrl   == 1) ? "C" : " ");  Serial.print((mod.bmLeftShift  == 1) ? "S" : " ");
  //Serial.print((mod.bmLeftAlt    == 1) ? "A" : " ");  Serial.print((mod.bmLeftGUI    == 1) ? "G" : " ");
  //Serial.print(" >");  PrintHex<uint8_t>(key, 0x80);  Serial.print("< ");
  //Serial.print((mod.bmRightCtrl   == 1) ? "C" : " ");  Serial.print((mod.bmRightShift  == 1) ? "S" : " ");
  //Serial.print((mod.bmRightAlt    == 1) ? "A" : " ");  Serial.println((mod.bmRightGUI    == 1) ? "G" : " ");
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  //Serial.print("DN "); PrintKey(mod, key);
  uint8_t c = OemToAscii(mod, key);
  if (c)   OnKeyPressed(c);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;
  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;
  //if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {    Serial.println("LeftCtrl changed");  }
  //if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {    Serial.println("LeftShift changed");  }
  //if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {    Serial.println("LeftAlt changed");  }
  //if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {    Serial.println("LeftGUI changed");  }
  //if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {    Serial.println("RightCtrl changed");  }
  //if (beforeMod.bmRightShift != afterMod.bmRightShift) {    Serial.println("RightShift changed");  }
  //if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {    Serial.println("RightAlt changed");  }
  //if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {    Serial.println("RightGUI changed");  }
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  //Serial.print("UP ");  PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
  //Serial.print("ASCII: ");
  if(key==0x13){
    int i,j,cnt;
    //if(BottleID==0){
      for(j=0; j<21; j++){    cnt=0;
        for(i=0; i<13; i++){
           if(Bottle[j][i]==BottleBf[i]) cnt++;
           if(cnt==13){   BottleIdBu=j;
              if(j<=5) BottleID='1';
              else if(j<=11) BottleID='2';
              else if(j<=20) BottleID='3';
              Serial.print("BottleKind:"); Serial.println(BottleID);   //'1'=맥주 큰거 , '2'=맥주 작은거, '3'=소주 
              i=13; j=21;
           }
        }
      }    
   // }
    for(i=0; i<13; i++){  BottleBf[i]='A';   }
    BottleCnt=0;
  }
  else{
    //Serial.print((char)key);
    BottleBf[BottleCnt]=(char)key;  BottleCnt++;
  }
};

USB     Usb;
USBHub     Hub(&Usb);
//HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);
HIDUniversal HidKeyboard(&Usb);
KbdRptParser Prs;

//--------------------------------------------------------------------------------------------------------------------------------------
unsigned char keyBf;
unsigned char keyCnt[4]={0,0,0,0};
unsigned char keyIn[4]={0,0,0,0};
void KeyScan(){
 keyBf= ~PINF;   
 keyBf &= 0x0f;
  if(keyBf&0x01){ //sw1
    if(++keyCnt[0]>250) keyCnt[0]=250;    if(keyCnt[0]==5) keyIn[0]=0x01;   
  }else keyCnt[0]=0;
  if(keyBf&0x02){ //sw2
    if(++keyCnt[1]>250) keyCnt[1]=250;    if(keyCnt[1]==5) keyIn[1]=0x01;   
  }else keyCnt[1]=0;
  if(keyBf&0x04){ //sw3
    if(++keyCnt[2]>250) keyCnt[2]=250;    if(keyCnt[2]==5) keyIn[2]=0x01;  if(keyCnt[2]==200) keyIn[2]=0x03;   
  }else keyCnt[2]=0;
  if(keyBf&0x08){ //sw4
    if(++keyCnt[3]>250) keyCnt[3]=250;    if(keyCnt[3]==5) keyIn[3]=0x01;
  }else keyCnt[3]=0;
}

//--------------------------------------------------------------------------------------------------------------------------------------
byte ledonoffCpr=0,ledonoffBf=0;
void SendLedCtr(char ch, char onoff){  //1,2,3,  9(전체onoff반복)
  if(ch<9){  if(onoff)  onoff=0;  else  onoff=1;  }
  //mySerial.write('a');  mySerial.write(ch+'0');  mySerial.write(onoff+'0');  mySerial.write('b');
  if(ch==1){       ledonoffCpr=0;  digitalWrite(12,onoff);  }
  else if(ch==2){  ledonoffCpr=0;  digitalWrite(11,onoff);  }
  else if(ch==3){  ledonoffCpr=0;  digitalWrite(13,onoff);  }  //digitalWrite(10,onoff);
  else if(ch==9){  
    ledonoffCpr=onoff;
    if(onoff==0){
      digitalWrite(12,1);  digitalWrite(11,1);  digitalWrite(13,1);
     }
  }
}
//next_time = millis() + 5000;

//--------------------------------------------------------------------------------------------------------------------------------------
unsigned char GetRdcCpr=0,ttt=0;
void GetRdc(){
  int val;
  if(GetRdcCpr==1){
    val=(int)(scale.getValue()/1000);   //val=scale.getGram();  
    if((val>=213)&&(val<241))  BottleCpr='3';  //소주 
    else if((val>=241)&&(val<268))  BottleCpr='2';  //맥주 소
    else if((val>=268)&&(val<=294))  BottleCpr='1';  //맥주 대    
    //else BottleCpr=0;   if(BottleCpr!=ttt){  ttt=BottleCpr;  Serial.println(BottleCpr);      }
    //Serial.println(val);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void StMotorStop(){
  //step1.step(0);
  digitalWrite(6, 0);  digitalWrite(7, 0);  digitalWrite(8, 0);  digitalWrite(9, 0);
}
//--------------------------------------------------------------------------------------------------------------------------------------

void uuid_mode()
{
    delay(200);
    mySerial.print("AT+IBEA 1");
    delay(200);   
    mySerial.println("AT+RESET");    
    delay(200);   
}
void uuid_change()
{
    delay(200);
    mySerial.print("AT+IBE0");
    mySerial.println(uuid1);
    delay(200);
    mySerial.print("AT+IBE1");
    mySerial.println(uuid2);
    delay(200);
    mySerial.print("AT+IBE2");
    mySerial.println(uuid3);
    delay(200);
    mySerial.print("AT+IBE3");
    mySerial.println(uuid4);
    delay(200);   
    mySerial.println("AT+RESET");    
}
void uuid_reset()
{  
  memcpy(uuid1, uuiddf, 8);
  memcpy(uuid2, uuiddf, 8);
  memcpy(uuid3, uuiddf, 8);
  memcpy(uuid4, uuiddf, 8);
  uuid_change();
}
void uuid_test()
{  
  memcpy(uuid1, "FFFFFFFF", 8);
  memcpy(uuid2, "FFFFFFFF", 8);
  memcpy(uuid3, "FFFFFFFF", 8);
  memcpy(uuid4, "FFFFFFFF", 8);
  uuid_change();
}
//--------------------------------------------------------------------------------------------------------------------------------------

void Action(){
  switch(SysCnt){
    case 1: SysCnt++;   //바코드 인식 후 Led 전체 OnOff 반복 동작 
      SendLedCtr(9,1); SysTimerCnt=600;  //60sec
      GetRdcCpr=0;  BottleCpr=0;   keyIn[3]=0;
      break;
    case 2:    //좌측스위치인식
      if(SysTimerCnt==0){  //TimeOut
        SysCnt=249; 
      }else{
           if(keyIn[3]==0x01){ keyIn[3]=0;      SysCnt++;  SysTimerCnt=50;  }
      }
      break;
    case 3:  if(SysTimerCnt==0){ SysCnt++;  SysTimerCnt=600; GetRdcCpr=1; }   break;
    case 4: //무게 측정 후 바코드와 일치하는 지 비교 
      if(BottleCpr){
        if(BottleCpr==BottleID){    SysCnt++;  SendLedCtr(9,0);  }
        else SysCnt=249;
        GetRdcCpr=0;
      }
      if(SysTimerCnt==0){ SysCnt=249;  GetRdcCpr=0;  }
      break;
    case 5:  SysCnt++; //서보동작
       if(BottleID=='1'){ //맥주 대
         SendLedCtr(3,1); 
       }
       else if(BottleID=='2'){ //맥주 소
          SendLedCtr(2,1);   pwm.setPWM(3, 0, 125);
       }
       else if(BottleID=='3'){ //소주 넣은 장소 저장 
         SendLedCtr(1,1);
         pwm.setPWM(BottlePosition, 0, 125);  BottlePositionBu=BottlePosition;
         BottlePosition++;   if(BottlePosition>2)  BottlePosition=0; 
         EEPROM.write(BottlePosition, 10);     
       }else SysCnt=249;
       SysTimerCnt=1800;  //3min
       keyIn[0]=0;  keyIn[1]=0;  keyIn[2]=0;  
       break;
    case 6:  //해당 병의 스위치 판단 
      if(BottleID=='1'){ //맥주 대
          if(keyIn[2]==0x01){ keyIn[2]=0; SysCnt++;  }
       }
       else if(BottleID=='2'){ //맥주 소
          if(keyIn[1]==0x01){ keyIn[1]=0; SysCnt++;  }
       }
       else if(BottleID=='3'){ //소주 넣은 장소 저장 
          if(keyIn[0]==0x01){ keyIn[0]=0; SysCnt++;  }
       }
      if(SysTimerCnt==0) SysCnt=249;
      break;
    case 7: SysCnt++; //스테핑 모터 덮게 open
      //step1.step(-4000);  StMotorStop(); SysTimerCnt=20;  //-4000
      digitalWrite(7,0);  digitalWrite(6,1);  SysTimerCnt=18;  Timer1SecCnt=0; 
      break;
    case 8:  if(SysTimerCnt==0){ SysCnt++; digitalWrite(7,0);  digitalWrite(6,0);  }   break;  
    case 9:  SysCnt++; 
      //step1.step(4000);  StMotorStop(); SysTimerCnt=2;
      digitalWrite(7,1);  digitalWrite(6,0);  SysTimerCnt=16;  Timer1SecCnt=0; 
      break;
    case 10:  if(SysTimerCnt==0){ SysCnt++; digitalWrite(7,0);  digitalWrite(6,0);  }   break;    
    case 11:   SysCnt++;  //서보모터 원위치 
      if(BottleID=='2'){ //맥주 소
          pwm.setPWM(3, 0, 65);
       }
       else if(BottleID=='3'){ //소주 넣은 장소 저장 
         pwm.setPWM(BottlePositionBu, 0, 65);
       }
      break;
    case 12: SysCnt++;  //beacon
      //BottleIdBu=1;  //TEST
      memcpy(uuid1, "1F00001F", 8);  
      memcpy(uuid2, "00000F12", 8);
      if(BottleID=='1')  uuid2[4]='1';
      else if(BottleID=='2')  uuid2[3]='1';
      else if(BottleID=='3')  uuid2[BottlePositionBu]='1';
      unsigned char i;
      memcpy(uuid3, "00F00000", 8);
      for(i=0; i<5; i++){   uuid3[3+i]=Bottle[BottleIdBu][i];     }
      for(i=0; i<8; i++){   uuid4[i]=Bottle[BottleIdBu][5+i];     }
      uuid_change();
      SysTimerCnt=50; 
      break;
    case 13:  if(SysTimerCnt==0){ SysCnt++;   }   break;         
    case 14:   //LED OFF
      uuid_reset();
      SysCnt=249;   //Serial.println("a"); 
      break;
      
    case 249:  SysCnt++;
       SendLedCtr(9,0);  
      break;  
    case 250:  SysCnt++;
      BottleID=0;  BottleIdBu=0;  
      break;
    default:   
      
      break;
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------


void setup()
{ 
  Serial.begin( 9600 );  //9600
#if !defined(__MIPSEL__)
  while (!Serial); 
#endif
  //Serial.println("Start");
  if (Usb.Init() == -1)  Serial.println("OSC did not start.");
  delay( 200 );  
  HidKeyboard.SetReportParser(0, &Prs);
  scale.setOffset(8338000);  scale.setScale(-122.0);
 // DDRF &= 0xF0;
  MsTimer2::set(10,timerISR);  MsTimer2::start();
//  PORTf |= 0x30;
  mySerial.begin(9600);
  pwm.begin();  pwm.setPWMFreq(1);
  pwm.setPWM(0, 0, 65);  //down65  up125  ; pwm.setPWM(servonum, 0, SERVOMIN);
  pwm.setPWM(1, 0, 65);
  pwm.setPWM(2, 0, 65);
  pwm.setPWM(3, 0, 65);
  pinMode(6,OUTPUT);  pinMode(7,OUTPUT);  pinMode(8,OUTPUT);  pinMode(9,OUTPUT);
 
  //step1.setSpeed(100);   // 회전속도 (분당 18회 회전)   2000->1cm
  //StMotorStop();  
  BottlePosition= EEPROM.read(10);   if(BottlePosition>2) BottlePosition=0;
  
 
  uuid_mode();  uuid_reset();   
  pinMode(12,OUTPUT); pinMode(11,OUTPUT); pinMode(13,OUTPUT);
  SendLedCtr(9, 0); 
  pinMode(7,OUTPUT); pinMode(6,OUTPUT);
}

unsigned char test=0,bf=0;
void loop()
{
  Usb.Task();   //바코드 
  GetRdc();     //무게측정
  //delay(500); 
  if(keyIn[2]==0x03){  keyIn[2]=0;
    if(test==0){ test=1;  SendLedCtr(1, 1);  SendLedCtr(2, 1);  }    
    else{ test=0;   SendLedCtr(1, 0);  SendLedCtr(2, 0); }
  }else{
    if(test){
      /*
       char keyBf= ~PINF;    keyBf &= 0x0f;
       if(keyBf==0x02){       digitalWrite(7,1);  digitalWrite(6,0);     } 
       else if(keyBf==0x01){  digitalWrite(7,0);  digitalWrite(6,1);     } 
       else { digitalWrite(7,0);  digitalWrite(6,0); }
       */
    }
  }
  
  if(test==0){
    if(BottleID){
        if(BottleIDCpr==0){ BottleIDCpr=1;  SysCnt=1;  }
    }else BottleIDCpr=0;
    Action();
  }

  if(Timer1mSecEvt){  Timer1mSecEvt=0;
     if(++TimerCnt>=50){  TimerCnt=0;
        if(ledonoffCpr){
            if(ledonoffBf) ledonoffBf=0;  else ledonoffBf=1;
            digitalWrite(12,ledonoffBf);  digitalWrite(11,ledonoffBf);  digitalWrite(13,ledonoffBf);   
            //Serial.println("ttt");          
        }
     }
  }

  /*for(int i=0; i<4; i++){
    if(keyIn[i]){  keyIn[i]=0;
      Serial.print("SW:"); Serial.println(i); 
      if(i==0){       pwm.setPWM(0, 0, 125);  }
      else if(i==1){  pwm.setPWM(0, 0, 65); }
      else if(i==2){  SendLedCtr(9,1);    }
      else if(i==3){  SendLedCtr(9,0);     }
    }  
  }*/

  //if(bf){ bf=0;     }
  //delay(500);   Serial.println("ttt");  
}


void timerISR(){
    KeyScan();   Timer1mSecEvt=1;
    if(++Timer1SecCnt>=10){  Timer1SecCnt=0;   
      if(SysTimerCnt) SysTimerCnt--;
    }
}

