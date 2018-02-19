/*
*HTTPS with follow-redirect example
*For data monitoring of temps and data around the house
**
**
*/
#include <WiFiClientSecure.h>
#include "Wire.h"

#include <DHT.h>
#define BUILTIN 14
// Setup for temp measurement
#define DHTPIN 2 // D10 ss on the board = GPIO2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
// Setup for WiFI
const char * ssid = "Burradoo";
const char * password = "2rachelle";
// Setup for google
const char * host = "script.google.com";
const char * googleRedirHost = "script.googleusercontent.com";
const char * GScriptId = "AKfycbzMHJsMDXQST4jIAy-cdjQBNZtk_PsNP7geaffLqpBy0A7r3Z8";
//https://script.google.com/a/wozware.biz/macros/s/AKfycbzMHJsMDXQST4jIAy-cdjQBNZtk_PsNP7geaffLqpBy0A7r3Z8/exec?tag=Tank&value=0

// Time to sleep (in seconds):
const int sleepTimeS = 600;
const int httpsPort = 443;
// set location
const char * Location = "Tank";
// const char * Location = "Garage%20Temp";
// const char * Location = "Lounge";
// const char * Location = "Filter";
// const char * Location = "Office%20Temp";
// IPAddress ip(192,168,1,220); // Office
// IPAddress ip(192,168,1,227); //Garage
// IPAddress ip(192,168,1,224); //lounge
// IPAddress ip(192,168,1,223); //Filter
IPAddress ip(192, 168, 1, 239); // Tank
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
int WiFiCount = 0;

// Write to Google Spreadsheet
String url = String("/a/wozware.biz/macros/s/") + GScriptId + "/exec?";
//https://script.google.com/a/wozware.biz/macros/s/AKfycbzMHJsMDXQST4jIAy-cdjQBNZtk_PsNP7geaffLqpBy0A7r3Z8/exec?
// Fetch Google Calendar events for 1 week ahead
String url2 = String("/macros/s/") + GScriptId + "/exec?cal";
// Read from Google Spreadsheet
String url3 = String("/macros/s/") + GScriptId + "/exec?read";
// setup ping
#define trigPin D1 // black
#define echoPin D2 // blue
#define TRIGGER_PIN D1 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN D2 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define TANK_OFFSET 120
#define TANK_DEPTH 1630
bool OutOfRange = false;


const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDfTCCAuagAwIBAgIDErvmMA0GCSqGSIb3DQEBBQUAME4xCzAJBgNVBAYTAlVT\n" \
"MRAwDgYDVQQKEwdFcXVpZmF4MS0wKwYDVQQLEyRFcXVpZmF4IFNlY3VyZSBDZXJ0\n" \
"aWZpY2F0ZSBBdXRob3JpdHkwHhcNMDIwNTIxMDQwMDAwWhcNMTgwODIxMDQwMDAw\n" \
"WjBCMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNR2VvVHJ1c3QgSW5jLjEbMBkGA1UE\n" \
"AxMSR2VvVHJ1c3QgR2xvYmFsIENBMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n" \
"CgKCAQEA2swYYzD99BcjGlZ+W988bDjkcbd4kdS8odhM+KhDtgPpTSEHCIjaWC9m\n" \
"OSm9BXiLnTjoBbdqfnGk5sRgprDvgOSJKA+eJdbtg/OtppHHmMlCGDUUna2YRpIu\n" \
"T8rxh0PBFpVXLVDviS2Aelet8u5fa9IAjbkU+BQVNdnARqN7csiRv8lVK83Qlz6c\n" \
"JmTM386DGXHKTubU1XupGc1V3sjs0l44U+VcT4wt/lAjNvxm5suOpDkZALeVAjmR\n" \
"Cw7+OC7RHQWa9k0+bw8HHa8sHo9gOeL6NlMTOdReJivbPagUvTLrGAMoUgRx5asz\n" \
"PeE4uwc2hGKceeoWMPRfwCvocWvk+QIDAQABo4HwMIHtMB8GA1UdIwQYMBaAFEjm\n" \
"aPkr0rKV10fYIyAQTzOYkJ/UMB0GA1UdDgQWBBTAephojYn7qwVkDBF9qn1luMrM\n" \
"TjAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjA6BgNVHR8EMzAxMC+g\n" \
"LaArhilodHRwOi8vY3JsLmdlb3RydXN0LmNvbS9jcmxzL3NlY3VyZWNhLmNybDBO\n" \
"BgNVHSAERzBFMEMGBFUdIAAwOzA5BggrBgEFBQcCARYtaHR0cHM6Ly93d3cuZ2Vv\n" \
"dHJ1c3QuY29tL3Jlc291cmNlcy9yZXBvc2l0b3J5MA0GCSqGSIb3DQEBBQUAA4GB\n" \
"AHbhEm5OSxYShjAGsoEIz/AIx8dxfmbuwu3UOx//8PDITtZDOLC5MH0Y0FWDomrL\n" \
"NhGc6Ehmo21/uBPUR/6LWlxz/K7ZGzIZOKuXNBSqltLroxwUCEm2u+WR74M26x1W\n" \
"b8ravHNjkOR/ez4iyz0H7V84dJzjA1BOoa+Y7mHyhD8S\n" \
"-----END CERTIFICATE-----\n";



