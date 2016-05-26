
#include <string>
#include <vector>

namespace wwatch {

std::wstring concat(const std::vector<std::wstring> &args);
bool endsWith(const std::wstring &str, const std::wstring &suffix);
std::vector<std::wstring> split(std::wstring str, wchar_t delimiter);
}
