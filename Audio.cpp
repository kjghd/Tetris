#include "Audio.h"

#pragma comment(lib, "xaudio2")

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

void Audio::Init()
{
	HRESULT hr(S_OK);
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr))
	{
		hr = XAudio2Create(&pXAudio, 0, XAUDIO2_DEFAULT_PROCESSOR);

		if (SUCCEEDED(hr))
		{
			hr = pXAudio->CreateMasteringVoice(&pXMastering);
		}
	}

}

HRESULT Audio::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
	HRESULT hr(S_OK);

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	DWORD dwChunkType	 (0);
	DWORD dwChunkDataSize(0);
	DWORD dwRIFFDataSize (0);
	DWORD dwFileType	 (0);
	DWORD bytesRead		 (0);
	DWORD dwOffset		 (0);


	// Add error checking
	while (hr == S_OK)
	{
		DWORD dwRead;
		ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL);
		ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL);

		if(dwChunkType == fourccRIFF)
		{
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL);
		}
		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}
	return S_OK;
}

HRESULT Audio::ReadChunk(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr(S_OK);
	DWORD dwRead;
	SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN);
	ReadFile(hFile, buffer, buffersize, &dwRead, NULL);
	return {};
}

void Audio::Populate()
{

}