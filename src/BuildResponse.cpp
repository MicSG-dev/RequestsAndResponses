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

void BuildResponse::addHeader(const char *chave, const char *valor)
{
    _client->print(chave);
    _client->print(": ");
    _client->println(valor);
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