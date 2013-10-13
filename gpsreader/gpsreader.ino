#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
//   Connect the GPS TX (transmit) pin to ARD_RXPIN
//   Connect the GPS RX (receive) pin to ARD_TXPIN

#define ARD_RXPIN 7
#define ARD_TXPIN 8

#define TERMBAUD 115200
// 9600 NMEA is the default baud rate for MTK - some use 4800
#define GPSBAUD  9600

HardwareSerial gpsSerial = HardwareSerial();
Adafruit_GPS GPS(&gpsSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

void setup()
{
	while (!Serial);

	pinMode(ARD_TXPIN, OUTPUT);
	pinMode(ARD_RXPIN, INPUT);

	Serial.begin(TERMBAUD);
	Serial.println("Cat tracker test one.");

	GPS.begin(GPSBAUD);

	useInterrupt(true);

	// You can adjust which sentences to have the module emit, below
	// Default is RMC + GGA
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

	delay(500);
	Serial.print("Cat tracking...");
	if (GPS.LOCUS_StartLogger())
		Serial.println(" commenced!");
	else
		Serial.println(" no response from GPS; debug connections");
	delay(1000);
}

void loop()
{
	// we're doing all the work in the interrupt
}

/******************************************************************/

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
	char c = GPS.read();
	// if you want to debug, this is a good time to do it!
	if (GPSECHO && c)
	{
		Serial.print(c);
	}
}

void useInterrupt(boolean v)
{
	if (v)
	{
		// Timer0 is already used for millis() - we'll just interrupt somewhere
		// in the middle and call the "Compare A" function above
		OCR0A = 0xAF;
		TIMSK0 |= _BV(OCIE0A);
		usingInterrupt = true;
	}
	else
	{
		// do not call the interrupt function COMPA anymore
		TIMSK0 &= ~_BV(OCIE0A);
		usingInterrupt = false;
	}
}

// GPS.sendCommand(PMTK_LOCUS_ERASE_FLASH);
