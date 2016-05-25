
#include <Windows.h>
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
    ::_flushall();
    ::_wsystem(fullCommand.data());
  }
}

namespace wwatch {
class Win32Error {
  unsigned code_;
  std::wstring message_;

public:
  Win32Error() : code_(0) {}
  Win32Error(unsigned code, const std::wstring &message)
      : code_(code), message_(message) {}
  const std::wstring &message() const { return message_; }
  const unsigned &code() const { return code_; }
  static Win32Error GetLastWin32Error() {
    const unsigned size = 512;
    Win32Error error;
    error.code_ = ::GetLastError();
    error.message_.reserve(size);
    ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error.code_,
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     error.message_.data(), size, NULL);
    return error;
  }
};

class FileWatcher {
  HANDLE directory_;
  std::unique_ptr<char> buffer_;
  const size_t bufferSize_;
  const size_t iterations_;

public:
  FileWatcher(const std::wstring &path, size_t bufferSize = 4096)
      : bufferSize_(bufferSize), iterations_(0) {
    buffer_ = unique_ptr<char>(new char[bufferSize_]);
    directory_ =
        ::CreateFileW(path.data(), FILE_LIST_DIRECTORY,
                      FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                      NULL, OPEN_EXISTING, NULL, NULL);
    if (directory_ == INVALID_HANDLE_VALUE) {
      throw Win32Error::GetLastWin32Error();
    }
  }
  template <class THandler> unsigned poll(THandler handler) {
    ::memset(buffer_.get(), 0, bufferSize_);
    unsigned dwBytesReturned = 0;
    if (::ReadDirectoryChangesW(hDir, static_cast<LPVOID>(buffer), bufferSize,
                                FALSE, FILE_NOTIFY_CHANGE_SIZE,
                                &dwBytesReturned, NULL, NULL) == FALSE) {
      throw Win32Error::GetLastWin32Error();
    } else {
      PFILE_NOTIFY_INFORMATION notifications =
          reinterpret_cast<PFILE_NOTIFY_INFORMATION>(buffer);
      size_t localIteration = 0;
      do {
        wstring changedPath(notifications->FileName,
                            notifications->FileNameLength / 2);
        handler(++iterations_, ++localIteration, notifications->Action,
                changedPath);
        notifications += notifications->NextEntryOffset;
      } while (notifications->NextEntryOffset != 0);
    }
    return dwBytesReturned;
  }
};
}

void printError() {
  auto error = wwatch::Win32Error::GetLastWin32Error();
  wcerr << "Error Code: " << error.code() << endl;
  wcerr << "Error Message: " << error.message() << endl;
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
