#pragma once
#include <d2d1.h>
#include <wincodec.h>
#include <atlbase.h>

class ComLibrary // Manages CoInitiazeEx and CoUnitialize so the COM Library may close after CComPtrs have released.
{
public:
	ComLibrary(DWORD dwCoInit);
	~ComLibrary();
};

class Graphics
{
public:
	ComLibrary CoLib;
	CComPtr<ID2D1Factory> pFactory;
	CComPtr<ID2D1HwndRenderTarget> pRenderTarget; // Not releasing?
	CComPtr<IWICImagingFactory> pImagingFactory;
	CComPtr<ID2D1SolidColorBrush>pDebugBrush;
	//IWICImagingFactory* pImagingFactory;
	

public:
	void Init(HWND hWnd);
	void BeginDraw();
	void EndDraw();
	void ClearScreen(float r = 0, float g = 0, float b = 0, float a = 1.f);
	void DrawBitmap(ID2D1Bitmap** ppBmp, D2D1_RECT_F destination);
	void DrawBitmapArea(ID2D1Bitmap** ppBmp, D2D1_RECT_F destination, D2D1_RECT_F uv_area);
	void DrawRect(D2D1_RECT_F destination);

	void CreateBitmap(LPCWSTR path, ID2D1Bitmap** ppBmp);

	

	Graphics();
	~Graphics();
};

