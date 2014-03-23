/**
 * Description:
 *   This is the server side code of a server-client system controlling a
 *   NeoPixel LED strip. The client side is an Android app that allows for 
 *   the turning on and off of the strip and the change of its color.
 *   
 * 	 For the server side, the equipment needed is an Arduino Ethernet or any
 *   Arduino compatible board with an Ethernet shield. For the client side, an
 *   Android phone or tablet is needed.
 * 
 * Configuration:
 *   Server side:
 *     Specify the following:  ( look for the arrows, <====< )
 *     > MAC address of the controller (it's on the back side)
 *     > IP address (one of your choice that's not already occupied in your subnet)
 *         and Port number of the server (You may leave them as is)
 *     > The NeoPixel LED strip data pin and the number of the LEDs on the strip
 *   Client side:
 *     > Install the Android app, ArduinoPixel, from Google Play.
 *     > Open the app, go to Settings, and update the host IP address and Port number
 *         (Not necessary if you didn't change them on the server side)
 *     > Enjoy!
 * 
 * Specifications:
 *   For the server side:
 *     A server that accepts the following HTTP requests:
 *     > GET / HTTP/1.1                  : Responds w/ "Hello from Arduino Server"
 *     > GET /strip/status/ HTTP/1.1     : Responds w/ "ON"/"OFF" for the power state of the strip
 *     > PUT /strip/status/on/ HTTP/1.1  : Turns the LED strip on
 *     > PUT /strip/status/off/ HTTP/1.1 : Turns the LED strip off
 *     > PUT /strip/color/ HTTP/1.1      : Changes the color of the LED strip
 *                                         The colors are provided as a JSON object: {
 *                                                                                     "r": x,
 *                                                                                     "g": x,
 *                                                                                     "b": x
 *                                                                                   }
 *   For the client side:
 *     An Android app that:
 *     > allows to configure the host's IP address and Port number
 *     > allows to turn the LED strip on and off
 *     > allows for the continuous real-time change of the strip's color
 *     > is available on Google Play, w/ the name ArduinoPixel
 * 
 * ArduinoPixel:
 *   You can find the app's source on github:
 *     https://github.com/pAIgn10/ArduinoPixel
 * 
 * Author:
 *   Nick Lamprianidis { paign10.ln [at] gmail [dot] com }
 * 
 * License:
 *   Copyright (c) 2014 Nick Lamprianidis 
 *   This code is released under the MIT license
 *   http://www.opensource.org/licenses/mit-license.php
 */

#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

// #define DEBUG

// !!! Specify the pin and number of leds for your LED strip              <====<
const int STRIP_PIN = 8;
const int NUM_OF_LEDS_ON_STRIP = 60;

// !!! Specify the MAC address and IP address for your controller         <====<
// The IP address will be dependent on your local network
byte mac[] = { 0x80, 0xC2, 0x2A, 0x4E, 0x71, 0xD9 };
IPAddress ip(192, 168, 1, 100);  // You may leave this as is
const int PORT = 80;  // You may leave this as is

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_OF_LEDS_ON_STRIP, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// Initializes an EthernetServer
// instance with port number
EthernetServer server(PORT);

EthernetClient client;

// HTTP request methods
enum Method
{
    GET,
    PUT
};

// Available URIs
enum Uri
{
    ROOT,        // "/"
    STATUS,      // "/strip/status/"
    STATUS_ON,   // "/strip/status/on/"
    STATUS_OFF,  // "/strip/status/off/"
    COLOR        // "/strip/color/"
};

// Specifies whether the LED strip is on or off
boolean power = false;

// Keeps the current color of the LED strip
uint8_t color[] = { 128, 128, 128 };


void setup() {
    #if defined(DEBUG)
    Serial.begin(9600);  // Initializes the Serial port
    while (!Serial) ;  // Needed for Leonardo only
    #endif

    // Starts the Ethernet connection and the server
    Ethernet.begin(mac, ip);
    server.begin();
    
    #if defined(DEBUG)
    Serial.print("Server is at ");
    Serial.println(Ethernet.localIP());
    Serial.println();
    #endif
  
    // Initializes the LED strip
    strip.begin();
    strip.show();
}

