    #define RFX_VERTEX_SHADER   0
    #define RFX_FRAGMENT_SHADER 1
    #define RFX_GEOMETRY_SHADER 2

    struct RFX_SHADER_CODE_UNIT
    {
        template<size_t N>
        constexpr RFX_SHADER_CODE_UNIT(const char (&data)[N], int type) : data(data), size(N), type(type) {}
        template<size_t N>
        constexpr RFX_SHADER_CODE_UNIT(const unsigned char (&data)[N], int type) : data(data), size(N), type(type) {}

        const void * const data;
        const size_t size;
        const int type;
    };

    struct RFX_SHADER_CODE_SET
    {
        template<size_t N>
        constexpr RFX_SHADER_CODE_SET(const char * id, const char * layout, const RFX_SHADER_CODE_UNIT(&units)[N]) : id(id), layout(layout), units(units), count(N) {}

        const char * id;
        const char * layout;
        const RFX_SHADER_CODE_UNIT * const units;
        const int count;
    };
    