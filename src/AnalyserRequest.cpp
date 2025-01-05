#include "RequestsAndResponses.h"

AnalyserRequest::AnalyserRequest()
{
    _contentLength = 0;
    _haveParameters = false;
    _numHeadersCustom = 0;
    _method = MethodsHttp::UNKNOWN;
}

Header AnalyserRequest::analyzeHttpLine(const char *line)
{
    if (_method == MethodsHttp::UNKNOWN && strncmp(line, "POST", 4) == 0)
    {
        _method = MethodsHttp::POST;

        // Extract the URL from the request
        char *urlStart = strstr(line, " ") + 1;
        char *urlEnd = strchr(urlStart, ' ');
        strncpy(_url, urlStart, urlEnd - urlStart);
        _url[urlEnd - urlStart] = '\0'; // Finalize the string

        // Remove the trailing slash if present
        int len = strlen(_url);
        if (len > 1 && _url[len - 1] == '/')
        {
            _url[len - 1] = '\0';
        }
    }
    else if (_method == MethodsHttp::UNKNOWN && strncmp(line, "GET", 3) == 0)
    {
        _method = MethodsHttp::GET;

        // Extract the URL from the request
        char *urlStart = strstr(line, " ") + 1;
        char *urlEnd = strchr(urlStart, ' ');
        strncpy(_url, urlStart, urlEnd - urlStart);
        _url[urlEnd - urlStart] = '\0'; // Finalize the string

        // Remove the trailing slash if present
        int len = strlen(_url);
        if (len > 1 && _url[len - 1] == '/')
        {
            _url[len - 1] = '\0';
        }

        // Check if the URL contains parameters
        char *paramsStart = strchr(_url, '?');

        if (paramsStart != NULL)
        {
            // Separate the URL from the parameters
            *paramsStart = '\0'; // End URL before '?'

            // Extract the parameters (after '?')
            _params = paramsStart + 1;
            _haveParameters = true;
        }
    }
    else if (_method == MethodsHttp::UNKNOWN && strncmp(line, "PUT", 3) == 0)
    {
        _method = MethodsHttp::PUT;

        // Extract the URL from the request
        char *urlStart = strstr(line, " ") + 1;
        char *urlEnd = strchr(urlStart, ' ');
        strncpy(_url, urlStart, urlEnd - urlStart);
        _url[urlEnd - urlStart] = '\0'; // Finalize the string

        // Remove the trailing slash, if it exists
        int len = strlen(_url);
        if (len > 1 && _url[len - 1] == '/')
        {
            _url[len - 1] = '\0';
        }
    }
    else if (_method == MethodsHttp::UNKNOWN && strncmp(line, "DELETE", 6) == 0)
    {
        _method = MethodsHttp::DELETE;

        // Extract the URL from the request
        char *urlStart = strstr(line, " ") + 1;
        char *urlEnd = strchr(urlStart, ' ');
        strncpy(_url, urlStart, urlEnd - urlStart);
        _url[urlEnd - urlStart] = '\0'; // Finalize the string

        // Remove the trailing slash if exists
        int len = strlen(_url);
        if (len > 1 && _url[len - 1] == '/')
        {
            _url[len - 1] = '\0';
        }
    }
    else if (strncmp(line, "Content-Length: ", 16) == 0)
    {
        _contentLength = atoi(line + 16);
    }
    else if (strncmp(line, "Content-Type: ", 14) == 0)
    {
        strncpy(_contentType, line + 14, sizeof(_contentType));
    }
    else if (strncmp(line, "Host: ", 6) == 0)
    {
        strncpy(_host, line + 6, sizeof(_host));
    }
    else if (strncmp(line, "User-Agent: ", 12) == 0)
    {
        strncpy(_userAgent, line + 12, sizeof(_userAgent));
    }
    else if (strncmp(line, "Authorization: ", 15) == 0)
    {
        strncpy(_authorization, line + 15, sizeof(_authorization));
    }
    else if (strncmp(line, "Cookie: ", 8) == 0)
    {
        strncpy(_cookie, line + 8, sizeof(_cookie));
    }
    else
    {
        
        Header headerCustom;
        
        char *pos = strstr(line, ": "); // Find the position of ": " in the string
        if (pos != NULL)
        {
            // Calculate the length of the part before ": "
            size_t chaveLen = pos - line;
            // Copy the part before ": " to "key"
            strncpy(headerCustom.key, line, chaveLen);
            headerCustom.key[chaveLen] = '\0'; // Ensures correct termination

            // Advance the position to after ": "
            pos += 2;
            // Copy the rest of the string to "value"
            strncpy(headerCustom.value, pos, sizeof(headerCustom.value) - 1);
            headerCustom.value[sizeof(headerCustom.value) - 1] = '\0'; // Ensures correct termination
        }
        else
        {
            // If ": " is not found, set "key" and "value" to empty
            headerCustom.key[0] = '\0';
            headerCustom.value[0] = '\0';
        }

        return headerCustom;
    }
    Header headerEmpty;
    headerEmpty.key[0] = '\0';
    headerEmpty.value[0] = '\0';

    return headerEmpty;
}

const char *AnalyserRequest::getUrl()
{
    return _url;
}
bool AnalyserRequest::urlIs(const char *url)
{
    return strcmp(_url, url) == 0;
}

bool AnalyserRequest::methodIs(MethodsHttp method)
{
    return _method == method;
}

const char *AnalyserRequest::getMethod()
{
    switch (_method)
    {
    case MethodsHttp::GET:
        return "GET";
    case MethodsHttp::POST:
        return "POST";
    case MethodsHttp::DELETE:
        return "DELETE";
    case MethodsHttp::PUT:
        return "PUT";
    default:
        return "Unknown";
    }
}

size_t AnalyserRequest::getContentLength()
{
    return _contentLength;
}

const char *AnalyserRequest::getParams()
{
    if (!_haveParameters)
    {
        return "";
    }
    return _params;
}

const char *AnalyserRequest::getParam(const char *param)
{
    char *paramStart = strstr(_params, param);

    if (paramStart != NULL)
    {
        paramStart += strlen(param) + 1; // Skip the parameter name and the '='
        char *paramEnd = strchr(paramStart, '&');

        if (paramEnd == NULL)
        {
            paramEnd = _params + strlen(_params);
        }

        static char value[256];
        strncpy(value, paramStart, paramEnd - paramStart);
        value[paramEnd - paramStart] = '\0'; // Finalize the string

        return value;
    }

    return NULL;
}

bool AnalyserRequest::paramExists(const char *param)
{
    if (!_haveParameters)
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
    char *cookieStart = strstr(_cookie, cookie);

    if (cookieStart != NULL)
    {
        cookieStart += strlen(cookie) + 1; // Skip the cookie name and the '='
        char *cookieEnd = strchr(cookieStart, ';');

        if (cookieEnd == NULL)
        {
            cookieEnd = _cookie + strlen(_cookie);
        }

        static char value[256];
        strncpy(value, cookieStart, cookieEnd - cookieStart);
        value[cookieEnd - cookieStart] = '\0'; // Finalize the string

        return value;
    }

    return NULL;
}

const char *AnalyserRequest::getCookies()
{
    return _cookie;
}