void loop()
{
    // If there is an available connection, 
    // it reads the request
    client = server.available();
    if (client) {
        uint8_t i = 0;
        char reqChar;
        char request[35];
        char buf[60];

        // Reads the request line
        while (client.available())
        {
            reqChar = client.read();

            if (reqChar != '\r')
            {
                request[i++] = reqChar;
            }
            else
            {
                client.read();  // Gets rid of '\n'
                request[i] = '\0';
                break;
            }
        }
        
        int8_t method, uri;
        parseRequestLine(method, uri, String(request));

        // Initiates the appropriate response
        switch (method)
        {
            case GET:
                switch (uri)
                {
                    case ROOT:
                        respond(200, "OK", false, "Hello from Arduino Server");
                        break;

                    case STATUS:
                        respond(200, "OK", false, power?"ON":"OFF");
                        break;

                    default:
                        respond(404, "Not Found", false, '\0');
                }
                break;

            case PUT:
                switch (uri)
                {
                    // Turns the LED strip on
                    case STATUS_ON:
                        power = true;
                        colorize(color[0], color[1], color[2]);
                        respond(200, "OK", false, '\0');
                        break;

                    // Turn the LED strip off
                    case STATUS_OFF:
                        power = false;
                        colorize(0, 0, 0);
                        respond(200, "OK", false, '\0');
                        break;

                    // Parses the JSON data (and updates the LED strip color)
                    case COLOR:
                        getJson(buf);  // Retrieves the JSON data
                        parseColors(String(buf));  // Parses the data
                        if (power)
                            colorize(color[0], color[1], color[2]);
                        respond(200, "OK", true, '\0');
                        break;

                    default:
                        respond(404, "Not Found", false, '\0');
                }
                break;

            default:
                respond(404, "Not Found", false, '\0');
        }
    }
}

// Parses the HTTP request line
// Parameters: method  - the method of the request
//             uri     - the requested uri
//             request - string that holds the request line
void parseRequestLine(int8_t& method, int8_t& uri, String request)
{
    #if defined(DEBUG)
    Serial.println(request);
    #endif
    
    // Parses the request method
    String reqMethod = request.substring(0, 3);
    if (reqMethod.equals("GET"))
        method = GET;
    else if (reqMethod.equals("PUT"))
        method = PUT;
    else
        method = -1;

    // Parses the URI
    String reqUri = request.substring(4, request.lastIndexOf(' '));
    if (reqUri.equals("/"))
        uri = ROOT;
    else if (reqUri.equals("/strip/status/"))
        uri = STATUS;
    else if (reqUri.equals("/strip/status/on/"))
        uri = STATUS_ON;
    else if (reqUri.equals("/strip/status/off/"))
        uri = STATUS_OFF;
    else if (reqUri.equals("/strip/color/"))
        uri = COLOR;
    else
        uri = -1;
        
    #if defined(DEBUG)
    Serial.print("Method: ");
    Serial.println(method);
    Serial.print("URI: ");
    Serial.println(uri);
    Serial.println();
    #endif
}

// Sends an HTTP response
// Parameters: statusCode - Status code for the response line
//             statusMsg  - Status message for the response line
//             keepAlive  - Whether to keep the connection open or not
//             data       - Data to return to client
void respond(int statusCode, const char* statusMsg, boolean keepAlive, const char* data)
{
        #if defined(DEBUG)
        Serial.print("HTTP/1.1 ");
        Serial.print(statusCode);
        Serial.print(" ");
        Serial.println(statusMsg);
        if (!keepAlive) Serial.println("Connection: close");
        Serial.println();
        if (data) { Serial.print(data);
                    Serial.println();
                    Serial.println(); }
        #endif

        client.print("HTTP/1.1 ");
        client.print(statusCode);
        client.print(" ");
        client.println(statusMsg);
        if (!keepAlive) client.println("Connection: close");
        if (data) { client.println();
                    client.print(data); }
        delay(1);
        client.stop();
}

// Retrieves the JSON data
// Parameters: buf - the buffer the store the data
void getJson(char* buf)
{
    uint8_t i = 0;
    char prevlastCharRead = '\0';
    char lastCharRead;

    // Gets rid of request headers
    while (client.available())
    {
        lastCharRead = client.read();
        if (prevlastCharRead == '\r' && lastCharRead == '\n')
        {
            char c1 = '\0', c2 = '\0';
            if (client.available()) c1 = client.read();
            if (client.available()) c2 = client.read();
            if (c1 == '\r' && c2 == '\n') break;
        }
        prevlastCharRead = lastCharRead;
    }

    // Retrieves the JSON data
    while (client.available()) buf[i++] = client.read();
    buf[i] = '\0';
    
    #if defined(DEBUG)
    Serial.println(buf);
    Serial.println();
    #endif
}

// Parses the colors from the JSON data
// Parameters: json - Holds the data
void parseColors(String json)
{
    uint8_t idx = 0;
    for (uint8_t i = 0; i < 3; ++i)
    {
        while (json[idx++] != ':') ;
        uint8_t startIdx = idx;
        while (json[idx] != ',' && json[idx] != '}') idx++;
        uint8_t endIdx = idx;
        String colorStr = json.substring(startIdx, endIdx);
        color[i] = (uint8_t) colorStr.toInt();
    }
}

// Updates the LED strip
// Parameters: r - red color value
//             g - green color value
//             b - blue color value
void colorize(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint16_t i = 0; i < strip.numPixels(); ++i)
        strip.setPixelColor(i, r, g, b);
    strip.show();
}
