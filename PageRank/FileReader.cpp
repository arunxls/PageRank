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

FileReader::FileReader(char*& filename)
{
    this->filename = filename;
    this->offset_current_read = 0;
    this->offset_overall = 0;

    this->getFileHandle();
    this->size = this->getFileSize();
}

FileReader::~FileReader()
{
    if (this->FileHandleOpen)
    {
        CloseHandle(this->hFile);
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

    this->offset_current_read = bytesTrasferred;
    this->offset_overall += bytesTrasferred;
}

bool FileReader::has_next()
{
    return this->offset_overall < this->size;
}

LONGLONG FileReader::getFileSize()
{
    LARGE_INTEGER size;
    GetFileSizeEx(this->hFile, &size);
    return size.QuadPart;
}

void FileReader::getFileHandle()
{
    HANDLE hFile = CreateFile(this->filename,               // file to open
        GENERIC_READ,          // open for reading
        0,       // exclusive reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, // normal file
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        hFile = CreateFile(this->filename,               // file to open
            GENERIC_READ,          // open for reading
            FILE_SHARE_READ,       // share for reading
            NULL,                  // default security
            OPEN_EXISTING,         // existing file only
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, // normal file
            NULL);
        if (DEBUG && hFile == INVALID_HANDLE_VALUE) {
            _tprintf("Terminal failure: unable to open file \"%s\" for read.\n", this->filename);
            std::abort();
        }
    }

    this->FileHandleOpen = true;
    this->hFile = hFile;
}

void FileReader::CloseFileHandle()
{
    CloseHandle(this->hFile);
    this->FileHandleOpen = false;
}