#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include <Arduino.h>
#include <FS.h>

/**
 * @enum MethodsHttp
 * @brief Represents HTTP methods.
 *
 * This enumeration defines the various HTTP methods that can be used in
 * HTTP requests. Each enumerator represents a specific HTTP method.
 */
enum MethodsHttp
{
    /**
     * @brief Represents an unknown HTTP method.
     */
    UNKNOWN,

    /**
     * @brief Represents the HTTP GET method.
     *
     * The GET method requests a representation of the specified resource.
     * Requests using GET should only retrieve data.
     */
    GET,

    /**
     * @brief Represents the HTTP POST method.
     *
     * The POST method is used to submit an entity to the specified resource,
     * often causing a change in state or side effects on the server.
     */
    POST,

    /**
     * @brief Represents the HTTP PUT method.
     *
     * The PUT method replaces all current representations of the target
     * resource with the request payload.
     */
    PUT,

    /**
     * @brief Represents the HTTP DELETE method.
     *
     * The DELETE method deletes the specified resource.
     */
    DELETE
};

/**
 * @brief Represents an HTTP header.
 *
 * This structure is used to store an HTTP header, which consists of a key-value pair.
 * The `key` represents the name of the header, and the `value` represents the value
 * associated with that header.
 */
struct Header
{
    /**
     * @brief The key of the HTTP header.
     *
     * This member represents the name of the HTTP header.
     */
    char key[128];

    /**
     * @brief The value of the HTTP header.
     *
     * This member represents the value associated with the HTTP header.
     */
    char value[512];
};

struct HeaderBig
{
    /**
     * @brief The key of the HTTP header.
     *
     * This member represents the name of the HTTP header.
     */
    const char chave[512];

    /**
     * @brief The value of the HTTP header.
     *
     * This member represents the value associated with the HTTP header.
     */
    const char valor[512];
};

/**
 * @namespace ContentType
 * @brief Contains constants for HTTP content types.
 *
 * This namespace includes constants that represent various HTTP content types.
 * These content types are used to specify the media type of the resource being
 * sent or received in an HTTP request or response.
 */
namespace ContentType
{
    const char TEXT_PLAIN[] = "text/plain";
    const char TEXT_HTML[] = "text/html";
    const char TEXT_CSS[] = "text/css";
    const char TEXT_JAVASCRIPT[] = "text/javascript";
    const char APPLICATION_JSON[] = "application/json";
    const char APPLICATION_XML[] = "application/xml";
    const char APPLICATION_X_WWW_FORM_URLENCODED[] = "application/x-www-form-urlencoded";
    const char MULTIPART_FORM_DATA[] = "multipart/form-data";
}

/**
 * @namespace StatusCode
 * @brief Contains HTTP status codes.
 *
 * This namespace includes constants for various HTTP status codes, organized
 * into different categories based on their purpose and meaning.
 *
 * The status codes listed here are defined by RFC 9110: https://httpwg.org/specs/rfc9110.html#overview.of.status.codes
 */
namespace StatusCode
{
    /**
     * @namespace Informational
     * @brief Contains HTTP status codes for informational responses (essentials from 100-199).
     *
     * This namespace includes constants for HTTP status codes that indicate
     * an informational response. These status codes are used to inform the
     * client that the request has been received and understood, and that the
     * client should continue with the request or switch protocols.
     */
    namespace Informational
    {

        /**
         * @brief HTTP status code indicating that the initial part of a request has been received and has not yet been rejected by the server.
         *
         * This constant represents the "100 Continue" HTTP status code, which is used to inform the client that the server has received the request headers and that the client should proceed to send the request body (in the case of a request that needs to send a body).
         */
        const char _100_CONTINUE[] = "100 Continue";

        /**
         * @brief HTTP status code for a request that has been received and the server is switching protocols.
         *
         * This constant represents the HTTP status code 101, which indicates that the server is switching protocols as requested by the client.
         */
        const char _101_SWITCHING_PROTOCOLS[] = "101 Switching Protocols";
    }

