
#include "FileWatcher.hh"

namespace wwatch {

FileWatcher::FileWatcher(const std::wstring &path, unsigned bufferSize)
    : bufferSize_(bufferSize) {
  buffer_ = std::unique_ptr<char>(new char[bufferSize_]);
  directory_ =
      ::CreateFileW(path.data(), FILE_LIST_DIRECTORY,
                    FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (directory_ == INVALID_HANDLE_VALUE) {
    throw Win32Error::GetLastWin32Error();
  }
}

PFILE_NOTIFY_INFORMATION FileWatcher::pollInternal() {
  ::memset(buffer_.get(), 0, bufferSize_);
  DWORD dwBytesReturned = 0;
  if (::ReadDirectoryChangesW(directory_, static_cast<LPVOID>(buffer_.get()),
                              bufferSize_, TRUE, FILE_NOTIFY_CHANGE_SIZE,
                              &dwBytesReturned, NULL, NULL) == FALSE) {
    throw Win32Error::GetLastWin32Error();
  }
  return reinterpret_cast<PFILE_NOTIFY_INFORMATION>(buffer_.get());
}

void FileWatcher::poll(std::function<void(const FileWatchEvent &)> handler) {
  auto notifications = pollInternal();
  do {
    FileWatchEvent event;
    event.action = notifications->Action;
    event.path = std::wstring(notifications->FileName,
                              notifications->FileNameLength / 2);
    
    handler(event);
    notifications = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(reinterpret_cast<char*>(notifications) 
      + notifications->NextEntryOffset);
  } while (notifications->NextEntryOffset != 0);
}
}
