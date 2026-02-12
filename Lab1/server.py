import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_addr = '0.0.0.0'
server_port = 8080
server_socket.bind((server_addr, server_port))
print("Server is active")
server_socket.listen(5)
print(f"Server is listening at {server_addr}")


conn, addr = server_socket.accept()

# .getpeername() gives the address for connected devices
# print(server_socket.getpeername())

print(f"Connected to client at {addr}")
print(conn.getpeername())  # same as addr

# conn.send(f"Hi Client, from server at {server_socket.getpeername()}".encode())

conn.send("Hi Client".encode())

server_socket.close()
