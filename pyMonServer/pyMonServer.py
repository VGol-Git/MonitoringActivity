import socket
import threading
import json
import time
import datetime




# Хост и порт, на котором будет работать сервер
HOST = '127.0.0.1'
PORT = 12345

clients = {}  # Список подключенных клиентов
def list_clients():
    print("Подключенные клиенты:")
    for i, (conn, addr) in enumerate(clients, start=1):
        print(f"{i}. {conn}, {addr}")


def receive_and_save_bmp(client_socket, file_name):
    try:
        with open(file_name, 'wb') as file:
            # Принимаем данные заголовка BMP файла
            bmp_header = client_socket.recv(54)
            if len(bmp_header) != 54:
                print("Ошибка: Не удалось получить заголовок BMP файла.")
                return

            file.write(bmp_header)

            # Извлекаем информацию о размере файла из заголовка
            file_size = int.from_bytes(bmp_header[2:6], byteorder='little')

            # Принимаем и записываем остальные данные файла
            remaining_data = file_size - 54
            while remaining_data > 0:
                data = client_socket.recv(min(1024, remaining_data))
                if not data:
                    break
                file.write(data)
                remaining_data -= len(data)

            print(f"Файл '{file_name}' успешно принят и сохранен.")
    except Exception as e:
        print(f"Произошла ошибка при приеме файла: {str(e)}")


def control_time_client(client_socket, addr):
    try:
        while True:
            client_socket.send(b'')
            time.sleep(1)
    except (ConnectionResetError, ConnectionAbortedError):
        print("Клиент разорвал соединение:", addr)
    clients.pop(addr)
    print(datetime.datetime.now())
    client_socket.close()
        
# Первое распараллеливание: принимаем новых клиентов
def accept_clients(server_socket):
    while True:
        # Получаем уникальный идентификатор клиента: IP-адрес и порт
        client_socket, addr = server_socket.accept()
        # Сохраняем соединение клиента в словаре
        clients[addr] = client_socket
        client_thread = threading.Thread(target=control_time_client, args=(client_socket, addr))
        client_thread.start()
      

# Второе распараллеливание: отправляем данные клиентам
def get_data():
    while True:
        command = input("Введите команду (screenshot [ip] [clientId], json [ip] [clientId], или view): ")
        if command == "view":
            list_clients()
            continue

        parts = command.split()
        if len(parts) == 3:
            try:
                message = parts[0]
                ip = parts[1]
                client_id = int(parts[2]) 
                if client_id >= 0 and len(message) > 0:
                    clients[(ip, client_id)].send(message.encode())
                    if message == 'screenshot':
                        receive_and_save_bmp(clients[(ip, client_id)], "screenshot.bmp")
                        

                 # Принимаем JSON файл
                    if message == 'json':
                        json_data = clients[(ip, client_id)].recv(40960)  
                        parsed_json = json.loads(json_data.decode('utf-8'))
                        print("Принят JSON файл:")
                        print(parsed_json)
                       
                        filename = "received_data.json"
                        with open(filename, 'w') as file:
                            json.dump(parsed_json, file)

                else:
                    print("Неправильный данные клиента")
            except ValueError:
                print("Неправильный формат команды")
            except json.JSONDecodeError:
                print("Ошибка при разборе JSON файла")

        else:
            print("Неправильный формат команды")
def main():
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((HOST, PORT))
        server_socket.listen()
        print(f"Сервер слушает на {HOST}:{PORT}")
        accept_thread = threading.Thread(target=accept_clients, args=(server_socket,))
        send_thread = threading.Thread(target=get_data, )

        accept_thread.start()
        send_thread.start()
       



if __name__ == "__main__":
    main()
