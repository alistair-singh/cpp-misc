
#pragma once

#include "Win32Error.hh"

#include <Windows.h>
#include <memory>
#include <string>

namespace wwatch {

class FileWatcher {
  HANDLE directory_;
  std::unique_ptr<char> buffer_;
  const unsigned bufferSize_;
  size_t iterations_;

  PFILE_NOTIFY_INFORMATION pollInternal();

public:
  FileWatcher(const std::wstring &path, unsigned bufferSize = 4096);

  template <class THandler> void poll(THandler handler) {
    auto notifications = pollInternal();
    size_t localIteration = 0;
    do {
      const std::wstring changedPath(notifications->FileName,
                                     notifications->FileNameLength / 2);
      handler(++iterations_, ++localIteration, notifications->Action,
              changedPath);
      notifications += notifications->NextEntryOffset;
    } while (notifications->NextEntryOffset != 0);
  }
};
}
