
#pragma once

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <stdexcept>
#include <string>
#include <vector>

namespace myio {

enum class Seek { Current = SEEK_CUR, Set = SEEK_SET, End = SEEK_END };

class CFile {
  FILE *handle_;
  bool own_;

public:
  CFile() : handle_(nullptr){};
  CFile(FILE *handle) : handle_(handle), own_(true){};
  CFile(FILE *handle, bool own) : handle_(handle), own_(own){};
  CFile(const char *path, const char *mode) : own_(true) {
    handle_ = nullptr;
#ifdef _WIN32
    errno_t error = fopen_s(&handle_, path, mode);
    if (error != 0) {
      const int bufferSize = 512;
      char buffer[bufferSize];
      ::memset(buffer, 0, bufferSize);
      strerror_s(buffer, bufferSize, error);
      throw std::runtime_error(buffer);
    }
#else
    handle_ = fopen(path, mode);
    if (!handle_) {
      throw std::runtime_error(strerror(errno));
    }
#endif
  };
  ~CFile() {
    if (!handle_ && own_)
      fclose(handle_);
  }

  FILE *handle() const noexcept { return handle_; }
  bool seek(long offset, Seek origin) noexcept {
    return 0 == fseek(handle_, offset, static_cast<int>(origin));
  }
  size_t tell() const noexcept { return ftell(handle_); }
  size_t read(char *data, int offset, int length) noexcept {
    return fread(data + offset, 1, length, handle_);
  }
  size_t write(char *data, int offset, int length) noexcept {
    return fwrite(data + offset, 1, length, handle_);
  }
};

std::vector<char> read_all_bytes(std::string path) {
  CFile file(path.c_str(), "rb");
  file.seek(0L, Seek::End);
  size_t length = file.tell();
  file.seek(0L, Seek::Set);

  std::vector<char> buffer(length);
  file.read(buffer.data(), 0, (int)length);
  return buffer;
}
}
