#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;
#define PORT 8080

void exit_the_chat(const int fd){
  // Close socket
  cout<<"you have successfully left the chat\n";
  close(fd);
}

// Receive thread
void* handle_recv(void* sock_fd) {
    int fd = *(int*)sock_fd;
    char buffer[1024];
    while (true) {
        int bytes = recv(fd, buffer, 1024, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        cout << buffer << endl;
    }
    exit_the_chat(fd);
    return nullptr;
}


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

  // User registration
  cout<<"Enter your username: ";
  string msg;
  getline(cin, msg);
  send(sock, msg.data(), msg.size(),0);
  int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
  if(bytes <= 0){
    cout<<"Lost connection to server\n";
    // break;
  }
  else{
    cout<<buffer<<endl;
  }

    /*
      will use another thread for receiving messages and main thread for sending the message
      if main thread for receiving
      and p_thread for sending
      terminating is complex (send EXIT to server and again receive EXIT from server and then terminate the main thread)

      therefore we do the sending from main thread,
      if user enters EXIT, main thread terminates (client disconnects)
    */
  cout<<"\n=====You can now send messages=====\n";
    
  string message;
    pthread_t recv_thread_id;
    pthread_create(&recv_thread_id, NULL, handle_recv, (void*)&sock);
    pthread_detach(recv_thread_id); // Let thread clean up itself

    // sending loop
    bool exit = false;
    while(true){
        getline(cin, message);
        if(message == "@EXIT"){
            exit = true;
        }
        if(message == "@LIST" || message == "@EXIT")
            message += " ";
        send(sock, message.data(), message.size(), 0);
        if (exit) break;
    }
    exit_the_chat(sock);
  return 0;
}
