//First MockUp for FablabLannion Trophy Project
/*
 * This example is a mix of several projects:
 * https://github.com/arduino-libraries/WiFi101/blob/master/examples/SimpleWebServerWiFi/SimpleWebServerWiFi.ino
 * https://www.hackster.io/Collie147/arduino-mkr1000-zero-neopixel-throwie-8592e1 
 * https://github.com/bblanchon/ArduinoJson/blob/master/examples/JsonHttpClient/JsonHttpClient.ino
 */
 
/*
Following libraries shall be installed
- 1.6.3 release of  "Arduino SAMD Boards (32bits ARM Cortex-M0+"  ( not the latest, take care!!!)
- ArduinoJson
- Adafruit_NeoPixel
- WiFi101
*/
 
 

 //Following define is a pure mistery. Depending on WIFI connection, we receive one additionnal header to filter out. WTF is this???
//#define SKIP_TP_HEADER
 
 
 
 /**********************************************************/
 /****    USER DATA, TO BE REMOVED BEFORE ANY COMMIT!!! ****/
 /**********************************************************/
 //thinkgspeak API KEY for Trophy Channel ( NOT TO BE SHARED!!!)
#define API_KEY "TOBEREPLACED"
#define SITE "TOBEREPLACED"  // TODO : shall be configurable from webserver
 

 
char ssid[] = "TOBEREPLACED";      //  your network SSID (name) // TODO : shall be configurable from webserver
char pass[] = "TOBEREPLACED";   // your network password // TODO : shall be configurable from webserver


//thinkgspeak API KEY for Trophy Channel ( NOT TO BE SHARED!!!)
unsigned long myChannelNumber = 0; //Integer value TOBEREPLACED
const char* resource = "/channels/0/feeds/last.json";                    // 0 to be replaced
 
 
 /**********************************************************/
 /****            WIFI CONFIGURATION                    ****/
 /**********************************************************/
 
#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
 #include <SPI.h>





char buffer[70];


int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int ledpin = 6;
bool val = true;

int status = WL_IDLE_STATUS;
WiFiServer server(80);
String IP;







 /**********************************************************/
 /****            THINGSPEAK CONFIGURATION              ****/
 /**********************************************************/

 //#include "ThingSpeak.h" : ThingSpeak Library not working for MKR1000 :-(


#define SERVER "api.thingspeak.com"  
#define THINGSPEAK_PORT_NUMBER 80
#define SLEEP_TIME 15000






 /**********************************************************/
 /****            JSON PARSER CONFIGURATION              ****/
 /**********************************************************/
#include <ArduinoJson.h>
const char* ThingSpeakServer = "api.thingspeak.com";  // server's address


const unsigned long HTTP_TIMEOUT = 20000;  // max response time from server
const size_t MAX_CONTENT_SIZE = 512;       // max size of the HTTP response


// The type of data that we want to extract from the page
struct UserData {
  char created_at[32]; 
  unsigned char field1; 
  unsigned char field2; 
};

unsigned char newSampleDetected=0;
char timestamp[32]="rindutout";

WiFiClient ThingSpeakClient;
StaticJsonBuffer<200> jsonBuffer;

 /**********************************************************/
 /****            NEOPIXEL CONFIGURATION                ****/
 /**********************************************************/



#include <Adafruit_NeoPixel.h>
#define PIN            12 //sets the pin on which the neopixels are connected
#define NUMPIXELS      5 //defines the number of pixels in the strip
#define interval       50 //defines the delay interval between running the functions
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.Color(255, 0, 0);
uint32_t blue = pixels.Color(0, 0, 255);
uint32_t green = pixels.Color(0, 255, 0);
uint32_t pixelColour;
uint32_t lastColor;
float activeColor[] = {255, 0, 0};//sets the default color to red
boolean NeoState[] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, true}; //Active Neopixel Function (off by default)
uint8_t brightness = 255; //sets the default brightness value
uint8_t R=0;
uint8_t G=0;
uint8_t B=0;

 /**********************************************************/
 /****            BRIGTHNESS CONTROLLER                 ****/
 /**********************************************************/
