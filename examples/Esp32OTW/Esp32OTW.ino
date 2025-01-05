/**
 * @file Esp32OTW.ino
 * @brief ESP32 Over-The-Wire (OTW) firmware update example using Ethernet
 *
 * This sketch demonstrates a web server implementation on ESP32 that enables
 * firmware updates over Ethernet. It provides endpoints for version checking and
 * firmware updates through HTTP requests.
 *
 * Features:
 * - Web server on port 80 for handling HTTP requests
 * - GET /version - Returns current firmware version in JSON format
 * - POST /otw - Handles firmware update upload via binary file
 * - DHCP support for network configuration
 * - Progress monitoring during firmware update
 * - Automatic restart after successful update
 * - Error handling and status reporting
 * - Serial debug output
 *
 * Required Hardware:
 * - ESP32 board
 * - W5500 Ethernet module (CS pin on GPIO5)
 *
 * Required Libraries:
 * - EthernetLarge (https://github.com/MicSG-dev/EthernetLarge)
 * - RequestsAndResponses (https://github.com/MicSG-dev/RequestsAndResponses)
 * - SPI (Built-in)
 *
 * Tips:
 * - For testing, use Postman to send HTTP requests to the server and check the responses.
 * - You can download Postman from https://www.postman.com/downloads/.
 * - To upload firmware with Postman, use the POST method and select "Body" -> "binary"
 *   and choose the compiled updated firmware. Then, upload the binary firmware file to the ESP32.
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
#include <Update.h>
#include "RequestsAndResponses.h"

// Network settings
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // Fictitious MAC address
IPAddress ip(192, 168, 0, 177);                    // Static IP
EthernetServer server(80);                         // Server on port 80

const char *VERSION_FIRMWARE = "0.0.1"; // Defines the firmware version as a constant string.

bool shouldRestart = false;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  while (!Serial)
  {
    ; // Wait for Serial to initialize
  }

  Serial.println("Example RequestsAndResponses ESP32 OTW (Over-The-Wire)");
  Serial.print("Firmware version ");
  Serial.println(VERSION_FIRMWARE);

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

  if (shouldRestart == true)
  {
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

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
            // Check if the URL is '/version'
            if (request.urlIs("/version"))
            {
              Serial.println("URL '/version' detected");

              // Build the response
              BuildResponse response(client);
              response.begin(StatusCode::Successful::_200_OK);                        // Set the response status code
              response.send(ContentType::APPLICATION_JSON, "{\"version\":\"", false); // Send the response as JSON, without sending line breaks (false argument)
              response.send(VERSION_FIRMWARE, false);                                 // Send the rest of the response without line breaks (false argument)
              response.send("\"}");                                                   // Send the rest of the response without line breaks (false argument)
            }
            else
            {
              // Build the response
              BuildResponse response(client);
              response.begin(StatusCode::ClientError::_404_NOT_FOUND); // Set the response status code
              response.send(ContentType::TEXT_PLAIN, "URL not found"); // Send the response with a content type and content
            }
            break;
          }
          else if (request.methodIs(MethodsHttp::POST)) // Check if the request method is POST
          {
            if (request.urlIs("/otw"))
            {
              Serial.println("URL '/otw' detected");

              if (request.getContentLength() > 0)
              {
                if (!Update.begin(request.getContentLength(), U_FLASH)) // Initiates the firmware update process with the specified content length and flash type (flash [U_FLASH] or file system [U_SPIFFS]).
                {
                  Serial.printf("Error starting OTW: %s\n", Update.errorString());

                  BuildResponse response(client);
                  response.begin(StatusCode::ServerError::_500_INTERNAL_SERVER_ERROR);
                  response.send("Error starting OTW: ", false);
                  response.send(Update.errorString());

                  break;
                }
              }
              else
              {
                Serial.println("Content-Length not found or invalid.");
                BuildResponse response(client);
                response.begin(StatusCode::ClientError::_400_BAD_REQUEST);
                response.send(ContentType::TEXT_PLAIN, "Content-Length not found or invalid.");

                break;
              }

              uint8_t buffer[512];
              size_t totalWritten = 0;
              while (client.available())
              {
                size_t bytesRead = client.read(buffer, sizeof(buffer));
                if (Update.write(buffer, bytesRead) != bytesRead) // Writes the received data to the flash memory. If the number of bytes written does not match the number of bytes read, an error occurred.
                {
                  Serial.println("Error writing OTA data block");
                  break;
                }

                totalWritten += bytesRead;
                Serial.print("Progress: ");
                Serial.print(totalWritten);
                Serial.println(" bytes written");
              }

              if (Update.end()) // Finalizes the firmware update process. If the update ends successfully, the function returns true.
              {
                if (Update.isFinished()) // Checks if the firmware update process has finished successfully.
                {
                  Serial.println("OTW completed successfully! Scheduled for Restart");
                  shouldRestart = true;

                  BuildResponse response(client);
                  response.begin(StatusCode::Successful::_200_OK);
                  response.send(ContentType::TEXT_PLAIN, "OTW completed successfully! Scheduled for Restart");
                }
                else
                {
                  Serial.println("OTW was not completed correctly");

                  BuildResponse response(client);
                  response.begin(StatusCode::ServerError::_500_INTERNAL_SERVER_ERROR);
                  response.send(ContentType::TEXT_PLAIN, "OTW was not completed correctly");
                }
              }
              else
              {
                Serial.printf("Error completing OTW: %s\r\n", Update.errorString());

                BuildResponse response(client);
                response.begin(StatusCode::ServerError::_500_INTERNAL_SERVER_ERROR);
                response.send(ContentType::TEXT_PLAIN, "Error completing OTW: ", false);
                response.send(Update.errorString());
              }
            }
            else
            {
              // Build the response
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