
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

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
    const unsigned reservedSize = 512;
    Win32Error error;
    error.code_ = ::GetLastError();
    error.message_.resize(reservedSize);
    auto actualSize =
        ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error.code_,
                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                         const_cast<wchar_t*>(error.message_.data()), reservedSize, NULL);
    error.message_.resize(actualSize);
    return error;
  }
};

class FileWatcher {
  HANDLE directory_;
  std::unique_ptr<char> buffer_;
  const unsigned bufferSize_;
  size_t iterations_;

public:
  FileWatcher(const std::wstring &path, unsigned bufferSize = 4096)
      : bufferSize_(bufferSize), iterations_(0) {
    buffer_ = std::unique_ptr<char>(new char[bufferSize_]);
    directory_ =
        ::CreateFileW(path.data(), FILE_LIST_DIRECTORY,
                      FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (directory_ == INVALID_HANDLE_VALUE) {
      throw Win32Error::GetLastWin32Error();
    }
  }
  template <class THandler> unsigned poll(THandler handler) {
    ::memset(buffer_.get(), 0, bufferSize_);
    DWORD dwBytesReturned = 0;
    if (::ReadDirectoryChangesW(directory_, static_cast<LPVOID>(buffer_.get()),
                                bufferSize_, FALSE, FILE_NOTIFY_CHANGE_SIZE,
                                &dwBytesReturned, NULL, NULL) == FALSE) {
      throw Win32Error::GetLastWin32Error();
    } else {
      PFILE_NOTIFY_INFORMATION notifications =
          reinterpret_cast<PFILE_NOTIFY_INFORMATION>(buffer_.get());
      size_t localIteration = 0;
      do {
        const std::wstring changedPath(notifications->FileName,
                                       notifications->FileNameLength / 2);
        handler(++iterations_, ++localIteration, notifications->Action,
                changedPath);
        notifications += notifications->NextEntryOffset;
      } while (notifications->NextEntryOffset != 0);
    }
    return dwBytesReturned;
  }
};

std::wstring concat(const std::vector<std::wstring> &args) {
  std::wstring result;
  for (const auto &arg : args) {
    result += L" ";
    result += arg;
  }
  return result;
}

bool endsWith(const std::wstring &str, const std::wstring &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::wstring> split(std::wstring str, wchar_t delimiter) {
  std::vector<std::wstring> result;
  std::wstring current;
  for (auto const &c : str) {
    if (c == delimiter) {
      result.push_back(current);
      current.clear();
      continue;
    }
    current += c;
  }
  if (current != L"")
    result.push_back(current);
  return result;
}
}

void handle(size_t global, size_t local, unsigned action,
            const std::wstring &name, const std::wstring &command,
            const std::vector<std::wstring> &extensions) {
  auto run =
      std::any_of(std::cbegin(extensions), std::cend(extensions),
                  [&](const auto &ext) { return wwatch::endsWith(name, ext); });
  if (run) {
    auto fullCommand = wwatch::concat({command, name});
    std::wcout << L"#######" << global << L":" << local << L":(" << std::hex
               << action << std::dec << L") " << name << L" - " << fullCommand
               << std::endl;
    ::_flushall();
    ::_wsystem(fullCommand.data());
  }
}

int wmain(int argc, wchar_t *argv[]) {
  if (argc < 4) {
    std::wcout << L"usage:" << std::endl;
    std::wcout << L"\t" << argv[0] << L" \\dir\\to\\watch extension command"
               << std::endl;
    return 1;
  }

  std::wstring dir(argv[1]);
  auto extensions = wwatch::split(argv[2], L',');

  auto begin = &argv[3];
  auto end = begin + (argc - 3);
  std::vector<std::wstring> args(begin, end);
  auto subcommand = wwatch::concat(args);

  try {
    wwatch::FileWatcher watcher(dir);
    while (true) {
      watcher.poll([&](auto gi, auto li, auto action, const auto &path) {
        handle(gi, li, action, path, subcommand, extensions);
      });
    }
  } catch (const wwatch::Win32Error &error) {
    std::wcerr << L"Error Code: " << error.code() << std::endl;
    std::wcerr << L"Error Message: " << error.message() << std::endl;
    return 2;
  }
}
