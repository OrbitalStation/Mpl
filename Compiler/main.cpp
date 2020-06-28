/* Компилируемый со статической типизацией + есть динамическая --> var */

/* Типы -->
 * short int -> целое знаковое число в 1 байт
 * int -> целое знаковое число в 2 байта
 * long int -> целое знаковое число в 4 байта
 * long long int -> целое знаковое число в 8 байт
 * для всего вышеперечисленного также можно использовать
 * модификатор unsigned, который делает их беззнаковыми
 * float -> дробное знаковое число в 4 байта
 * long float -> дробное знаковое число в 8 байт
 * long long float -> дробное знаковое число в 16 байт
 * bool -> логическое значение -> true или false
 * char -> unsigned short int, за исключением того что это символ
 * long char -> unsigned int, за исключением того что это символ
 * long long char -> unsigned long int, за исключением того что это символ
 */

/*
 * bool
 * short int
 * short
 * unsigned short
 * unsigned short int
 * signed short
 * signed short int
 * int
 * signed int
 * unsigned int
 * long
 * signed long
 * unsigned long
 * long int
 * signed long int
 * unsigned long int
 * long long
 * signed long long
 * unsigned long long
 * long long int
 * signed long long int
 * unsigned long long int
 * float
 * long float
 * char
 * signed char
 * unsigned char
 * long char
 * signed long char
 * unsigned long char
 * long long char
 * signed long long char
 * unsigned long long char
 *
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

namespace mpl {

    inline int ispunct(int c) {
        return (c == '=');
    }

    inline int isdigit(int c) {
        return ::isdigit(c) || c == '.' || c == '-';
    }

    inline int isalpha(int c) {
        return ::isalpha(c) || c == '_' || c == '$';
    }

    inline int isalnum(int c) {
        return isalpha(c) || isdigit(c);
    }

}

#include "src/tokens.hpp"
#include "src/defines.hpp"
#include "src/read_and_compress.hpp"
#include "src/config.hpp"
#include "src/translator.hpp"
#include "src/preprocessor.hpp"


std::ostream& operator << (std::ostream &out, const mpl::Token &token) {

    switch (token.type) {
        case mpl::keyword_signed_char:
            return (out << "[signed char]");
        case mpl::keyword_unsigned_char:
            return (out << "[unsigned char]");
        case mpl::keyword_signed_long_char:
            return (out << "[signed long char]");
        case mpl::keyword_unsigned_long_char:
            return (out << "[unsigned long char]");
        case mpl::keyword_signed_long_long_char:
            return (out << "[signed long long char]");
        case mpl::keyword_unsigned_long_long_char:
            return (out << "[unsigned long long char]");
        case mpl::keyword_signed_int:
            return (out << "[signed int]");
        case mpl::keyword_unsigned_int:
            return (out << "[unsigned int]");
        case mpl::keyword_signed_short_int:
            return (out << "[signed short int]");
        case mpl::keyword_unsigned_short_int:
            return (out << "[unsigned short int]");
        case mpl::keyword_signed_long_int:
            return (out << "[signed long int]");
        case mpl::keyword_unsigned_long_int:
            return (out << "[unsigned long int]");
        case mpl::keyword_signed_long_long_int:
            return (out << "[signed long long int]");
        case mpl::keyword_unsigned_long_long_int:
            return (out << "[unsigned long long int]");
        case mpl::keyword_float:
            return (out << "[float]");
        case mpl::keyword_long_float:
            return (out << "[long float]");
        case mpl::keyword_bool:
            return (out << "[bool]");
        case mpl::user_identifier:
            return (out << "[user_id: \"" << token.value << "\"]");
        case mpl::var_number:
            return (out << "[number: \"" << token.value << "\"]");
        case mpl::expr_equal:
            return (out << "[=]");
        case mpl::newline:
            return (out << "[newline]\n");
        case mpl::keyword_return:
            return (out << "[return]");
        default: return out;
    }

}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        std::cerr << "Error number of arguments:\n\t"
        << "First argument is path to file which compiler need to compile.\n\t"
        << "Second argument is path to file which compiler will create and put into "
        << "the compiled source.\n\tThird argument is path to config file.\n"
        << "Please try again.";
        return 1;
    }

    mpl::configure(argv[3]);

    mpl::file_t file;

    mpl::TV_t tokens;

    std::string data = "section .data\n\t";

    std::vector <std::string> constants_names;

    mpl::readfile(argv[1], file);

    mpl::uncommentate(file);

    mpl::compressfile(file);

    mpl::tokenize(file, tokens);

    mpl::compress_tokens(tokens);

    mpl::preprocess(tokens, argv[1]);

    mpl::constantinize(tokens, constants_names, data);

/*    std::cout << ' ';
    for (auto &i: tokens) std::cout << i << ' ';*/

    mpl::translate(argv[2], tokens, data, constants_names);

    return 0;

    /* 2^16 - 1 = 65535 */
    /* 2^15 - 1 = 32767 */
    /* -2^15 = -32768 */
    /* 2^31 - 1 = 2147483647 */
    /* -2^31 = -2147483648 */
}
