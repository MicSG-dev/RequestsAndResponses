#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include <Arduino.h>

enum MetodosHttp
{
    UNKNOWN,
    GET,
    POST,
    PUT,
    DELETE
};

struct Header
{
    const char *chave;
    const char *valor;
};

class AnalyserRequest
{
public:
    AnalyserRequest();
    Header analisarLinhaHttp(const char *linha);

    MetodosHttp getMetodo();
    bool metodoIs(MetodosHttp metodo);
    const char *getUrl();
    bool urlIs(const char *url);
    size_t getContentLength();
    const char *getContentType();
    const char *getParam(const char *param);
    const char *getParams();
    bool paramExists(const char *param);
    const char *getHost();
    const char *getUserAgent();
    const char *getAuthorization();
    const char *getCookie(const char *cookie);
    const char *getCookies();

private:
    char _host[128];
    int _numHeadersCustom;
    MetodosHttp _metodo;

    char _url[512];
    size_t _contentLength;
    char _contentType[128];
    char _userAgent[128];
    char _authorization[128];
    char _cookie[512];

    char *_params;
    bool _haParametros;
};

class BuildResponse{
    public:
        BuildResponse();
        void send(const char *message);
        void send(const String &message);
        void send(int code, const char *message);
        void send(int code, const String &message);
        void send(int code, const char *contentType, const char *message);
        void send(int code, const char *contentType, const String &message);
        void send(int code, const char *contentType, const char *message, size_t length);
        void send(int code, const char *contentType, const String &message, size_t length);
    private:
        void sendHeader(int code, const char *contentType, size_t length);
   
};

#endif // HTTPPARSER_H