unsigned long lastConnectionTime = 0;            // last time brightness was updated, in milliseconds
const unsigned long postingInterval = 1L * 60L; // delay between updates, in milliseconds
int direction = 1; //decide wether we have to increase or decrease brightness


/*********************************************************/
 /****            NEOPIX MANAGER CONTROLLER            ****/
 /**********************************************************/
int NEOPIX_MODE_1 = 14;                
int NEOPIX_MODE_2 = 13;                 

/*********************************************************/
 /****            NEOPIX MANAGER CONTROLLER            ****/
 /**********************************************************/
int WEBSERVER_MODE_O = 0;                
int WEBSERVER_MODE_I = 1;    

int webserver_mode =0;
 int old_webserver_mode =0;          



 /**********************************************************/
 /****            HTML WEBPAGES                         ****/
 /**********************************************************/

/*HTML code is split into 4 strings as it doesnt seem to be able to handle strings over 1500 or so chars*/
const char html1[] PROGMEM = 
"<!DOCTYPE html><html><head><meta http-equiv='X-UA-Compatible' content='IE=Edge'><meta charset='utf-8'><title>Neopixel Color Picker</title>"
"<style type='text/css'>.bt{display:block;width:250px;height:100px;padding:10px;margin:10px;text-align:center;border-radius:5px;color:white;font-weight:bold;font-size:40px;text-decoration:none;}body{background:#000;}"
".red{background:red;color:white;}.green{background:#0C0;color:white;}.blue{background:blue;color:white;}"
".white{background:white;color:black;border:1px solid black;}.off{background:#666;color:white;}.colorPicker{background:white;color:black;}.colorWipe{font-size:40px;  background:linear-gradient(to right, red, #0C0, blue);}"
".theatreChase{font-size:40px;background:linear-gradient(to right, red, black, red, black, #0C0, black, #0C0, black, blue, black, blue);}"
".rainbow{font-size:40px;background:red;background:linear-gradient(to right, red, orange, yellow, green, blue, indigo, violet,red, orange, yellow, green, blue, indigo, violet);}"
".rainbowCycle{font-size:40px;background:red;background:linear-gradient(to right, red, orange, yellow, green, blue, indigo, violet);}"
".rainbowChase{font-size:40px;background:red;background:linear-gradient(to right, red, black, orange, black, yellow, black, green, black, blue, black, indigo, black, violet);}";

const char html2[] PROGMEM = 
".breathe{background:blue;color:white;}.cylon{background:red;color:black;}.heartbeat{background:red;color:white;}.XMAS{font-size:40px;background:red;background:linear-gradient(to right, red, green, red, green, red, green, red, green, red, green, red, green);}"
".ALL{background:white;color:blue;}.y{background:#EE0;height:100px;width:100px;border-radius:50px;}.b{background:#000;height:100px;width:100px;border-radius:50px;}.a{font-size:35px;}td{vertical-align:middle;}"
"td{vertical-align:middle;}</style></head>"
"<script type='text/javascript'>";

const char html3[] PROGMEM =
"function ResetWebpage(){ if (window.location.href != 'http://#IPADDRESS/'){    window.open ('http://#IPADDRESS/','_self',true) }};" //change the website value here to your static website
"function myFunction(){ document.getElementById('brightnessLevel').submit();}</script>"
"<body onload='ResetWebpage()'><table><tr><td width='100'><div class='TGT00'></div></td><td><a class='bt red' href='/L00?v=1'>Red</a></td><td><a class='bt colorWipe' href='/L01?v=1'>YaRien</a></td><td><div class='TGT01'></div></td></tr>"
"<tr><td><div class='TGT02'></div></td><td><a class='bt green' href='/L02?v=1'>Green</a></td><td><a class='bt theatreChase' href='/L03?v=1'> Disco </a></td><td><div class='TGT03'></div></td></tr>"
"<tr><td><div class='TGT04'></div></td><td><a class='bt blue' href='/L04?v=1'>Blue</a></td><td><a class='bt rainbow' href='/L05?v=1'>Rainbow</a></td><td><div class='TGT05'></div></td></tr>"
"<tr><td><div class='TGT06'></div></td><td><a class='bt white' href='/L06?v=1'>White</a></td><td><a class='bt rainbowChase' href='/L07?v=1'>YaRien</a></td><td><div class='TGT07'></div></td></tr>";

