#include <array>
#include <iostream>

#include "test/test.hpp"


using namespace Aesthetic::test;

int main()
{
    const std::array<Test, 0UL> tests = {};

    for (const auto& test: tests)
        std::cout << test.Perform() << '\n';

    return 0;
}
