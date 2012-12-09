/*
    Cat Tracker 2012.
    This is an Arduino sketch to control a GPS module + an SD card module.
    It logs location data to the configured logfile in bursts every N
    seconds. 
 */

#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <FlexiTimer2.h>

// Define which pins you will use on the Arduino to communicate with your 
// GPS. In this case, the GPS module's TX pin will connect to the 
// Arduino's RXPIN which is pin 3.
#define RXPIN 2
#define TXPIN 3

// Baud rate of Arduino serial terminal. Must be fast.
#define TERMBAUD    115200
// Baud rate of the GPS module.
#define GPSBAUD  4800

TinyGPS gps;
SoftwareSerial uart_gps(RXPIN, TXPIN);

// SD card setup.
const int chipSelect = 4;
File logfile;
#define LOGFILE "datalog.js"

#define BURST_COUNT 60
#define SLEEP_DURATION_MS 15 * 1000

//------------------------------------------------------------------------------

void setup()
{
    Serial.begin(TERMBAUD);
    uart_gps.begin(GPSBAUD);
    
    Serial.println("Cat Tracker starting up.");
    
    pinMode(10, OUTPUT);
    if (SD.begin(chipSelect))
        startLogging();
    else
        Serial.println("SD card failed or not present; not logging data.");
    
    // Log one burst, then run the timer to control the next bursts.
    logGPSData();
        
	FlexiTimer2::set(SLEEP_DURATION_MS, logGPSData); // 500ms period
	FlexiTimer2::start();
}

void loop()
{
}

//------------------------------------------------------------------------------

void logGPSData()
{
	int counter = BURST_COUNT;

    while (uart_gps.available() && counter)
    {
		int c = uart_gps.read();
		if (gps.encode(c) && logfile)
		{
			addLogEntry(gps, logfile);
			counter--;
		}
        
        readSerialCommand();
    }
}

void addLogEntry(TinyGPS &gps, File &logfile)
{
    // log entries are formatted as JSON: { date:"", lat:"", lon:"" }
    // date format: ddmmyy hhmmsscc 

    float latitude, longitude;
    gps.f_get_position(&latitude, &longitude);

    unsigned long date, time, age;
    gps.get_datetime(&date, &time, &age);
 
    logfile.print("{\"date\":\"");
    logfile.print(date, DEC);
    logfile.print(" ");
    logfile.print(time, DEC);
    
    logfile.print("\",\"lat\":\"");
    logfile.print(latitude, 10);

    logfile.print("\",\"lon\":\"");
    logfile.print(longitude, 10);

    logfile.println("\"}");
}


void reset()
{
    if (logfile)
    {
    	logfile.close();
    	SD.remove(LOGFILE);
    	startLogging();
    }
}

void startLogging()
{
	// TODO name file using timestamp (time from gps?)
	logfile = SD.open(LOGFILE, FILE_WRITE);
	if (!logfile)
		Serial.println("Unable to open log file for writing.");
	else
		Serial.println("Data logging started.");
	logfile.println("------ start");
}

void dumpLog()
{
	unsigned long pos = 0;

    if (logfile)
    {
    	pos = logfile.position();
    	logfile.seek(0);
    }
    else
		logfile = SD.open(LOGFILE);

    if (logfile)
    {
        Serial.println("------");
        while (logfile.available())
            Serial.write(logfile.read());
        Serial.println("------");
        Serial.println(logfile.position());
    }
    else
        Serial.println("Unable to open log file for reading.");
        
}

void readSerialCommand()
{
    // Serial.readBytesUntil(character, buffer, length)
    if (Serial.available())
    {
        char command = Serial.read();
    
        switch (command)
        {
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
                // status
                // TODO
                break;
        }
    }
}
