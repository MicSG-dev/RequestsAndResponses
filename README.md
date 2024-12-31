# RequestsAndResponses Library

Welcome to the **RequestsAndResponses** library for the Arduino projects. This library is designed to simplify handling HTTP requests and responses for your ESP32 projects.

## Features

- **Easy-to-use API**: Simplifies the process of sending and receiving HTTP requests.
- **Support for various HTTP methods**: GET, POST, PUT, DELETE, etc.
- **Customizable headers and payloads**: Easily add headers and payloads to your responses.
- **Error handling**: Built-in error handling for robust applications.
## Installation

To install the RequestsAndResponses library in your PlatformIO project, follow these steps:

1. Open your `platformio.ini` file and add the following line to the `lib_deps` section:
    ```ini
    lib_deps =
        https://github.com/yourusername/RequestsAndResponses.git
    ```

2. Save the `platformio.ini` file. PlatformIO will automatically download and install the library when you build your project.

3. Include the library in your project:
    ```cpp
    #include <RequestsAndResponses.h>
    ```

## Usage

Here is a simple example to get you started with Ethernet:

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <RequestsAndResponses.h>

// Enter a MAC address and IP address for your controller below.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

void setup() {
    Serial.begin(115200);
    // Initialize the Ethernet client library
    Ethernet.begin(mac, ip);
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
        while (true) {
            delay(1); // do nothing, no point running without Ethernet hardware
        }
    }
    // Check for Ethernet connection
    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }

    server.begin();
}

void loop() {
    EthernetClient client = server.available();
    if (client)
    {
        char currentLine[512] = "";  // Linha atual da requisição
        size_t currentLineIndex = 0; // Índice da linha atual

        AnalyserRequest request;

        char myHeader[50]; // tamanho necessário customizavel

        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                Serial.write(c);

                // Detecta o fim do cabeçalho HTTP (linha vazia)
                if (c == '\n' && currentLineIndex == 0)
                {
                    if (request.metodoIs(MetodosHttp::GET))
                    {
                        Serial.println("metodo GET");
                        Serial.print("URL: ");
                        Serial.println(request.getUrl());
                        Serial.print("Content-Length: ");
                        Serial.println(request.getContentLength());
                        Serial.print("Content-Type: ");
                        Serial.println(request.getContentType());

                        // Verifica se a URL é a /teste
                        if (request.urlIs("/teste"))
                        {
                            Serial.println("URL /teste detectada");
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/plain");
                            client.println("Connection: close");
                            client.println();
                            client.println("URL /teste detectada");

                            Serial.print("Parametros: ");
                            Serial.println(request.getParams());

                            if (request.paramExists("nome"))
                            {
                                Serial.print("Parametro 'nome' existe: ");
                                Serial.println(request.getParam("nome"));
                            }
                            else
                            {
                                Serial.println("Parametro 'nome' não existe");
                            }

                            Serial.print("Todos cookies: ");
                            Serial.println(request.getCookies());
                            Serial.print("Cookie 'myCookie': ");
                            Serial.println(request.getCookie("myCookie"));
                            Serial.print("Header 'myHeader': ");
                            Serial.println(myHeader);
                        }
                        else
                        {
                            client.println("HTTP/1.1 404 Not Found");
                            client.println("Content-Type: text/plain");
                            client.println("Connection: close");
                            client.println();
                            client.println("URL não encontrada");
                        }
                        break;
                    }
                    else if (request.metodoIs(MetodosHttp::POST))
                    {
                        Serial.println("metodo POST");
                        Serial.print("URL: |");
                        Serial.print(request.getUrl());
                        Serial.println("|");

                        if(request.urlIs("/ola"))
                        {
                        
                            Serial.println("Endpoint '/ola' acessada");
                            while (client.available())
                            {
                                char c = client.read();
                                Serial.write(c);
                            }
                            
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/plain");
                            client.println("Connection: close");
                            client.println();
                            client.println("Olá, mundo!");
                        }
                        else
                        {
                            client.println("HTTP/1.1 404 Not Found");
                            client.println("Content-Type: text/plain");
                            client.println("Connection: close");
                            client.println();
                            client.println("URL não encontrada");
                        }

                        break;
                    }
                    else if (request.metodoIs(MetodosHttp::PUT))
                    {
                        Serial.println("metodo PUT");
                        break;
                    }
                    else if (request.metodoIs(MetodosHttp::DELETE))
                    {
                        Serial.println("metodo DELETE");
                        break;
                    }
                }

                // Processa o cabeçalho HTTP
                if (c == '\n')
                {
                    currentLine[currentLineIndex] = '\0'; // Termina a string

                    Header headerAtual = request.analisarLinhaHttp(currentLine);
                    if (headerAtual.chave != NULL)
                    {
                        if (strcmp(headerAtual.chave, "abacaxi") == 0)
                        {
                        strncpy(abacaxi, headerAtual.valor, sizeof(abacaxi));
                        }
                    }

                    // verifica se o header

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
                        Serial.println("Erro: Capacidade do array currentLine excedida!");
                        currentLineIndex = 0; // Reinicia o índice para evitar erros futuros
                    }
                }
            }
        }

        delay(1);
        client.stop();
        Serial.println("Cliente desconectado.");
    }
}
```

## Documentation

For detailed documentation and advanced usage, please refer to the [official documentation](https://github.com/yourusername/RequestsAndResponses/wiki).

## Contributing

We welcome contributions! Please read our [contributing guidelines](https://github.com/yourusername/RequestsAndResponses/blob/main/CONTRIBUTING.md) before submitting a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/yourusername/RequestsAndResponses/blob/main/LICENSE) file for more details.

## Contact

For any questions or suggestions, feel free to open an issue or contact us at [contato@micsg.com.br](mailto:contato@micsg.com.br).

Happy coding!
