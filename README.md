# HTTPServer
Simple implementation of HTTP server using C

Application layer is the top most layer in the TCP/IP stack or OSI model. It is a layer at which the packet journey starts on sending machine and ends on destination machine. It also lies outside the OS(Kernel). The user space program that we right in our day to day life are all examples of applications. The TCP client server program that we wrote were also and example of Application programs running in application layer of TCP/IP stack. One Network application program we use is the Browser which is making use of Application layer protocol HTTP. Other examples of Network application program is ICMP protocol.  

Just like other layers of TCP/IP stack, our application protocol will also have its own application header. In addition, our application protocol will have its own sub-messages type as well. Application protocol can be implemented in three ways:
* On top of Transport Layer protocol (HTTP webserver)
* On top of Network Layer protocol ()
* On top of Data Link layer Protocol ()

TCP/UDP protocol will take the responsibility for packet transmission between sending and receiving processes. Network Layer takes the responsibility for packet transmission from source subnet to destination subnet. And Data Link layer takes the responsibility for packet transmission hop by hop.

The HTTP webserver works as per the following steps:
1. client (The Browser prepares the HTML request message) -> application payload
2. The application payload is pushed down the TCP/IP stack
3. Client machine sends the HTTP request to the server asking for some information
4. HTTP request is received by the server  and moves up the TCP/IP stack.
5. Server does the processing  as per the client request
6. Server prepares the HTML response. This is  application payload from the server side.
7. Server pushes down the application payload down the TCP/IP stack
8. Server sends HTML response back to the client.
9. This HTML response is processed by the client TCP/IP stack and delivered to the Browser.

The request generated by HTTP client are of several types.
* GET request
  * Fetch the data from the webserver
    * example: Entering the roll no. and getting the exam result
    * Link clicks to load/navigate to new page
* POST request
  * Sending the data to the webserver, which will be saved on the server side.
    * example: filling up job forms
    * uploading resumes
    * Facebook posts, comments, etc.

When HTTP client sends the HTTP request to the webserver, client has to encode the request type in the request. Also it sends it in a defined/standard format.

HTTP request:
```
GET /XXX HTTP/1.1           (Request line)
Host: X.X.X.X:80
Connection:
Upgrade-Insecure-Requests:
User-Agent:
Accept:
Accept-Encoding:
Accept-Language:
```

HTTP response:
```
HTTP/1.1 200 OK
Server:
Content-Length:
Content-Type:

<html>
  ... HTML webpage code ...
</html>
 
```
As you can see, HTTP msg types are simple plain text strings.
