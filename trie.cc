
#include <string>
#include <vector>

struct Row {
  static const int Size = 27;
  char keys[Size];
  unsigned maps[Size];

  Row() {
    for(auto i = 0; i < Size; i++)
    {
      keys[i] = static_cast<char>(static_cast<int>('a') + i);
      maps[i] = 0;
    }
    keys[Size-1] = '*';
  }
};

using Trie = std::vector<Row>;

void add_word(Trie &t, const std::string &word) {
  for(const auto& c : word) {
    Row r;
    r.entries[0].key = c;
    t.push_back(r);
  }
}

int main() {
  std::vector<std::string> words;
  Trie data;
  add_word(data, "test");
  add_word(data, "hello");
  add_word(data, "how");
  add_word(data, "alistair");
  return 0;
}
