#include <cstring>

namespace mpl {

    namespace detail {

        struct name_and_type {
            std::string name;
            Types type;
            name_and_type(std::string name, const Types &type) : name(std::move(name)), type(type) { }
        };

        bool operator == (const name_and_type &n1, const std::string &n2) { return n1.name == n2; }

        bool check_existing_name(bool &isFindCoincidence, std::vector <name_and_type> &user_values,
                const std::string &value, const Types &type, const bool &isFoundTrueIsBad = false) {
            if (!isFoundTrueIsBad) {
                isFindCoincidence = false;
                for (auto &i: user_values)
                    if (i.name == value && i.type == type) {
                        isFindCoincidence = true;
                        break;
                    } else if (i.name == value) return true;
                if (!isFindCoincidence) exit(1);
                return true;
            } else for (auto &i: user_values) if (i.name == value) exit(1);
            return false;
        }

        void check_number(const Types &type, const std::string &number, char * &end) {

            if (type == SignedInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtol(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value > 32767 || value < -32768) exit(1);

            } else if (type == UnsignedInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtol(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value > 65535 || value < 0) exit(1);

            } else if (type == Bool) {

                if (number != "1" && number != "0") exit(1);

            } else if (type == SignedShortInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtol(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value < -128 || value > 127) exit(1);

            } else if (type == UnsignedShortInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtol(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value < 0 || value > 255) exit(1);

            } else if (type == SignedLongInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtol(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value < -2147483648 || value > 2147483647) exit(1);

            } else if (type == UnsignedLongInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtol(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value < 0 || value > 4294967295) exit(1);

            } else if (type == SignedLongLongInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtol(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value < -9223372036854775807L || value > 9223372036854775807L) exit(1);

            }  else if (type == UnsignedLongLongInt) {

                if (std::find(number.cbegin(), number.cend(), '.') != number.cend()) exit(1);

                auto value = strtoul(number.c_str(), &end, 10);

                if (strlen(end) != 0 || value > 18446744073709551615UL) exit(1);

            }

        }

        bool declare_new_variable(TV_t::iterator &iterator,
                std::vector <name_and_type> &user_values, std::string &var_name,
                TV_t &tokens, std::string &bss, std::string &text, bool &isFindCoincidence, bool &isGoNext,
                const TokenType &type, string_t reserve, const std::string &asm_type, const std::string &reg,
                const Types &type2, char * &end) {
            if (iterator->type == type) {
                tokens.erase(iterator);
                if (iterator->type != user_identifier) exit(1);
                var_name = "usr_" + iterator->value;
                check_existing_name(isFindCoincidence, user_values, var_name, type2, true);
                bss.append(var_name + ' ' + reserve + "\n\t");
                user_values.emplace_back(var_name, type2);
                tokens.erase(iterator);
                if (iterator->type == newline) {
                    tokens.erase(iterator);
                    return true;
                } else if (iterator->type == expr_equal) {
                    tokens.erase(iterator);
                    if (iterator->type == var_number) {
                        check_number(type2, iterator->value, end);
                        text.append("mov " + asm_type + " [" + var_name + "], " + iterator->value + "\n\t");
                        tokens.erase(iterator);
                    } else if (iterator->type == user_identifier) {
                        iterator->value.insert(0, "usr_");
                        check_existing_name(isFindCoincidence, user_values, iterator->value, type2);
                        text.append("mov " + reg + ", [" + iterator->value + "]\n\tmov " + asm_type + " ["
                                    + var_name + "], " + reg + "\n\t");
                        tokens.erase(iterator);
                    } else exit(1);
                } else exit(1);
                isGoNext = false;
            } else isGoNext = true;
            return false;
        }

        void assign_variable_helper(TV_t::iterator &iterator,
                std::string &text, std::vector <name_and_type> &user_values,
                bool &isFindCoincidence, std::string &var_name, TV_t &tokens,
                Types &cmp, const std::string &asm_type, const std::string &reg, char * &end) {
            if (iterator->type == var_number) {
                check_number(cmp, iterator->value, end);
                text.append("mov " + asm_type + " [" + var_name + "], " + iterator->value + "\n\t");
                tokens.erase(iterator);
            } else if (iterator->type == user_identifier) {
                iterator->value.insert(0, "usr_");
                if (!check_existing_name(isFindCoincidence, user_values, iterator->value, cmp)) return;
                text.append("mov " + reg + ", [" + iterator->value + "]\n\tmov " + asm_type + " ["
                    + var_name + "], " + reg + "\n\t");
                tokens.erase(iterator);
            } else exit(1);
        }

