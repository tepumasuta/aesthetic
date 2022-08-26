#pragma once

#include <ostream>
#include <cstddef>
#include <string_view>
#include <optional>
#include <memory>
#include <array>
#include <string>

namespace Aesthetic
{
    using namespace std::string_literals;

    enum class NumberLiteralType : size_t
    {
        HEX = 0UL,
        OCT = 1UL,
        BIN = 2UL,
        DEC = 3UL,
    };

    enum class OperationType : size_t
    {
        ADDITION         = 0UL,
        SUBSTRACTION     = 1UL,
        MULTIPLICATION   = 2UL,
        FLOAT_DIVISION   = 3UL,
        INTEGER_DIVISION = 4UL,
        PLAIN_COPY       = 5UL,
        DEFINE_BINDING   = 6UL,
        BOOSTY_BINDING   = 7UL,
        COMMENT_LINE     = 8UL,
        PIPE             = 9UL,
        DELETE           = 10UL,
        RECURSION_DELETE = 11UL,
    };

    enum class KeywordType : size_t
    {
        IF       = 0UL,
        WHEN     = 1UL,
        WHENEVER = 2UL,
        EXIST    = 3UL,
        ON       = 4UL,
    };

    enum class PunctuationType : size_t
    {
        SEMICOLON         = 0UL,
        SCOPE_OPEN        = 1UL,
        SCOPE_CLOSE       = 2UL,
        PARENTHESES_OPEN  = 3UL,
        PARENTHESES_CLOSE = 4UL,
        BRACKET_OPEN      = 5UL,
        BRACKET_CLOSE     = 6UL,
        COMMA             = 7UL,
        DOT               = 8UL,
        LINE_END          = 9UL,
        COLON             = 10UL,
    };

    struct Position
    {
        size_t line;
        size_t col;

        Position(size_t line, size_t column);
        Position(std::string_view sv);
        friend std::ostream& operator<<(std::ostream& out, const Position& pos);

        Position operator+(const Position& other);
        Position& operator+=(const Position& other);
    };

    struct BasicToken
    {
        bool valid;
        Position pos;
        Position length;

        BasicToken(bool valid, Position pos, Position length);
        BasicToken(bool valid, Position pos, size_t length);

        friend std::ostream& operator<<(std::ostream& out, const BasicToken& token);
    protected:
        virtual void CommonString(std::ostream& out) const;
    private:
        virtual std::string ToString() const;
    };

    struct EndOfFileToken : public BasicToken
    {
        EndOfFileToken(Position pos);

        static std::optional<std::shared_ptr<EndOfFileToken>> Find(const std::string_view& text, const Position& pos);
    private:
        std::string ToString() const;
    };

    struct OperatorToken : public BasicToken
    {
        static const std::array<std::string, 12UL> representations;

        OperationType type;

        OperatorToken(Position pos, OperationType type);

        size_t OperationTypeToLength(OperationType type);
        static std::optional<std::shared_ptr<OperatorToken>> Find(const std::string_view& text, const Position& pos);
    private:
        std::string ToString() const;
    };
    
    struct KeywordToken : public BasicToken
    {
        static const std::array<std::string, 5UL> representations;

        KeywordType type;

        KeywordToken(Position pos, KeywordType type);

        size_t KeywordTypeToLength(KeywordType type);
        static std::optional<std::shared_ptr<KeywordToken>> Find(const std::string_view& text, const Position& pos);
    private:
        std::string ToString() const;
    };
    
    struct PunctuationToken : public BasicToken
    {
        static const std::array<std::string, 11UL> representations;

        PunctuationType type;

        PunctuationToken(Position pos, PunctuationType type);

        size_t PunctuationTypeToLength(PunctuationType type);
        static std::optional<std::shared_ptr<PunctuationToken>> Find(const std::string_view& text, const Position& pos);
    private:
        std::string ToString() const;
    };
    
    struct SymbolToken : public BasicToken
    {
        std::string_view contents;

        SymbolToken(Position pos, std::string_view contents);

        static bool StartSymbolic(const char& sym);
        static bool Symbolic(const char& sym);
        static std::optional<std::shared_ptr<SymbolToken>> Find(const std::string_view& text, const Position& pos);
    };
    
    struct ValueToken : public BasicToken
    {
        std::string_view contents;

        ValueToken(bool valid, Position pos, std::string_view contents);
    };

    struct StringToken : public ValueToken
    {
        size_t stringLength;

        StringToken(bool valid, Position pos, std::string_view contents, size_t length);

        static bool StringBound(const char& sym);
        static std::optional<std::shared_ptr<StringToken>> Find(const std::string_view& text, const Position& pos);
    };
    
    struct NumberToken : public ValueToken
    {
        size_t digitsCount;
        NumberLiteralType type;

        static bool IsDigit(const char& sym, NumberLiteralType type);

        static std::optional<NumberLiteralType> FindPrefix(const std::string_view& text);

        NumberToken(bool valid, Position pos, std::string_view contents, NumberLiteralType type);
    };
    
    struct FloatingPointToken : public NumberToken
    {
        FloatingPointToken(bool valid, Position pos, std::string_view contents, NumberLiteralType type);

        static std::optional<std::shared_ptr<FloatingPointToken>> Find(const std::string_view& text, const Position& pos);
    };
    
    struct IntegerToken : public NumberToken
    {
        IntegerToken(bool valid, Position pos, std::string_view contents, NumberLiteralType type);

        static std::optional<std::shared_ptr<IntegerToken>> Find(const std::string_view& text, const Position& pos);
    };

    using Token = BasicToken;
    using TokenRef = std::shared_ptr<Token>;
    using EOFToken = EndOfFileToken;
    using EOFTokenRef = std::shared_ptr<EndOfFileToken>;
    using OperatorTokenRef = std::shared_ptr<OperatorToken>;
    using ValueTokenRef = std::shared_ptr<ValueToken>;
    using KeywordTokenRef = std::shared_ptr<KeywordToken>;
    using SymbolTokenRef = std::shared_ptr<SymbolToken>;
    using PunctuationTokenRef = std::shared_ptr<PunctuationToken>;
    using StringTokenRef = std::shared_ptr<StringToken>;
    using NumberTokenRef = std::shared_ptr<NumberToken>;
    using FloatingPointTokenRef = std::shared_ptr<FloatingPointToken>;
    using IntegerTokenRef = std::shared_ptr<IntegerToken>;
} // namespace Aesthetic