    /**
     * @namespace Successful
     * @brief Contains HTTP status codes for successful responses (essentials from 200-299).
     *
     * This namespace includes constants for HTTP status codes that indicate
     * a successful response. These status codes signify that the client's
     * request was successfully received, understood, and accepted.
     */
    namespace Successful
    {
        /**
         * @brief HTTP status code for a successful request.
         *
         * This constant represents the "200 OK" status code, indicating that
         * the request has succeeded. The meaning of the success depends on
         * the request method: GET (resource obtained), HEAD (headers received),
         * POST (resource created/updated), TRACE (message received).
         */
        const char _200_OK[] = "200 OK";

        /**
         * @brief HTTP status code for a resource that has been created.
         *
         * This constant represents the "201 Created" status code, indicating that
         * the request has been fulfilled and has resulted in one or more new resources
         * being created.
         */
        const char _201_CREATED[] = "201 Created";

        /**
         * @brief HTTP status code for a request that has been accepted for processing.
         *
         * This constant represents the "202 Accepted" status code, indicating that
         * the request has been accepted for processing, but the processing has not
         * been completed.
         */
        const char _202_ACCEPTED[] = "202 Accepted";

        /**
         * @brief HTTP status code for a request that has been successfully processed but has no content to return.
         *
         * This constant represents the "204 No Content" status code, indicating that
         * the server has successfully processed the request, but there is no content
         * to send in the response.
         */
        const char _204_NO_CONTENT[] = "204 No Content";
    }

    /**
     * @namespace Redirection
     * @brief Contains HTTP status codes for redirection responses (essentials from 300-399).
     *
     * This namespace includes constants for HTTP status codes that indicate
     * a redirection response. These status codes inform the client that further
     * action is needed to complete the request, such as following a different
     * URI or using a different method.
     */
    namespace Redirection
    {
        /**
         * @brief HTTP status code for a resource that has been moved permanently.
         *
         * This constant represents the "301 Moved Permanently" status code, indicating that
         * the requested resource has been assigned a new permanent URI and any future references
         * to this resource should use one of the returned URIs.
         */
        const char _301_MOVED_PERMANENTLY[] = "301 Moved Permanently";

        /**
         * @brief HTTP status code for a resource that has been found.
         *
         * This constant represents the "302 Found" status code, indicating that
         * the requested resource resides temporarily under a different URI.
         */
        const char _302_FOUND[] = "302 Found";

        /**
         * @brief HTTP status code for a resource that has not been modified since the last request.
         *
         * This constant represents the "304 Not Modified" status code, indicating that
         * the resource has not been modified since the version specified by the request headers.
         * This is used for caching purposes.
         */
        const char _304_NOT_MODIFIED[] = "304 Not Modified";

        /**
         * @brief HTTP status code for a resource that has been temporarily moved.
         *
         * This constant represents the "307 Temporary Redirect" status code, indicating that
         * the requested resource resides temporarily under a different URI and the client should
         * use the original URI for future requests.
         */
        const char _307_TEMPORARY_REDIRECT[] = "307 Temporary Redirect";

        /**
         * @brief HTTP status code for a resource that has been permanently moved.
         *
         * This constant represents the "308 Permanent Redirect" status code, indicating that
         * the requested resource has been assigned a new permanent URI and any future references
         * to this resource should use one of the returned URIs.
         */
        const char _308_PERMANENT_REDIRECT[] = "308 Permanent Redirect";
    }

    /**
     * @namespace ClientError
     * @brief Contains HTTP status codes for client error responses (essentials from 400-499).
     *
     * This namespace includes constants for HTTP status codes that indicate
     * a client error response. These status codes signify that there was an
     * error with the client's request, such as a malformed request syntax,
     * invalid request message framing, or deceptive request routing.
     */
    namespace ClientError
    {
        /**
         * @brief HTTP status code for a request that could not be processed.
         *
         * This constant represents the "400 Bad Request" status code, indicating that
         * the server could not understand the request due to invalid syntax.
         */
        const char _400_BAD_REQUEST[] = "400 Bad Request";

