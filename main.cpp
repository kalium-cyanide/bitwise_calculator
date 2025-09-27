#include "bitwise_calculator.hpp"
#include <vector>

int main()
try
{
    auto bin = [](Integer value)
    {
        auto result = String();
        while (value > 0)
        {
            result = Char((value % 2) + Ch_Zero) + result;
            value /= 2;
        };
        return result;
    };

    auto token_stream = Token_Stream(std::cin);
    auto variables = std::map<String, std::pair<bool, Integer>>();

    auto parser = Parser(token_stream, variables);

    auto value = Integer();

    auto buffer = std::vector<Token>();

    while (std::cin)
    {
        try
        {
            auto token = token_stream.get();

            if (token.type == Token::Type::QUIT) { break; }
            if (token.type == Token::Type::PRINT) { std::cout << "= " << value << '\n'; }
            else if (token.type == Token::Type::BIN_PRINT) { std::cout << "= " << bin(value) << '\n'; }
            else
            {
                token_stream.putback(token);
                value = parser.parse();
            }
        }
        catch (std::exception& while_error)
        {
            std::cout << while_error.what() << '\n';
        }
    }
}
catch (std::exception& e)
{
    std::cerr << "error: " << e.what() << '\n';
    return 1;
}
catch (...)
{
    std::cerr << "Oops: unknown exception!\n";
    return 2;
}
