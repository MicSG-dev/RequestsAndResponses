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

Here's a simple example to get you started:

```cpp
// ...
// ...

AnalyserRequest request;

// ...
// ...
// ...
                    
Serial.print("Is the method 'POST'? ");
if(request.metodoIs(MetodosHttp::POST)){
    Serial.println("yes");
}else{
    Serial.println("no");
}

Serial.print("So know that the current method is: ");
Serial.println(request.getMetodo());

Serial.print("Is the URL '/test'? ");
if(request.urlIs("/test")){
    Serial.println("yes");
}else{
    Serial.println("no");
}

Serial.print("Be aware that the current URL is: ");
Serial.println(request.getUrl());

Serial.print("Host: ");
Serial.println(request.getHost());

Serial.print("User-Agent: ");
Serial.println(request.getUserAgent());

Serial.print("Authorization: ");
Serial.println(request.getAuthorization());

Serial.print("Content-Length: ");
Serial.println(request.getContentLength());

Serial.print("Content-Type: ");
Serial.println(request.getContentType());

Serial.print("List of all parameters: ");
Serial.println(request.getParams());

Serial.print("Does the 'name' parameter exist? ");
if (request.paramExists("name"))
{
    Serial.println("yes");
}
else
{
    Serial.println("no");
}

Serial.print("The 'name' parameter has the value: ");
Serial.println(request.getParam("nome"));

Serial.print("List of all cookies: ");
Serial.println(request.getCookies());
Serial.print("The value of the Cookie 'myCookie' is: ");
Serial.println(request.getCookie("myCookie"));
Serial.print("Header 'myHeader': ");
Serial.println(myHeader);
Serial.print("Header 'dog': ");
Serial.println(dog);

// ...
// ...
// ...
// ...
// ...

```

## Limitations

By default, some AnalyserRequest parameters have a maximum character limit. See below:

Parameter                  | Maximum character length (bytes) | Example
------------               | ---                              | ------------------- |
Host                       | 128                              | ``arduino.local`` (has 13 characters)
Url (including GET Params) | 512                              | ``/page1?led=high`` (has 15 characters)
Content-Type               | 128                              | ``application/x-www-form-urlencoded`` (has 33 characters)
User-Agent                 | 256                              | ``Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36 Edg/131.0.0.0`` (possui 125 caracteres)
Authorization              | 256                              | ``Basic YWRtaW46ZXNwMzIgw6kgbGVnYWw=`` (has 34 characters)
Cookie                     | 512                              | ``Cookie: Name=ESP32; Memory=4MB`` (has 30 characters)
Others Headers             | 128 (Key) + 512 (Value)          | ``Fruit: Pineapple`` (has 5 characters [Key] + 9 characters [Value])

Note that you can change these default values ​​to suit your needs by modifying the [`RequestsAndResponses.h`](https://github.com/MicSG-dev/RequestsAndResponses/blob/86c00fa5a755a7af7c25987d90a2f1600e2f19f3/src/RequestsAndResponses.h#L431-L440) file.

## License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/MicSG-dev/RequestsAndResponses/blob/main/LICENSE) file for more details.

## Contact

For any questions or suggestions, feel free to open an issue or contact us at [contato@micsg.com.br](mailto:contato@micsg.com.br).

Happy coding!
