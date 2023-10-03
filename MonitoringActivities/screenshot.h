#pragma once

#include <iostream>
#include <Windows.h>
#include <fstream>

//int SendFileOverSocket(SOCKET clientSocket, const char* filePath);
//int SaveBitmapToFile(HBITMAP hBitmap, const char* fileName);
int CaptureScreenshot(SOCKET clientSocket);