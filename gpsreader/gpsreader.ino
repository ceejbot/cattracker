#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
//   Connect the GPS TX (transmit) pin to ARD_RXPIN
//   Connect the GPS RX (receive) pin to ARD_TXPIN

#define ARD_RXPIN 7
#define ARD_TXPIN 8

#define TERMBAUD 115200
#define GPSBAUD  9600

HardwareSerial gpsSerial = HardwareSerial();
Adafruit_GPS GPS(&gpsSerial);

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
	startLogging();
	delay(1000);
}

void loop()
{
	readSerialCommand();
}

//-------------------------------------------------------------------

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

//-------------------------------------------------------------------

void startLogging()
{
	Serial.print("Cat tracking...");
	if (GPS.LOCUS_StartLogger())
		Serial.println(" commenced!");
	else
		Serial.println(" no response from GPS; debug connections");
}

void reset()
{
	Serial.println("reset() unimplemented");
}

void erase()
{
	Serial.println("erasing stored data");
	GPS.sendCommand(PMTK_LOCUS_ERASE_FLASH);
}

void dumpLog()
{
	Serial.println("dumpLog() unimplemented");
}

void status()
{
	if (GPS.LOCUS_ReadStatus())
	{
		Serial.print("Status code: ");
		Serial.print(GPS.LOCUS_status, DEC);
		Serial.println(" ");

		// look at GPS.LOCUS_percent
		Serial.print("Storage: ");
		Serial.print(GPS.LOCUS_percent, DEC);
		Serial.println("\% full");

		Serial.print("Location: ");
		Serial.print(GPS.latitude, 10);
		Serial.print(" lat; ");
		Serial.print(GPS.longitude, 10);
		Serial.print(" lon; ");
		Serial.print(GPS.altitude, 10);
		Serial.println(" alt");
	}
}

void help()
{
	Serial.println("Cat tracker commands:");
	Serial.println("d: dump logs to bluetooth");
	Serial.println("e: erase GPS log flash");
	Serial.println("g: start logging");
	Serial.println("r: reset logging");
	Serial.println("s: emit GPS module status");
}

void readSerialCommand()
{
	// Serial.readBytesUntil(character, buffer, length)
	if (Serial.available())
	{
		char command = Serial.read();

		switch (command)
		{
			case 'h':
				help();
				break;

			case 'r':
				reset();
				break;

			case 'd':
				dumpLog();
				break;

			case 'g':
				startLogging();
				break;

			case 's':
				status();
				break;

			case 'e':
				erase();
				break;
		}
	}
}