        /**
         * @brief HTTP status code for a request that requires user authentication.
         *
         * This constant represents the "401 Unauthorized" status code, indicating that
         * the request requires user authentication. The client must authenticate itself
         * to get the requested response.
         */
        const char _401_UNAUTHORIZED[] = "401 Unauthorized";

        /**
         * @brief HTTP status code for a request that has been refused.
         *
         * This constant represents the "403 Forbidden" status code, indicating that
         * the server understood the request but refuses to authorize it.
         */
        const char _403_FORBIDDEN[] = "403 Forbidden";

        /**
         * @brief HTTP status code for a resource that could not be found.
         *
         * This constant represents the "404 Not Found" status code, indicating that
         * the server could not find the requested resource.
         */
        const char _404_NOT_FOUND[] = "404 Not Found";

        /**
         * @brief HTTP status code for a request that has been made using an HTTP method that is not supported.
         *
         * This constant represents the "405 Method Not Allowed" status code, indicating that
         * the request method is known by the server but is not supported by the target resource.
         */
        const char _405_METHOD_NOT_ALLOWED[] = "405 Method Not Allowed";

        /**
         * @brief HTTP status code for a request that could not be processed because of a conflict.
         *
         * This constant represents the "409 Conflict" status code, indicating that
         * the request could not be processed because of a conflict in the request.
         */
        const char _409_CONFLICT[] = "409 Conflict";

        /**
         * @brief HTTP status code for a request that has been made without a content length.
         *
         * This constant represents the "411 Length Required" status code, indicating that
         * the server refuses to accept the request without a defined Content-Length header.
         */
        const char _411_LENGTH_REQUIRED[] = "411 Length Required";

        /**
         * @brief HTTP status code for a request that has been made with a payload that is too large.
         *
         * This constant represents the "413 Payload Too Large" status code, indicating that
         * the request entity is larger than the server is willing or able to process.
         */
        const char _413_PAYLOAD_TOO_LARGE[] = "413 Payload Too Large";

        /**
         * @brief HTTP status code for a request that has been made with a URI that is too long.
         *
         * This constant represents the "414 URI Too Long" status code, indicating that
         * the URI provided was too long for the server to process.
         */
        const char _414_URI_TOO_LONG[] = "414 URI Too Long";

        /**
         * @brief HTTP status code for a request that has been made to a teapot.
         *
         * This constant represents the "418 I'm a teapot" status code, which is an April Fools' joke
         * from the IETF. It indicates that the server refuses to brew coffee because it is, permanently,
         * a teapot.
         */
        const char _418_IM_A_TEAPOT[] = "418 I'm a teapot";

        /**
         * @brief HTTP status code for a request that has been made with an unprocessable entity.
         *
         * This constant represents the "422 Unprocessable Entity" status code, indicating that
         * the server understands the content type of the request entity, but was unable to process
         * the contained instructions.
         */
        const char _422_UNPROCESSABLE_ENTITY[] = "422 Unprocessable Entity";

        /**
         * @brief HTTP status code for a request that has been made to a resource that is locked.
         *
         * This constant represents the "423 Locked" status code, indicating that
         * the resource that is being accessed is locked.
         */
        const char _423_LOCKED[] = "423 Locked";

        /**
         * @brief HTTP status code for a request that has been made too many times in a given amount of time.
         *
         * This constant represents the "429 Too Many Requests" status code, indicating that
         * the user has sent too many requests in a given amount of time ("rate limiting").
         */
        const char _429_TOO_MANY_REQUESTS[] = "429 Too Many Requests";

