#include "stdafx.h"
#include "FileReader.h"
#include "utils.h"
#include <algorithm>

VOID CALLBACK FileIOCompletionRoutine(
    __in  DWORD dwErrorCode,
    __in  DWORD dwNumberOfBytesTransfered,
    __in  LPOVERLAPPED lpOverlapped)
{
    if (DEBUG)
    {
        _tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
        _tprintf(TEXT("Number of bytes:\t%I32u\n"), dwNumberOfBytesTransfered);
    }
}



void FileReader::read(LPVOID buffer, uint32 bytesTotransfer, uint32& bytesTrasferred)
{
    OVERLAPPED ol = { 0 };

    ol.OffsetHigh = (uint32)(offset_overall >> 32);
    ol.Offset = (uint32)offset_overall;
    ol.hEvent = *(HANDLE*) this;

    if (DEBUG)
    {
        printf("Reading %s\n", filename);
    }

    bool flag = ReadFileEx(this->hFile, (char*)buffer, bytesTotransfer, &ol, FileIOCompletionRoutine);
    if (DEBUG && !flag)
    {
        //DisplayError(TEXT("ReadFile"));
        printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
        return;
    }

    SleepEx(5000, TRUE);
    bytesTrasferred = ol.InternalHigh;

    this->offset_overall += bytesTrasferred;
}