        void assign_variable_float_helper(TV_t::iterator &iterator, std::string &text, std::vector <std::string> &constants,
                bool &isFindCoincidence, std::vector <name_and_type> &user_values, Types &type, TV_t &tokens,
                std::string &var_name, const std::string &asm_type) {
            if (iterator->type == user_identifier) {
                if (std::find(constants.begin(), constants.end(), iterator->value) == constants.end()) {
                    iterator->value.insert(0, "usr_");
                    check_existing_name(isFindCoincidence, user_values, iterator->value, type);
                }
                text.append("fld " + asm_type + " [" + iterator->value + "]\n\tfst " +
                            asm_type + " [" + var_name + "]\n\t");
                tokens.erase(iterator);
            } else exit(1);
        }

        void assign_variable(TV_t::iterator &iterator, bool &isFindCoincidence,
                std::string &var_name, Types &type, std::vector <name_and_type> &user_values,
                TV_t &tokens, std::string &text, char * &end, std::vector <std::string> &constants) {
            if (iterator->type == user_identifier) {
                iterator->value.insert(0, "usr_");
                if (!check_existing_name(isFindCoincidence, user_values, iterator->value, type)) return;
                var_name = iterator->value;
                type = std::find(user_values.cbegin(), user_values.cend(), var_name)->type;
                tokens.erase(iterator);
                if (iterator->type == newline) {
                    tokens.erase(iterator);
                    return;
                } else if (iterator->type == expr_equal) {
                    tokens.erase(iterator);
                    if (type == SignedInt || type == UnsignedInt)
                        assign_variable_helper(iterator, text, user_values, isFindCoincidence,
                            var_name, tokens, type, "word", "ax", end);
                    else if (type == Bool || type == SignedShortInt || type == UnsignedShortInt)
                        assign_variable_helper(iterator, text, user_values, isFindCoincidence,
                             var_name, tokens, type, "byte", "al", end);
                    else if (type == SignedLongInt || type == UnsignedLongInt)
                        assign_variable_helper(iterator, text, user_values, isFindCoincidence,
                             var_name, tokens, type, "dword", "eax", end);
                    else if (type == SignedLongLongInt || type == UnsignedLongLongInt)
                        assign_variable_helper(iterator, text, user_values, isFindCoincidence,
                             var_name, tokens, type, "qword", "rax", end);
                    else if (type == Float)
                        assign_variable_float_helper(iterator, text, constants, isFindCoincidence, user_values,
                             type, tokens, var_name, "dword");
                    else if (type == LongFloat) {
                        assign_variable_float_helper(iterator, text, constants, isFindCoincidence, user_values,
                             type, tokens, var_name, "qword");
                    }
                }
            }
        }

        bool declare_new_float_variable(TV_t &tokens, std::vector <std::string> &constants,
                const TokenType &type, const Types &type2, TV_t::iterator &iterator, bool &isGoNext,
                std::string &bss, std::string &var_name, const std::string &reserve, bool &isFindCoincidence,
                std::vector <name_and_type> &user_values, std::string &text, char * &end,
                const std::string &asm_type) {
            if (iterator->type == type) {
                tokens.erase(iterator);
                if (iterator->type != user_identifier) exit(1);
                var_name = "usr_" + iterator->value;
                check_existing_name(isFindCoincidence, user_values, var_name, type2, true);
                bss.append(var_name + ' ' + reserve + "\n\t");
                user_values.emplace_back(var_name, type2);
                tokens.erase(iterator);
                if (iterator->type == newline) {
                    tokens.erase(iterator);
                    return true;
                } else if (iterator->type == expr_equal) {
                    tokens.erase(iterator);
                    if (iterator->type == var_number) exit(1);
                    else if (iterator->type == user_identifier) {
                        if (std::find(constants.begin(), constants.end(), iterator->value) == constants.end()) {
                            iterator->value.insert(0, "usr_");
                            check_existing_name(isFindCoincidence, user_values, iterator->value, type2);
                        }
                        text.append("fld " + asm_type + " [" + iterator->value + "]\n\tfst " +
                            asm_type + " [" + var_name + "]\n\t");
                        tokens.erase(iterator);
                    }
                }
                isGoNext = false;
            } else isGoNext = true;
            return false;
        }

#define DeclareNewVariable(type, reserve, asm_type, reg, type2)\
    /* I use 'isFindCoincidence' two times because this can some economy memory */\
    declare_new_variable(iterator, user_values, var_name, tokens, bss, text,\
    isFindCoincidence, isFindCoincidence, type, reserve, asm_type, reg, type2, end)

