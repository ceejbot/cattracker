/*
    Cat Tracker 2012.
 */

#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

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

void getgps(TinyGPS &gps);

void setup()
{
    Serial.begin(TERMBAUD);
    uart_gps.begin(GPSBAUD);
    
    Serial.println("Cat Tracker is in the ceiling, watching your cat.
    Serial.println("GPS waiting for lock.");
    
    pinMode(10, OUTPUT);
    if (SD.begin(chipSelect))
        startLogging();
    else
        Serial.println("SD card failed or not present; no data will be logged.");
}

void loop()
{
    while (uart_gps.available())
    {
		int c = uart_gps.read();
		if (gps.encode(c) && logfile)
			addLogEntry(gps, logfile);
        
        readSerialCommand();
    }
}

void addLogEntry(TinyGPS &gps, File &logfile)
{
    // log entries are formatted as JSON:
    // { date:"", lat:"", lon:"" }
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
    // TODO
    // probably close file, remove, open fresh
}

void startLogging()
{
	// TODO name file using timestamp (time from gps?)
	logfile = SD.open(LOGFILE, FILE_WRITE);
	if (!logfile)
		Serial.println("Opening log for writing failed. Grumpy cat is grumpy.");
	else
		Serial.println("Data logging started.");
	logfile.println("------ new session ------");
}

void dumpLog()
{
    if (logfile)
    {
        logfile.flush();
        logfile.close();
    }
	
	logfile = SD.open(LOGFILE);

    if (logfile)
    {
        Serial.println("------");
        while (logfile.available())
            Serial.write(logfile.read());
        Serial.println("------");
        logfile.close();
		startLogging();
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

