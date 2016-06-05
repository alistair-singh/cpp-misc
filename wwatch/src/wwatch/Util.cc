
#include "Util.hh"

namespace wwatch {

std::wstring concat(const std::vector<std::wstring> &args,
                    const std::wstring &seperator) {
  std::wstring result;

  auto first = true;
  for (const auto &arg : args) {
    if (first) {
      first = false;
    } else {
      result += seperator;
    }
    result += arg;
  }
  return result;
}

bool endsWith(const std::wstring &str, const std::wstring &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::wstring> split(std::wstring str, wchar_t delimiter) {
  std::vector<std::wstring> result;
  std::wstring current;
  for (auto const &c : str) {
    if (c == delimiter) {
      result.push_back(current);
      current.clear();
      continue;
    }
    current += c;
  }
  if (current != L"")
    result.push_back(current);
  return result;
}
}
