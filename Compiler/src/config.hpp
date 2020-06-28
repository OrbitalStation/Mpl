namespace mpl {

    namespace detail {

        std::string single_comment;

        std::string multi_comment_begin;

        std::string multi_comment_end;

        std::string isCheckSingleCommentBeforeMultiple;

        void uncommentate_config(file_t &file) {

            for (auto iterator = file.cbegin(); iterator != file.cend(); ++iterator) {
                if (*iterator == '%' && *(iterator + 1) == '%') {
                    do file.erase(iterator);
                    while (*iterator != '\n');
                }
            }

        }

        void configure_helper(file_t &file, string_t find, unsigned long int &pos) {

            if ((pos = file.find(find)) == file_t::npos) exit(1);

            file.erase(pos, strlen(find));

        }

        void configure_helper2(string_t find, std::string &what, file_t &file, unsigned long int &pos) {
            detail::configure_helper(file, find, pos);
            if (file[pos] != ':') exit(1);
            file.erase(pos, 1);
            if (file[pos] == '\n') file.erase(pos, 1);
            if (file[pos] == '\t') file.erase(pos, 1);
            while (file[pos] != '\n') {
                what.append(1, file[pos]);
                file.erase(pos, 1);
            }
            file.erase(pos, 1);
        }

    }

    void configure(string_t filepath) {

        file_t file;

        unsigned long int pos;

        readfile(filepath, file);

        detail::uncommentate_config(file);

        detail::configure_helper2("SimpleComment", detail::single_comment, file, pos);

        detail::configure_helper2("MultiCommentBegin", detail::multi_comment_begin, file, pos);

        detail::configure_helper2("MultiCommentEnd", detail::multi_comment_end, file, pos);

        detail::configure_helper2("IsCheckSingleCommentBeforeMultiple", detail::isCheckSingleCommentBeforeMultiple, file, pos);

    }

}
