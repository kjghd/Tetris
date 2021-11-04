#pragma once
#include <xaudio2.h>
#include <atlbase.h> // CComPtr

class Audio
{
public:
	CComPtr<IXAudio2> pXAudio;
	CComPtr<IXAudio2MasteringVoice> pXMastering;

	void Init();
	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	HRESULT ReadChunk(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
	void Populate();
};

