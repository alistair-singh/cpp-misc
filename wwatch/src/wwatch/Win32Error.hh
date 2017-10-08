
#pragma once

#include <string>
#include <windows.h>

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
  static Win32Error GetLastWin32Error();
	static Win32Error GetLastWin32Error(unsigned code);
};
}
