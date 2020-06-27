namespace mpl {

    typedef std::string file_t;

    typedef const char * const & string_t;

    typedef std::vector <Token> TV_t;

    enum Types {

        SignedShortInt,
        UnsignedShortInt,

        SignedInt,
        UnsignedInt,

        SignedLongInt,
        UnsignedLongInt,

        SignedLongLongInt,
        UnsignedLongLongInt,

        Float,
        LongFloat,

        Bool,

        SignedChar,
        UnsignedChar,

        SignedLongChar,
        UnsignedLongChar,

        SignedLongLongChar,
        UnsignedLongLongChar

    };

}