const char html4[] PROGMEM =
"<tr><td><div class='TGT08'></div></td><td><a class='bt cylon' href='/L08?v=1'>Mode 1</a></td><td><a class='bt rainbowCycle' href='/L09?v=1'>Mode 4</a></td><td><div class='TGT09'></div></td></tr>"
"<tr><td><div class='TGT10'></div></td><td><a class='bt breathe' href='/L10?v=1'>Mode 2</a></td><td><a class='bt heartbeat' href='/L11?v=1'>LaNonPlus</a></td><td><div class='TGT11'></div></td></tr>"
"<tr><td><div class='TGT12'></div></td><td><a class='bt XMAS' href='/L12?v=1'>Mode 3</a></td><td><a class='bt ALL' href='/L13?v=1'>ToujoursRien</a></td><td><div class='TGT13'></div></td></tr>"
"<tr><td><div class='TGT14'></div></td><td><a class='bt off' href='/L14?v=1'>Off</a></td><td><form id='brightnessLevel'><input type='range' name='bright' max='255' min='0' value='#BRIGHT' onchange='myFunction()' class='bt off'></form></td></tr>"
"</table></body></html>";

String sendHtml3 = html3;//create a string version of the html3 to sub values into
String sendHtml4 = html4;//create a string version of the html4 to sub values into
String currentLine; //string returned by the client connected









void setup() {


	  pixels.begin(); //starts the neopixels
	  pixels.setBrightness(brightness); // sets the inital brightness of the neopixels
	  
	  Serial.print("Start Serial ");
	  Serial.begin(9600);      // initialize serial communication
	  
	  
	  

	   R=0;
	   G=0;
       B=255;
	  writeLEDS(R, G, B); //set neopixel to blue by default, befault blue is a cool value :-)
	  

	  
	  pinMode(ledpin, OUTPUT);      // set the LED pin mode ( debug purpose only )
	  pinMode(NEOPIX_MODE_1, OUTPUT);      // set the LED pin mode ( debug purpose only )
	  pinMode(NEOPIX_MODE_2, OUTPUT);      // set the LED pin mode ( debug purpose only )	  
             
	  pinMode(WEBSERVER_MODE_O, OUTPUT);      // set the LED pin mode ( debug purpose only )	 
	  pinMode(WEBSERVER_MODE_I, INPUT);      // set the LED pin mode ( debug purpose only )	 



	  // Check for the presence of the shield
	  Serial.print("WiFi101 shield: ");
	  if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("NOT PRESENT");
		return; // don't continue
	  }
	  Serial.println("DETECTED");
	  // attempt to connect to Wifi network:
	  while ( status != WL_CONNECTED) {
		digitalWrite(ledpin, LOW); //( debug purpose only )

		Serial.print("Attempting to connect to Network named: ");
		Serial.println(ssid);                   // print the network name (SSID);
		
		digitalWrite(ledpin, HIGH); //( debug purpose only )
		digitalWrite(WEBSERVER_MODE_O, HIGH); //3.3V source
		pinMode(WEBSERVER_MODE_O, OUTPUT);      // set the LED pin mode ( debug purpose only )	 
	  	 
		webserver_mode = digitalRead(WEBSERVER_MODE_I);   // read the input pin
		
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		status = WiFi.begin(ssid, pass);
		// wait 10 seconds for connection:
		delay(10000);
		
		//Twist for a while to show we are still not connected to the WIFI
		   for (int i=0; i <= 10; i++){
				//  White	 Band
				R=0;
				G=0;
				B=255;	
				writeLEDS(R, G, B);   
				delay (50);
				//  White	 Band
				R=0;
				G=0;
				B=0;	
				writeLEDS(R, G, B);   
				delay (50);		
		   } 		
	  }
	  server.begin();                           // start the web server on port 80
	  printWifiStatus();                        // you're connected now, so print out the status
	  digitalWrite(ledpin, HIGH);
	  
  



	   R=0;
	   G=255;
       B=0;
	  writeLEDS(R, G, B); //set neopixel to green , to notice we are ready to rock!  
	  
  
	
	if (webserver_mode==0)
	{
		Serial.println("Mode: WebServer");
	}
	else
	{
		Serial.println("Mode: ThingSpeak Sniffer");
	}

}