byte sensorInterrupt = 17; //D5;// 0 = digital pin 2
byte sensorPin = 0;//D2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 6.6;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

// pressure
#define I2Caddress 0x48

// Running average
unsigned int readings[20] = {0};
unsigned char readCnt = 0;




void setup()
{
	 // Initialize a serial connection for reporting values to the host
	Serial.begin(115200);
	pinMode(BUILTIN, OUTPUT);

	// Join the I2C bus as a master (call this only once)
	Wire.begin();

	 
	// Set up the status LED line as an output
	pinMode(statusLed, OUTPUT);
	digitalWrite(statusLed, HIGH);// We have an active-low LED attached
	
	pinMode(sensorPin, INPUT);
	digitalWrite(sensorPin, HIGH);

	pulseCount = 0;
	flowRate = 0.0;
	flowMilliLitres = 0;
	totalMilliLitres = 0;
	oldTime = 0;

	// The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
	// Configured to trigger on a FALLING state change (transition from HIGH
	// state to LOW state)
	attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

	
}

void loop()
{
	SendResultsToGoogle();
	Serial.println("ESP8266 in sleep mode");
	digitalWrite(BUILTIN, LOW); // turn the LED Built in LED off
	//ESP.deepSleep(sleepTimeS * 1000000);
	delay(10000);
}

void SendResultsToGoogle()
{
	WiFiClientSecure client;

	digitalWrite(BUILTIN, LOW); // turn the LED on (HIGH is the voltage level)
	//WiFi.config(ip, gateway, subnet);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.println();
	Serial.print("Connecting to wifi: ");
	Serial.println(ssid);
	// in case the wireless connection doesn't go through
	Serial.flush();
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	// Use HTTPSRedirect class to create TLS connection



	client.setCACert(ca_cert);


	/*
	* add measurements here
	*/
	OutOfRange = false;
	float depth = CalcTankDepth();
	String postStr = url;
	postStr += "&tag=";
	postStr += Location;
	postStr += "&value=";
	//if (OutOfRange)
		//postStr += "OOR";
	//else
		postStr += depth;
		
	postStr += "&value1=NA";
	Serial.println("Connect to server via port 443");
	if (!client.connect(host, 443))
	{
		Serial.println("Connection failed!");
	} 
	else 
	{
		Serial.println("Connected to server!");
		/* create HTTP request */
		client.println("GET " + postStr + " HTTP/1.0");
		client.print("Host: ");
		client.println( host);
		client.println("Connection: close");
		client.println();

		Serial.print("Waiting for response ");
		while (!client.available()){
			delay(50); //
			Serial.print(".");
		}
		/* if data is available then receive and print to Terminal */
		while (client.available()) {
			char c = client.read();
			Serial.write(c);
		}

		/* if the server disconnected, stop the client */
		if (!client.connected()) 
		{
			Serial.println();
			Serial.println("Server disconnected");
			client.stop();
			
		}
		//client.close();
	}
}

float CalcTankDepth()
{
	return 0.0;
}
/*
Insterrupt Service Routine
*/
void pulseCounter()
{
	// Increment the pulse counter
	pulseCount++;
}

