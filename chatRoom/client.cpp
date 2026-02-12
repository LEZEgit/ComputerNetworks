#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;
#define PORT 8080

int main() {
  char buffer[1024];

  int sock = 0;
  struct sockaddr_in serv_addr;
  // const char *hello = "Hello from client";

  // Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    cerr << "Socket creation error" << endl;
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    cerr << "Invalid address/ Address not supported" << endl;
    return -1;
  }

  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    cerr << "Connection Failed" << endl;
    return -1;
  }
  else {
    cout<<"Connection request sent to the server.\n";
    read(sock, buffer, 1024);
    cout<<"Server says: "<<buffer<<endl;
  }

  // Send data
  cout<<"Enter your username: ";
  string msg;
  getline(cin, msg);
  send(sock, msg.data(), msg.size(),0);
  cout << "Message sent" << endl;

  // Close socket
  close(sock);
  return 0;
}
