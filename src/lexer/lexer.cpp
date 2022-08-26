#include "lexer.hpp"

namespace Aesthetic
{
    Lexer::Lexer(const std::string& program)
        : m_Program(program), m_Left(program), m_CurrentPosition(1, 1) {}
    
    Lexer::~Lexer() {}

    std::vector<TokenRef> Lexer::LexProgram()
    {
        std::vector<TokenRef> result;
        TokenRef token;

        while ((token = LexToken())->valid && (dynamic_cast<EOFToken*>(token.get()) == nullptr))
            result.push_back(token);

        result.push_back(token);

        return result;
    }

    TokenRef Lexer::LexToken()
    {
        SkipGap();

        if (auto tokenRef = FindToken<EOFToken>()) { return tokenRef.value(); }
        if (auto tokenRef = FindToken<StringToken>()) { return tokenRef.value(); }
        if (auto tokenRef = FindToken<FloatingPointToken>()) { return tokenRef.value(); }
        if (auto tokenRef = FindToken<IntegerToken>()) { return tokenRef.value(); }
        if (auto tokenRef = FindToken<OperatorToken>()) { return tokenRef.value(); }
        if (auto tokenRef = FindToken<KeywordToken>()) { return tokenRef.value(); }
        if (auto tokenRef = FindToken<PunctuationToken>()) { return tokenRef.value(); }
        if (auto tokenRef = FindToken<SymbolToken>()) { return tokenRef.value(); }

        return std::make_shared<Token>(false, m_CurrentPosition, 0UL);
    }

    template<typename T>
    requires requires(T t, const std::string_view& sv, const Position& pos)
    {
        { T::Find(sv, pos) } -> std::same_as<std::optional<std::shared_ptr<T>>>;
        t.pos;
    }
    std::optional<std::shared_ptr<T>> Lexer::FindToken()
    {
        using TRef = std::shared_ptr<T>;
        if (std::optional<TRef> tokenRef = T::Find(m_Left, m_CurrentPosition))
        {
            Position length = tokenRef.value()->length;
            m_CurrentPosition += length;

            while (length.line--)
                m_Left.remove_prefix(m_Left.find('\n'));
            m_Left.remove_prefix(length.col);

            return tokenRef;
        }
        return std::nullopt;
    }

    void Lexer::SkipGap()
    {
        bool skipped = false;
        while (!m_Left.empty() && !skipped)
        {
            skipped = true;
            if (m_Left.starts_with('\n') || m_Left.starts_with("\r\n"))
            {
                skipped = false;
                m_Left.remove_prefix(m_Left.find_first_of('\n') + 1);
                m_CurrentPosition.col = 1UL;
                m_CurrentPosition.line++;
            }

            auto start = m_Left.begin();
            m_Left.remove_prefix(std::min(m_Left.find_first_not_of(" \t"), m_Left.size()));
            size_t spaces = m_Left.begin() - start;
            m_CurrentPosition.col += spaces;
        }
    }

} // namespace Aesthetic

