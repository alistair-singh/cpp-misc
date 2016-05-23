
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

template <class TStreamType>
class Stream {
  TStreamType m_front;
  TStreamType m_back;
public:
  Stream(TStreamType front, TStreamType back) : m_front(front), m_back(back) {}
  const auto current() const { return *m_front; }
  auto next() const { 
    return Stream<TStreamType>(m_front+1, m_back); 
  }
  auto end() const { return m_front == m_back; }
};

template <class T>
auto make_stream(T container) {
  auto begin = std::cbegin(container);
  auto end = std::cend(container);
  return Stream<decltype(begin)>(begin, end);
}

class StreamPos {
  size_t m_absolute;
  size_t m_line;
  size_t m_column;
public:
  StreamPos(size_t absolute) : m_absolute(absolute) {}
  size_t absolute() const { return m_absolute; }
  size_t line() const { return m_line; }
  size_t column() const { return m_column; }
};

enum class ResultType {
  Success, Error
};

template <class TStreamType, class TValue>
class Result {
  Stream<TStreamType> m_stream;
  ResultType m_type;
  union {
    TValue m_value;
    std::string m_error;
  };
  StreamPos m_postion;
public:
  Result(const Stream<TStreamType>& stream, const StreamPos& position,
         TValue& value) : m_stream(stream), m_postion(position), 
                          m_type(ResultType::Success), m_value(value) {}
  Result(const Stream<TStreamType>& stream, const StreamPos& position,
         std::string& error) : m_stream(stream), m_postion(position), 
                               m_type(ResultType::Error), m_error(error) {}
  const Stream<TStreamType>& stream() const { return m_stream; }
  const StreamPos& postion() const { return m_postion; }
  const ResultType type() const { return m_type; }
  const TValue& value() const { 
    if(m_type == ResultType::Error)
      throw std::logic_error("Invoking the Result::value() on error result.");
    return m_value; 
  }
  const std::string error() const { 
    if(m_type == ResultType::Success)
      throw std::logic_error("Invoking the Result::error() on success result.");
    return m_error; 
  }
};

template <class TStreamType, class TValue>
class Parser {
public:
  const Result<TStreamType, TValue>& parse(Stream<TStreamType> stream, 
                                           StreamPos position) const;
};

int wmain() { 
  std::wstring text = L"Hello World";
  auto stream = make_stream(text);
  while(!stream.end()) {
    std::wcout << L'\'' << stream.current() << L'\'' << std::endl;
    stream = stream.next();
  }
  return 0; 
}

