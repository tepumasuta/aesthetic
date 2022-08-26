#pragma once

#include <string>
#include <utility>
#include <ostream>

namespace Aesthetic
{
    namespace test
    {
        struct TestResult
        {
            bool Passed;
            const std::string& TestName;

            friend std::ostream& operator<<(std::ostream& out, const TestResult& result)
            {
                out << result.TestName << ": " << (result.Passed ? "passed" : "failed");
                return out;
            }
        };


        class Test
        {
        public:
            Test(const std::string& name) : m_Name(name) {}
            virtual ~Test(){}
            virtual TestResult Perform() const { return { false, GetName()}; }

            virtual const std::string& GetName() const { return m_Name; };
        private:
            const std::string& m_Name;
        };        
    } // namespace test
} // namespace Aesthetic

