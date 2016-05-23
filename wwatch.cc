
#include <Windows.h>
#include <iostream>

using namespace std;

void handle(int iteration, unsigned action, unsigned, wchar_t *name) {
  wcout << "(" << hex << action << ") " << name << endl;
}

void printError() {
  DWORD errorCode = GetLastError();
  const size_t errorMessageSize = 512;
  wchar_t errorMessage[errorMessageSize];
  FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errorMessage,
                 errorMessageSize, NULL);
  wcerr << "Error Code: " << errorCode << endl;
  wcerr << "Error Message: " << errorMessage << endl;
}

int wmain(int argc, wchar_t *argv[]) {
  LPWSTR lpDir;
  if (argc != 2) {
    wcout << "usage:" << endl;
    wcout << "\t" << argv[0] << " \\dir\\to\\watch" << endl;
    return 1;
  } else {
    lpDir = argv[1];
  }

  HANDLE hDir =
      CreateFileW(lpDir, FILE_LIST_DIRECTORY,
                  FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL,
                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

  if (hDir == INVALID_HANDLE_VALUE) {
    printError();
    wcerr << "Cannot open directory '" << argv[1] << "'" << endl;
    return 2;
  }

  FILE_NOTIFY_INFORMATION strFileNotifyInfo[1024];
  DWORD dwBytesReturned = 0;

  while (TRUE) {
    if (ReadDirectoryChangesW(
            hDir, (LPVOID)&strFileNotifyInfo, sizeof(strFileNotifyInfo), FALSE,
            FILE_NOTIFY_CHANGE_LAST_WRITE, &dwBytesReturned, NULL, NULL) == 0) {
      printError();
      wcerr << L"Error Reading Directory Change" << endl;
      return 2;
    } else {
      PFILE_NOTIFY_INFORMATION notifications = strFileNotifyInfo;
      int iteration = 0;
      do {
        handle(iteration++, notifications->Action,
               notifications->FileNameLength, notifications->FileName);
        notifications += notifications->NextEntryOffset;
      } while (notifications->NextEntryOffset != 0);
    }
  }
}
