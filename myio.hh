
#pragma once

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <stdexcept>
#include <string>
#include <vector>

namespace myio {

template <typename T> class Span {
  const T *begin_;
  const T *end_;

public:
  Span() : begin_(nullptr), end_(nullptr) {}
  Span(const T *begin, const T *end) : begin_(begin), end_(end) {}
  Span(const T *begin, size_t length) : begin_(begin), end_(begin + length) {}

  const T *begin() const noexcept { return begin_; }
  const T *end() const noexcept { return end_; }
  size_t length() const noexcept { return end() - begin(); }
  bool empty() const noexcept { return length() == 0; }
};

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
    if (bad()) {
      throw std::runtime_error(strerror(errno));
    }
#endif
  };
  ~CFile() noexcept {
    if (!bad() && own_)
      fclose(handle_);
  }

  FILE *handle() const noexcept { return handle_; }
  bool seek(long offset, Seek origin) noexcept {
    return 0 == fseek(handle_, offset, static_cast<int>(origin));
  }
  size_t tell() const noexcept { return ftell(handle_); }
  size_t read(char *data, size_t offset, size_t length) noexcept {
    return fread(data + offset, 1, length, handle_);
  }
  size_t write(char *data, size_t offset, size_t length) noexcept {
    return fwrite(data + offset, 1, length, handle_);
  }
  bool bad() const noexcept { return handle_ == nullptr; }
  bool eof() const noexcept { return 0 != feof(handle_); }
  bool error() const noexcept { return 0 != ferror(handle_); }

  CFile &operator=(const CFile &) = delete;
  CFile(const CFile &) = delete;
};

template <typename BlockFunc>
void read_block_by_block(CFile &file, BlockFunc &&func,
                         const size_t bufferSize = 4096) {
  if (bufferSize == 0)
    return;
  if (file.bad())
    return;

  std::vector<char> buffer(bufferSize);
  bool open = true;

  auto runCallBack = [&](char *buf, size_t available) {
    auto data = Span<char>(buf, available);
    size_t processed = func(data);

    if (processed > available)
      processed = available;
    if (processed < 1)
      processed = 1;
    return processed;
  };

  auto readFile = [&](size_t offset, size_t remaining) {
    if (!open)
      return size_t(0);
    return file.read(buffer.data(), offset, remaining);
  };

  auto shiftBuffer = [&](size_t available, size_t processed) {
    auto newOffset = available - processed;
    ::memmove(buffer.data(), buffer.data() + processed, newOffset);
    return newOffset;
  };

  size_t offset = 0;
  size_t available = 0;
  do {
    auto remaining = bufferSize - offset;
    auto numRead = readFile(offset, remaining);
    available = offset + numRead;
    auto processed = runCallBack(buffer.data(), available);

    if (open && numRead != remaining && (file.eof() || file.error()))
      open = false;

    offset = shiftBuffer(available, processed);
  } while (available > 0);
}

std::vector<char> read_all_bytes(CFile &file) {
  file.seek(0L, Seek::End);
  size_t length = file.tell();
  file.seek(0L, Seek::Set);

  std::vector<char> buffer(length);
  file.read(buffer.data(), 0, length);
  return buffer;
}

std::vector<char> read_all_bytes(std::string path) {
  CFile file(path.c_str(), "rb");
  return read_all_bytes(file);
}
}
