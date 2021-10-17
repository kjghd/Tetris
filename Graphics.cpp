#include "Graphics.h"
#pragma comment(lib, "d2d1")

ComLibrary::ComLibrary(DWORD dwCoInit)
{
	CoInitializeEx(NULL, dwCoInit);
}

ComLibrary::~ComLibrary()
{
	CoUninitialize();
}

Graphics::Graphics()
	:
	CoLib(ComLibrary(COINIT_APARTMENTTHREADED)),
	pFactory(NULL),
	pRenderTarget(NULL),
	pImagingFactory(NULL),
	pDebugBrush(NULL)
{
}
Graphics::~Graphics()
{
}

void Graphics::Init(HWND hWnd)
{
	HRESULT hr = S_OK;
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (!pFactory)
	{
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
		if (SUCCEEDED(hr))
		{
			RECT rc{ NULL };
			GetClientRect(hWnd, &rc);
			hr = pFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right, rc.bottom)),
				&pRenderTarget);
		}
		if (pRenderTarget)
		{
			pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1, 1, 0), &pDebugBrush);
		}
	}
	if (!pImagingFactory)
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pImagingFactory));
	}
}

void Graphics::BeginDraw()
{
	pRenderTarget->BeginDraw();
}
void Graphics::EndDraw()
{
	pRenderTarget->EndDraw();
}

void Graphics::ClearScreen(float r, float g, float b, float a)
{
	pRenderTarget->Clear(D2D1::ColorF(r, g, b, a));
}

void Graphics::DrawBitmap(ID2D1Bitmap** ppBmp, D2D1_RECT_F destination)
{
	D2D1_RECT_F rc
	{
		destination.left / 1.25,
		destination.top / 1.25,
		destination.right / 1.25,
		destination.bottom / 1.25
	};
	pRenderTarget->DrawBitmap(*ppBmp, rc, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
}

void Graphics::DrawBitmapArea(ID2D1Bitmap** ppBmp, D2D1_RECT_F destination, D2D1_RECT_F uv_area)
{
	D2D1_RECT_F rc
	{
		destination.left / 1.25,
		destination.top / 1.25,
		destination.right / 1.25,
		destination.bottom / 1.25
	};
	pRenderTarget->DrawBitmap(*ppBmp, rc, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, uv_area);
}


void Graphics::DrawRect(D2D1_RECT_F destination)
{
	D2D1_RECT_F rc
	{
		destination.left / 1.25,
		destination.top / 1.25,
		destination.right / 1.25,
		destination.bottom / 1.25
	};
	pRenderTarget->FillRectangle(rc, pDebugBrush);
}

void Graphics::CreateBitmap(LPCWSTR path, ID2D1Bitmap** ppBmp)
{
	HRESULT hr{ S_OK };
	CComPtr<IWICBitmapDecoder> pDecoder{ NULL };
	hr = pImagingFactory->CreateDecoderFromFilename(
		path,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&pDecoder
	);
	if (SUCCEEDED(hr))
	{
		CComPtr<IWICBitmapFrameDecode> pFrame{ NULL };
		hr = pDecoder->GetFrame(0, &pFrame);
		if (SUCCEEDED(hr))
		{
			CComPtr<IWICFormatConverter> pConverter{ NULL };
			hr = pImagingFactory->CreateFormatConverter(&pConverter);
			if (SUCCEEDED(hr))
			{
				hr = pConverter->Initialize(
					pFrame,
					GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone,
					NULL,
					0.f,
					WICBitmapPaletteTypeMedianCut
				);
				if (SUCCEEDED(hr))
				{
					hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, ppBmp);
				}
			}
		}
	}
}
