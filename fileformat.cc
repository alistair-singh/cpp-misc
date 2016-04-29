#include <cstdint>
#include <iostream>

using namespace std;

template <typename T> struct Array {
  const uint32_t size;
  const T *items;
};

enum class FieldType { String, Atom, Word, DWord, QWord };

struct String {
  const int32_t offset;
  const uint32_t size;
};

struct Field {
  const FieldType type;
  union {
    String string;
    int8_t atom;
    int16_t word;
    int32_t dword;
    int64_t qword;
  };
};

struct Record {
  const Array<Field> header;
  const Array<char> data;
};

int main() {
  cout << "fileformat" << endl;
  return 0;
}
