
//ref. https://github.com/Burningsweetpotato/arduino/blob/master/barcode.inohttps://github.com/Burningsweetpotato/arduino/blob/master/barcode.ino
#include <HID.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <Usb.h>
#include <usbhid.h>
#include <hidboot.h>


USB     Usb;                              
HIDUniversal      Hid(&Usb);   //barcod6e scanner will be recognized, I use "Hid" below 
char barcode[15] = {};
int cnt = 0;
bool isPrint = true;
bool inputCheck = false;

class KbdRptParser : public KeyboardReportParser
{
  protected:
  virtual void OnKeyDown  (uint8_t mod, uint8_t key);
  virtual void OnKeyPressed(uint8_t key);
};
 
void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)  
{
    uint8_t c = OemToAscii(mod, key);
 
    if (c)
        OnKeyPressed(c);
}
 
/* what to do when symbol arrives */
void KbdRptParser::OnKeyPressed(uint8_t key)  
{

  if(key != 19)
  {
    barcode[cnt] = key;
    cnt++;
  }
  else
  {
    barcode[cnt] = '\n';
    cnt = 0;
  }
    Serial.print( (char)key );      //프린트 바코드
 
};
 
KbdRptParser Prs;

void setup() {
  // put your setup code here, to run once:

    Serial.begin( 115200 );
    
    while (!Serial); Serial.println("Start");
    if (Usb.Init() == -1) {
        Serial.println("OSC did not start.");
    }

    delay( 200 );
    Hid.SetReportParser(0, (HIDReportParser*)&Prs);
     
}

void loop() {
  // put your main code here, to run repeatedly:

  Usb.Task();


}