void loop() {


webserver_mode = digitalRead(WEBSERVER_MODE_I);   // read the input pin


if (webserver_mode !=old_webserver_mode)
{
	Serial.println("Mode Change detected!");

	old_webserver_mode=webserver_mode;
	//Twist for a while to show change mode has been detected
   for (int i=0; i <= 10; i++){
		//  White	 Band
		R=128;
		G=10;
		B=240;	
		writeLEDS(R, G, B);   
		delay (50);
		//  White	 Band
		R=0;
		G=0;
		B=0;	
		writeLEDS(R, G, B);   
		delay (50);		
   } 

   	if (webserver_mode==0)
	{
		Serial.println("Mode: WebServer");
		R=0;
	   G=255;
       B=0;
	  writeLEDS(R, G, B); //set neopixel to green , to notice we are ready to rock!  
	     

			
	}
	else
	{
		Serial.println("Mode: ThingSpeak Sniffer");
		R=255;
	   G=0;
       B=0;
		writeLEDS(R, G, B); //set neopixel to green , to notice we are ready to rock!  
	     

			
	}
   
	
}


if (webserver_mode==0)
{
  //------------------------------------  
  // 1st : WebServer Page Processing 
  //------------------------------------  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    //Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(F(html1));//send the html part1
            client.print(F(html2));//send the html part2
            client.print(sendHtml3);//send the modified html part3
            client.print(sendHtml4);//send the modified html part4
            client.println();

            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
		  
		    if (currentLine.indexOf("Referer") < 0) {//so long as Referer is not in the returned value
              if (currentLine.indexOf("/L00") > 0) {//if /L00 is present in the string
                handle_L00();//run the function
              }
              if (currentLine.indexOf("/L01") > 0) {//same as above
                handle_L01();
              }
              if (currentLine.indexOf("/L02") > 0) {
                handle_L02();
              }
              if (currentLine.indexOf("/L03") > 0) {
                handle_L03();
              }
              if (currentLine.indexOf("/L04") > 0) {
                handle_L04();
              }
              if (currentLine.indexOf("/L05") > 0) {
                handle_L05();
              }
              if (currentLine.indexOf("/L06") > 0) {
                handle_L06();
              }
              if (currentLine.indexOf("/L07") > 0) {
                handle_L07();
              }
              if (currentLine.indexOf("/L08") > 0) {
                handle_L08();
              }
              if (currentLine.indexOf("/L09") > 0) {
                handle_L09();
              }
              if (currentLine.indexOf("/L10") > 0) {
                handle_L10();
              }
              if (currentLine.indexOf("/L11") > 0) {
                handle_L11();
              }
              if (currentLine.indexOf("/L12") > 0) {
                handle_L12();
              }
              if (currentLine.indexOf("/L13") > 0) {
                handle_L13();
              }
              if (currentLine.indexOf("/L14") > 0) {
                handle_L14();
              }
              if (currentLine.indexOf("bright") > 0) {
                handle_bright();
              }
             
            }
            currentLine = "";//clear the string

          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(ledpin, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(ledpin, LOW);                // GET /L turns the LED off
        }
		
		 if (currentLine.endsWith("/generate_204")) {
          handle_root();
        }
      }
    }
    
    // close the connection:
    client.stop();
    //Serial.println("client disconnected");
  }
  
  

  
  
}
else
{

 
 
	   //Default mode : Mode 2
 	  digitalWrite(NEOPIX_MODE_1, HIGH);
	  digitalWrite(NEOPIX_MODE_2, LOW);


		//------------------------------------ 
		// 2nd : Interaction with ThingSpeak  
		//------------------------------------	
		// Retrieve latest value from ThingSpeak Database
		if (ThingSpeakClient.connect(ThingSpeakServer,80)) {
			//Serial.println("connected");  
			
			if (sendRequest(ThingSpeakServer, resource) && skipResponseHeaders() && skipThingSpeakHeader() ) {
			  char response[MAX_CONTENT_SIZE];
			  readReponseContent(response, sizeof(response));

			  UserData userData;
			  if (parseUserData(response, &userData)) {
				printUserData(&userData);
			  }
			}
		}
		else{
			Serial.println("ThingSpeakClient connection error");
		}
	  
		// close the connection:
		ThingSpeakClient.stop();
		//Serial.println("ThingSpeakClient disconnected");  
		delay(2000); 
		

 
 }

  
}




 /**********************************************************/
 /****            JSON FUNCTIONS                        ****/
 /**********************************************************/



