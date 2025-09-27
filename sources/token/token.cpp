#include "token.hpp"

Token::Token(Type type) : type(type), value(std::monostate())
{}

Token::Token(Type type, Value value) : type(type), value(value)
{}