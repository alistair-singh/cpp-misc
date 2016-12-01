
#include <iostream>
#include <stdexcept>
#include <string>

template <class TIterator> class Stream {
  TIterator m_front;
  TIterator m_back;

public:
  explicit Stream(const TIterator &front, const TIterator &back)
      : m_front(front), m_back(back) {}
  const auto &current() const noexcept { return *m_front; }
  auto next() const { return Stream<TIterator>(std::next(m_front), m_back); }
  auto end() const noexcept { return m_front == m_back; }
};

template <typename T> auto make_stream(const T &iterable) {
  return Stream<T::const_iterator>(std::cbegin(iterable), std::cend(iterable));
}

class Error {
  int m_line;
  int m_column;
  std::string m_message;

public:
  Error(int line, int column, const std::string &message)
      : m_line(line), m_column(column), m_message(message) {}

  auto line() const noexcept { return m_line; }
  auto column() const noexcept { return m_column; }
  auto message() const noexcept { return &m_message; }
  std::string formattedMessage() const {
    return "(" + std::to_string(m_line) + ", " + std::to_string(m_column) +
           "): " + m_message;
  }
};

template <class TItem, class TStream> class Result {
  bool m_hasError;
  union {
    TItem m_item;
    Error m_error;
  };
  TStream m_stream;

public:
  explicit Result(const TItem &item, const TStream &stream)
      : m_item(item), m_stream(stream), m_hasError(false) {}
  explicit Result(const Error &error, const TStream &stream)
      : m_error(error), m_stream(stream), m_hasError(true) {}
  Result(const Result<TItem, TStream> &result)
      : m_stream(result.m_stream), m_hasError(result.m_hasError) {
    if (m_hasError) {
      m_error = result.m_error;
    } else {
      m_item = result.m_item;
    }
  }
  ~Result() {
    if (m_hasError) {
      m_error.~Error();
    } else {
      m_item.~TItem();
    }
    m_stream.~TStream();
  }
  auto item() const {
    if (m_hasError)
      throw std::runtime_error(
          "Result::item() called on a result with error: " +
          m_error.formattedMessage());
    return m_item;
  }
  auto hasError() const noexcept { return m_hasError; }
  auto error() const {
    if (!m_hasError)
      throw std::runtime_error(
          "Result::error() called on a result without error.");
    return m_error;
  }
};

template <class TStream> class EosParser {
public:
  Result<char, TStream> RunParser(const TStream &stream) {
    if (!stream.end()) {
      Error error{0, 0, "End of stream expected."};
      return Result<char, TStream>(error, stream);
    }
    return Result<char, TStream>(0, stream);
  }
};

template <class TItem, class TStream> class ItemParser {
public:
  Result<TItem, TStream> RunParser(const TStream &stream) {
    if (stream.end()) {
      Error error{0, 0, "End of stream."};
      return Result<TItem, TStream>(error, stream);
    }
    return Result<TItem, TStream>(stream.current(), stream.next());
  }
};

template <class TItem, class TStream, class TParserLeft, class TParserRight>
class OrParser {
  TParserLeft m_left;
  TParserRight m_right;

public:
  OrParser(TParserLeft left, TParserRight right)
      : m_left(left), m_right(right){};
  Result<TItem, TStream> RunParser(const TStream &stream) {
    auto leftResult = m_left.RunParser(stream);
    if (!leftResult.hasError())
      return leftResult;
    return m_right.RunParser(stream);
  }
};

int main() {
  std::string source = "hello world";
  auto stream = make_stream(source);
  while (!stream.end()) {
    auto c = stream.current();
    std::cout << '\'' << c << '\'' << std::endl;
    stream = stream.next();
  }
  Error error(1, 2, "err");
  std::cout << error.formattedMessage() << std::endl;
  Result<char, Stream<std::string::const_iterator>> resErr(error, stream);

  try {
    resErr.error();
    resErr.item();
  } catch (std::runtime_error &error) {
    std::cerr << error.what() << std::endl;
  }
  Result<char, Stream<std::string::const_iterator>> res1('a', stream);
  try {
    if ('a' == res1.item())
      std::wcout << "equal a" << std::endl;
    res1.error();
  } catch (std::runtime_error &error) {
    std::cerr << error.what() << std::endl;
  }

  auto stream2 = make_stream(source);
  ItemParser<char, Stream<std::string::const_iterator>> parser1;
  auto res2 = parser1.RunParser(stream2);
  std::cout << res2.item() << std::endl;

  EosParser<Stream<std::string::const_iterator>> parser2;
  auto res3 = parser2.RunParser(stream);
  std::cout << res3.item() << std::endl;

  OrParser<char, Stream<std::string::const_iterator>,
           ItemParser<char, Stream<std::string::const_iterator>>,
           EosParser<Stream<std::string::const_iterator>>>
      parser3(parser1, parser2);
  parser3.RunParser(stream2);
  return 0;
}
