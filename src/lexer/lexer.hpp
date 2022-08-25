#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <optional>
#include <memory>
#include <concepts>

#include "token.hpp"


namespace Aesthetic
{
    class Lexer
    {
    private:
        std::string m_Program;
        std::string_view m_Left;
        Position m_CurrentPosition;
    public:
        Lexer(const std::string& program);
        ~Lexer();

        std::vector<TokenRef> LexProgram();
    private:
        template<typename T>
        requires requires(T t, const std::string_view& sv, const Position& pos)
        {
            { T::Find(sv, pos) } -> std::same_as<std::optional<std::shared_ptr<T>>>;
            t.pos;
        }
        std::optional<std::shared_ptr<T>> FindToken();
        TokenRef LexToken();
        void SkipGap();
    };
} // namespace Aesthetic
