#include "RequestsAndResponses.h"

BuildResponse::BuildResponse(Client &client)
{
    _client = &client;
}

void BuildResponse::begin(const char *code)
{
    _client->print("HTTP/1.1 ");
    _client->println(code);
}

void BuildResponse::addHeader(const char *key, const char *value)
{
    _client->print(key);
    _client->print(": ");
    _client->println(value);
}
void BuildResponse::send(const char *contentType, const char *message, bool newLine)
{
    if (!_alreadyClosed)
    {
        _client->print("Content-Type: ");
        _client->println(contentType);
        _client->println("Connection: close");
        _client->println();
        _alreadyClosed = true;
    }

    if (newLine)
    {
        _client->println(message);
    }
    else
    {
        _client->print(message);
    }
}

void BuildResponse::send(const char *message, bool newLine)
{

    if (!_alreadyClosed)
    {
        _client->print("Content-Type: ");
        _client->println(ContentType::TEXT_PLAIN);
        _client->println("Connection: close");
        _client->println();
        _alreadyClosed = true;
    }

    if (newLine)
    {
        _client->println(message);
    }
    else
    {
        _client->print(message);
    }
}

void BuildResponse::send(const char *contentType, const uint8_t *contentGzip, uint32_t size, std::function<void()> callback)
{
    if (!_alreadyClosed)
    {
        _client->print("Content-Type: ");
        _client->println(contentType);
        _client->println("Connection: close");
        _client->println();
        _alreadyClosed = true;
    }

    // Send the compressed data (the GZIP content)
    for (uint32_t i = 0; i < size; i++)
    {
        // Execute the callback function if it is provided
        if (callback)
        {
            callback();
        }
        
        _client->write(pgm_read_byte_near(&contentGzip[i]));
    }
}

void BuildResponse::send()
{
    if (!_alreadyClosed)
    {
        _client->println("Connection: close");
        _client->println();
        _alreadyClosed = true;
    }
}

void BuildResponse::send(const char *contentType, const char *progmemContent, size_t size)
{
    if (!_alreadyClosed)
    {
        _client->print("Content-Type: ");
        _client->println(contentType);
        _client->println("Connection: close");
        _client->println();
        _alreadyClosed = true;
    }

    // Envia o conteúdo em PROGMEM com um loop for
    for (size_t i = 0; i < size; i++)
    {
        char byteFromProgmem = pgm_read_byte_near(progmemContent + i);
        _client->write(byteFromProgmem);
    }
}

void BuildResponse::send(const char *contentType, fs::FS &fs, const char *path)
{
    if (!_alreadyClosed)
    {
        _client->print("Content-Type: ");
        _client->println(contentType);
        _client->println("Connection: close");
        _client->println();
        _alreadyClosed = true;
    }

    File file = fs.open(path);
    // Verifica se o ponteiro do arquivo é válido e o arquivo não é um diretório
    if (!file || file.isDirectory())
    {
        _client->println("Error: Invalid file");
        return;
    }

    // Envia o conteúdo do arquivo em partes
    uint8_t buffer[512]; // Buffer para leitura do arquivo
    size_t bytesRead;
    while ((bytesRead = file.read(buffer, sizeof(buffer))) > 0)
    {
        _client->write(buffer, bytesRead);
    }

    // Fecha o arquivo após o envio
    file.close();
}