// Send the HTTP GET request to the server
//---------------------------------------- 
bool sendRequest(const char* host, const char* resource) {
//---------------------------------------- 
  
  Serial.println("Is there anybody out there?");
  //Serial.println(resource);
  

  ThingSpeakClient.print("GET ");
  ThingSpeakClient.print(resource);
  ThingSpeakClient.println(" HTTP/1.1");
  ThingSpeakClient.print("Host: ");
  ThingSpeakClient.println(host);
  ThingSpeakClient.println("Connection: close");
  ThingSpeakClient.println();

  return true;
}




// Skip HTTP headers so that we are at the beginning of the response's body
//---------------------------------------- 
bool skipResponseHeaders() {
//---------------------------------------- 
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  ThingSpeakClient.setTimeout(HTTP_TIMEOUT);
  bool ok = ThingSpeakClient.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!");
	
	//Twist for a while to show we found something wrong
	   for (int i=0; i <= 10; i++){
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);
			//  White	 Band
			R=0;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);		
	   } 
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   	
  }

  return ok;
}

// Skip HTTP headers so that we are at the beginning of the response's body
//-------------------------------------------------------
bool skipThingSpeakHeader() {
//-------------------------------------------------------

#ifdef SKIP_TP_HEADER
  // Flush first line to go to align to json body
  char endOfHeaders[] = "\r\n";

  ThingSpeakClient.setTimeout(HTTP_TIMEOUT);
  bool ok = ThingSpeakClient.find(endOfHeaders);

  if (!ok) {
    Serial.println("ThingSpeak Header not found :-(");
	//Twist for a while to show we found something wrong
	   for (int i=0; i <= 10; i++){
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);
			//  White	 Band
			R=0;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   
			delay (50);		
	   } 
			//  Red	 Band
			R=255;
			G=0;
			B=0;	
			writeLEDS(R, G, B);   	   
	   
  }
#else 
 bool ok=true;
#endif  


  return ok;
}



// Read the body of the response from the HTTP server
//-------------------------------------------------------
void readReponseContent(char* content, size_t maxSize) {
//-------------------------------------------------------
  size_t length = ThingSpeakClient.readBytes(content, maxSize);
  content[length] = 0;
  Serial.println(content);
}



// Parse the JSON from the input string and extract the interesting values
//-------------------------------------------------------
bool parseUserData(char* content, struct UserData* userData) {
//-------------------------------------------------------
  // Compute optimal size of the JSON buffer according to what we need to parse.
  // This is only required if you use StaticJsonBuffer.
  const size_t BUFFER_SIZE =
      JSON_OBJECT_SIZE(8)     // the root object has 8 elements
      + JSON_OBJECT_SIZE(5)   // the "address" object has 5 elements
      + JSON_OBJECT_SIZE(2)   // the "geo" object has 2 elements
      + JSON_OBJECT_SIZE(3);  // the "company" object has 3 elements

  // Allocate a temporary memory pool on the stack
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  // If the memory pool is too big for the stack, use this instead:
  // DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(content);

  if (!root.success()) {
    Serial.println("JSON parsing failed!");
    return false;
  }

  // Here were copy the strings we're interested in
  
  strcpy(userData->created_at, root["created_at"]);  
  userData->field1=atoi(root["field1"]);
  userData->field2=atoi(root["field2"]);
  //strcpy(userData->field2, root["field2"]);
  //strcpy(userData->field2, root["company"]["name"]);
  // It's not mandatory to make a copy, you could just use the pointers
  // Since, they are pointing inside the "content" buffer, so you need to make
  // sure it's still in memory when you read the string

  return true;
}

