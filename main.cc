
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <gsl.h>
#include <array_view.h>

typedef uint64_t PageId;

class Header {
  PageId id_;
  PageId nid_;
  PageId pid_;
  uint32_t rows_;
  char signature_[34];

public:
  Header(PageId id, PageId nid, PageId pid, unsigned int rows)
      : id_(id), nid_(nid), pid_(pid), rows_(rows) {}

  PageId id() const { return id_; }
  PageId nid() const { return nid_; }
  PageId pid() const { return pid_; }
  uint32_t rows() const { return rows_; }
  const char *signature() const { return signature_; }
};

template <size_t S> class VariableSizePage {
  Header header_;
  std::array<const char, S - sizeof(Header)> data_;

public:
  VariableSizePage(Header header, array_view<char> array)
      : header_(std::move(header)) {
    // TODO: Size check
    // std::copy_n(begin(array), array.size(), data_)
  }

  Header header() const { return header_; }
  char const *data() const { return data_.data(); }
};

typedef VariableSizePage<8192> Page;

struct best {
  const int fst;
  const int snd;
  const char text[100];

  std::string toString() const {
    std::ostringstream stream;
    stream << text << " (" << fst << ", " << snd << ")";
    return stream.str();
  }
};

int main() {
  best b = {0, 2, "hello"};
  std::cout << b.toString() << '\n';
  std::cout << "size of header " << sizeof(Header) << '\n';
  std::cout << "size of page " << sizeof(Page) << '\n';
  return 0;
}