        /**
         * @brief HTTP status code for a request that has been made with header fields that are too large.
         *
         * This constant represents the "431 Request Header Fields Too Large" status code, indicating that
         * the server is unwilling to process the request because its header fields are too large.
         */
        const char _431_REQUEST_HEADER_FIELDS_TOO_LARGE[] = "431 Request Header Fields Too Large";

    }

    /**
     * @namespace ServerError
     * @brief Contains HTTP status codes for server error responses (essentials from 500-599).
     *
     * This namespace includes constants for HTTP status codes that indicate
     * a server error response. These status codes signify that the server
     * failed to fulfill a valid request due to an error on the server side.
     */
    namespace ServerError
    {
        /**
         * @brief HTTP status code for a request that could not be processed due to an internal server error.
         *
         * This constant represents the "500 Internal Server Error" status code, indicating that
         * the server encountered an unexpected condition that prevented it from fulfilling the request.
         */
        const char _500_INTERNAL_SERVER_ERROR[] = "500 Internal Server Error";

        /**
         * @brief HTTP status code for a request that has not been implemented.
         *
         * This constant represents the "501 Not Implemented" status code, indicating that
         * the server does not support the functionality required to fulfill the request.
         */
        const char _501_NOT_IMPLEMENTED[] = "501 Not Implemented";

        /**
         * @brief HTTP status code for a request that has received an invalid response from an upstream server.
         *
         * This constant represents the "502 Bad Gateway" status code, indicating that
         * the server, while acting as a gateway or proxy, received an invalid response from the upstream server.
         */
        const char _502_BAD_GATEWAY[] = "502 Bad Gateway";

        /**
         * @brief HTTP status code for a request that has not been processed because the server is unavailable.
         *
         * This constant represents the "503 Service Unavailable" status code, indicating that
         * the server is currently unable to handle the request due to temporary overloading or maintenance.
         */
        const char _503_SERVICE_UNAVAILABLE[] = "503 Service Unavailable";

        /**
         * @brief HTTP status code for a request that could not be processed because the server is unable to store the representation needed to complete the request.
         *
         * This constant represents the "507 Insufficient Storage" status code, indicating that
         * the server is unable to store the representation needed to complete the request.
         */
        const char _507_INSUFFICIENT_STORAGE[] = "507 Insufficient Storage";
    }

}

class AnalyserRequest
{
public:
    AnalyserRequest();
    Header analyzeHttpLine(const char *line);

    const char *getMethod();
    bool methodIs(MethodsHttp method);
    const char *getUrl();
    bool urlIs(const char *url);
    size_t getContentLength();
    const char *getContentType();
    const char *getParam(const char *param);
    const char *getParams();
    bool paramExists(const char *param);
    const char *getHost();
    const char *getAuthorization();
    const char *getCookie(const char *cookie);
    const char *getCookies();
    const char *getUserAgent();

private:
    char _host[128] = "";
    int _numHeadersCustom;
    MethodsHttp _method;

    char _url[512];
    size_t _contentLength;
    char _contentType[128] = "";
    char _authorization[256] = "";
    char _cookie[512] = "";
    char _userAgent[128] = "";

    char *_params;
    bool _haveParameters;
};

/**
 * @class BuildResponse
 * @brief A class to build and send HTTP responses.
 *
 * This class provides methods to construct and send HTTP responses to a client.
 * It allows setting the response code, adding headers, and sending the response
 * message with optional content type and length.
 */
class BuildResponse
{
public:
    BuildResponse(Client &client);
    void begin(const char *code);
    void addHeader(const char *key, const char *value);

    void send(const char *message, bool newLine = true);
    void send(const char *contentType, const char *message, bool newLine = true);
    void send(const char *contentType, const uint8_t *contentGzip, uint32_t size);
    void send(const char *contentType, const char *progmemContent, size_t size);
    void send(const char *contentType, fs::FS &fs, const char *path);
    void send();

private:
    Client *_client;
    bool _alreadyClosed = false;
};

#endif // HTTPPARSER_H