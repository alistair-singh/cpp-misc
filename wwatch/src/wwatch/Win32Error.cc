
#include "Win32Error.hh"

namespace wwatch {

Win32Error Win32Error::GetLastWin32Error() {
  unsigned code = ::GetLastError();
	return GetLastWin32Error(code);
}

Win32Error Win32Error::GetLastWin32Error(unsigned code) {
	const unsigned reservedSize = 512;
	Win32Error error;
	error.code_ = code;
	error.message_.resize(reservedSize);
	auto actualSize = ::FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, error.code_,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		const_cast<wchar_t *>(error.message_.data()), reservedSize, NULL);
	error.message_.resize(actualSize);
	return error;
}
}
