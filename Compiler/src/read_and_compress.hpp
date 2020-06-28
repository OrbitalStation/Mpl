namespace mpl {

    namespace detail {

        extern std::string single_comment;

        extern std::string multi_comment_begin;

        extern std::string multi_comment_end;

        extern std::string isCheckSingleCommentBeforeMultiple;

        bool multi_comment_check(unsigned int &chk, bool &multi_comment_check,
                                 unsigned int &len2, file_t::const_iterator &iterator, file_t &file,
                                 unsigned int &len3) {
            for (chk = 0, multi_comment_check = true; chk < len2; ++chk) {
                if (*(iterator + chk) != detail::multi_comment_begin[chk]) {
                    multi_comment_check = false;
                    break;
                }
            }
            if (multi_comment_check) {
                file.erase(iterator, iterator + len2);
                do {
                    if (file.empty()) exit(1);
                    file.erase(iterator);
                    for (chk = 0, multi_comment_check = false; chk < len3; ++chk) {
                        if (*(iterator + chk) != detail::multi_comment_end[chk]) {
                            multi_comment_check = true;
                            break;
                        }
                    }
                } while (multi_comment_check);
                file.erase(iterator, iterator + len3 + 1);
                return true;
            }
            return false;
        }

        bool single_comment_check(unsigned int &chk, bool &single_comment_check,
                                  unsigned int &len, file_t::const_iterator &iterator, file_t &file) {
            for (chk = 0, single_comment_check = true; chk < len; ++chk) {
                if (*(iterator + chk) != detail::single_comment[chk]) {
                    single_comment_check = false;
                    break;
                }
            }
            if (single_comment_check) {
                do file.erase(iterator);
                while (*iterator != '\n');
                return true;
            }
            return false;
        }

    }

    /*
     * Читает файл(полностью(т.е. вплоть до символа '\0' невключительно))
     * и записывает его в строку.
     * Вместо '\0' в конце вставляет '\n'.
     */

    void readfile(string_t path, file_t &to) {
        std::ifstream file(path);
        std::getline(file, to, '\0');
        file.close();
        to.push_back('\n');
    }

    /*
     * Сжимает строку, удаляя лишние пробелы и символы новой строки
     */

    void compressfile(file_t &file) {

        auto cmp = [](const char &c) {
            return isalnum(c) || c == '\"' || c == '\'';
        };

        bool isMetQuote = false;

        for (auto iterator = file.cbegin(); iterator != file.cend(); ++iterator) {
            if (*iterator == '\"') isMetQuote = !isMetQuote;
            if (isMetQuote) continue;
            if (*iterator == ' ') {
                if (!cmp(*(iterator - 1)) || !cmp(*(iterator + 1))) {
                    file.erase(iterator--);
                }
            } else if (*iterator == '\n') {
                if (*(iterator + 1) == '\n') {
                    file.erase(iterator--);
                }
            }
        }
    }



    /*
     * Удаляет все комментарии
     */

    void uncommentate(file_t &file) {
        bool single_comment_check, multi_comment_check;
        unsigned int chk, len = detail::single_comment.length(),
            len2 = detail::multi_comment_begin.length(),
            len3 = detail::multi_comment_end.length();
        if (detail::isCheckSingleCommentBeforeMultiple == "true") {
            for (auto iterator = file.cbegin(); iterator != file.cend(); ++iterator)
                if (!detail::single_comment_check(chk, single_comment_check, len, iterator, file))
                    detail::multi_comment_check(chk, multi_comment_check, len2, iterator, file, len3);
        } else if (detail::isCheckSingleCommentBeforeMultiple == "false") {
            for (auto iterator = file.cbegin(); iterator != file.cend(); ++iterator)
                if (!detail::multi_comment_check(chk, multi_comment_check, len2, iterator, file, len3))
                    detail::single_comment_check(chk, single_comment_check, len, iterator, file);
        } else exit(1);
    }

}