void loopFlow()
{
 
	if((millis() - oldTime) > 1000)// Only process counters once per second
	{ 
		// Disable the interrupt while calculating flow rate and sending the value to
		// the host
		detachInterrupt(sensorInterrupt);
			
		// Because this loop may not complete in exactly 1 second intervals we calculate
		// the number of milliseconds that have passed since the last execution and use
		// that to scale the output. We also apply the calibrationFactor to scale the output
		// based on the number of pulses per second per units of measure (litres/minute in
		// this case) coming from the sensor.
		flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
		
		// Note the time this processing pass was executed. Note that because we've
		// disabled interrupts the millis() function won't actually be incrementing right
		// at this point, but it will still return the value it was set to just before
		// interrupts went away.
		oldTime = millis();
		
		// Divide the flow rate in litres/minute by 60 to determine how many litres have
		// passed through the sensor in this 1 second interval, then multiply by 1000 to
		// convert to millilitres.
		flowMilliLitres = (flowRate / 60) * 1000;
		
		// Add the millilitres passed in this second to the cumulative total
		totalMilliLitres += flowMilliLitres;
		
		unsigned int frac;
		
		// Print the flow rate for this second in litres / minute
		Serial.print("Flow rate: ");
		Serial.print(int(flowRate));// Print the integer part of the variable
		Serial.print("."); // Print the decimal point
		// Determine the fractional part. The 10 multiplier gives us 1 decimal place.
		frac = (flowRate - int(flowRate)) * 10;
		Serial.print(frac, DEC) ;// Print the fractional part of the variable
		Serial.print("L/min");
		// Print the number of litres flowed in this second
		Serial.print("Current Liquid Flowing: "); // Output separator
		Serial.print(flowMilliLitres);
		Serial.print("mL/Sec");

		// Print the cumulative total of litres flowed since starting
		Serial.print("Output Liquid Quantity: "); // Output separator
		Serial.print(totalMilliLitres);
		Serial.println("mL"); 

		// Reset the pulse counter so we can start incrementing again
		pulseCount = 0;
		
		// Enable the interrupt again now that we've finished sending output
		attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
	}

}

void loopADC()
{
		// Step 1: Point to Config register - set to continuous conversion
	Wire.beginTransmission(I2Caddress);

		// Point to Config Register
	Wire.write(0b00000001);

		// Write the MSB + LSB of Config Register
		// MSB: Bits 15:8
		// Bit150=No effect, 1=Begin Single Conversion (in power down mode)
		// Bits 14:12 How to configure A0 to A3 (comparator or single ended)
		// Bits 11:9Programmable Gain 000=6.144v 001=4.096v 010=2.048v .... 111=0.256v
		// Bits 8 0=Continuous conversion mode, 1=Power down single shot
	Wire.write(0b01000010);

		// LSB: Bits 7:0
		// Bits 7:5 Data Rate (Samples per second) 000=8, 001=16, 010=32, 011=64,
		//100=128, 101=250, 110=475, 111=860
		// Bit4 Comparator Mode 0=Traditional, 1=Window
		// Bit3 Comparator Polarity 0=low, 1=high
		// Bit2 Latching 0=No, 1=Yes
		// Bits 1:0 Comparator # before Alert pin goes high
		//00=1, 01=2, 10=4, 11=Disable this feature
	Wire.write(0b00000010);

		// Send the above bytes as an I2C WRITE to the module
	Wire.endTransmission();

		// ====================================

		// Step 2: Set the pointer to the conversion register
	Wire.beginTransmission(I2Caddress);

		//Point to Conversion register (read only , where we get our results from)
	Wire.write(0b00000000);

		// Send the above byte(s) as a WRITE
	Wire.endTransmission();

		// =======================================

		// Step 3: Request the 2 converted bytes (MSB plus LSB)
	Wire.requestFrom(I2Caddress, 2);

		// Read two bytes and convert to full 16-bit int
	uint16_t convertedValue;

		// Read the the first byte (MSB) and shift it 8 places to the left then read
		// the second byte (LSB) into the last byte of this integer
	convertedValue = (Wire.read() << 8 | Wire.read());

		// Debug the value
		//Serial.println(convertedValue >> 6 << 6);
	Serial.println(map(convertedValue, 0, 32767, 0, 5000));
		//Serial.println(convertedValue);
	readings[readCnt] = convertedValue;
	readCnt = readCnt == 19 ? 0 : readCnt + 1;

		// Get the average
	unsigned long totalReadings = 0;
	for (unsigned char cnt = 0; cnt < 20; cnt++)
	{
		totalReadings += readings[cnt];
	}
		//Serial.print(convertedValue); Serial.print("\t"); Serial.println(totalReadings / 20);


}
