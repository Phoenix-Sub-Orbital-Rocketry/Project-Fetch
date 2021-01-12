/**************************************************************************
This is the current GUI code for the Fetch Tracking system

Written by Christopher Texler
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int RSSi; 	// declare global


#define LOGO_HEIGHT   32
#define LOGO_WIDTH    64
static const unsigned char PROGMEM logo_bmp[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x0f, 0xc0, 0x00, 
  0x00, 0x7f, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0xf9, 0x00, 
  0x0e, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0xfd, 0xc0, 0x1f, 0x7f, 0xff, 0xff, 0xff, 0xfe, 0xfb, 0xe0, 
  0x3f, 0xfc, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xf0, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0xfd, 0xff, 0xf0, 
  0x5f, 0xfc, 0x00, 0x00, 0x00, 0x3c, 0xff, 0xf8, 0x7f, 0xfd, 0x00, 0x00, 0x00, 0x0a, 0xff, 0xf8, 
  0x7f, 0xfd, 0x80, 0x00, 0x18, 0x06, 0xff, 0xf8, 0x3f, 0xfe, 0xe0, 0x07, 0xf0, 0x1f, 0xff, 0xf0, 
  0x1f, 0xff, 0xff, 0x3f, 0xe3, 0xff, 0xff, 0xe0, 0x07, 0xcf, 0xff, 0x8f, 0xe7, 0xff, 0xcf, 0x00, 
  0x01, 0xff, 0xff, 0xcf, 0xef, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
  0x00, 0x03, 0xfb, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7f, 0xfa, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfb, 0xbc, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1b, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x20, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,};

void setup() {

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    // Clear the buffer
  display.clearDisplay();
  Serial.begin(9600);
  

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }


  drawlogo();    // Draw a splashscreen of the Old Phoenix Sub-Orbital Logo

}

void loop() {
      // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      // received a packet
      Serial.print("Received packet '");

      // read packet
      while (LoRa.available()) {
        Serial.print((char)LoRa.read());
        // read the RSSI, push out serial port for debug and display on OLED
        RSSi = LoRa.packetRssi();
      }
      // print RSSI of packet
      Serial.print("' with RSSI ");
      Serial.println(RSSi);

  }

      drawRSSI();    // Draw the current RSSI value based on a varbile set earlier in the code
      delay(100);
      display.clearDisplay();
      display.display();
}


void drawRSSI(void) {
  display.clearDisplay();
  
  display.setTextSize(2);             // Draw 3X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);             // Start at top-left corner
  display.print("RSSI:");
  display.println(RSSi);

  display.display();
  delay(100);
}



void drawlogo(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(3000);
}
