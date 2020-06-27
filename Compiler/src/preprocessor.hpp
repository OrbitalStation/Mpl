#include <utility>

namespace mpl {

    namespace detail {

        struct preprocess_macro {
            std::string name;
            explicit preprocess_macro(std::string &name) : name(std::move(name)) { }
            TV_t tokens;
        };

        void pp_macro(TV_t::iterator &iterator, TV_t &tokens, std::vector <preprocess_macro> &macros,
                      std::vector <preprocess_macro>::iterator &it2) {
            tokens.erase(iterator);
            if (iterator->type == newline) tokens.erase(iterator);
            if (iterator->type != user_identifier) exit(1);
            for (it2 = macros.begin(); it2 != macros.end(); ++it2) {
                if (it2->name == iterator->value) {
                    macros.erase(it2);
                    break;
                }
            }
            macros.emplace_back(iterator->value);
            tokens.erase(iterator);
            if (iterator->type == newline) tokens.erase(iterator);
            for (; iterator->type != preprocessor_sharp || (iterator + 1)->value != "end"; tokens.erase(iterator)) {
                if (iterator == tokens.end()) exit(1);
                macros.back().tokens.emplace_back(iterator->type, iterator->value);
            }
            tokens.erase(iterator, iterator + 2);
            if (iterator != tokens.end() && iterator->type == newline) tokens.erase(iterator);
            --iterator;
        }

    }

    void preprocess(TV_t &tokens, string_t filename) {

        std::vector <detail::preprocess_macro> macros;

        std::vector <detail::preprocess_macro>::iterator it2;

        std::string temp, temp2;

        for (auto iterator = tokens.begin(); iterator != tokens.end(); ++iterator) {

            if (iterator->type == preprocessor_sharp) {
                tokens.erase(iterator);
                if (iterator->type == newline) {
                    tokens.erase(iterator);
                    continue;
                } else if (iterator->type == user_identifier) {
                    if (iterator->value == "macro") {
                        detail::pp_macro(iterator, tokens, macros, it2);
                    } else if (iterator->value == "remove") {
                        tokens.erase(iterator);
                        if (iterator->type != user_identifier) exit(1);
                        for (it2 = macros.begin(); it2 != macros.end(); ++it2) {
                            if (it2->name == iterator->value) {
                                macros.erase(it2);
                                break;
                            }
                        }
                        tokens.erase(iterator);
                    } else if (iterator->value == "include") {
                        tokens.erase(iterator);
                        if (iterator->type != double_quote) exit(1);
                        tokens.erase(iterator);
                        if (iterator->type != user_identifier) exit(1);
                        temp = filename;
                        for (auto it = temp.end() - 1; true; --it) {
                            if (*it == '/') break;
                            temp.erase(it);
                            if (it == temp.begin()) break;
                        }
                        temp.append(iterator->value);
                        tokens.erase(iterator);
                        if (iterator->type != double_quote) exit(1);
                        tokens.erase(iterator);
                        if (temp == filename) exit(1);
                        TV_t tmp;
                        readfile(temp.c_str(), temp2);
                        uncommentate(temp2);
                        compressfile(temp2);
                        tokenize(temp2, tmp);
                        compress_tokens(tmp);
                        tokens.insert(iterator, tmp.begin(), tmp.end());
                        iterator = tokens.begin() - 1;
                    } else exit(1);
                } else exit(1);
            } else {

                if (iterator->type != user_identifier) continue;

                for (it2 = macros.begin(); it2 != macros.end(); ++it2) {
                    if (iterator->value == it2->name) {
                        tokens.erase(iterator);
                        if (iterator->type == newline) {
                            tokens.erase(iterator);
                            tokens.insert(iterator, it2->tokens.begin(), it2->tokens.end());
                        } else tokens.insert(iterator + 1, it2->tokens.begin(), it2->tokens.end());
                        it2 = macros.begin();
                    }
                }

            }

        }

    }

}
