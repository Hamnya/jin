 // 2018/06/01 Hanna' Test
#include <HID.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <Usb.h>
#include <usbhid.h>
#include <hidboot.h>

USB Usb; 
HIDUniversal Hid(&Usb);


#include <TFT_ILI9163C.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <qrcode.h>

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0 
#define WHITE 0xFFFF
#define __CS 4
#define __DC 3


QRCode qrcode;

TFT_ILI9163C display = TFT_ILI9163C(__CS,2,__DC); //digital pin setting;

char barcode[13] = {'A','A','A','A','A','A','A','A','A','A','A','A','A'}; //save barcdoe number
char realCode[] = "1234567890123";
int cnt = 0;
 

void initLCD()
{

  display.clearScreen();
  display.setCursor(0,0);
  display.print("start!!!!!!!!!  ><");
  delay(500);
  display.clearScreen();
  display.fillRect(0, 0, display.width(),display.height(), WHITE);
  
}

void setQR(char* text)
{

    display.clearScreen();
    display.fillRect(0, 0, display.width(),display.height(), WHITE);
  
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, text);  
    for (uint8_t y = 0; y < qrcode.size; y++) {
        // Each horizontal module
         for (uint8_t x = 0; x < qrcode.size; x++) {
               if(qrcode_getModule(&qrcode, x, y)) {
                    //TFTscreen.rect(5+(x*4), 10+(y*4), 4, 4);
                    display.fillRect(5+(x*4), 10+(y*4),4,4, BLACK);
                }
        }
    }
}
class KbdRptParser : public KeyboardReportParser
{
protected:
virtual void OnKeyDown (uint8_t mod, uint8_t key);
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
if(key != 0x13)
{
   if(cnt < 13){
    barcode[cnt] = (char)key;
   }
   cnt++;
}
else
{


  memcpy(realCode, barcode,13);
  setQR(realCode);
  cnt = 0;
}
//Serial.print( (char)key ); //print barcode number.
};



KbdRptParser Prs;

void setup() {

  
// put your setup code here, to run once:
  Serial.begin( 115200 );
  display.begin();
  initLCD();
  
  while (!Serial); Serial.println("Start");
  if (Usb.Init() == -1) Serial.println("OSC did not start.");
  
  delay( 200 );
  Hid.SetReportParser(0, (HIDReportParser*)&Prs);

  
}
void loop() {
// put your main code here, to run repeatedly:
Usb.Task();
}