// Print the data extracted from the JSON
//-------------------------------------------------------
void printUserData(const struct UserData* userData) {
//-------------------------------------------------------

  
  Serial.print("old timestamp: ");
  Serial.println(timestamp);
  
 
  Serial.print("created_at = ");
  Serial.println(userData->created_at);
  Serial.print("Field1 = ");
  Serial.println(userData->field1);
  Serial.print("Field2 = ");
  Serial.println(userData->field2);
  
  //Check If this is a new sample
  if (strcmp(timestamp,userData->created_at)  != 0)
  {
    Serial.println("Hey Dude! We have something new here, let's rock!");
	newSampleDetected=1;
	

	strcpy(timestamp,userData->created_at);
	
	
	//writeLEDS(47, 55, 118); //Don't know which color is it, but no matter :-p  
    //Sleep for a while to display the color
	//delay(3000);	
	
	if (userData->field1!=0) 
	{
		Serial.println("Hey Man! Tweets are rocking around the clock!!");
		

		 //  White	 Band
		R=255;
	    G=255;
	    B=255;	
	    writeLEDS(R, G, B);   	
	 
		// Mode 1
		Serial.println("Mode 1");
		digitalWrite(NEOPIX_MODE_1, LOW);
		digitalWrite(NEOPIX_MODE_2, LOW);	
		delay(3000);
		// Mode 4
		Serial.println("Mode 4");
	    digitalWrite(NEOPIX_MODE_1, HIGH);
		digitalWrite(NEOPIX_MODE_2, HIGH);		
		delay(6000);	
		// Mode 3	
		Serial.println("Mode 3");
	    digitalWrite(NEOPIX_MODE_1, LOW);
		digitalWrite(NEOPIX_MODE_2, HIGH);			
		delay(3000);	
	
	}	
	
	
	
  }
  else
  { 
    Serial.println("Ronpichhhhhhh");
	newSampleDetected=0;	


	 //  Red	 Band
	R=255;
	G=0;
    B=0;	
	writeLEDS(R, G, B);   
  }  
  
}

 /**********************************************************/
 /****            WEBSERVER FUNCTIONS                    ****/
 /**********************************************************/


//-------------------------------------------------------
void printWifiStatus() {
//-------------------------------------------------------
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}




/*
 * HANDLER FUNCTIONS
 */
//-------------------------------------------------------
void handleNotFound() {
//-------------------------------------------------------
  //possible addition
}


//-------------------------------------------------------
void handle_root() {//handle what happens to the root page
//-------------------------------------------------------
  String brightString = String(brightness);//turn the brightness int into a string to put into the HTML
  sendHtml3 = html3;//copy the original html3 into a string to replace values
  sendHtml4 = html4;//copy the original html4 into a string to replace values
  sendHtml3.replace("#IPADDRESS", IP);
  sendHtml3.replace("TGT00", NeoState[0] ? "y" : "b");
  sendHtml3.replace("TGT01", NeoState[1] ? "y" : "b");
  sendHtml3.replace("TGT02", NeoState[2] ? "y" : "b");
  sendHtml3.replace("TGT03", NeoState[3] ? "y" : "b");
  sendHtml3.replace("TGT04", NeoState[4] ? "y" : "b");
  sendHtml3.replace("TGT05", NeoState[5] ? "y" : "b");
  sendHtml3.replace("TGT06", NeoState[6] ? "y" : "b");
  sendHtml3.replace("TGT07", NeoState[7] ? "y" : "b");
  sendHtml4.replace("TGT08", NeoState[8] ? "y" : "b");
  sendHtml4.replace("TGT09", NeoState[9] ? "y" : "b");
  sendHtml4.replace("TGT10", NeoState[10] ? "y" : "b");
  sendHtml4.replace("TGT11", NeoState[11] ? "y" : "b");
  sendHtml4.replace("TGT12", NeoState[12] ? "y" : "b");
  sendHtml4.replace("TGT13", NeoState[13] ? "y" : "b");
  sendHtml4.replace("TGT14", NeoState[14] ? "y" : "b");
  sendHtml4.replace("#BRIGHT", brightString);
  /*
  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.print(" | brightString: ");
  Serial.println(brightString);
  */
  
  //send data to thingSpeak
  /*
  Serial.println("Send Data to Thingpspeak");	
  pixels.setBrightness(20); // sets the inital brightness of the neopixels  
  sendDataToThingSpeak(brightness);
  delay(SLEEP_TIME);  
  pixels.setBrightness(brightness); // sets the inital brightness of the neopixels   
  */  
}

