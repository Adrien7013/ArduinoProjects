

#include <avr/wdt.h> // Watchdog security
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <mcp_CAN.h>
#include "./CANHelperLibrary/Connector/MCPCAN/CMCPCANConnector.h"
#include "./CANHelperLibrary/Modules/TOYOTA/FRS/COilDisplay.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



static const unsigned char PROGMEM oil_logo[] =
{
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00,
0x00, 0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x09, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x80, 0x00,
0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F,
0x80, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00,
0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00,
0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3F,
0xC0, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xE0, 0x00, 0x00, 0x00,
0x00, 0x7F, 0x60, 0x00, 0x18, 0x00, 0x00, 0x5F, 0xE0, 0x00, 0x1C, 0x00, 0xFE, 0x3F, 0xC0, 0x00,
0x3F, 0x03, 0xFF, 0x1F, 0x80, 0x00, 0x3F, 0x87, 0xFF, 0x8F, 0x00, 0x60, 0x7F, 0xE0, 0x38, 0x00,
0x00, 0xF0, 0x7F, 0xF8, 0x38, 0x00, 0x03, 0xF0, 0xFF, 0xFF, 0xFF, 0xE0, 0x0F, 0xF8, 0x3F, 0xFF,
0xF0, 0x30, 0x1F, 0xD8, 0x07, 0xFF, 0xFF, 0x30, 0x7F, 0x80, 0x00, 0xFF, 0xFF, 0x99, 0xFF, 0x18,
0x00, 0x3F, 0xFF, 0xDF, 0xFE, 0x1C, 0x00, 0x3F, 0xFF, 0xDF, 0xFC, 0x3C, 0x00, 0x3F, 0xFF, 0xFF,
0xF8, 0x7E, 0x00, 0x3F, 0xFF, 0xFF, 0xF0, 0x7F, 0x00, 0x3F, 0xFF, 0xFF, 0xE0, 0xFF, 0x00, 0x3F,
0xFF, 0xFF, 0xC0, 0xFF, 0x00, 0x3F, 0xFF, 0xFF, 0x80, 0x7E, 0x00, 0x3F, 0xFF, 0xFF, 0x00, 0x3E,
0x00, 0x37, 0xFF, 0xFE, 0x00, 0x1C, 0x00, 0x30, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xF8,
0x00, 0x00, 0x00, 0x3F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const unsigned char PROGMEM zap_logo[] =
{
0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFC, 0x00,
0x00, 0x00, 0x00, 0x0F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x8E, 0x00, 0x00, 0x00, 0x00, 0xFE,
0x0F, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x07, 0x80, 0x00, 0x00, 0x0F, 0xE0, 0x03, 0x80, 0x00, 0x00,
0x3F, 0x80, 0x03, 0xC0, 0x00, 0x00, 0x7E, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x7C, 0x00, 0x00, 0xE0,
0x00, 0x00, 0x3F, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x78, 0x00, 0x00, 0x07, 0xE0,
0x00, 0x38, 0x00, 0x00, 0x01, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x7E, 0x00, 0xF8, 0x00, 0x00,
0x00, 0x3F, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0xF8, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x7C,
0x00, 0x00, 0x00, 0x03, 0xE0, 0x3E, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0x0F,
0x00, 0x0F, 0x80, 0x00, 0x00, 0x0F, 0x80, 0x07, 0xC0, 0x00, 0x00, 0x07, 0xC0, 0x03, 0xE0, 0x00,
0x00, 0x03, 0xF0, 0x01, 0xF0, 0x00, 0x00, 0x00, 0xFC, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x3E, 0x00,
0xF8, 0x00, 0x00, 0x00, 0x1F, 0x83, 0xF0, 0x00, 0x00, 0x00, 0x07, 0xC3, 0xC0, 0x00, 0x00, 0x00,
0x03, 0xF1, 0xE0, 0x00, 0x00, 0x00, 0x01, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x03, 0xF0, 0xF0, 0x00,
0x00, 0x00, 0x07, 0xC0, 0x78, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x3C, 0x00, 0x00, 0x00, 0x01, 0xF8,
0x1C, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x0F, 0x00, 0x00, 0x00,
0x00, 0x0F, 0xC7, 0x80, 0x00, 0x00, 0x00, 0x03, 0xE3, 0x80, 0x00, 0x00, 0x00, 0x01, 0xFB, 0xC0,
0x00, 0x00, 0x00, 0x00, 0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00,
0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFC, 0x00, 0x00,
0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E
};

const unsigned char gt86_mat [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x1F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x17, 0xC0, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x0F, 0x00, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x3C, 0x1F, 0xF8, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x38, 0x07, 0xFC, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
0x06, 0x18, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x0E,
0x38, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xC0, 0x0C, 0x30,
0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x70, 0x00,
0xE0, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0x80, 0x38, 0x60, 0x00, 0x70,
0x00, 0x00, 0x79, 0x24, 0x92, 0xAA, 0xAA, 0x92, 0x4B, 0x87, 0x00, 0x30, 0xC0, 0x00, 0x30, 0x00,
0x00, 0xE0, 0x00, 0x00, 0x28, 0x00, 0x00, 0x03, 0x07, 0x00, 0x7F, 0xC3, 0xFF, 0x38, 0x00, 0x01,
0x80, 0x00, 0x00, 0x30, 0x00, 0x00, 0x07, 0x06, 0x00, 0x3F, 0x80, 0xFF, 0x38, 0x00, 0x01, 0x8F,
0xFF, 0xFF, 0xDF, 0xFE, 0x1F, 0xFC, 0x1E, 0x00, 0x00, 0x01, 0xC7, 0x18, 0x00, 0x03, 0x07, 0xBB,
0xBB, 0xD5, 0xDC, 0x1B, 0xD8, 0x0E, 0x00, 0x00, 0x01, 0x86, 0x1C, 0x00, 0x02, 0x0C, 0x00, 0x00,
0x00, 0x0C, 0x30, 0x00, 0x0C, 0x00, 0x00, 0x03, 0x87, 0xDE, 0x00, 0x06, 0x0C, 0x00, 0x00, 0x00,
0x08, 0x30, 0x00, 0x0C, 0x0F, 0xFC, 0x03, 0x0F, 0xF8, 0x00, 0x06, 0x18, 0x3F, 0xFF, 0x80, 0x08,
0x60, 0x00, 0x2C, 0x07, 0xFC, 0x06, 0x00, 0x80, 0x00, 0x06, 0x18, 0x3D, 0x6F, 0x80, 0x18, 0x60,
0x00, 0x0C, 0x07, 0x0C, 0x0E, 0x00, 0x00, 0x00, 0x0C, 0x30, 0x60, 0x01, 0x80, 0x10, 0x40, 0x00,
0x1C, 0x06, 0x18, 0x0F, 0xFF, 0xE0, 0x00, 0x0C, 0x30, 0x7F, 0xC3, 0x00, 0x30, 0xC0, 0x00, 0x0C,
0x0E, 0x38, 0x0F, 0xFF, 0xF8, 0x00, 0x08, 0x30, 0x29, 0xC3, 0x00, 0x20, 0xC0, 0x00, 0x0C, 0x1C,
0x30, 0x00, 0x00, 0x1C, 0x00, 0x18, 0x60, 0x01, 0x82, 0x00, 0x20, 0x80, 0x00, 0x2E, 0x18, 0x70,
0x00, 0x00, 0x1C, 0x00, 0x18, 0x75, 0x63, 0x86, 0x00, 0x61, 0x80, 0x00, 0x0E, 0x18, 0x60, 0x00,
0x00, 0x1E, 0x00, 0x10, 0x3F, 0xFF, 0x06, 0x00, 0x41, 0x80, 0x00, 0x06, 0x30, 0xC1, 0xFF, 0x00,
0x18, 0x00, 0x10, 0x0A, 0xAC, 0x0C, 0x00, 0xC3, 0x00, 0x00, 0x16, 0x7F, 0xC0, 0xFF, 0x80, 0x18,
0x00, 0x10, 0x00, 0x00, 0x1C, 0x00, 0x82, 0x00, 0x00, 0x07, 0x3F, 0x80, 0xC3, 0x80, 0x38, 0x00,
0x18, 0x00, 0x00, 0x38, 0x01, 0x86, 0x00, 0x00, 0x07, 0x00, 0x00, 0xC3, 0x00, 0x30, 0x00, 0x1F,
0xFF, 0xFF, 0xF0, 0x01, 0xEC, 0x00, 0x00, 0x03, 0x80, 0x01, 0x87, 0x00, 0x70, 0x00, 0x07, 0xFF,
0xFF, 0xC0, 0x00, 0xF8, 0x00, 0x00, 0x03, 0x80, 0x03, 0x06, 0x00, 0xE8, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x03, 0x0C, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x02, 0xE0, 0x06, 0x1C, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xF0, 0x0E, 0x18, 0x03, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x78, 0x0F, 0xF0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x1C, 0x0F, 0xF0, 0x0E, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x1F, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x07, 0xC0, 0x00, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
0xF8, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xFF,
0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};
const int SPI_CS_PIN = 10;
MCP_CAN CAN_module(SPI_CS_PIN);

// Pin alocation
  int select = 2;
  
  long unsigned int rxId;
  int bar_fill=0;
  unsigned char len = 0;
  unsigned char buf[8];
  int oil_temp = 0;
  long batt_volt = 0;
  bool display_buf=false;
void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  pinMode(select, INPUT);
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN_module.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK){
    Serial.println("MCP2515 Initialized Successfully!");
  }
    
  else{
    Serial.println("Error Initializing MCP2515...");
  }
  
  CAN_module.setMode(MCP_STD);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
 buf[3]=30;


}



