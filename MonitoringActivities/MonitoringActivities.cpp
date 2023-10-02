#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib") // Чтобы компилятор знал о библиотеке Winsock
#include <Winsock2.h> // Для работы с Winsock
#include <Windows.h>
#include <Ws2tcpip.h> 
#include "screenshot.h"
#include <iostream>
#include <map>
#include <chrono>

using namespace std::chrono;

// Структура для хранения информации о приложении
struct AppInfo {
    std::string name;
    milliseconds activeTime;

    AppInfo() : activeTime(0) {}
};



int main() {
    // Устанавливаем кодировку вывода на CP1251 (ANSI)
    SetConsoleOutputCP(1251);

    // Словарь для хранения информации о приложениях
    std::map<std::string, AppInfo> appInfoMap;

    // Получаем начальное активное приложение и его заголовок
    HWND prevHwnd = GetForegroundWindow();
    char windowTitle[256];
    GetWindowTextA(prevHwnd, windowTitle, sizeof(windowTitle));
    std::string prevAppName = windowTitle;

    high_resolution_clock::time_point prevTimePoint = high_resolution_clock::now();

    // Инициализация библиотеки Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock." << std::endl;
        return 1;
    }

    // Создаем сокет для отправки данных на сервер
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета." << std::endl;
        WSACleanup();
        return 1;
    }

    // Задаем адрес сервера и порт
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Порт сервера
    //inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP адрес сервера

    // Подключаемся к серверу
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка подключения к серверу." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        bool key = false;
        if (key) {
            // Снимаем скриншот
            CaptureScreenshot();
        }
        HWND hwnd = GetForegroundWindow(); // Получаем текущее активное окно

        if (hwnd != NULL && hwnd != prevHwnd) {
            // Получаем заголовок окна
            GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
            std::string appName = windowTitle;

            // Обновляем информацию о времени активности
            high_resolution_clock::time_point currentTime = high_resolution_clock::now();
            milliseconds elapsed = duration_cast<milliseconds>(currentTime - prevTimePoint);
            appInfoMap[prevAppName].activeTime += elapsed;

            // Выводим информацию о времени активности
            std::cout << "Приложение: " << prevAppName << std::endl;
            std::cout << "Время активности (мс): " << appInfoMap[prevAppName].activeTime.count() << std::endl;

            // Обновляем предыдущее активное окно и его заголовок
            prevHwnd = hwnd;
            prevAppName = appName;
            prevTimePoint = currentTime;
        }

        // Отправляем данные на сервер
        // Например, можно отправить информацию о времени активности appInfoMap на сервер

        Sleep(1000); // Пауза в 1 секунду (1000 миллисекунд)
    }

    // Закрываем сокет и завершаем работу библиотеки Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
