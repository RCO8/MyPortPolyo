#include <windows.h>
//#include <cmath>//각을 구할때 필요한 헤더
#include "resource.h"
//#include <mmsystem.h>//사운드를 넣을때 필요한 헤더
//#include <commctrl.h>//공통 컨트롤을 위한(프로그래스 바) 헤더
//#pragma comment(lib."winmm.lib")//멀티미디어 함수를 사용하기 위한 lib파일

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;//인스턴스 핸들
LPCTSTR lpszClass = TEXT("Fortress");
HWND hWndMain;

//비트맵 리소스 핸들(배경 및 아이콘)
HBITMAP background[10];

const int imgSize = 50;
char stage[14][23] = {
	//x : 벽, # : 블록, * : 폭탄
	{"                      "},
	{" x x#x x x xx x x x x "},
	{"    #      xx         "},
	{" x x x#x x##  x x#x x "},
	{"           xx# #      "},
	{" x x x x x xx x x x x "},
	{"        ##            "},
	{"           #          "},
	{" x x x x x xx x x x#x "},
	{"           xx#        "},
	{" x x x x x  ##x x x x "},
	{"     ###   xx         "},
	{" x x x x x xx x x#x x "},
	{"                      "}
};
struct Player {
	int x=0, y=0;
	int bombCount = 1;
} me;
struct Bomb {
	int x, y;
	int length;
}bmb[10];
int blockCount = 0;

//비트맵 이미지의 필요 없는 배경을 없애 주는 함수
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask);
//비트맵을 윈도우상에 그려주는 함수
void DrawBitmap(HDC hdc);

//윈도우메인 부분
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	//윈도우 클래스 등록
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	//메뉴 등록 부분
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//UpdateWindow(hWnd);

	/*인삿말
	MessageBox(hWnd, TEXT("안녕하세요.\n혜광 컴퓨터 학원입니다."), TEXT("WELCOME TO
	FORTRESS WORLD!"), MB_OK);
	*/

	//메시지 루프
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

