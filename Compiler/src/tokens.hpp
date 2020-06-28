#include <utility>

namespace mpl {

    enum TokenType {

        _kw_short,
        _kw_int,
        _kw_long,
        _kw_float,
        _kw_char,
        _kw_signed,
        _kw_unsigned,

        keyword_signed_char,
        keyword_unsigned_char,

        keyword_signed_long_char,
        keyword_unsigned_long_char,

        keyword_signed_long_long_char,
        keyword_unsigned_long_long_char,

        keyword_signed_int,
        keyword_unsigned_int,

        keyword_signed_short_int,
        keyword_unsigned_short_int,

        keyword_signed_long_int,
        keyword_unsigned_long_int,

        keyword_signed_long_long_int,
        keyword_unsigned_long_long_int,

        keyword_float,
        keyword_long_float,

        keyword_bool,

        keyword_return,

        user_identifier,

        var_number,

        expr_equal,

        newline,

        preprocessor_sharp,

        double_quote

    };

    struct Token {

        TokenType type;

        std::string value;

        Token(const TokenType &type, std::string &value)
        : type(type), value(std::move(value)) { }

        explicit Token(const TokenType &type)
        : type(type) { }

    };

    namespace detail {

        extern std::string PPBegin;

        bool get_next_word_helper(std::string &from, std::string &to, char &c, int(* const &func)(int), int(* const &func2)(int)) {
            if (func(c)) {
                c = from[0];
                while (func2(c) && !from.empty()) {
                    from.erase(0, 1);
                    to.push_back(c);
                    c = from[0];
                }
                return true;
            }
            return false;
        }

        bool get_next_word_helper(std::string &from, std::string &to, char &c, int(* const &func)(int)) {
            if (func(c)) {
                c = from[0];
                while (func(c) && !from.empty()) {
                    from.erase(0, 1);
                    to.push_back(c);
                    c = from[0];
                }
                return true;
            }
            return false;
        }

        /* return true if read a number */
        bool get_next_word(std::string &from, std::string &to) {

            to.clear();

            if (from.empty()) return false;

            for (unsigned int i = 0, len = PPBegin.length(); i < len; ++i) {
                if (from[i] != PPBegin[i]) break;
                else if (i == len - 1) {
                    from.erase(0, len);
                    to = PPBegin;
                    return false;
                }
            }

            if (from[0] == '\"') {
                to.assign(1, '\"');
                from.erase(0, 1);
                return false;
            }

            char c = from[0];

            from.erase(0,1);

            if (c != ' ') to.push_back(c);
            else c = from[0];

            if (from.empty()) return false;

            if (get_next_word_helper(from, to, c, isalpha, isalnum)) return false;

            if (get_next_word_helper(from, to, c, isdigit)) return true;

            get_next_word_helper(from, to, c, ispunct);

            return false;

        }

    }

    void tokenize(std::string &file, std::vector <Token> &tokens) {

        tokens.clear();

        std::string word;

        while (!file.empty()) {

            if (detail::get_next_word(file, word)) {
                tokens.emplace_back(var_number, word);
            } else if (word == detail::PPBegin) {
                tokens.emplace_back(preprocessor_sharp);
            } else if (word == "int") {
                tokens.emplace_back(_kw_int);
            } else if (word == "short") {
                tokens.emplace_back(_kw_short);
            } else if (word == "long") {
                tokens.emplace_back(_kw_long);
            } else if (word == "float") {
                tokens.emplace_back(_kw_float);
            } else if (word == "char") {
                tokens.emplace_back(_kw_char);
            } else if (word == "unsigned") {
                tokens.emplace_back(_kw_unsigned);
            } else if (word == "signed") {
                tokens.emplace_back(_kw_signed);
            } else if (word == "bool") {
                tokens.emplace_back(keyword_bool);
            } else if (word == "=") {
                tokens.emplace_back(expr_equal);
            } else if (word == "\n") {
                tokens.emplace_back(newline);
            } else if (word == "\"") {
                tokens.emplace_back(double_quote);
            } else if (word.empty()) {
                /* nothing */
            } else if (word == "return") {
                tokens.emplace_back(keyword_return);
            } else {
                tokens.emplace_back(user_identifier, word);
            }

        }

    }

