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

void BuildResponse::send(const char *contentType, const uint8_t *contentGzip, uint32_t size)
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