// 윈도우 메시지 처리
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT crt;
	LPMINMAXINFO pmmi;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 1200, 800);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top, SWP_NOMOVE | SWP_NOZORDER);

		//비트맵 리소스들을 Backgroud[]배열에 등록
		background[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));//벽 리소스
		background[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));//블록 리소스
		background[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));//플레이어 리소스
		background[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));//폭탄 리소스
		background[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));//폭발 리소스

		//배경음 지정.
		//Playsound(TEXT("my.wav"),NULL,SND_ASYNC);

		SetTimer(hWnd, 1, 1000, NULL);
		KillTimer(hWnd, 1);

		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			if(stage[me.y - 1][me.x] == ' ' || stage[me.y - 1][me.x] == '+') me.y -= 1;
			break;
		case VK_DOWN:
			if (stage[me.y + 1][me.x] == ' ' || stage[me.y + 1][me.x] == '+') me.y += 1;
			break;
		case VK_LEFT:
			if (stage[me.y][me.x - 1] == ' ' || stage[me.y][me.x - 1] == '+') me.x -= 1;
			break;
		case VK_RIGHT:
			if (stage[me.y][me.x + 1] == ' ' || stage[me.y][me.x + 1] == '+') me.x += 1;
			break;
		case 'Z':
			//폭탄 놓기
			if (me.bombCount > 0)
			{
				me.bombCount--;
				stage[me.y][me.x] = '*';
				bmb[0].x = me.x;
				bmb[0].y = me.y;
				SetTimer(hWnd, 1, 1000, NULL);
			}
			break;
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			//폭탄 터트리기
			stage[bmb[0].y][bmb[0].x] = '+';
			if (stage[bmb[0].y - 1][bmb[0].x] != 'x' && bmb[0].y-1 >= 0)stage[bmb[0].y-1][bmb[0].x] = '+';
			if (stage[bmb[0].y + 1][bmb[0].x] != 'x' && bmb[0].y+1 <= 14)stage[bmb[0].y+1][bmb[0].x] = '+';
			if (stage[bmb[0].y][bmb[0].x - 1] != 'x' && bmb[0].x-1 >= 0)stage[bmb[0].y][bmb[0].x-1] = '+';
			if (stage[bmb[0].y][bmb[0].x + 1] != 'x' && bmb[0].x+1 <= 22)stage[bmb[0].y][bmb[0].x+1] = '+';

			KillTimer(hWnd, 1);

			me.bombCount++;
			SetTimer(hWnd, 2, 500, NULL);
			break;
		case 2:
			KillTimer(hWnd, 2);

			if (stage[me.y][me.x] == '+') {
				if (MessageBox(hWnd, TEXT("게임 오버!!!"), TEXT("GAME"), MB_OK) == IDOK)
					PostQuitMessage(0);
			}

			stage[bmb[0].y][bmb[0].x] = ' ';
			if(stage[bmb[0].y - 1][bmb[0].x] == '+') stage[bmb[0].y - 1][bmb[0].x] = ' ';
			if (stage[bmb[0].y + 1][bmb[0].x] == '+')stage[bmb[0].y + 1][bmb[0].x] = ' ';
			if (stage[bmb[0].y][bmb[0].x - 1] == '+')stage[bmb[0].y][bmb[0].x - 1] = ' ';
			if (stage[bmb[0].y][bmb[0].x + 1] == '+')stage[bmb[0].y][bmb[0].x + 1] = ' ';

			if (blockCount == 0)
			{
				if (MessageBox(hWnd, TEXT("게임 클리어!!!"), TEXT("GAME"), MB_OK) == IDOK)
					PostQuitMessage(0);
			}
			break;
		}
		InvalidateRect(hWnd, NULL, false);
		return 0;
	case WM_GETMINMAXINFO:	//윈도우 창이 변경되지 않기 위한 이벤트
		pmmi = (LPMINMAXINFO)lParam;
		pmmi->ptMinTrackSize.x = 125;
		pmmi->ptMinTrackSize.y = 840;
		pmmi->ptMaxTrackSize.x = 1250;
		pmmi->ptMaxTrackSize.y = 840;

		pmmi->ptMaxSize.x = 1250;
		pmmi->ptMaxSize.y = 840;
		pmmi->ptMaxPosition.x = 100;
		pmmi->ptMaxPosition.y = 100;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawBitmap(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

/*비트맵 파일을 화면에 보이게할 함수.
더블 버퍼링을 이용하여 화면의 깜박이는 현상 없앰.
프로그래스와 에디트 부분은 아직 깜박이는 현상이 있음.
*/
void DrawBitmap(HDC hdc)
{
	//실제 비트멥 이미지를 가질 DC핸들과 가상의 DC핸들 변수선언
	HDC hMemDC, hMemDC2;
	HBITMAP hOldBitmap, hBackBit;

	//실제 비트멥 이미지를 가질 DC핸들 지정
	hMemDC = CreateCompatibleDC(hdc);
	hBackBit = CreateCompatibleBitmap(hdc, 1200, 800);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackBit);

	//임시 비트맵 이미지를 가질 가상 DC핸들 지정
	hMemDC2 = CreateCompatibleDC(hdc);
	//배경을 선택
	SelectObject(hMemDC2, background[1]);

	//가상의 hMemDC2를 실제 hMemDC에 복사
	int i=0, j=1;
	while (i < 24 || j < 15)	//테두리벽
	{
		if (i < 24)
		{
			BitBlt(hMemDC, i * imgSize, 0, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			BitBlt(hMemDC, i * imgSize, 750, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			i++;
		}
		if (j < 15)
		{
			BitBlt(hMemDC, 0, j * 50, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			BitBlt(hMemDC, 1150, j * 50, imgSize, imgSize, hMemDC2, 0, 0, SRCCOPY);
			j++;
		}
	}
	DeleteDC(hMemDC2);
	
	blockCount = 0;
	//투명색으로 빨간색으로 지정 후 hMemDC에 복사
	for (int y = 0; y < 14; y++)
		for (int x = 0; x < 22; x++)
		{
			//벽, 블록 그리기
			if(stage[y][x] == 'x')
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[1], RGB(255, 255, 255));
			else if (stage[y][x] == '#')
			{
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[2], RGB(255, 255, 255));
				blockCount++;
			}
			else if(stage[y][x] == '*')
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[4], RGB(255, 255, 255));
			else if(stage[y][x] == '+')
				TransBlt(hMemDC, (x * imgSize) + 50, (y * imgSize) + 50, background[5], RGB(255, 255, 255));
		}
	TransBlt(hMemDC, (me.x* imgSize) + 50, (me.y* imgSize) + 50, background[3], RGB(255, 255, 255));
	//실제의 이미지를 그린다
	BitBlt(hdc, 0, 0, 1200, 800, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBackBit);
	DeleteDC(hMemDC);
}
/*
비트맵의 필요없는 배경부분을 없애줄수 있는 함수.
TransparentBle함수에 비하여 속도나 성능이 뛰어남.
*/
//* TransBlt 함수
// 투명 비트맵을 출력하는 함수. (x,y)에 출력 위치. clrMask에 투명색을 준다.
void TransBlt(HDC hdc, int x, int y, HBITMAP hbitmap, COLORREF clrMask)
{
	BITMAP bm;
	COLORREF cColor;
	HBITMAP bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC		hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT	ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hbitmap);
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;
	DPtoLP(hdcTemp, &ptSize, 1);

	hdcBack = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem = CreateCompatibleDC(hdc);
	hdcSave = CreateCompatibleDC(hdc);

	bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndMem = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	bmBackOld = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld = (HBITMAP)SelectObject(hdcSave, bmSave);

	SetMapMode(hdcTemp, GetMapMode(hdc));

	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	cColor = SetBkColor(hdcTemp, clrMask);

	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	SetBkColor(hdcTemp, cColor);

	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, x, y, SRCCOPY);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);
	BitBlt(hdc, x, y, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}