//-------------------------------------------------------
void handle_L00() {

  changeNeoState(0);
  handle_root();
}
void handle_L01() {
  changeNeoState(1);
  handle_root();
}
void handle_L02() {
  changeNeoState(2);
  handle_root();
}
void handle_L03() {
  changeNeoState(3);
  handle_root();
}
void handle_L04() {
  changeNeoState(4);
  handle_root();
}
void handle_L05() {
  changeNeoState(5);
  handle_root();
}
void handle_L06() {
  changeNeoState(6);
  handle_root();
}
void handle_L07() {
  changeNeoState(7);
  handle_root();
}
void handle_L08() {
  changeNeoState(8);
  handle_root();
}
void handle_L09() {
  changeNeoState(9);
  handle_root();
}
void handle_L10() {
  changeNeoState(10);
  handle_root();
}
void handle_L11() {
  changeNeoState(11);
  handle_root();
}
void handle_L12() {
  changeNeoState(12);
  handle_root();
}
void handle_L13() {
  changeNeoState(13);
  handle_root();
}
void handle_L14() {
  changeNeoState(14);
  handle_root();
}
//-------------------------------------------------------

//-------------------------------------------------------
void handle_bright()
//This function is not working :-( . BUG TO FIX ...
//-------------------------------------------------------
{
  
  
  int brightCharNum = (currentLine.indexOf("=") +1);
  //brightness = currentLine.substring(brightCharNum).toInt();
  //brightness = server.arg(0).toInt();//changes the argument0 to an int
  Serial.print(F("Brightness set to: "));
  Serial.println(brightness);
  Serial.println("BUGGY=>Cancelled");
  //pixels.setBrightness(brightness);//sets it as the global brightness
  //pixels.show();//displays new brightness level
  handle_root();
}

//-------------------------------------------------------
void changeNeoState(int tgt) {
//-------------------------------------------------------

  if (currentLine.indexOf("v") > 0) {
    for (int x = 0; x < sizeof(NeoState); x++)
    {
      NeoState[x] = LOW;
      if (tgt == x)
      {
        NeoState[x] = HIGH;
      }
    }
  }
  
  
//CB: shortcut for debug purpose only, to be enhanced!
switch (tgt) {
    case 0://Red	
	   R=255;
	   G=0;
       B=0;
	   writeLEDS(R, G, B);   
   

      break;
    case 2://Green	  
		R=0;
	   G=255;
       B=0;
	   writeLEDS(R, G, B);   
	
      break;
    case 3:  
	  theaterChaseRainbow(50);
      break;	  
    case 4://Blue
		R=0;
	   G=0;
       B=255;	
	   writeLEDS(R, G, B);   
    
      break;
	  
	 case 5://Green	  
	  rainbow(20);
      break;	  
	  
	 case 8://Mode1  
	  digitalWrite(NEOPIX_MODE_1, LOW);	 
	  digitalWrite(NEOPIX_MODE_2, LOW);

      break;
	  
		  
	 case 10://Mode2 
	  digitalWrite(NEOPIX_MODE_1, HIGH);
	  digitalWrite(NEOPIX_MODE_2, LOW);
      break;	  
	  
	 case 12://Mode3
	  digitalWrite(NEOPIX_MODE_1, LOW);
	  digitalWrite(NEOPIX_MODE_2, HIGH);
      break;	

	 case 9://Mode4
	  digitalWrite(NEOPIX_MODE_1, HIGH);
	  digitalWrite(NEOPIX_MODE_2, HIGH);
      break;		  
		  
	
	  
    case 6://White	
		R=255;
	   G=255;
       B=255;	
	   writeLEDS(R, G, B);   
      break;	
    case 14://OFF
		R=0;
	   G=0;
       B=0;	
	  writeLEDS(R, G, B);   
      break;		  
    default: 
      // if nothing else matches, do the default
      // default is optional
    break;
  }
  
	  
  
}




 /**********************************************************/
 /****            THINGSPEAK FUNCTIONS                  ****/
 /**********************************************************/

