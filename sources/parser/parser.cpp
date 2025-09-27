#include "parser.hpp"

Parser::Parser(Token_Stream& token_stream, std::map<String, std::pair<bool, Integer>>& variables)
    : token_stream(token_stream), variables(variables)
{}

Integer Parser::primary()
{
    auto token = token_stream.get();

    if (token.type == Token::Type::INTEGER) { return std::get<Integer>(token.value); }
    if (token.type == Token::Type::NAME)
    {
        auto name = std::get<String>(token.value);

        if (not variables.contains(name)) { throw std::runtime_error("variable don't defined"); }

        return variables.at(name).second;
    }

    if (token.type == Token::Type::NOT) { return !primary(); }
    if (token.type == Token::Type::BITWISE_NOT) { return ~primary(); }

    if (token.type == Token::Type::LPAR)
    {
        auto value = expression();

        token = token_stream.get();
        if (token.type != Token::Type::RPAR) { throw std::runtime_error("')' excepted"); }

        return value;
    }

    throw std::runtime_error("primary excepted");
}

Integer Parser::term()
{
    auto left = primary();
    auto token = token_stream.get();

    if (token.type == Token::Type::BITWISE_AND) { return left & term(); }

    token_stream.putback(token);
    return left;
}

Integer Parser::subterm()
{
    auto left = term();
    auto token = token_stream.get();

    if (token.type == Token::Type::BITWISE_XOR) { return left ^ subterm(); }

    token_stream.putback(token);
    return left;
}

Integer Parser::expression()
{
    auto left = subterm();
    auto token = token_stream.get();

    if (token.type == Token::Type::BITWISE_OR) { return left | expression(); }

    token_stream.putback(token);
    return left;
}

void Parser::define_variable()
{
    auto token = token_stream.get();
    auto is_mutable = token.type == Token::Type::LET;

    token = token_stream.get();
    if (token.type != Token::Type::NAME) { throw std::runtime_error("name of variable excepted"); }

    auto name = std::get<String>(token.value);

    if (variables.contains(name)) { throw std::runtime_error("variable is already defined"); }

    token = token_stream.get();

    if (not is_mutable and token.type != Token::Type::INTEGER) { throw std::runtime_error("excepted value for const"); }

    variables.insert({name, {is_mutable, std::get<Integer>(token.value)}});
}

void Parser::set_variable()
{
    auto token = token_stream.get();

    if (token.type != Token::Type::NAME) { throw std::runtime_error("name of variable excepted"); }

    auto name = std::get<String>(token.value);

    token = token_stream.get();

    if (token.type != Token::Type::INTEGER) { throw std::runtime_error("value of variable excepted"); }

    if (not variables[name].first) { throw std::runtime_error("can't set const value"); }

    variables[name] = {true, std::get<Integer>(token.value)};
}

Integer Parser::parse()
{

    while (true)
    {
        auto token = token_stream.get();
        if (token.type == Token::Type::LET or token.type == Token::Type::CONST)
        {
            token_stream.putback(token);
            define_variable();
        }
        else if (token.type == Token::Type::SET) { set_variable(); }
        else
        {
            token_stream.putback(token);
            break;
        }
    }
    return expression();
}