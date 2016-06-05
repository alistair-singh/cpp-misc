
#pragma once

#include "Win32Error.hh"

#include <Windows.h>
#include <functional>
#include <memory>
#include <string>

namespace wwatch {

struct FileWatchEvent {
  unsigned action;
  std::wstring path;
};

class FileWatcher {
  HANDLE directory_;
  std::unique_ptr<char> buffer_;
  const unsigned bufferSize_;

  PFILE_NOTIFY_INFORMATION pollInternal();

public:
  FileWatcher(const std::wstring &path, unsigned bufferSize = 4096);
  void poll(std::function<void(const FileWatchEvent &)> handler);
};
}
