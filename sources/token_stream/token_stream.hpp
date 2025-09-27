#pragma once

#include "../token/token.hpp"
#include <istream>
#include <optional>

using Istream = std::istream;

class Token_Stream
{
  private:
    Istream& istream;
    std::optional<Token> buffer;

    static inline bool isvarallow(Char character);
    Token parse();

  public:
    Token_Stream(Istream& istream);
    Token get();
    void putback(Token token);
};
