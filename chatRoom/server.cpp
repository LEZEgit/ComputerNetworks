#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
using namespace std;
#define PORT 8080



unordered_map<string, int> username_to_clientfd;
unordered_map<int, string> clientfd_to_username;

unordered_set<int> active_client_fds;

void send_to_all_clients(const string response, const int sender_fd){
  for(int fd : active_client_fds){
    if(fd == sender_fd) continue;  
    send(fd, response.data(), response.size(), 0);
  }
}


void delete_client(const string client_username){
  int client_fd = username_to_clientfd[client_username];
  active_client_fds.erase(client_fd);
  username_to_clientfd.erase(client_username);
  clientfd_to_username.erase(client_fd);
  /*
    should i close the thread here or in the handle_client??
    ok so first we close the client_socket
    then exit the thread
  */
  string res = client_username + " has left the chat\n";
  cout<<res;
  send_to_all_clients(res, -1);
  // Close client socket
  close(client_fd);
}

void send_active_clients_list(int client_fd){
  string response = "";
  for(auto e : username_to_clientfd){
    response += e.first;
    response += string(1, '\n');
  }
  send(client_fd, response.data(), response.size(), 0);
}

// struct thread_data {
//   string cmd;
//   string message;
//   int client_fd;
//   string sender_name;
// };
void *handle_client(void *arg) {
  int client_fd = *(int*) arg;
  string sender_name = clientfd_to_username[client_fd];
  char buffer[1024];

  while(true){
    int bytes = recv(client_fd, buffer, sizeof(buffer)-1, 0);
    if (bytes < 0) break;

    if (bytes == 0) continue; // if sent empty message ignore it

    buffer[bytes] = '\0';
    string msg(buffer);
    string cmd = "";
    string message = "";
    string response = "";
    if(msg[0] == '@'){
      int delim_pos = msg.find(' '); // will make sure at the client side to append a space at the end of @LIST or @EXIT command
      if(delim_pos != string::npos){
        cmd = msg.substr(0, delim_pos); 
        if(delim_pos+1 < msg.size()) message = msg.substr(delim_pos+1);
      }
    } else{
      message = msg;
    }
    // if(message.size()==0){
    //   response = "Can't send empty message\n";
    //   send(client_fd, response.data(), response.size(), 0);
    //   continue;
    // }  
    response = sender_name + ": " + message;
    if(cmd == "@LIST"){
      send_active_clients_list(client_fd);
    } else if(cmd == "@EXIT"){
      break;
    } else if(cmd == "@all" || cmd == ""){
      send_to_all_clients(response, client_fd);
      cout<< sender_name << " (to @all)\n\t"<<message<<endl;
    } else if(cmd[0] == '@'){
      string recv_client_username = cmd.substr(1);
      if(username_to_clientfd.find(recv_client_username) == username_to_clientfd.end()){
        string nf_msg = recv_client_username + " is not in the chat\n"; 
        cout<<nf_msg<<endl;
        send(client_fd, nf_msg.data(), nf_msg.size(), 0);
      }
      int recv_client_fd = username_to_clientfd[recv_client_username];
      send(recv_client_fd, response.data(), response.size(), 0);
      cout<<sender_name<<" sent a private message to "<<recv_client_username<<endl;
    } 
  }
  delete_client(sender_name);
  pthread_exit(NULL);
}

void register_client(const int client_fd){
  // Read client's username
  char buffer[1024] = {0};
  read(client_fd, buffer, 1024);
  // add client to map
  username_to_clientfd[buffer] = client_fd;
  clientfd_to_username[client_fd] = buffer;
  // add client to active users
  if(active_client_fds.count(client_fd) == 0)
    active_client_fds.insert(client_fd);
  else {
    cout<<"Couldn't add client_fd to active clients\n";
    return;
  }
  string res = buffer; 
  res += " joined the chat\n";
  send_to_all_clients(res, client_fd);
  cout<< res;

  // assign thread to each client
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, handle_client, (void*)&client_fd);
  pthread_detach(thread_id); // Let thread clean up itself
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
  cout<<"Listening...\n\n";
  while(listen(server_fd, 5)>=0){
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
        
    register_client(client_fd);
  } 

  // Close server socket
  close(server_fd);
  return 0;
}
