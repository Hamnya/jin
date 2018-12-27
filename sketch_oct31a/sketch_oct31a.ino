#include <HID.h>


/*   ******************************  www.electroingenio.com **********************************
 
/*
    USB BAR CODE SCANNER USB HOST SHIELD TO DISPLAY BARCODE ON LCD
            ADAPTED USING OLEG MAZUROV ARDUINO CODE
 
  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 6
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
*/
#include <hiduniversal.h>                  //Add to Oleg Mazurov code to Bar Code Scanner
#include <usbhub.h>

#include <avr/pgmspace.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <hidboot.h>
#define DISPLAY_WIDTH 16
 
//initialize the LCD library with the numbers of the interface pins//

 
USB     Usb;
USBHub     Hub(&Usb);                                          //I enable this line
HIDUniversal      Hid(&Usb);                                  //Add this line so that the barcode scanner will be recognized, I use "Hid" below 
HIDBoot<HID_PROTOCOL_KEYBOARD>    Keyboard(&Usb);
 
class KbdRptParser : public KeyboardReportParser
{
        void PrintKey(uint8_t mod, uint8_t key);             // Add this line to print character in ASCII
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
 
    Serial.print( (char)key );      //Add char to print correct number in ASCII
};
 
KbdRptParser Prs;
 
void setup()
{
    Serial.begin( 115200 );
    Serial.println("Start");
 
    if (Usb.Init() == -1) {
        Serial.println("OSC did not start.");
    }
 
    delay( 200 );
 
    Hid.SetReportParser(0, (HIDReportParser*)&Prs);        //Here I change  "Keyboard" for "Hid"
    // set up the LCD's number of columns and rows: 
    delay( 200 );
}
 
void loop()
{
  Usb.Task();
}