    void compress_tokens(std::vector <Token> &tokens) {
        for (auto iterator = tokens.begin(); iterator != tokens.end(); ++iterator) {
            if (iterator->type == _kw_signed) {
                tokens.erase(iterator);
                if (iterator->type == _kw_short) {
                    if ((iterator + 1)->type == _kw_int) tokens.erase(iterator);
                    iterator->type = keyword_signed_short_int;
                } else if (iterator->type == _kw_int) {
                    iterator->type = keyword_signed_int;
                } else if (iterator->type == _kw_long) {
                    if ((iterator + 1)->type == _kw_long) {
                        if ((iterator + 2)->type == _kw_int) {
                            tokens.erase(iterator, iterator + 2);
                            iterator->type = keyword_signed_long_long_int;
                        } else if ((iterator + 2)->type == _kw_char) {
                            tokens.erase(iterator, iterator + 2);
                            iterator->type = keyword_signed_long_long_char;
                        } else {
                            tokens.erase(iterator);
                            iterator->type = keyword_signed_long_long_int;
                        }
                    } else if ((iterator + 1)->type == _kw_int) {
                        tokens.erase(iterator, iterator + 1);
                        iterator->type = keyword_signed_long_int;
                    } else if ((iterator + 1)->type == _kw_char) {
                        tokens.erase(iterator, iterator + 1);
                        iterator->type = keyword_signed_long_char;
                    } else {
                        iterator->type = keyword_signed_long_int;
                    }
                } else if (iterator->type == _kw_char) {
                    iterator->type = keyword_signed_char;
                } else exit(1);
            } else if (iterator->type == _kw_unsigned) {
                tokens.erase(iterator);
                if (iterator->type == _kw_short) {
                    if ((iterator + 1)->type == _kw_int) tokens.erase(iterator);
                    iterator->type = keyword_unsigned_short_int;
                } else if (iterator->type == _kw_int) {
                    iterator->type = keyword_unsigned_int;
                } else if (iterator->type == _kw_long) {
                    if ((iterator + 1)->type == _kw_long) {
                        if ((iterator + 2)->type == _kw_int) {
                            tokens.erase(iterator, iterator + 2);
                            iterator->type = keyword_unsigned_long_long_int;
                        } else if ((iterator + 2)->type == _kw_char) {
                            tokens.erase(iterator, iterator + 2);
                            iterator->type = keyword_unsigned_long_long_char;
                        } else {
                            tokens.erase(iterator);
                            iterator->type = keyword_unsigned_long_long_int;
                        }
                    } else if ((iterator + 1)->type == _kw_int) {
                        tokens.erase(iterator, iterator + 1);
                        iterator->type = keyword_unsigned_long_int;
                    } else if ((iterator + 1)->type == _kw_char) {
                        tokens.erase(iterator, iterator + 1);
                        iterator->type = keyword_unsigned_long_char;
                    } else {
                        iterator->type = keyword_unsigned_long_int;
                    }
                } else if (iterator->type == _kw_char) {
                    iterator->type = keyword_unsigned_char;
                } else exit(1);
            } else if (iterator->type == _kw_int) {
                iterator->type = keyword_signed_int;
            } else if (iterator->type == _kw_short) {
                if ((iterator + 1)->type == _kw_int) tokens.erase(iterator);
                iterator->type = keyword_signed_short_int;
            } else if (iterator->type == _kw_long) {
                if ((iterator + 1)->type == _kw_long) {
                    if ((iterator + 2)->type == _kw_int) {
                        tokens.erase(iterator, iterator + 2);
                        iterator->type = keyword_signed_long_long_int;
                    } else if ((iterator + 2)->type == _kw_char) {
                        tokens.erase(iterator, iterator + 2);
                        iterator->type = keyword_unsigned_long_long_char;
                    } else {
                        tokens.erase(iterator);
                        iterator->type = keyword_signed_long_long_int;
                    }
                } else if ((iterator + 1)->type == _kw_int) {
                    tokens.erase(iterator);
                    iterator->type = keyword_signed_long_int;
                } else if ((iterator + 1)->type == _kw_char) {
                    tokens.erase(iterator);
                    iterator->type = keyword_unsigned_long_char;
                } else if ((iterator + 1)->type == _kw_float) {
                    tokens.erase(iterator);
                    iterator->type = keyword_long_float;
                } else {
                    iterator->type = keyword_signed_long_int;
                }
            } else if (iterator->type == _kw_char) {
                iterator->type = keyword_unsigned_char;
            } else if (iterator->type == _kw_float) {
                iterator->type = keyword_float;
            }
        }
    }

}