#include <array>
#include <functional>
#include <algorithm>
#include <cctype>
#include <sstream>

#include "token.hpp"

namespace Aesthetic
{
    template<typename T, typename U>
    requires requires(T t, U u)
    {
        T::representations;
        t.type;
        static_cast<size_t>(u);
    }
    size_t TTypeToLength(U type)
    {
        return T::representations.at(static_cast<size_t>(type)).length();
    }

    template<typename T, typename U>
    requires requires(T t, U u)
    {
        T::representations;
    }
    std::optional<std::shared_ptr<T>> FindHardToken(
        const std::string_view& text,
        const Position& pos,
        std::function<bool (const std::string_view&, const std::string&)>&& check
    )
    {
        std::array tmp = T::representations;
        std::sort(tmp.begin(), tmp.end(), std::greater<const std::string&>());

        for (auto& start: tmp)
            if (check(text, start))
                return std::make_shared<T>(
                    pos,
                    static_cast<U>(std::find(
                            T::representations.begin(),
                            T::representations.end(),
                            start
                        ) - T::representations.begin()
                    )
                );

        return std::nullopt;
    }


    Position::Position(size_t line, size_t column)
         : line(line), col(column) {}

    Position::Position(std::string_view contents)
        : line(std::ranges::count(contents, '\n')),
          col(contents.size() - std::max(contents.rfind('\n'), contents.rfind("\r\n"))) {}

    Position Position::operator+(const Position& other)
    {
        if (!other.line)
            return Position(line, col + other.col);

        return Position(line + other.line, other.col);
    }

    Position& Position::operator+=(const Position& other)
    {
        line += other.line;
        col = other.line ? other.col : col + other.col;
        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const Position& pos)
    {
        out << pos.line << ':' << pos.col;
        return out;
    }


    BasicToken::BasicToken(bool valid, Position pos, size_t length)
        : valid(valid), pos(pos), length(0, length) {}

    BasicToken::BasicToken(bool valid, Position pos, Position length)
        : valid(valid), pos(pos), length(length) {}
    
    void BasicToken::CommonString(std::ostream& out) const
    {
        out << "[" << pos << "-" << length << "]" << "(" << (valid ? "" : "in") << "valid)";
    }

    std::string BasicToken::ToString() const
    {
        std::stringstream stream;
        stream << "BasicToken";
        CommonString(stream);
        return stream.str();
    }
        
    std::ostream& operator<<(std::ostream& out, const BasicToken& token)
    {
        out << token.ToString();
        return out;
    }


    EndOfFileToken::EndOfFileToken(Position pos)
        : BasicToken(true, pos, 0) {}

    std::optional<EOFTokenRef> EndOfFileToken::Find(const std::string_view& text, const Position& pos)
    {
        if (text.empty())
            return std::make_shared<EndOfFileToken>(pos);
        
        return std::nullopt;
    }

    std::string EndOfFileToken::ToString() const
    {
        std::stringstream stream;
        stream << "EOFToken";
        CommonString(stream);
        return stream.str();
    }


    const std::array<std::string, 12UL> OperatorToken::representations  = {
        "+"s, "-"s, "*"s, "/"s, "//"s, "="s, "::="s, ":="s, "///"s, "~>"s, "!!"s, "~!"s
    };

    OperatorToken::OperatorToken(Position pos, OperationType type)
        : BasicToken(true, pos, OperationTypeToLength(type)), type(type) {}

    size_t OperatorToken::OperationTypeToLength(OperationType type)
    {
        return TTypeToLength<OperatorToken, OperationType>(type);
    }

    std::optional<OperatorTokenRef> OperatorToken::Find(const std::string_view& text, const Position& pos)
    {
        return FindHardToken<OperatorToken, OperationType>(text, pos,
            [](const std::string_view& text, const std::string& start) -> bool {
                return text.starts_with(start);
            }
        );
    }

    std::string OperatorToken::ToString() const
    {
        std::stringstream stream;
        stream << "OperatorToken";
        CommonString(stream);
        stream << " `" << representations[static_cast<size_t>(type)] << '`';
        return stream.str();
    }


    const std::array<std::string, 5UL> KeywordToken::representations = {
        "if", "when", "whenever", "exist", "on"
    };

    KeywordToken::KeywordToken(Position pos, KeywordType type)
        : BasicToken(true, pos, KeywordTypeToLength(type)), type(type) {}

    size_t KeywordToken::KeywordTypeToLength(KeywordType type)
    {
        return TTypeToLength<KeywordToken, KeywordType>(type);
    }

