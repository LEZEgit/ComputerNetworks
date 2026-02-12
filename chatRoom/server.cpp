#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <unordered_map>
using namespace std;
#define PORT 8080

struct thread_data {
  string cmd;
  string message;
};

unordered_map<string, int> username_to_clientfd;

void *handle_client(void *threadarg) {
  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;
  
  string cmd = my_data->cmd;
  string message = my_data->message;

  if(cmd == "@all" || cmd == ""){
    // send msg to all clients
    cout<</*client_name*/": "<<message<<endl;
  } else if(cmd[0] == '@'){
    string recv_client_username = cmd.substr(1);
    cout<<"Send message to "<<recv_client_username<<endl;
  } else if(cmd == "LIST"){
    // list all users
  } else if(cmd == "EXIT"){
    cout<<"User "<</*client_name*/" has left the chat\n";
  }
   
  pthread_exit(NULL);
}

int main() {
  int server_fd, client_fd;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  // Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0); // an integer variable that will store the socket file descriptor
  if (server_fd == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to the port
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen
  if (listen(server_fd, 5) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // Accept a connection
  client_fd =
      accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
  if (client_fd < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  } else{
    string response = "Connection established\n";
    send(client_fd, response.data(), response.size(), 0);
  }



  // Read client's username
  read(client_fd, buffer, 1024);
  // add client to map
  username_to_clientfd[buffer] = client_fd;
  cout<<"client_fd: "<<client_fd<<endl;
  cout<< buffer << " joined the chat" << endl;

  // Close socket
  close(client_fd);
  close(server_fd);
  return 0;
}
