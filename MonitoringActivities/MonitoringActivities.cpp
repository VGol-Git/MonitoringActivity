#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <Winsock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <map>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

using namespace std::chrono;

struct AppInfo {
    std::string name;
    milliseconds activeTime;

    AppInfo() : activeTime(0) {}
};

void send_json(SOCKET clientSocket, std::map<std::string, AppInfo>& appInfoMap) {
    nlohmann::json jsonData;
    for (const auto& pair : appInfoMap) {
        jsonData[pair.first]["name"] = pair.second.name;
        jsonData[pair.first]["activeTime"] = pair.second.activeTime.count();
    }

    std::string jsonString = jsonData.dump();

    int bytesSent = send(clientSocket, jsonString.c_str(), jsonString.size(), 0);
    if (bytesSent == SOCKET_ERROR) {
        // ��������� ������ ��� �������� ������ �� ������
    }
}

void HandleClientData(SOCKET clientSocket, std::map<std::string, AppInfo>& appInfoMap) {
    while (true) {
        char key[1024];
        byte bytesRead = recv(clientSocket, key, sizeof(key), 0);
        if (bytesRead == -1) {
            // ��������� ������ ��� ������ ������
        }
        else {
            key[bytesRead] = '\0';
            if (strcmp(key, "screenshot") == 0) {
                // ����� ����� ���� ������ ��� ��������� ������� "screenshot"
            }
            if (strcmp(key, "json") == 0) {
                send_json(clientSocket, appInfoMap);
            }
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    SetConsoleOutputCP(1251);

    std::map<std::string, AppInfo> appInfoMap;

    HWND prevHwnd = GetForegroundWindow();
    char windowTitle[256];
    GetWindowTextA(prevHwnd, windowTitle, sizeof(windowTitle));
    std::string prevAppName = windowTitle;

    high_resolution_clock::time_point prevTimePoint = high_resolution_clock::now();

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        // ��������� ������ ��� ������������� Winsock
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        // ��������� ������ ��� �������� ������
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        // ��������� ������ ��� ����������� � �������
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::thread(HandleClientData, clientSocket, std::ref(appInfoMap)).detach();
    while (true) {
        HWND hwnd = GetForegroundWindow();

        if (hwnd != NULL && hwnd != prevHwnd) {
            GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));
            std::string appName = windowTitle;
            if (appName == "")
                continue;

            high_resolution_clock::time_point currentTime = high_resolution_clock::now();
            milliseconds elapsed = duration_cast<milliseconds>(currentTime - prevTimePoint);
            appInfoMap[prevAppName].activeTime += elapsed;
            appInfoMap[prevAppName].name = appName;

            prevHwnd = hwnd;
            prevAppName = appName;
            prevTimePoint = currentTime;
        }

        Sleep(1000);
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