    std::optional<KeywordTokenRef> KeywordToken::Find(const std::string_view& text, const Position& pos)
    {
        return FindHardToken<KeywordToken, KeywordType>(text, pos,
            [](const std::string_view& text, const std::string& start) -> bool {
                return text.starts_with(start) && (
                    text.size() == start.size()
                    || !SymbolToken::Symbolic(text.at(start.size()))
                );
            }
        );
    }

    std::string KeywordToken::ToString() const
    {
        std::stringstream stream;
        stream << "KeywordToken";
        CommonString(stream);
        stream << " `" << representations[static_cast<size_t>(type)] << '`';
        return stream.str();
    }


    const std::array<std::string, 11UL> PunctuationToken::representations = {
        ";"s, "{"s, "}"s, "("s, ")"s, "["s, "]"s, ","s, "."s, "\n"s, ":"s
    };

    PunctuationToken::PunctuationToken(Position pos, PunctuationType type)
        : BasicToken(true, pos, Position(
            type == PunctuationType::LINE_END,
            PunctuationTypeToLength(type)
        )), type(type) {}

    size_t PunctuationToken::PunctuationTypeToLength(PunctuationType type)
    {
        return TTypeToLength<PunctuationToken, PunctuationType>(type);
    }

    std::optional<PunctuationTokenRef> PunctuationToken::Find(const std::string_view& text, const Position& pos)
    {
        return FindHardToken<PunctuationToken, PunctuationType>(text, pos,
            [](const std::string_view& text, const std::string& start) {
                if (start == "."s)
                    return text.starts_with('.')
                        && (text.length() == 1 || !NumberToken::IsDigit(text[1], NumberLiteralType::DEC));
                return text.starts_with(start);
            }
        );
    }
    
    std::string PunctuationToken::ToString() const
    {
        std::stringstream stream;
        stream << "PunctuationToken";
        CommonString(stream);
        const std::string& value = representations[static_cast<size_t>(type)];
        stream << " `" << (value == "\n"s ? "\\n" : value) << '`';
        return stream.str();
    }


    SymbolToken::SymbolToken(Position pos, std::string_view contents)
        : BasicToken(true, pos, contents), contents(contents) {}

    bool SymbolToken::StartSymbolic(const char& sym)
    {
        return std::isalpha(sym) || sym == '_';
    }

    bool SymbolToken::Symbolic(const char& sym)
    {
        return std::isalnum(sym) || sym == '_';
    }

    std::optional<SymbolTokenRef> SymbolToken::Find(const std::string_view& text, const Position& pos)
    {
        if (text.empty() || !text.front())
            return std::nullopt;
        
        auto x = std::find_if_not(text.begin(), text.end(), Symbolic);

        if (x != text.end() && x != text.begin())
            return std::make_shared<SymbolToken>(
                pos, std::string_view(text.begin(), x)
            );
        
        return std::nullopt;
    }

    std::string SymbolToken::ToString() const
    {
        std::stringstream stream;
        stream << "SymbolToken";
        CommonString(stream);
        stream << " `" << contents << '`';
        return stream.str();
    }


    ValueToken::ValueToken(bool valid, Position pos, std::string_view contents)
        : BasicToken(valid, pos, contents.length()), contents(contents) {}
    
    void ValueToken::CommonString(std::ostream& out) const
    {
        BasicToken::CommonString(out);
        out << " contents: `" << contents << '`';
    }

    std::string ValueToken::ToString() const
    {
        std::stringstream stream;
        stream << "ValueToken";
        CommonString(stream);
        return stream.str();
    }

    StringToken::StringToken(bool valid, Position pos, std::string_view contents, size_t length)
        : ValueToken(valid, pos, contents), stringLength(length) {}
    
    bool StringToken::StringBound(const char& sym)
    {
        return sym == '\'' || sym == '"';
    }

    std::optional<StringTokenRef> StringToken::Find(const std::string_view& text, const Position& pos)
    {
        if (text.empty() || !StringBound(text[0]))
            return std::nullopt;
        
        auto x = std::find_if(text.begin() + 1, text.end(), StringBound);

        if (x != text.end())
            return std::make_shared<StringToken>(
                true, pos, std::string_view(text.begin(), x + 1), x - text.begin() - 2
            );

        return std::make_shared<StringToken>(false, pos, text, text.length() - 1);
    }

    std::string StringToken::ToString() const
    {
        std::stringstream stream;
        stream << "StringToken";
        CommonString(stream);
        stream << " of length " << stringLength;
        return stream.str();
    }


