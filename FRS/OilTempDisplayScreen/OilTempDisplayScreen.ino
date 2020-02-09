// Includes
#include <avr/wdt.h> // Watchdog security
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "./CANHelperLibrary/Connector/MCPCAN/CMCPCANConnector.h"
#include "./CANHelperLibrary/Modules/TOYOTA/FRS/COilDisplay.h"


// Declare the specific connector we want to use
CAN::CMCPCANConnector S_CAN ;

// A generic message used for read
CAN::CReadCANFrame F_READ_DATA ;

// Modules to use
FRS::COilDisplay S_OIL_DISPLAY(S_CAN, F_READ_DATA) ;


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

//Creation of the display entity
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/*****************************************************************************/
void setup()
{
	// Open log
	OPEN_LOG() ;
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    display.display();
    DELAY(2000);
    display.clearDisplay();

  // Wait some time for initialization to be always correct (if powered with ACC or IGN only)
  DELAY(5000) ;

	// While CAN initialization fails
	while (!CAN::InitializeCAN(S_CAN, &FRS::FiltersAndMasksConfiguration))
	{
		// Tell user
		PRINTLN_STR("CAN initialization failed") ;

		// Wait some time before trying again
		DELAY(100) ;
	}

	// Ok, everything seems good
	PRINTLN_STR("CAN initialization OK") ;

}

void loadbar_update(Adafruit_SSD1306 display, int bar_min, int bar_max, int bar_value){
              display.clearDisplay();
             // display.drawBitmap(0, 0, oil_logo, 48, 48, 1,0);
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



/*****************************************************************************/
void loop()
{
	// Setup the watchdog
	wdt_enable(WDTO_8S) ;

	// Update oil show module
 S_OIL_DISPLAY.Update(MILLIS()) ;
 loadbar_update(display,49,128,S_OIL_DISPLAY.GetCurrentOilTemperature());

/*

	// No errors occured on CAN ?
	if (!S_CAN.HasError())
	{
		// Reset watchdog
		wdt_reset() ;
	}
	else
	{
		// Reset watchdog
		wdt_reset() ;

		// Tell user which error it is
		PRINTLN_STR("Error in CAN, restarting the whole thing");
		PRINTLNHEX(S_CAN.GetError()) ;

		// Restart in 15ms
		wdt_enable(WDTO_15MS) ;

		// Wait :(
		while (true) ;
	}*/

}
