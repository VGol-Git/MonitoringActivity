#include "screenshot.h"

int SaveBitmapToFile(HBITMAP hBitmap, const char* fileName) {
    BITMAP bmp;
    if (!GetObject(hBitmap, sizeof(BITMAP), &bmp)) {
        return -1;
    }

    int width = bmp.bmWidth;
    int height = bmp.bmHeight;

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;

    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::ofstream file(fileName, std::ios::binary);
    if (!file) {
        return -1;
    }

    file.write(reinterpret_cast<const char*>(&bmfHeader), sizeof(BITMAPFILEHEADER));
    file.write(reinterpret_cast<const char*>(&bi), sizeof(BITMAPINFOHEADER));

    BYTE* pBuffer = new BYTE[dwBmpSize];
    if (!pBuffer) {
        return -1;
    }

    HDC hdcScreen = GetDC(NULL);
    if (!GetDIBits(hdcScreen, hBitmap, 0, height, pBuffer, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
        delete[] pBuffer;
        ReleaseDC(NULL, hdcScreen);
        return -1;
    }

    file.write(reinterpret_cast<const char*>(pBuffer), dwBmpSize);

    file.close();
    delete[] pBuffer;
    ReleaseDC(NULL, hdcScreen);

    return 0;
}

int CaptureScreenshot() {
    HDC hdcScreen = GetDC(NULL);

    if (!hdcScreen) {
        std::cerr << "������ ��� ��������� ����������� ������." << std::endl;
        return -1;
    }

    int screenWidth = GetDeviceCaps(hdcScreen, HORZRES);
    int screenHeight = GetDeviceCaps(hdcScreen, VERTRES);

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    if (SaveBitmapToFile(hBitmap, "screenshot.bmp") == 0) {
        std::cout << "�������� ������� �������� � ���� 'screenshot.bmp'." << std::endl;
    }
    else {
        std::cerr << "������ ��� ���������� ���������." << std::endl;
    }

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}