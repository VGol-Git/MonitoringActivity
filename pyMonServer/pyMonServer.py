
import socket
import subprocess
import os

# Хост и порт, на котором будет работать сервер
HOST = '127.0.0.1'

PORT = 12345

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen()

        print(f"Сервер слушает на {HOST}:{PORT}")

        while True:
            conn, addr = server_socket.accept()
            with conn:
                print(f"Подключение от {addr}")
                data = conn.recv(1024)
                if not data:
                    break

                command = data.decode()

                if command == "screenshot":
                    conn.send("screenshot")
                #if command == "delete":
                #    conn.send("")
                else:
                    conn.send("Неизвестная команда.")

if __name__ == "__main__":
    main()
