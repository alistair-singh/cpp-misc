
#pragma once

#include "Win32Error.hh"

#include <Windows.h>
#include <memory>
#include <string>
#include <vector>

namespace wwatch {

struct FileWatchEvent {
  unsigned action;
  std::wstring path;
};

std::vector<FileWatchEvent> toEvents(PFILE_NOTIFY_INFORMATION notifications);

class FileWatcher {
  HANDLE directory_;
  std::unique_ptr<char> buffer_;
  const unsigned bufferSize_;

  PFILE_NOTIFY_INFORMATION pollInternal();

public:
  FileWatcher(const std::wstring &path, unsigned bufferSize = 4096);
  std::vector<FileWatchEvent> poll();
	HANDLE directory() const { return directory_; }
};
}