void loadbar_update(Adafruit_SSD1306 display, int bar_min, int bar_max, int bar_value){
              display.clearDisplay();  
              display.drawBitmap(0, 0, oil_logo, 48, 48, 1,0);
              display.setTextSize(4); // Draw 2X-scale text
              display.setTextColor(SSD1306_WHITE);
              display.setCursor(50, 10);
              display.println(bar_value,DEC);
              display.setCursor(100, 5);
              display.setTextSize(2); // Draw 2X-scale text
              display.println(F("C"));
              display.drawRoundRect(0,48,128,15,4,1);
              display.fillRoundRect(0,48,50,15,4,1);

}
void zap_update(Adafruit_SSD1306 display, int bar_min, int bar_max, int bar_value){
              display.clearDisplay();        
              delay(1);
              display.drawBitmap(0, 0, zap_logo, 48, 48, 1,0);
              display.setTextSize(4); // Draw 2X-scale text
              display.setTextColor(SSD1306_WHITE);
              display.setCursor(50, 10);
              display.println(bar_value,DEC);
              display.setCursor(100, 5);
              display.setTextSize(2); // Draw 2X-scale text
              display.println(F("V"));
              display.drawRoundRect(0,48,128,15,4,1);
              display.fillRoundRect(0,48,50,15,4,1);

}

//void gauge_update(Adafruit_SSD1306 display, int bar_min, int bar_max, int bar_value){
//              display.clearDisplay();
//              display.drawBitmap(0, 0,logo, 48, 48, 1,0);
//              display.setTextSize(4); // Draw 2X-scale text
//              display.setTextColor(SSD1306_WHITE);
//              display.setCursor(50, 10);
//              display.println(bar_value,DEC);
//              display.setCursor(100, 5);
//              display.setTextSize(2); // Draw 2X-scale text
//              display.println(F("C"));
//              display.setTextColor(SSD1306_BLACK);
//              display.drawRoundRect(0,49,128,15,4,1);
//              display.fillRoundRect(0,49,50,15,4,1);
// }

void loop() {
  

        CAN_module.readMsgBuf(&rxId, &len, buf);    // read data,  len: data length, buf: data buf
        delay(50);
        if(rxId==0x001){
         zap_update(display,49, 128, buf[3]);
         display.display();
        }
        else{
         loadbar_update(display,49, 128, buf[3]);
         display.display();
          }
}