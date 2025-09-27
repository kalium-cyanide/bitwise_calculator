#pragma once

#include "../token/token.hpp"
#include "../token_stream/token_stream.hpp"
#include <map>

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