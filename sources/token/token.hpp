#pragma once

#include <algorithm>
#include <stdint.h>
#include <string>
#include <variant>

using Integer = uint32_t;
using Char = char;
using String = std::string;
using Value = std::variant<Integer, String, std::monostate>;

class Token
{
  public:
    enum class Type
    {
        QUIT,
        PRINT,
        BIN_PRINT,
        LET,
        CONST,
        SET,
        NAME,
        INTEGER,
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_NOT,
        NOT,
        LPAR,
        RPAR,
    };

    Token(Type type);
    Token(Type type, Value value);

    Type type;
    Value value;
};