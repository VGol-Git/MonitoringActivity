#pragma once

#include <iostream>
#include <Windows.h>
#include <fstream>

int SaveBitmapToFile(HBITMAP hBitmap, const char* fileName);
int CaptureScreenshot();