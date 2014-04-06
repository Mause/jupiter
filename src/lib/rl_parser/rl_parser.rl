#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "token.hpp"
#include "parser.cpp"
#include "parser.h"


std::string getStr(const char* beg, const char* end)
{
    return std::string(beg).substr(0, end-beg);
}

#define EMIT(token_type) emit(lparser, ts, te, token_type, opcodes);

void emit(void* lparser,
          const char* ts, const char* te,
          int token_type, opcode_vector opcodes) {

    auto token = new Token(token_type, getStr(ts, te));

    // tell the parser what we've got
    Parse(
        lparser,
        token_type,
        token,
        &opcodes
    );
}

%%{
    machine asm_parser;
    include asm_grammar "asm_grammar.rl";

    main := |*
        label_name =>           { EMIT(LABEL_NAME); };
        basic_opcode_prim =>    { EMIT(BASIC_OPCODE_LITERAL); };
        special_opcode_prim =>  { EMIT(SPECIAL_OPCODE_LITERAL); };
        semicolon =>            { EMIT(SEMICOLON); };
        colon =>                { EMIT(COLON); };

        space;
    *|;

    write data;
}%%

class Scan {
public:
    ~Scan();
    void init();
    opcode_vector execute(const char* data, size_t len);

private:
    int cs;
    int act;
    const char* ts;
    const char* te;

    void* lparser;
};

Scan::~Scan()
{
    ParseFree(lparser, free);
}

void Scan::init()
{
    lparser = ParseAlloc(malloc);
    %% write init;
}

opcode_vector Scan::execute(const char* data, size_t len)
{
    const char* p =     data;
    const char* pe =    data + len;
    const char* eof =   pe;

    opcode_vector opcodes;

    %% write exec;

    Parse(lparser, 0, NULL, &opcodes);

    return opcodes;
}


opcode_vector parse(std::string input) {
    auto p = Scan();
    p.init();

    return p.execute(
        input.c_str(),
        input.length()
    );
}


int main(int argc, char const *argv[])
{
    const std::string input = (
        ":word"
    );

    auto opcodes = parse(input);

    for (opcode_vector::iterator i = opcodes.begin(); i != opcodes.end(); ++i)
    {
        std::cout << (*i)->repr() << std::endl;
    }
}
