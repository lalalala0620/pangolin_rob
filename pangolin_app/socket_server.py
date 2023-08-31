import socket
import threading

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = ("0.0.0.0", 8000)
server_socket.bind(server_address)

server_socket.listen(1)

print("socket start")

def handle_watch_message(watch_socket):
    while True:
        data = watch_socket.recv(8192).decode('utf-8')
        print(data)
    server_socket.close()

while True:
    try:
        watch_socket, watch_address = server_socket.accept()
        receive_socket_thread = threading.Thread(target=handle_watch_message, args=(watch_socket,), daemon=True)
        receive_socket_thread.start()
    except socket.error:
        server_socket.close()
server_socket.close()