        bool handle_tokens_string(std::vector <name_and_type> &user_values, std::string &bss,
                std::string &data, std::string &text, TV_t &tokens, TV_t::iterator &iterator,
                std::string &var_name, bool &isFindCoincidence, Types &type, char * &end,
                std::vector <std::string> &constants) {

            iterator = tokens.begin();

            while (iterator->type != newline) {

                if (DeclareNewVariable(keyword_signed_int, "resw 1", "word", "ax", SignedInt)) return false;

                /* This is using like 'isGoNext' in previous function */
                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_unsigned_int, "resw 1", "word", "ax", UnsignedInt)) return false;

                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_bool, "resb 1", "byte", "al", Bool)) return false;

                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_signed_short_int, "resb 1", "byte", "al", SignedShortInt)) return false;

                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_unsigned_short_int, "resb 1", "byte", "al", UnsignedShortInt)) return false;

                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_signed_long_int, "resd 1", "dword", "eax", SignedLongInt)) return false;

                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_unsigned_long_int, "resd 1", "dword", "eax", UnsignedLongInt)) return false;

                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_signed_long_long_int, "resq 1", "qword", "rax", SignedLongLongInt)) return false;

                if (!isFindCoincidence) continue;

                if (DeclareNewVariable(keyword_unsigned_long_long_int, "resq 1", "qword", "rax", UnsignedLongLongInt)) return false;

                if (!isFindCoincidence) continue;

                if (declare_new_float_variable(tokens, constants, keyword_float, Float, iterator, isFindCoincidence, bss,
                        var_name, "resd 1", isFindCoincidence, user_values, text, end, "dword")) return false;

                if (!isFindCoincidence) continue;

                if (declare_new_float_variable(tokens, constants, keyword_long_float, LongFloat, iterator, isFindCoincidence, bss,
                                               var_name, "resq 1", isFindCoincidence, user_values, text, end, "qword")) return false;

                if (!isFindCoincidence) continue;

                assign_variable(iterator, isFindCoincidence, var_name, (type = SignedInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = UnsignedInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = Bool), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = SignedShortInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = UnsignedShortInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = SignedLongInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = UnsignedLongInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = SignedLongLongInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = UnsignedLongLongInt), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = Float), user_values,
                        tokens, text, end, constants);

                assign_variable(iterator, isFindCoincidence, var_name, (type = LongFloat), user_values,
                        tokens, text, end, constants);

                if (iterator->type == keyword_return) {
                    tokens.erase(iterator);
                    if (iterator->type != var_number) exit(1);
                    return true;
                }

            }

            tokens.erase(iterator);

            return false;

        }

    }

    void translate(string_t path, TV_t &tokens, std::string &data, std::vector <std::string> &constants) {

        std::string bss = "section .bss\n\t";

        std::string text1 = "section .text\n\tglobal _start:\n\n", text2 = "\n_start:\n\t";

        std::vector <detail::name_and_type> user_values;

        auto iterator = tokens.begin();

        std::string var_name;

        bool isFindCoincidence, isFindReturn = false;

        Types type;

        char *end = nullptr;

        while (!tokens.empty()) {
            if (detail::handle_tokens_string(user_values, bss, data, text2, tokens,
                                         iterator, var_name, isFindCoincidence, type, end, constants)) {
                detail::check_number(SignedLongInt, iterator->value, end);
                text1.append("_exit:\n\tmov eax, 1\n\tmov ebx, " + iterator->value + "\n\tint 0x80\n");
                isFindReturn = true;
                break;
            }
        }

        if (!isFindReturn) {
            text1.append("_exit:\n\tmov eax, 1\n\tmov ebx, 0\n\tint0x80\n");
        }

        text2.append("jmp _exit\n");

        if (detail::print_source == "true") std::cout << data << '\n' << bss << '\n' << text1 << text2;
        else if (detail::print_source == "false") {
            std::ofstream file(path);
            file << data << '\n' << bss << '\n' << text1 << text2;
            file.close();
        } else exit(1);

    }

    void constantinize(TV_t &tokens, std::vector <std::string> &constants_names, std::string &data) {

        std::string val, name;

        std::string::iterator it2;

        long double check;

        char *end;

        for (auto iterator = tokens.begin(); iterator != tokens.end(); ++iterator) {
            if (iterator->type == var_number &&
                    (it2 = std::find(iterator->value.begin(), iterator->value.end(), '.')) != iterator->value.end()) {
                val = iterator->value;
                check = strtold(val.c_str(), &end);
                *it2 = '_';
                data.append(name = ("@f_" + iterator->value));
                if (check >= 3.40282347e-38f && check <= 3.40282347e+38f) data.append(" dd ");
                else if (check >= 1.7976931348623157e-308 && check <= 1.7976931348623157e+308) data.append(" dq ");
                else exit(1);
                data.append(val + "\n\t");
                constants_names.push_back(name);
                *it2 = '.';
                for (iterator = tokens.begin(); iterator != tokens.end(); ++iterator) {
                    if (iterator->type == var_number && iterator->value == val) {
                        iterator->type = user_identifier;
                        iterator->value = name;
                    }
                }
                iterator = tokens.begin();
            }
        }

    }

}

#undef DeclareNewVariable
