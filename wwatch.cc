
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

bool hasExtension(const wstring &str, const wstring &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void handle(size_t global, size_t local, unsigned action, const wstring &name,
            const wstring &command, const wstring &extension) {
  if (hasExtension(name, extension)) {
    auto fullCommand = (command + L" " + name);
    wcout << "#######" << global << ":" << local << ":(" << hex << action
          << ") " << name << " - " << fullCommand << endl;
    _flushall();
    _wsystem(fullCommand.data());
  }
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

  size_t globalIteration = 0;
  const size_t bufferSize = 4096;
  int8_t buffer[bufferSize];

  while (TRUE) {
    memset(buffer, 0, bufferSize);
    DWORD dwBytesReturned = 0;
    if (ReadDirectoryChangesW(hDir, static_cast<LPVOID>(buffer), bufferSize,
                              FALSE, FILE_NOTIFY_CHANGE_SIZE, &dwBytesReturned,
                              NULL, NULL) == FALSE) {
      printError();
      wcerr << L"Error Reading Directory Change" << endl;
      return 2;
    } else {
      PFILE_NOTIFY_INFORMATION notifications =
          reinterpret_cast<PFILE_NOTIFY_INFORMATION>(buffer);
      size_t localIteration = 0;
      do {
        handle(
            ++globalIteration, ++localIteration, notifications->Action,
            wstring(notifications->FileName, notifications->FileNameLength / 2),
            subcommand, extension);
        notifications += notifications->NextEntryOffset;
      } while (notifications->NextEntryOffset != 0);
    }
  }
}
