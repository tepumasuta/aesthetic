#pragma once

#include <fstream>
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
            virtual TestResult Perform() const { return { false, GetName() }; }

            virtual const std::string& GetName() const { return m_Name; };

            static std::string ReadFileInto(const std::string& filename)
            {
                std::ifstream file(filename, std::ios::ate);
                uint64_t sourceSize = file.tellg();
                std::string result(sourceSize, '\0');
                file.seekg(0);
                file.read(&result[0], sourceSize);
                return result;
            }
        protected:
            std::string m_Name;
        };        
    } // namespace test
} // namespace Aesthetic

