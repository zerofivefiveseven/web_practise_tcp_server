#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <string.h>
#include <iostream>
int main() {
  //create a socket
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  if (listening == -1) {
    std::cerr << "cant' create a socket";
    return -1;
  }
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(54000);
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
  //Bind the socket to ip/port
  if (bind(listening, reinterpret_cast<sockaddr *>(&hint), sizeof(hint))) {
    std::cerr << "can't bind to ip/port";
    return -2;
  }
  //Mark the socket for listening in
  if (listen(listening, SOMAXCONN) == -1) {
    std::cerr << "can't listen";
    return -3;
  }
  sockaddr_in client;
  socklen_t clientSize;
  char host[NI_MAXHOST];
  char svc[NI_MAXSERV];
  //Accept a call
  int clientSocket = accept(listening, reinterpret_cast<sockaddr *>(&client), &clientSize);
  if (clientSocket == -1) {
    std::cerr << "problem with client connection";
    return -4;
  }
  //Close the listening socket
  close(listening);
  memset(host, 0, NI_MAXHOST);
  memset(svc, 0, NI_MAXSERV);

  int result = getnameinfo(reinterpret_cast<sockaddr *>(&client), sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
  if (result) {
    std::cout << host << "connected on" << svc << std::endl;
  } else {
    inet_ntop(AF_INET, &client.sin_addr,host, NI_MAXHOST);
    std::cout<<host<<" connected on " << ntohs(client.sin_port)<<std::endl;
  }
  //While recieving display message, echo message
  char buf[4096];
  while(true){
    //clear the buffer
    memset(buf,0,4096);
    // Wait for client to send data
    int bytesReceived = recv(clientSocket, buf, 4096, 0);
    if (bytesReceived == -1)
    {
      std::cerr << "Error in recv(). Quitting" << std::endl;
      break;
    }

    if (bytesReceived == 0)
    {
      std::cout << "Client disconnected " << std::endl;
      break;
    }

    std::cout << std::string(buf, 0, bytesReceived) << std::endl;

    // Echo message back to client
    send(clientSocket, buf, bytesReceived + 1, 0);
  }

  // Close the socket
  close(clientSocket);

  return 0;
}