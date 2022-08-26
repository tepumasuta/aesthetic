#include <sstream>
#include <iostream>

#include "test_lexer.hpp"
#include "lexer/lexer.hpp"

namespace Aesthetic
{
    namespace test
    {
        LexerTest::LexerTest(const std::string& testName, const std::string& sourcePath, const std::string& resultPath)
            : Test(testName),
              m_SourceContents(Test::ReadFileInto(sourcePath)),
              m_ExpectedResult(Test::ReadFileInto(resultPath)) {}
        
        LexerTest::~LexerTest() {}
        
        TestResult LexerTest::Perform() const
        {
            Lexer lexer(m_SourceContents);
            const auto lexedProgram = lexer.LexProgram();
            std::stringstream stream;
            for (const auto& token: lexedProgram)
                stream << *token << '\n';

            return {
                stream.str() == m_ExpectedResult,
                GetName(),
                stream.str(),
                m_ExpectedResult
            };
        }
        
        const std::string& LexerTest::GetName() const
        {
            return m_Name;
        }
        
        std::array<LexerTest, 1> LexerTest::LoadTests(const std::string& testDirectory)
        {
            const std::string prefix = testDirectory + "/lexer/"s;
            std::array<LexerTest, 1> entries = {
                LexerTest("Lexer Operators"s, prefix + "01-operators.ae", prefix + "01-operators.txt"),
            };
            return entries;
        }
    } // namespace test
} // namespace Aesthetic

