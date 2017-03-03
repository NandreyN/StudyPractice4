#undef UNICODE
#include <windows.h>
#include  <math.h>
#include <cmath>
using namespace std;

struct Graph
{
	int a;
	int b; 
	int c;
};
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void Draw(HDC& hdc, int x ,int y, Graph& graph);

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
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_SIZE:
		x = LOWORD(lparam);
		y = HIWORD(lparam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		Graph gr; gr.a = 1; gr.b = 2; gr.c = 0;
		Draw(hdc, x, y, gr);
		EndPaint(hwnd, &ps);
		break;
	case WM_CLOSE:
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
void Draw(HDC& hdc, int x, int y, Graph& gr)
{
	int xPoints, yPoints, divValueX, divValueY; // Кол-во делений
	xPoints = 10; yPoints = 5;
	divValueX = x / (xPoints * 2);
	divValueY = y / (yPoints * 2);

	SetMapMode(hdc, MM_ANISOTROPIC);
	SetWindowExtEx(hdc, x, y, NULL);
	SetViewportExtEx(hdc, x, -y, NULL);
	SetViewportOrgEx(hdc, x / 2, y / 2, NULL);

	HPEN newPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, newPen);

	int newX, newY; newX = x / 2; newY = y / 2;


	MoveToEx(hdc, -newX, 0, NULL);
	LineTo(hdc, newX, 0);
	MoveToEx(hdc, 0, newY, NULL);
	LineTo(hdc, 0, -newY);


	DeleteObject(newPen);
	SelectObject(hdc, oldPen);

	for (int xx  = 0;  xx <= newX; xx += divValueX)
	{
		MoveToEx(hdc, xx, 5, NULL);
		LineTo(hdc, xx, -5);
		MoveToEx(hdc, -xx, 5, NULL);
		LineTo(hdc, -xx, -5);
	}

	for (int yy = 0; yy <= newY; yy += divValueY)
	{
		MoveToEx(hdc, 5, yy, NULL);
		LineTo(hdc, -5, yy);
		MoveToEx(hdc, 5, -yy, NULL);
		LineTo(hdc, -5, -yy);
	}
	MoveToEx(hdc, -newX, 0, NULL);
	// Drawing
	for (double xArg = -newX; xArg <= newX; xArg += 0.1)
	{
		double fx = gr.a * ((xArg / divValueX) * (xArg / divValueX)) + gr.b * (xArg / divValueX) + gr.c;
		LineTo(hdc, xArg, fx * divValueY);
		//SetPixel(hdc, xArg, fx * divValueY, RGB(0,0,0));
	}
}