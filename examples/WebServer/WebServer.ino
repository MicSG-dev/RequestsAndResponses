/**
 * @file WebServer.ino
 * @brief Example sketch demonstrating usage of RequestsAndResponses library for HTTP server implementation on ESP32
 *
 * This sketch implements a web server using the RequestsAndResponses library and EthernetLarge library.
 * It handles HTTP GET, POST, PUT and DELETE methods and demonstrates request parsing and response building.
 * 
 * Features:
 * - HTTP method handling (GET, POST, PUT, DELETE)
 * - URL routing (/test, /status-led endpoints)
 * - Request header parsing
 * - Parameter and cookie parsing
 * - Response building with status codes and headers
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
 * Tips: 
 * - For testing, use Postman to send HTTP requests to the server and check the responses.
 * - You can download Postman from https://www.postman.com/downloads/.
 *
 * @author Michel Galvão
 * @see https://github.com/MicSG-dev
 * @see https://github.com/MicSG-dev/RequestsAndResponses
 * @contact contato@micsg.com.br
 * 
 * @date Created: 2025-01-03
 * @version 1.0.0
 * @copyright MIT License
 */


#include "Arduino.h"
#include <SPI.h>
#include <EthernetLarge.h>
#include "RequestsAndResponses.h"

// Configurações da rede
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // Endereço MAC fictício
IPAddress ip(192, 168, 0, 177);                    // IP estático
EthernetServer server(80);                         // Servidor na porta 80

void setup()
{
  Serial.begin(115200);
  delay(1000);

  while (!Serial)
  {
    ; // Aguarda a inicialização da Serial
  }

  Serial.println("Example RequestsAndResponses WebServer");

  Ethernet.init(5); // CS pin
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    while (1)
      ; // loop infinito
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

    char fruit[50] = ""; // increase the array size as needed

    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c); // For debugging purposes, every request received is printed in the Serial Monitor

        // Detect end of HTTP header (empty line)
        if (c == '\n' && currentLineIndex == 0)
        {
          if (request.metodoIs(MetodosHttp::GET)) // Check if the request method is GET
          {
            Serial.println("GET method!");
            Serial.print("URL: ");
            Serial.println(request.getUrl());
            Serial.print("Content-Length: ");
            Serial.println(request.getContentLength());
            Serial.print("Content-Type: ");
            Serial.println(request.getContentType());

            // Check if the URL is '/test'
            if (request.urlIs("/test"))
            {
              Serial.println("URL '/test' detected");

              // Build the response
              BuildResponse response(client);
              response.begin(StatusCode::Successful::_202_ACCEPTED);          // Set the response status code
              response.addHeader("Test", "Test value");                       // Add a custom header
              response.send(ContentType::TEXT_PLAIN, "URL '/test' detected"); // Send the response with a content type and content

              Serial.print("Parameters: ");
              Serial.println(request.getParams());

              // Check if the parameter 'name' exists
              if (request.paramExists("name"))
              {
                Serial.print("Parameter 'name' exists: ");
                Serial.println(request.getParam("name"));
              }
              else
              {
                Serial.println("Parameter 'name' does not exist");
              }

              Serial.print("All cookies: ");
              Serial.println(request.getCookies());
              Serial.print("Cookie 'Car': ");
              Serial.println(request.getCookie("Car"));
              Serial.print("Header 'Fruit': ");
              Serial.println(fruit);
            }
            else
            {
              // Build the response
              BuildResponse response(client);
              response.begin(StatusCode::ClientError::_404_NOT_FOUND); // Set the response status code
              response.addHeader("Hello", "World!");                   // Add a custom header
              response.send(ContentType::TEXT_PLAIN, "URL not found"); // Send the response with a content type and content
            }
            break;
          }
          else if (request.metodoIs(MetodosHttp::POST)) // Check if the request method is POST
          {
            Serial.println("POST method!");
            Serial.print("URL: ");
            Serial.println(request.getUrl());
            Serial.print("Content-Length: ");
            Serial.println(request.getContentLength());
            Serial.print("Content-Type: ");
            Serial.println(request.getContentType());

            if (request.urlIs("/status-led"))
            {
              Serial.println("URL '/status-led' detected");

              while (client.available())
              {
                char c = client.read();
                Serial.write(c);
              }
              Serial.println();

              // Build the response
              BuildResponse response(client);
              response.begin(StatusCode::Successful::_202_ACCEPTED);                      // Set the response status code
              response.send(ContentType::TEXT_PLAIN, "LED status changed successfully!"); // Send the response with a content type and content
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
          else if (request.metodoIs(MetodosHttp::PUT))
          {
            Serial.println("PUT method!");
            Serial.print("URL: ");
            Serial.println(request.getUrl());

            BuildResponse response(client);
            response.begin(StatusCode::Successful::_202_ACCEPTED);
            response.send(ContentType::TEXT_PLAIN, "PUT method detected");
            break;
          }
          else if (request.metodoIs(MetodosHttp::DELETE))
          {
            Serial.println("DELETE method!");
            Serial.print("URL: ");
            Serial.println(request.getUrl());

            BuildResponse response(client);
            response.begin(StatusCode::Successful::_202_ACCEPTED);
            response.send(ContentType::TEXT_PLAIN, "DELETE method detected");
            break;
          }else{
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

          Header headerAtual = request.analisarLinhaHttp(currentLine);
          if (headerAtual.chave != NULL)
          {
            if (strcmp(headerAtual.chave, "Fruit") == 0)
            {
              strncpy(fruit, headerAtual.valor, sizeof(fruit));
            }
          }

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