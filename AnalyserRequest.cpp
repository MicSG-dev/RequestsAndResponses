#include "RequestsAndResponses.h"

AnalyserRequest::AnalyserRequest()
{
    _contentLength = 0;
    _haParametros = false;
    _contentType[0] = ' ';
    _numHeadersCustom = 0;
    _metodo = MetodosHttp::UNKNOWN;
}

Header AnalyserRequest::analisarLinhaHttp(const char *linha)
{
    if (_metodo == MetodosHttp::UNKNOWN && strncmp(linha, "POST", 4) == 0)
    {
        _metodo = MetodosHttp::POST;

        // Extrai a URL da requisição
        char *urlInicio = strstr(linha, " ") + 1;
        char *urlFim = strchr(urlInicio, ' ');
        strncpy(_url, urlInicio, urlFim - urlInicio);
        _url[urlFim - urlInicio] = '\0'; // Finaliza a string
    }
    else if (_metodo == MetodosHttp::UNKNOWN && strncmp(linha, "GET", 3) == 0)
    {
        _metodo = MetodosHttp::GET;

        // Extrai a URL da requisição
        char *urlInicio = strstr(linha, " ") + 1;
        char *urlFim = strchr(urlInicio, ' ');
        strncpy(_url, urlInicio, urlFim - urlInicio);
        _url[urlFim - urlInicio] = '\0'; // Finaliza a string

        // Verifica se a URL contém parâmetros
        char *paramsInicio = strchr(_url, '?');

        if (paramsInicio != NULL)
        {
            // Separar a URL dos parâmetros
            *paramsInicio = '\0'; // Termina a URL antes de '?'

            // Extrai os parâmetros (após '?')
            _params = paramsInicio + 1;
            _haParametros = true;
        }
    }
    else if (_metodo == MetodosHttp::UNKNOWN && strncmp(linha, "PUT", 3) == 0)
    {
        _metodo = MetodosHttp::PUT;

        // Extrai a URL da requisição
        char *urlInicio = strstr(linha, " ") + 1;
        char *urlFim = strchr(urlInicio, ' ');
        strncpy(_url, urlInicio, urlFim - urlInicio);
        _url[urlFim - urlInicio] = '\0'; // Finaliza a string
    }
    else if (_metodo == MetodosHttp::UNKNOWN && strncmp(linha, "DELETE", 6) == 0)
    {
        _metodo = MetodosHttp::DELETE;

        // Extrai a URL da requisição
        char *urlInicio = strstr(linha, " ") + 1;
        char *urlFim = strchr(urlInicio, ' ');
        strncpy(_url, urlInicio, urlFim - urlInicio);
        _url[urlFim - urlInicio] = '\0'; // Finaliza a string
    }
    else if (strncmp(linha, "Content-Length: ", 16) == 0)
    {
        _contentLength = atoi(linha + 16);
    }
    else if (strncmp(linha, "Content-Type: ", 14) == 0)
    {
        strncpy(_contentType, linha + 14, sizeof(_contentType));
    }
    else if (strncmp(linha, "Host: ", 6) == 0)
    {
        strncpy(_host, linha + 6, sizeof(_host));
    }
    else if (strncmp(linha, "User-Agent: ", 12) == 0)
    {
        strncpy(_userAgent, linha + 12, sizeof(_userAgent));
    }
    else if (strncmp(linha, "Authorization: ", 15) == 0)
    {
        strncpy(_authorization, linha + 15, sizeof(_authorization));
    }
    else if (strncmp(linha, "Cookie: ", 8) == 0)
    {
        strncpy(_cookie, linha + 8, sizeof(_cookie));
    }
    else
    {
        // Verifica se é um header no formato "Chave: Valor"
        Header headerCustom;
        headerCustom.chave = strtok((char *)linha, ":");
        headerCustom.valor = strtok(NULL, ": ");
        return headerCustom;
    }
    return Header();
}

const char *AnalyserRequest::getUrl()
{
    return _url;
}
bool AnalyserRequest::urlIs(const char *url)
{
    return strcmp(_url, url) == 0;
}

bool AnalyserRequest::metodoIs(MetodosHttp metodo)
{
    return _metodo == metodo;
}

MetodosHttp AnalyserRequest::getMetodo()
{
    return _metodo;
}

size_t AnalyserRequest::getContentLength()
{
    return _contentLength;
}

const char *AnalyserRequest::getParams()
{
    if (!_haParametros)
    {
        return "";
    }
    return _params;
}

const char *AnalyserRequest::getParam(const char *param)
{
    char *paramInicio = strstr(_params, param);

    if (paramInicio != NULL)
    {
        paramInicio += strlen(param) + 1; // Pula o nome do parâmetro e o '='
        char *paramFim = strchr(paramInicio, '&');

        if (paramFim == NULL)
        {
            paramFim = _params + strlen(_params);
        }

        static char valor[256];
        strncpy(valor, paramInicio, paramFim - paramInicio);
        valor[paramFim - paramInicio] = '\0'; // Finaliza a string

        return valor;
    }

    return NULL;
}

bool AnalyserRequest::paramExists(const char *param)
{
    if (!_haParametros)
    {
        return false;
    }
    return strstr(_params, param) != NULL;
}

const char *AnalyserRequest::getContentType()
{
    return _contentType;
}

const char *AnalyserRequest::getHost()
{
    return _host;
}

const char *AnalyserRequest::getUserAgent()
{
    return _userAgent;
}

const char *AnalyserRequest::getAuthorization()
{
    return _authorization;
}

const char *AnalyserRequest::getCookie(const char *cookie)
{
    char *cookieInicio = strstr(_cookie, cookie);

    if (cookieInicio != NULL)
    {
        cookieInicio += strlen(cookie) + 1; // Pula o nome do cookie e o '='
        char *cookieFim = strchr(cookieInicio, ';');

        if (cookieFim == NULL)
        {
            cookieFim = _cookie + strlen(_cookie);
        }

        static char valor[256];
        strncpy(valor, cookieInicio, cookieFim - cookieInicio);
        valor[cookieFim - cookieInicio] = '\0'; // Finaliza a string

        return valor;
    }

    return NULL;
}

const char *AnalyserRequest::getCookies()
{
    return _cookie;
}