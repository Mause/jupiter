#ifndef _TOKEN_HPP
#define _TOKEN_HPP

#include <vector>

namespace galaxy { namespace jupiter {

class token; // forward declare
typedef std::vector<galaxy::jupiter::token*> token_vector;

class token {
public:
    int token_type;
    std::uint16_t line_no;
    std::string contents;
    token_vector contents_vector;
    std::string token_name = "";

    token(int token_type, std::uint16_t line_num, std::string contents) :
        token_type(token_type),
        line_no(line_num + 1),
        contents(contents) {};

    token(int token_type, std::uint16_t line_num, token_vector contents) :
        token_type(token_type),
        line_no(line_num + 1),
        contents_vector(contents) {};

    void set_token_name(std::string token_name) {
        this->token_name = token_name;
    }

    std::string repr();
    std::string normalized();
    std::uint16_t to_uint16_t();
};

}}


#endif
