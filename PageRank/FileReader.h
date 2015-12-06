#pragma once

#include <windows.h>
#include "include.h"

class FileReader
{
public:
    char* filename;
    uint64 offset_overall;
    uint64 size;

    HANDLE hFile;

    inline FileReader(char*& filename)
    {
        this->filename = filename;
        this->offset_overall = 0;

        this->getFileHandle();

        LARGE_INTEGER size;
        GetFileSizeEx(this->hFile, &size);
        this->size = size.QuadPart;
    }
    
    inline ~FileReader()
    {
        if (this->FileHandleOpen)
        {
            CloseHandle(this->hFile);
        }
    }

    void read(LPVOID buffer, uint32 bytesTotransfer, uint32& bytesTrasferred);
    
    inline bool has_next()
    {
        return this->offset_overall < this->size;
    }
    
    inline void getFileHandle()
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
            if (hFile == INVALID_HANDLE_VALUE) {
                _tprintf("Terminal failure: unable to open file \"%s\" for read.\n", this->filename);
                std::abort();
            }
        }

        this->FileHandleOpen = true;
        this->hFile = hFile;
    }
    
    inline void CloseFileHandle()
    {
        CloseHandle(this->hFile);
        this->FileHandleOpen = false;
    }

private:
    bool FileHandleOpen;
};