#pragma once

#include <cstdint>
#include <iostream>
#include <istream>
#include <map>
#include <optional>
#include <string>
#include <variant>

using Integer = uint32_t;
using Char = char;
using String = std::string;
using Istream = std::istream;
constexpr auto Ch_Zero = '0';

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

class Parser
{
  private:
    Integer primary();
    Integer term();
    Integer subterm();
    Integer expression();
    void define_variable();
    void set_variable();

    Token_Stream& token_stream;
    std::map<String, std::pair<bool, Integer>>& variables;

  public:
    Parser(Token_Stream& token_stream, std::map<String, std::pair<bool, Integer>>& variables);
    Integer parse();
};