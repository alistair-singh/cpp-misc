
#include <iostream>
#include <cstdint>
#include <algorithm>

typedef uint64_t PageId;

class Header {
  PageId id_;
  PageId nid_;
  PageId pid_;
  uint32_t rows_;
  // char signature[34];

public:
  Header(PageId id, PageId nid, PageId pid, unsigned int rows)
      : id_(id), nid_(nid), pid_(pid), rows_(rows) {}

  PageId id() const { return id_; }
  PageId nid() const { return nid_; }
  PageId pid() const { return pid_; }
  unsigned int rows() const { return rows_; }
};

template <size_t S> class VariableSizePage {
  Header header_;
  char data_[S - sizeof(Header)];

public:
  VariableSizePage(Header header, const char *data, size_t offset, size_t count)
      : header_(std::move(header)) {
    // TODO: Size check
    std::copy_n(data + offset, count, &data_);
  }

  static const size_t Size = S;
  Header header() const;
  char const *data() const;
};

typedef VariableSizePage<4096> Page;

int main() {
  std::cout << "size of header " << sizeof(Header) << std::endl;
  std::cout << "size of page " << sizeof(Page) << std::endl;
  return 0;
}
