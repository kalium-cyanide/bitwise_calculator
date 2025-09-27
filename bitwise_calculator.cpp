#include "bitwise_calculator.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>

Token::Token(Type type) : type(type), value(std::monostate())
{}

Token::Token(Type type, Value value) : type(type), value(value)
{}

Token_Stream::Token_Stream(Istream& istream) : istream(istream)
{}

bool Token_Stream::isvarallow(Char character)
{
    return std::isalpha(character) or std::isdigit(character) or character == '_';
}

Token Token_Stream::parse()
{
    Char character;
    istream >> character;

    switch (character)
    {
    case '\0':
    case 'q':
        return Token(Token::Type::QUIT);
    case ';':
        return Token(Token::Type::PRINT);
    case ':':
        return Token(Token::Type::BIN_PRINT);

    case '&':
        return Token(Token::Type::BITWISE_AND);
    case '|':
        return Token(Token::Type::BITWISE_OR);
    case '^':
        return Token(Token::Type::BITWISE_XOR);
    case '~':
        return Token(Token::Type::BITWISE_NOT);

    case '!':
        return Token(Token::Type::NOT);

    case '(':
        return Token(Token::Type::LPAR);
    case ')':
        return Token(Token::Type::RPAR);

    default:
        if (std::isdigit(character))
        {
            auto value = String();

            value += character;

            while (istream.get(character) and isvarallow(character))
            {
                if (character == '_') { continue; }
                value += character;
            }

            istream.putback(character);

            auto number = Integer();

            if (value.size() > 1 and value[1] == 'b')
            {
                number = std::stoull(std::string(value.begin() + 2, value.end()), nullptr, 2);
            }
            else if (value.size() > 1 and value[1] == 'x')
            {
                number = std::stoull(std::string(value.begin() + 2, value.end()), nullptr, 16);
            }
            else { number = std::stoull(std::string(value.begin(), value.end()), nullptr, 10); }

            return Token(Token::Type::INTEGER, number);
        }

        if (isvarallow(character))
        {
            auto value = String();

            value += character;

            while (istream.get(character) and isvarallow(character))
            {
                value += character;
            }

            istream.putback(character);

            if (value == "let") { return Token(Token::Type::LET); }
            else if (value == "const") { return Token(Token::Type::CONST); }
            else if (value == "set") { return Token(Token::Type::SET); }

            if (not std::all_of(value.begin(), value.end(), isvarallow)) { throw std::runtime_error("wrong let name"); }

            return Token(Token::Type::NAME, value);
        }

        throw std::runtime_error("bad token");
    };
}

Token Token_Stream::get()
{
    if (buffer)
    {
        auto temp = *buffer;
        buffer.reset();
        return temp;
    }

    return parse();
}

void Token_Stream::putback(Token token)
{
    if (buffer) { throw std::runtime_error("buffer isn't empty"); }
    buffer = token;
}

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

        if (not variables.contains(name)) { throw std::runtime_error("variable don'r defined"); }

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
    auto token = token_stream.get();

    if (token.type == Token::Type::LET or token.type == Token::Type::CONST)
    {
        token_stream.putback(token);
        define_variable();

        return Integer();
    }
    else if (token.type == Token::Type::SET)
    {
        set_variable();
        return Integer();
    }
    else { token_stream.putback(token); }

    return expression();
}