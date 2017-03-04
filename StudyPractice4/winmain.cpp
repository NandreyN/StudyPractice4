﻿#undef UNICODE
#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
using namespace std;

struct Graph
{
	int a;
	int b;
	int c;
};
struct DrawAreaInfo
{
	int xPoints, yPoints, divValueX, divValueY, newX, newY;
};
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
DrawAreaInfo GetAreaInfo(int x, int y);
DrawAreaInfo Draw(HDC& hdc, int x, int y, Graph& graph);
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld);
double GetDistance(int x1, int y1, int x2, int y2);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevHinstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!InitApplication(hinstance))
	{
		MessageBox(NULL, "Unable to Init App", "Error", MB_OK);
		return FALSE;
	}

	if (!InitInstance(hinstance, nCmdShow))
	{
		MessageBox(NULL, "Unable to Init Instance", "Error", MB_OK);
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL InitApplication(HINSTANCE hinstance)
{
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "Graph";
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, "Cannot register class", "Error", MB_OK);
		return FALSE;
	}
	return TRUE;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int x, y;
	static HDC hdc;
	static DrawAreaInfo dAInfo;
	PAINTSTRUCT ps;
	static RECT allocRect;
	static RECT prevRect;
	RECT clientRect;
	static bool isPressed;
	static Graph gr;
	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hwnd);
		isPressed = false;
		gr.a = 1; gr.b = 2; gr.c = 0;

		break;
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
	{
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, x, y, NULL);
		SetViewportExtEx(hdc, x, -y, NULL);
		SetViewportOrgEx(hdc, x / 2, y / 2, NULL);
		dAInfo = Draw(hdc, x, y, gr);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		isPressed = true;
		allocRect.left = LOWORD(lparam);
		allocRect.top = HIWORD(lparam);
		allocRect.right = allocRect.left;
		allocRect.bottom = allocRect.top;
		prevRect = allocRect;
		break;
	}
	case WM_LBUTTONUP:
	{
		isPressed = false;
		//InvalidateRect(hwnd, &allocRect, true);
		break;
	}
	case WM_RBUTTONUP:
	{
		DrawAreaInfo di = GetAreaInfo(x, y);
		int cx = LOWORD(lparam);
		int cy = HIWORD(lparam);
		POINT clickedCoord = ConvertCoordinates(cx, cy, x, y);
		double res = (gr.a*((double)clickedCoord.x / di.divValueX)*((double)clickedCoord.x / di.divValueX) + gr.b*((double)clickedCoord.x / di.divValueX) + gr.c);

		//double newY = (double)clickedCoord.y / di.divValueY;
		double distance = abs(res - ((double)clickedCoord.y) / di.divValueY);
		if (distance <= 0.3)
		{
			string text;
			double t = (double)clickedCoord.x / di.divValueX;
			text += "X: " + to_string(t); text += ", Y : " + to_string(res);
			TextOut(hdc, 0, 0, text.c_str(), text.size());
		}
	}
	case WM_MOUSEMOVE:
	{
		if (!isPressed) return 0;

		allocRect.right = LOWORD(lparam);
		allocRect.top = HIWORD(lparam);

		POINT LT = ConvertCoordinates(allocRect.left, allocRect.top, x, y);
		POINT RB = ConvertCoordinates(allocRect.right, allocRect.bottom, x, y);
		ValidateRect(hwnd, NULL);
		Rectangle(hdc,LT.x, LT.y, RB.x, RB.y);
		break;
	}

	case WM_CLOSE:
		ReleaseDC(hwnd, hdc);
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return FALSE;
}
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	HWND hwnd;
	hwnd = CreateWindow(
		"Graph",
		"Graph",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		NULL,
		NULL,
		hinstance,
		NULL);


	if (!hwnd)
		return FALSE;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld)
{
	// center is (x/2, y/2)
	POINT pt;
	pt.x = x - widthOld / 2;
	pt.y = heightOld / 2 - y;
	return pt;
}

DrawAreaInfo GetAreaInfo(int x, int y)
{
	int xPoints, yPoints, divValueX, divValueY; // Кол-во делений
	xPoints = 5; yPoints = 10;
	divValueX = x / (xPoints * 2);
	divValueY = y / (yPoints * 2);
	int newX, newY; newX = x / 2; newY = y / 2;
	DrawAreaInfo di;
	di.divValueX = divValueX; di.divValueY = divValueY; di.newX = newX; di.newY = newY; di.xPoints = xPoints; di.yPoints = yPoints;
	return di;
}
DrawAreaInfo Draw(HDC& hdc, int x, int y, Graph& gr)
{
	DrawAreaInfo dai = GetAreaInfo(x, y);

	HPEN newPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, newPen);

	MoveToEx(hdc, -dai.newX, 0, NULL);
	LineTo(hdc, dai.newX, 0);
	MoveToEx(hdc, 0, dai.newY, NULL);
	LineTo(hdc, 0, -dai.newY);


	DeleteObject(newPen);
	SelectObject(hdc, oldPen);

	for (int xx = 0; xx <= dai.newX; xx += dai.divValueX)
	{
		MoveToEx(hdc, xx, 5, NULL);
		LineTo(hdc, xx, -5);
		MoveToEx(hdc, -xx, 5, NULL);
		LineTo(hdc, -xx, -5);
	}

	for (int yy = 0; yy <= dai.newY; yy += dai.divValueY)
	{
		MoveToEx(hdc, 5, yy, NULL);
		LineTo(hdc, -5, yy);
		MoveToEx(hdc, 5, -yy, NULL);
		LineTo(hdc, -5, -yy);
	}
	MoveToEx(hdc, -dai.newX, 0, NULL);
	// Drawing
	for (double xArg = -dai.newX; xArg <= dai.newX; xArg += 0.1)
	{
		double fx = gr.a * ((xArg / dai.divValueX) * (xArg / dai.divValueX)) + gr.b * (xArg / dai.divValueX) + gr.c;
		LineTo(hdc, xArg, fx * dai.divValueY);
		//SetPixel(hdc, xArg, fx * divValueY, RGB(0,0,0));
	}
	return dai;
}
double GetDistance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}
