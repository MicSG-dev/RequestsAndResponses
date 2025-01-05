/**
 * @file WebServerGzip.ino
 * @brief Example sketch demonstrating usage of RequestsAndResponses library with Gzip compression on ESP32
 *
 * This sketch implements a web server using the RequestsAndResponses library and EthernetLarge library
 * that serves gzipped web content. It handles HTTP GET requests and demonstrates serving compressed
 * static files like HTML, CSS and JavaScript.
 *
 * Features:
 * - HTTP GET method handling
 * - URL routing (/, /index.html, /assets/*)
 * - Serving gzipped static files
 * - Content-Encoding handling
 * - 302 redirects
 *
 * Hardware Requirements:
 * - ESP32 board
 * - Ethernet W5500 module (CS pin on GPIO5)
 *
 * Required Libraries:
 * - EthernetLarge (https://github.com/MicSG-dev/EthernetLarge)
 * - RequestsAndResponses (https://github.com/MicSG-dev/RequestsAndResponses)
 * - SPI (Built-in)
 *
 * @author Michel Galvão
 * @see https://github.com/MicSG-dev
 * @see https://github.com/MicSG-dev/RequestsAndResponses
 * @contact contato@micsg.com.br
 *
 * @date Created: 2025-01-04
 * @version 1.0.0
 * @copyright MIT License
 */

#include "Arduino.h"
#include <SPI.h>
#include <EthernetLarge.h>
#include "RequestsAndResponses.h"
#include "gzip.h" // Use the software https://github.com/MicSG-dev/Script_export_files_web_gzip_decimal_arduino to convert files to gzip array

// Network settings
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // Fictitious MAC address
IPAddress ip(192, 168, 0, 177);                    // Static IP
EthernetServer server(80);                         // Server on port 80

void setup()
{
    Serial.begin(115200);
    delay(1000);

    while (!Serial)
    {
        ; // Wait for Serial to initialize
    }

    Serial.println("Example RequestsAndResponses WebServer");

    Ethernet.init(5); // CS pin
    if (Ethernet.begin(mac) == 0)
    {
        Serial.println("Failed to configure Ethernet using DHCP");
        while (1)
            ; // infinite loop
    }

    Serial.print("Server started. IP: ");
    Serial.println(Ethernet.localIP());

    server.begin();
}

void loop()
{

    EthernetClient client = server.available();

    if (client)
    {
        IPAddress remoteClient = client.remoteIP();
        Serial.printf("\r\nConnected client: %u.%u.%u.%u\r\n", remoteClient[0], remoteClient[1], remoteClient[2], remoteClient[3]);

        char currentLine[640] = "";  // Current line of the request. 640 is the maximum a line can have by default, where this is the result of the sum of Key (128) + Value (512) in Others Headers.
        size_t currentLineIndex = 0; // Current line index

        AnalyserRequest request;

        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                Serial.write(c); // For debugging purposes, every request received is printed in the Serial Monitor

                // Detect end of HTTP header (empty line)
                if (c == '\n' && currentLineIndex == 0)
                {
                    if (request.methodIs(MethodsHttp::GET)) // Check if the request method is GET
                    {
                        Serial.println("GET method!");
                        Serial.print("URL: ");
                        Serial.println(request.getUrl());
                        Serial.print("Content-Length: ");
                        Serial.println(request.getContentLength());
                        Serial.print("Content-Type: ");
                        Serial.println(request.getContentType());

                        // Check if the URL is '/test'
                        if (request.urlIs("/"))
                        {
                            BuildResponse response(client);
                            response.begin(StatusCode::Redirection::_302_FOUND); // Set the HTTP status code to 302 Found, indicating that the requested resource resides temporarily under a different URI
                            response.addHeader("Location", "/index.html");       // Add a Location header to specify the new URI where the requested resource can be found
                            response.send();                                     // Send the HTTP response to the client
                        }
                        else if (request.urlIs("/index.html"))
                        {
                            BuildResponse response(client);
                            response.begin(StatusCode::Successful::_200_OK);                                                    // Set the response status code
                            response.addHeader("Content-Encoding", "gzip");                                                     // Set the Content-Encoding header to indicate that the response content is compressed using gzip
                            response.send(ContentType::TEXT_HTML, web_gzip::_INDEX_HTML::content, web_gzip::_INDEX_HTML::size); // Send the response with a content type, content and size of content
                        }
                        else if (request.urlIs("/assets/bootstrap/css/bootstrap.min.css"))
                        {
                            BuildResponse response(client);
                            response.begin(StatusCode::Successful::_200_OK);
                            response.addHeader("Content-Encoding", "gzip");
                            response.send(ContentType::TEXT_CSS, web_gzip::_ASSETS_BOOTSTRAP_CSS_BOOTSTRAP_MIN_CSS::content, web_gzip::_ASSETS_BOOTSTRAP_CSS_BOOTSTRAP_MIN_CSS::size);
                        }
                        else if (request.urlIs("/assets/bootstrap/js/bootstrap.min.js"))
                        {
                            BuildResponse response(client);
                            response.begin(StatusCode::Successful::_200_OK);
                            response.addHeader("Content-Encoding", "gzip");
                            response.send(ContentType::TEXT_JAVASCRIPT, web_gzip::_ASSETS_BOOTSTRAP_JS_BOOTSTRAP_MIN_JS::content, web_gzip::_ASSETS_BOOTSTRAP_JS_BOOTSTRAP_MIN_JS::size);
                        }
                        else
                        {
                            BuildResponse response(client);
                            response.begin(StatusCode::ClientError::_404_NOT_FOUND); // Set the response status code
                            response.send(ContentType::TEXT_PLAIN, "URL not found"); // Send the response with a content type and content
                        }
                        break;
                    }
                    else
                    {
                        Serial.println("Unknown method.");
                        BuildResponse response(client);
                        response.begin(StatusCode::ClientError::_405_METHOD_NOT_ALLOWED);
                        response.send(ContentType::TEXT_PLAIN, "Method not allowed");
                        break;
                    }
                }

                // Process the HTTP header
                if (c == '\n')
                {
                    currentLine[currentLineIndex] = '\0'; // Ends the string

                    request.analyzeHttpLine(currentLine); // Use "Header currentHeader = request.analyzeHttpLine..." to get the current Header

                    currentLineIndex = 0;
                }
                else if (c != '\r')
                {
                    if (currentLineIndex < sizeof(currentLine) - 1)
                    {
                        currentLine[currentLineIndex++] = c;
                    }
                    else
                    {
                        Serial.println("Error: CurrentLine array capacity exceeded!");
                        currentLineIndex = 0; // Reset the index to avoid future errors
                    }
                }
            }
        }

        delay(1);
        client.stop();
        Serial.println("Client disconnected.");
    }
}