    NumberToken::NumberToken(bool valid, Position pos, std::string_view contents, NumberLiteralType type)
        : ValueToken(valid, pos, contents), type(type) {}
    
    std::optional<NumberLiteralType> NumberToken::FindPrefix(const std::string_view& text)
    {
        if (text.starts_with("0x")) return NumberLiteralType::HEX;
        if (text.starts_with("0o")) return NumberLiteralType::OCT;
        if (text.starts_with("0b")) return NumberLiteralType::BIN;

        return std::nullopt;
    }

    void NumberToken::CommonString(std::ostream& out) const
    {
        ValueToken::CommonString(out);
        out << " literal `";
        switch (type)
        {
        case NumberLiteralType::BIN:
            out << "0b";
            break;
        case NumberLiteralType::OCT:
            out << "0o";
            break;
        case NumberLiteralType::DEC:
            out << "dec";
            break;
        case NumberLiteralType::HEX:
            out << "0x";
            break;
        }
        out << '`';
    }

    std::string NumberToken::ToString() const
    {
        std::stringstream stream;
        stream << "NumberToken";
        CommonString(stream);
        return stream.str();
    }

    bool NumberToken::IsDigit(const char& sym, NumberLiteralType type)
    {
        switch (type)
        {
        case NumberLiteralType::BIN: return sym == '0' || sym == '1';
        case NumberLiteralType::OCT: return '0' <= sym && sym <= '8';
        case NumberLiteralType::DEC: return '0' <= sym && sym <= '9';
        case NumberLiteralType::HEX:
            return ('0' <= sym && sym <= '9')
                || ('a' <= sym && sym <='f')
                || ('A' <= sym && sym <= 'F');
        }

        return false;
    }


    FloatingPointToken::FloatingPointToken(bool valid, Position pos, std::string_view contents, NumberLiteralType type)
        : NumberToken(valid, pos, contents, type) {}

    std::optional<FloatingPointTokenRef> FloatingPointToken::Find(const std::string_view& text, const Position& pos)
    {
        NumberLiteralType literalType = NumberToken::FindPrefix(text).value_or(NumberLiteralType::DEC);

        auto decimal = std::find_if_not(
            text.begin() + (literalType != NumberLiteralType::DEC) * 2,
            text.end(),
            [literalType](const char& sym){ return NumberToken::IsDigit(sym, literalType); }
        );

        if (decimal == text.end() || *decimal != '.')
            return std::nullopt;
        
        auto fractional = std::find_if_not(
            decimal + 1,
            text.end(),
            [literalType](const char& sym){ return NumberToken::IsDigit(sym, literalType); }
        );

        if ((fractional - decimal == 1) && !(decimal - text.begin()))
        {
            if (literalType != NumberLiteralType::DEC)
                return std::make_shared<FloatingPointToken>(false, pos, std::string_view(
                    text.begin(),
                    text.begin() + (literalType != NumberLiteralType::DEC) * 2 + 1
                ), literalType);
            
            return std::nullopt;
        }

        return std::make_shared<FloatingPointToken>(
            true,
            pos,
            std::string_view(text.begin(), fractional),
            literalType
        );

        return std::nullopt;
    }

    std::string FloatingPointToken::ToString() const
    {
        std::stringstream stream;
        stream << "FloatingPointToken";
        CommonString(stream);
        return stream.str();
    }


    IntegerToken::IntegerToken(bool valid, Position pos, std::string_view contents, NumberLiteralType type)
        : NumberToken(valid, pos, contents, type) {}
    
    std::optional<IntegerTokenRef> IntegerToken::Find(const std::string_view& text, const Position& pos)
    {
        NumberLiteralType literalType = NumberToken::FindPrefix(text).value_or(NumberLiteralType::DEC);

        auto number = std::find_if_not(
            text.begin() + (literalType != NumberLiteralType::DEC) * 2,
            text.end(),
            [literalType](const char& sym){ return NumberToken::IsDigit(sym, literalType); }
        );

        if (literalType != NumberLiteralType::DEC && number == (text.begin() + 2))
            return std::make_shared<IntegerToken>(
                false,
                pos,
                std::string_view(text.begin(), text.begin() + 3),
                literalType
            );

        if (number == text.begin())
            return std::nullopt;
        
        return std::make_shared<IntegerToken>(
            true,
            pos,
            std::string_view(text.begin(), number),
            literalType
        );
    }

    std::string IntegerToken::ToString() const
    {
        std::stringstream stream;
        stream << "IntegerToken";
        CommonString(stream);
        return stream.str();
    }
} // namespace Aesthetic
