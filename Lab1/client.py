import socket


client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host_addr = '0.0.0.0'
host_port = 8080
client_socket.connect((host_addr, host_port))
ack = client_socket.recv(1024).decode()

print(f"Connected to server at {host_addr}")
print(ack)

client_socket.close()
