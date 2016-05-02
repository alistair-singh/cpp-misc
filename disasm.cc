
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

struct print_options {
  int columns = 80;
  int lines = 1;
  std::string seperator = " ";
  bool upper_case = true;
};

std::string print_hex(unsigned char const *begin, unsigned char const *end,
                      print_options const &options) {
  std::ostringstream stream;
  auto length = std::distance(begin, end);

  if (length < 1)
    return std::string();

  auto sepLen = options.seperator.length();
  size_t numPerLine = options.columns / (2 + sepLen);

  if (options.upper_case)
    stream << std::uppercase;

  int lines = 0;
  for (auto const *cur = begin; cur < end; ++cur) {
    stream << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<unsigned>(*cur);

    size_t position = (cur - begin);
    if ((position + 1) % numPerLine == 0 || cur + 1 == end) {
      lines++;
      stream << std::endl;
      if (lines == options.lines) {
        auto rem = static_cast<int>(std::ceil(length * 1.0 / numPerLine)) -
                   options.lines;
        if (rem == 0)
          break;
        stream << "... " << rem << " more" << std::endl;
        break;
      }
    } else
      stream << options.seperator;
  }

  return stream.str();
}

int main() {
  std::cout << __FILE__ << std::endl;

  auto data = (unsigned char const *)(&print_hex);
  auto body = print_hex(data, data + 120, print_options{});
  std::cout << (void*)(&print_hex) << std::endl;
  std::cout << body << std::endl;
  std::cout << body.length() << std::endl;
  return 0;
}
