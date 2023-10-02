#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib") // ����� ���������� ���� � ���������� Winsock
#include <Winsock2.h> // ��� ������ � Winsock
#include <Windows.h>
#include <Ws2tcpip.h> 
#include "screenshot.h"
#include <iostream>
#include <map>
#include <chrono>

using namespace std::chrono;

// ��������� ��� �������� ���������� � ����������
struct AppInfo {
    std::string name;
    milliseconds activeTime;

    AppInfo() : activeTime(0) {}
};



int main() {
    // ������������� ��������� ������ �� CP1251 (ANSI)
    SetConsoleOutputCP(1251);

    // ������� ��� �������� ���������� � �����������
    std::map<std::string, AppInfo> appInfoMap;

    // �������� ��������� �������� ���������� � ��� ���������
    HWND prevHwnd = GetForegroundWindow();
    char windowTitle[256];
    GetWindowTextA(prevHwnd, windowTitle, sizeof(windowTitle));
    std::string prevAppName = windowTitle;

    high_resolution_clock::time_point prevTimePoint = high_resolution_clock::now();

    // ������������� ���������� Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "������ ������������� Winsock." << std::endl;
        return 1;
    }

    // ������� ����� ��� �������� ������ �� ������
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "������ �������� ������." << std::endl;
        WSACleanup();
        return 1;
    }

    // ������ ����� ������� � ����
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // ���� �������
    //inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP ����� �������

    // ������������ � �������
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "������ ����������� � �������." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    while (true) {
        bool key = false;
        if (key) {
            // ������� ��������
            CaptureScreenshot();
        }
        HWND hwnd = GetForegroundWindow(); // �������� ������� �������� ����

        if (hwnd != NULL && hwnd != prevHwnd) {
            // �������� ��������� ����
            GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
            std::string appName = windowTitle;

            // ��������� ���������� � ������� ����������
            high_resolution_clock::time_point currentTime = high_resolution_clock::now();
            milliseconds elapsed = duration_cast<milliseconds>(currentTime - prevTimePoint);
            appInfoMap[prevAppName].activeTime += elapsed;

            // ������� ���������� � ������� ����������
            std::cout << "����������: " << prevAppName << std::endl;
            std::cout << "����� ���������� (��): " << appInfoMap[prevAppName].activeTime.count() << std::endl;

            // ��������� ���������� �������� ���� � ��� ���������
            prevHwnd = hwnd;
            prevAppName = appName;
            prevTimePoint = currentTime;
        }

        // ���������� ������ �� ������
        // ��������, ����� ��������� ���������� � ������� ���������� appInfoMap �� ������

        Sleep(1000); // ����� � 1 ������� (1000 �����������)
    }

    // ��������� ����� � ��������� ������ ���������� Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
