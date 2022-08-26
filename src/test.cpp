#include <array>
#include <iostream>

#include "test/test.hpp"
#include "test/test_lexer.hpp"


using namespace Aesthetic::test;

int main()
{
    const auto lexerTests = LexerTest::LoadTests("tests");

    std::cout << "Lexer:\n";
    for (const auto& test: lexerTests)
        std::cout << "- " << test.Perform() << '\n';
    
    std::cout << std::flush;

    return 0;
}
