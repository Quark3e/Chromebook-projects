/**
 * @file main.cpp
 * @author https://learn.microsoft.com/sv-se/windows/win32/fileio/listing-the-files-in-a-directory?redirectedfrom=MSDN
 * @brief List all files and directories in a directory
 * @version 0.1
 * @date 2024-12-14
 * 
 */

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include <iostream>
#include <iomanip>


#pragma comment(lib, "User32.lib");

void DisplayErrorBox(LPTSTR lpszFunction);

int _tmain(int argc, TCHAR *argv[]) {
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER   filesize;
    TCHAR           szDir[MAX_PATH];
    size_t          length_of_arg;
    HANDLE          hFind = INVALID_HANDLE_VALUE;
    DWORD           dwError = 0;

    // If the directory is not specified as a command-line argument,
    // print usage.
    if(argc!=2) {
        _tprintf(TEXT("\nUsage: %s <directory name>\n"), argv[0]);
        return -1;
    }

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.
    StringCchLength(argv[1], MAX_PATH, &length_of_arg);
    if(length_of_arg > (MAX_PATH - 3)) {
        _tprintf(TEXT("\nDirectory path is too long.\n"));
        return -1;
    }


    _tprintf(TEXT("\nTarget directory is %s\n\n"), argv[1]);

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.
    StringCchCopy(szDir, MAX_PATH, argv[1]);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.
    hFind = FindFirstFile(szDir, &ffd);

    if(INVALID_HANDLE_VALUE == hFind) {
        DisplayErrorBox(TEXT("FindFirstFile"));
        return dwError;
    }

    // List all the files in the directory with some info about them.
    do {
        if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // _tprintf(TEXT("  %s  <DIR>\n"), ffd.cFileName);
            std::cout<<std::setw(10)<<std::fixed<<    "<DIR>";
            std::cout<<std::setw(10)<<" "<<std::setw(6)<<" ";
            std::cout<<std::string(3, ' ')<<ffd.cFileName<<std::endl;
        }
        else {
            filesize.LowPart    = ffd.nFileSizeLow;
            filesize.HighPart   = ffd.nFileSizeHigh;
            // _tprintf(TEXT("  %s  %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
            std::cout<<std::setw(10)<<std::fixed<<"<NON-DIR>";
            std::cout<<std::setw(10)<<filesize.QuadPart<<std::setw(6)<<"bytes";
            std::cout<<std::string(3, ' ')<<ffd.cFileName<<std::endl;
        }
    }
    while(FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    if(dwError != ERROR_NO_MORE_FILES) {
        DisplayErrorBox(TEXT("FindFirstFile"));
    }

    FindClose(hFind);
    return dwError;
}

void DisplayErrorBox(LPTSTR lpszFunction) {
    // Retrieve the system error for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );

    // Display the error message and clean up
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf)/sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}