
#include "myio.hh"

#include <array>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>

enum class Suit : unsigned char { Invalid, Spades, Diamonds, Hearts, Clubs };

enum class Face : unsigned char { Invalid, Queen, King, Ten, Ace, Nine, Jack };

int rank(Face f) noexcept {
  static std::array<int, 7> faces = {{0, 2, 3, 10, 11, 20, 30}};
  return faces[static_cast<size_t>(f)];
}

char toChar(Suit s) noexcept {
  static std::array<char, 5> suits = {{'\0', 'S', 'D', 'H', 'C'}};
  return suits[static_cast<size_t>(s)];
}

char toChar(Face f) noexcept {
  static std::array<char, 7> faces = {{'\0', 'Q', 'K', 'T', 'A', '9', 'J'}};
  return faces[static_cast<size_t>(f)];
}

struct Card {
  Suit suit;
  Face face;
  Card(Suit s, Face f) : suit(s), face(f) {}
  Card() : Card(Suit::Invalid, Face::Invalid) {}
  static Card Invalid() noexcept { return {Suit::Invalid, Face::Invalid}; }
};

struct CardHash {
  auto operator()(const Card &c) const {
    auto hash = static_cast<unsigned char>(c.suit) << (sizeof(c.suit) * 8);
    hash = hash | static_cast<unsigned char>(c.face);
    return hash;
  }
};

bool operator<(const Card &a, const Card &b) noexcept {
  return a.suit < b.suit && a.face < b.face;
}
bool operator==(const Card &a, const Card &b) noexcept {
  return a.suit == b.suit && a.face == b.face;
}
bool operator!=(const Card &a, const Card &b) noexcept { return !(a == b); }

void printcard(Card c) { printf("%c%c", toChar(c.suit), toChar(c.face)); }

std::vector<Card> read_cards(const myio::Span<char> &data) {
  auto start = std::cbegin(data);
  auto end = std::cend(data);

  auto upper = [&](char c) noexcept { return c & 0xDF; };
  auto space = [&](char c) noexcept {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
  };
  auto toFace = [&](char s) noexcept {
    if (s == '9')
      return Face::Nine;
    s = static_cast<char>(upper(s));
    switch (s) {
    case 'Q':
      return Face::Queen;
    case 'K':
      return Face::King;
    case 'T':
      return Face::Ten;
    case 'A':
      return Face::Ace;
    case 'J':
      return Face::Jack;
    default:
      return Face::Invalid;
    }
  };
  auto toSuit = [&](char s) noexcept {
    s = static_cast<char>(upper(s));
    switch (s) {
    case 'S':
      return Suit::Spades;
    case 'D':
      return Suit::Diamonds;
    case 'H':
      return Suit::Hearts;
    case 'C':
      return Suit::Clubs;
    default:
      return Suit::Invalid;
    }
  };
  auto getCard = [&] {
    if (start == end)
      return Card::Invalid();
    auto suit = toSuit(*start);
    if (++start == end || suit == Suit::Invalid)
      return Card::Invalid();
    auto face = toFace(*start);
    if (face == Face::Invalid)
      return Card::Invalid();
    start++;
    return Card{suit, face};
  };

  std::vector<Card> result;
  result.reserve(data.length());
  while (start != end) {
    // Skip Whitespace
    while (space(*start) && start != end)
      start++;
    Card c = getCard();
    if (c != Card::Invalid())
      result.push_back(c);
  }
  return result;
}

std::string toString(const Card &card) {
  return std::string() + toChar(card.suit) + toChar(card.face);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "\t%s path/to/cards/file.txt\n", *argv);
    return 1;
  }
  try {
    std::vector<char> buffer;
    if (std::string(argv[1]) == std::string("-")) {
      buffer.reserve(4096);
      myio::CFile f(stdin, false);
      myio::read_block_by_block(
          f,
          [&](myio::Span<char> s) {
            buffer.insert(std::end(buffer), std::cbegin(s), std::cend(s));
            printf("%.*s %zi\n", static_cast<int>(s.length()), s.begin(),
                   s.length());
            return 10;
          },
          20);
    } else {
      buffer = myio::read_all_bytes(argv[1]);
    }
    auto cards = read_cards(myio::Span<char>(buffer.data(), buffer.size()));
    std::unordered_map<const Card, int, CardHash> stats;
    for (const auto &c : cards) {
      stats[c]++;
    }

    printf("Card Summary\n");
    printf("#\tCard\tCount\n");
    int i = 1;
    CardHash h;
    for (const auto &p : stats) {
      printf("%i\t%s\t%i\t%x\n", i++, toString(p.first).c_str(), p.second,
             h(p.first));
    }
  } catch (std::runtime_error &e) {
    fprintf(stderr, "An error occured:\n");
    fprintf(stderr, "%s\n", e.what());
    return 2;
  }
  return 0;
}
