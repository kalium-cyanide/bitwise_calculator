#include "token_stream.hpp"

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
    case 'q':
        return Token(Token::Type::QUIT);
    case '\0':
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