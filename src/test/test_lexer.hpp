#pragma once

#include <array>

#include "test.hpp"

namespace Aesthetic
{
    namespace test
    {
        class LexerTest : public Test
        {
        public:
            LexerTest(const std::string& testName, const std::string& sourcePath, const std::string& resultPath);
            ~LexerTest();

            TestResult Perform() const;
            const std::string& GetName() const;

            static std::array<LexerTest, 1> LoadTests(const std::string& testDirectory);
        private:
            std::string m_SourceContents;
            std::string m_ExpectedResult;
        };
    } // namespace test
} // namespace Aesthetic
