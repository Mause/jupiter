#include <vector>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <unordered_set>

#include "parser.hpp"
#include <opcodes/opcodes.hpp>


opcode_vector galaxy::jupiter::parser::Parser::parse(){
    // parse takes an reference to an empty std::vector<Opcode>, and returns nothing
    opcode_vector opcodes;

    while (!tokens.empty()) {
        auto token = pop(tokens);

        galaxy::jupiter::opcodes::Opcode* op = NULL;
        std::cout << "Parsing token: " << token->repr() << std::endl;

        if (token->name == "punctuation") {
            op = handle_punctuation(token, tokens);

        } else if (token->name == "word"){
            op = handle_word(token, tokens);

        } else {
            throw InvalidInstruction("Misplaced token: ", token->contents);
        }

        if (op != NULL){
            std::cout << op->repr() << std::endl;
            opcodes.push_back(op);
        }
    }
    return opcodes;
}

galaxy::jupiter::opcodes::Opcode* galaxy::jupiter::parser::handle_punctuation(HANDLER_SIGNATURE) {
    galaxy::jupiter::opcodes::Opcode* op = NULL;

    if (token->contents == ":"){
        std::cout << "Semicolon so label" << std::endl;
        op = handle_label(token, tokens);

    } else if (token->contents == "."){
        token = pop(tokens);

        if (token->normalized() == "dat"){
            op = handle_dat(token, tokens);

        } else {
            std::cout << token->repr() << std::endl;
            throw InvalidInstruction("Unknown instruction", token);
        }

    } else if (token->contents == "\n") {
        // just ignore newlines. this could admittely mean that you can have something like this;
        // ADD a,1 SUB a,1
        // and it would work fine...

    } else if (token->contents == ";") {
        handle_comments(token, tokens);

    } else {
        throw InvalidInstruction("Misplaced punctuation", token);
    }

    return op;
}

galaxy::jupiter::opcodes::Opcode* galaxy::jupiter::parser::handle_word(HANDLER_SIGNATURE) {
    galaxy::jupiter::opcodes::Opcode* op = NULL;

    if (token->normalized() == "export") {
        op = handle_export(token, tokens);

    } else if (token->normalized() == "orig"){
        op = handle_orig(token, tokens);

    } else if (token->normalized() == "fill") {
        op = handle_fill(token, tokens);

    } else if (token->normalized() == "dat"){
        op = handle_dat(token, tokens);

    } else {
        // TODO: check if it is a valid instruction, and whether it is basic or complex
        op = handle_inst(token, tokens);
    }
    return op;
}


galaxy::jupiter::opcodes::OrigOpcode* galaxy::jupiter::parser::handle_orig(HANDLER_SIGNATURE){
    auto mem_loco = pop(tokens)->contents;
    auto location = strtol(mem_loco.c_str(), NULL, 0);

    return new galaxy::jupiter::opcodes::OrigOpcode(location);
}


galaxy::jupiter::opcodes::BasicOpcode* galaxy::jupiter::parser::handle_inst(HANDLER_SIGNATURE) {

    std::string name = token->contents;
    galaxy::jupiter::opcodes::Part* a = grab_part(tokens);

    // at index 1 is the comma... or at least there should be :P
    if (tokens.front()->contents != ","){
        throw galaxy::jupiter::parser::InvalidInstruction(
            "Invalid instruction; ", tokens.front()->repr()
        );
    } else {
        pop(tokens);
    }

    galaxy::jupiter::opcodes::Part* b = grab_part(tokens);

    // if (pop(tokens)->contents)

    return new galaxy::jupiter::opcodes::BasicOpcode(name, a, b);
}

void galaxy::jupiter::parser::handle_comments(HANDLER_SIGNATURE) {
    // nukes comments
    do {
        pop(tokens);
    } while (tokens.front()->contents != "\n");
}

galaxy::jupiter::opcodes::DATOpcode* galaxy::jupiter::parser::handle_dat(HANDLER_SIGNATURE) {
    auto contained = grab_quoted(tokens);

    std::string contents = "";
    for(auto it: contained) {
        contents += it->contents;
    }

    return new galaxy::jupiter::opcodes::DATOpcode(contents);
}

galaxy::jupiter::opcodes::LabelOpcode* galaxy::jupiter::parser::handle_label(HANDLER_SIGNATURE) {
    auto t = pop(tokens);

    if (t->name != "word"){
        throw galaxy::jupiter::parser::InvalidInstruction("Invalid label name", t);
    }

    return new galaxy::jupiter::opcodes::LabelOpcode(t->contents);
}

token_vector galaxy::jupiter::parser::grab_quoted(token_vector &tokens) {
    token_vector contained_tokens;
    // pop the opening quote
    pop(tokens);

    while (tokens.front()->contents != "\"" && (!tokens.size() != 1)) {
        contained_tokens.push_back(pop(tokens));
    }

    if (tokens.front()->contents != "\""){
        throw InvalidAssembly(".DAT missing closing quote.");
    }

    // pop the closing quote
    pop(tokens);

    return contained_tokens;
}


// we return a subset of the given tokens
galaxy::jupiter::opcodes::Part* galaxy::jupiter::parser::grab_part(token_vector &tokens) {
    std::vector<galaxy::jupiter::Token*> new_tokens;
    bool is_reference = false;

    new_tokens.push_back(pop(tokens));

    if (new_tokens.at(0)->contents == "[") {
        is_reference = true;
        do {
            new_tokens.push_back(pop(tokens));
        } while (tokens.at(0)->contents != "]");
    }

    return new galaxy::jupiter::opcodes::Part(new_tokens, is_reference);
}

galaxy::jupiter::opcodes::ExportOpcode* galaxy::jupiter::parser::handle_export(HANDLER_SIGNATURE) {
    std::vector<std::string> label_names;
    const std::unordered_set<std::string> OPCODE_END({"\n", ";"});

    do {
        Token* t = pop(tokens);
        if (t->name == "word") {
            label_names.push_back(t->contents);
        }
    } while (OPCODE_END.find(tokens.front()->contents) == OPCODE_END.end());


    return new galaxy::jupiter::opcodes::ExportOpcode(label_names);
}

galaxy::jupiter::opcodes::FillOpcode* galaxy::jupiter::parser::handle_fill(HANDLER_SIGNATURE) {
    auto contents = pop(tokens)->to_uint16_t();
    auto length = pop(tokens)->to_uint16_t();

    return new galaxy::jupiter::opcodes::FillOpcode(contents, length);
}
