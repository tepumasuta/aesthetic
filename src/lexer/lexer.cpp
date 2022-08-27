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

        for (token = LexFullToken(); dynamic_cast<EOFToken*>(token.get()) == nullptr; token = LexFullToken())
        {
            StepPosition(token->length);
            result.push_back(token);
        }
        StepPosition(token->length);
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

    TokenRef Lexer::LexFullToken()
    {
        TokenRef token = LexToken();
        if (token->length != Position(0, 0) || token->valid)
            return token;

        const char* start = &m_Left[0];
        const std::string_view previous = m_Left;

        while (token->length == Position(0, 0) && !token->valid)
        {
            m_Left.remove_prefix(1);
            token = LexToken();
        }

        token = std::make_shared<UnknownToken>(
            m_CurrentPosition,
            std::string_view(start, m_Left.begin())
        );

        m_Left = previous;

        return token;
    }

    template<typename T>
    requires requires(T t, const std::string_view& sv, const Position& pos)
    {
        { T::Find(sv, pos) } -> std::same_as<std::optional<std::shared_ptr<T>>>;
        t.pos;
    }
    std::optional<std::shared_ptr<T>> Lexer::FindToken()
    {
        return T::Find(m_Left, m_CurrentPosition);
    }

    void Lexer::StepPosition(Position length)
    {
        m_CurrentPosition += length;

        while (length.line--)
            m_Left.remove_prefix(m_Left.find('\n'));
        m_Left.remove_prefix(length.col);
    }

    void Lexer::SkipGap()
    {
        auto start = m_Left.begin();
        m_Left.remove_prefix(std::min(m_Left.find_first_not_of(" \t"), m_Left.size()));
        if (m_Left.starts_with("///"))
            m_Left.remove_prefix(std::min(m_Left.find_first_of('\n'), m_Left.size()));
        size_t spaces = m_Left.begin() - start;
        m_CurrentPosition.col += spaces;
    }

} // namespace Aesthetic

