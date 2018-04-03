//BEFOREUPLOAD Generate new keys for Blynk auth (SetSpecific.h) once project is
//BEFOREUPLOAD complete since this is going to be a public repo

//BEFOREUPLOAD Comment out the following unless testing.
#define MYDEBUG
#define BLYNK_PRINT Serial

//BEFOREUPLOAD Uncomment the appropriate ESP and comment out the rest.
#define TESTESP
//#define BED
//#define COUCH
//#define TV
//#define TVSTAND
//#define WALLDESK
//#define GLASSDESK

#define FASTLED_INTERRUPT_RETRY_COUNT 0




////////////////////////////////////////////////////////////////////////////////
//INCLUDES                                                                    //
////////////////////////////////////////////////////////////////////////////////
#include "ESP8266WiFi.h"
#include "ESP8266mDNS.h"
#include "WiFiManager.h"
#include "BlynkSimpleEsp8266.h"
#include "TimeLib.h"
#include "WidgetRTC.h"
#include "FastLED.h"
#include "Debug.h"
#include "SetSpecific.h"




////////////////////////////////////////////////////////////////////////////////
//CONFIG                                                                      //
////////////////////////////////////////////////////////////////////////////////
#define SWITCHPIN 		V0 //On/off switch
#define AUTOSWITCHPIN 	V1 //On/off for auto-turn-on LEDs at specified time
#define AUTOTIMEPIN 	V2 //Time to auto-turn-on LEDs
#define BRIGHTNESSPIN 	V3 //Brightness slider (range 0-255 in Blynk app)
#define MICPIN 			V4 //Mic sensitivity (range 0-255 in Blynk app)
#define SPEEDPIN 		V5 //Animation speed (range 0-255 in Blynk app)
#define EFFECTPIN 		V6 //Effect selection drop-down menu
#define RGBPIN 			V7 //ZeRGBa (set to "merge" in Blynk app)
#define ESPTIMEPIN 		V8 //Update the Blynk app with current ESP time
#define LCDPIN 			V9

#define DATAPIN			D5
#define COLORORDER		RGB
#define CHIPSET			WS2812B
#define MAXVOLTAGE		5
#define MAXAMPS			900 //Units in milliamps




////////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES                                                            //
////////////////////////////////////////////////////////////////////////////////
bool onOff; //True = LEDs on
bool autoOnOff; //True = LEDs will automatically turn on at specified time
bool autoOnOffTime; //True = Time set for LEDs to turn on automatically
bool effectChange; //True = Effect was changed since last loop
bool stopCurrentEffect; //True = Stop current effect to load new paramenters

uint8_t selectedEffect = 0; //Store currently selected effect from Blynk
uint8_t brightness = 0; //Range is 0-255
uint8_t micSensitivity = 0; //Range is 0-255
uint8_t animationSpeed = 0; //Range is 0-255
uint8_t currentRed = 0; //Range is 0-255
uint8_t currentGreen = 0; //Range is 0-255
uint8_t currentBlue = 0; //Range is 0-255

WidgetRTC clock;
WiFiManager wifiManager;
struct CRGB leds[NUMLEDS];

//When a command needs to be sent to only one set of LEDs, this variable is set
//to the corresponding LED group from "SetSpecific.h." For global commands to
//all the sets simultaneously, set "selectedLedGroup" equal to "LEDGROUP."
uint8_t selectedLedGroup = 0;

//This will store strings to be easily called later if needed for something like
//the Blynk.setProperty function to populate a drop down menu in Blynk.
BlynkParamAllocated effectsList(512);
BlynkParamAllocated ledGroupsList(128);




////////////////////////////////////////////////////////////////////////////////
//SETUP FUNCTIONS                                                             //
////////////////////////////////////////////////////////////////////////////////
//BEFOREUPLOAD Make sure all the effects are listed in the function below or
//BEFOREUPLOAD they will not show up in the Blynk app.
void addEffectsToList(){
	DEBUG_PRINTLN("Populating 'effectsList' with effects.");

	effectsList.add("First effect");
	effectsList.add("Second effect");

	DEBUG_PRINTLN("Finished populating 'effectList.'");
}

//BEFOREUPLOAD Make sure all the LED groups are listed in the function below or
//BEFOREUPLOAD they will not show up in the Blynk app.
void addLedGroupsToList(){
	DEBUG_PRINTLN("Populating 'ledGroupList' with effects.");

	ledGroupsList.add("All");
	ledGroupsList.add("Test");
	ledGroupsList.add("Bed");
	ledGroupsList.add("Couch");
	ledGroupsList.add("TV");
	ledGroupsList.add("TV Stand");
	ledGroupsList.add("Wall Desk");
	ledGroupsList.add("Glass Desk");

	DEBUG_PRINTLN("Finished populating 'ledGroupList.'");
}

void setupLeds(){
	DEBUG_PRINTLN("Setting up LEDs. They should turn on when ready.");

	animationSpeed 			= 0;
	currentRed		= 255;
	currentGreen	= 255;
	currentBlue		= 255;
	onOff 			= true;

	FastLED.addLeds<CHIPSET, DATAPIN, COLORORDER>(leds, NUMLEDS);
	FastLED.setCorrection(TypicalLEDStrip);

	set_max_power_in_volts_and_milliamps(MAXVOLTAGE, MAXAMPS);

	DEBUG_PRINTLN("LED setup complete. If they didn't turn on, check wiring.");
}

void setupWiFi(){
	DEBUG_PRINT("Setting up the Wifi connection.");

	//If there are no recognized networks to connect to, this will create an
	//ad-hoc network with the name of the current LED group. You can connect
	//to this network to setup the WiFi further.
	wifiManager.autoConnect(SENSORNAME);

	while(WiFi.status() != WL_CONNECTED){
		DEBUG_PRINT(".");
		delay(500);
	}

	DEBUG_PRINT("Wifi setup is complete, the current IP Address is: ");
	DEBUG_PRINTLN(WiFi.localIP());
}

void setupBlynk(){
	DEBUG_PRINT("Setting up connection to Blynk servers...");

	//Make sure to use Blynk.config instead of Blynk.begin, since the Wifi
	//is being managed by WiFiManager.
	Blynk.config(blynkAuth);

	while(!Blynk.connect()){
		DEBUG_PRINT(".");
		delay(500);
	}

	clock.begin();

	//Once connected to blynk, download the current settings (this is in case
	//there was an unexpected disconnect--the settings on the ESP will revert
	//back to where they were before the disconnect).
	DEBUG_PRINTLN("\nSuccessfully connected to Blynk, now downloading values.");

	Blynk.syncAll();

	DEBUG_PRINTLN("Finished downloading values.");

}



void setup(){
	DEBUG_BEGIN(115200);

	setupWiFi();

	//Only setup everything else if the WiFi connects.
	if(WiFi.status() == WL_CONNECTED){
		addLedGroupsToList();
		addEffectsToList();
		setupLeds();
		setupBlynk();
	}
}

void loop(){}