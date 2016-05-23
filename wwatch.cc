
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

bool hasExtension(const wstring &str, const wstring &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void handle(int iteration, unsigned action, const wstring &name,
            const wstring &command, const wstring &extension) {
  wcout << iteration << ":(" << hex << action << ") " << name << " - "
        << command << endl;
  if (hasExtension(name, extension))
  {wcout << "Executing command: " << command << endl;
    _wsystem((command + L" " + name).data());
  }
  else
    wcout << "Skipping command." << endl;
}

void printError() {
  unsigned errorCode = GetLastError();
  const size_t errorMessageSize = 512;
  wchar_t errorMessage[errorMessageSize];
  FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errorMessage,
                 errorMessageSize, NULL);
  wcerr << "Error Code: " << errorCode << endl;
  wcerr << "Error Message: " << errorMessage << endl;
}

wstring joinCommand(int argc, wchar_t **argv) {
  wstring c;
  for (int i = 0; i < argc; i++) {
    c += L" ";
    c += argv[i];
  }
  return c;
}

int wmain(int argc, wchar_t *argv[]) {
  wstring dir;
  wstring extension;
  wstring subcommand;

  if (argc < 4) {
    wcout << "usage:" << endl;
    wcout << "\t" << argv[0] << " \\dir\\to\\watch extension command" << endl;
    return 1;
  } else {
    dir = argv[1];
    extension = argv[2];
    subcommand = joinCommand(argc - 3, &argv[3]);
  }

  HANDLE hDir =
      CreateFileW(dir.data(), FILE_LIST_DIRECTORY,
                  FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL,
                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

  if (hDir == INVALID_HANDLE_VALUE) {
    printError();
    wcerr << "Cannot open directory '" << argv[1] << "'" << endl;
    return 2;
  }

  int8_t buffer[4096];
  DWORD dwBytesReturned = 0;

  while (TRUE) {
    if (ReadDirectoryChangesW(hDir, static_cast<LPVOID>(buffer), sizeof(buffer),
                              FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE,
                              &dwBytesReturned, NULL, NULL) == FALSE) {
      printError();
      wcerr << L"Error Reading Directory Change" << endl;
      return 2;
    } else {
      PFILE_NOTIFY_INFORMATION notifications =
          reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&buffer[0]);
      int iteration = 0;
      do {
        handle(
            ++iteration, notifications->Action,
            wstring(notifications->FileName, notifications->FileNameLength / 2),
            subcommand, extension);
        notifications += notifications->NextEntryOffset;
      } while (notifications->NextEntryOffset != 0);
    }
  }
}
