

namespace mpl {

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

        for (auto iterator = file.cbegin(); iterator != file.cend(); ++iterator) {
            if (*iterator == ';') {
                if (*(iterator + 1) == '$') {
                    file.erase(iterator, iterator + 2);
                    if (file.empty()) exit(1);
                    while (*iterator != '$' || *(iterator + 1) != ';') {
                        file.erase(iterator);
                        if (file.empty()) exit(1);
                    }
                    file.erase(iterator, iterator + 2);
                } else {
                    do file.erase(iterator);
                    while (*iterator != '\n');
                    file.erase(iterator--);
                }
            }
        }

    }

}