//-------------------------------------------------------
void sendDataToThingSpeak(int data){
//Not needed for this project ( data sent from arduino to thingspeak ).
//Kept for information only
//-------------------------------------------------------

	WiFiClient client;
  
    if (client.connect(SERVER, THINGSPEAK_PORT_NUMBER)) {
    Serial.print("connected to server to send data :");
    Serial.println(data);
    // Make a HTTP request:
    sprintf(buffer,"GET /update?api_key=%s&field1=%d&field2=%d HTTP/1.1",API_KEY,SITE,data); 
    Serial.println(buffer);
    client.println(buffer);
    client.print("Host: ");
    client.println(SERVER);
    client.println("Connection: close");
    client.println();
    Serial.println("data sent");
  }else{
    Serial.println("connection error");
  }
  delay(SLEEP_TIME); // ThingSpeak will only accept updates every 15 seconds.
}






 /**********************************************************/
 /****            NEOPIXEL FUNCTIONS                    ****/
 /**********************************************************/


//-------------------------------------------------------
void writeLEDS(byte R, byte G, byte B)//basic write colors to the neopixels with RGB values
//-------------------------------------------------------
{
  for (int i = 0; i < pixels.numPixels(); i ++)
  {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
  }
  pixels.show();
}

//-------------------------------------------------------
void writeLEDS(byte R, byte G, byte B, byte bright)//same as above with brightness added
//-------------------------------------------------------
{
  float fR = (R / 255) * bright;
  float fG = (G / 255) * bright;
  float fB = (B / 255) * bright;
  for (int i = 0; i < pixels.numPixels(); i ++)
  {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
  }
  pixels.show();
}

 

//-------------------------------------------------------
void updateBrightness()
// Increase/Decrease brightness in a smooth mode
//-------------------------------------------------------
{


if ( brightness ==255 )
{
	direction=0;
}


if ( brightness ==0 )
{
	direction=1;
}

if ( direction ==0 )
{
	brightness--;
}
else
{
	brightness++;
}

brightness=255;

  for (int i = 0; i < pixels.numPixels(); i ++)
  {
    //pixels.setPixelColor(i, pixels.Color(R, G, B));
	//pixels.setPixelColor(i, R, G, B, brightness);
	pixels.setBrightness(brightness);
	pixels.setPixelColor(i, R, G, B);
	pixels.show();
	
  }
  //pixels.show();
  //pixels.setBrightness(brightness); // sets the inital brightness of the neopixels
  //pixels.show();//displays new brightness level


// note the time that the connection was made:
lastConnectionTime = millis();

}

//-------------------------------------------------------
void writeLEDS(byte R, byte G, byte B, byte bright, byte LED)// same as above only with individual LEDS
//-------------------------------------------------------
{
  float fR = (R / 255) * bright;
  float fG = (G / 255) * bright;
  float fB = (B / 255) * bright;
  pixels.setPixelColor(LED, pixels.Color(R, G, B));
  pixels.show();
}

//-------------------------------------------------------
unsigned int RGBValue(const char * s)//converts the value to an RGB value
//-------------------------------------------------------
{
  unsigned int result = 0;
  int c ;
  if ('0' == *s && 'x' == *(s + 1)) {
    s += 2;
    while (*s) {
      result = result << 4;
      if (c = (*s - '0'), (c >= 0 && c <= 9)) result |= c;
      else if (c = (*s - 'A'), (c >= 0 && c <= 5)) result |= (c + 10);
      else if (c = (*s - 'a'), (c >= 0 && c <= 5)) result |= (c + 10);
      else break;
      ++s;
    }
  }
  return result;
}




//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 128; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+2*j) % 255));    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}




void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}



// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}