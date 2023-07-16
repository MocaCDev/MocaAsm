#ifndef Moca_assembly_parser
#define Moca_assembly_parser

#include "assembler_backend/asm_response.hpp"
using namespace MocaAsm_assembler_response;

namespace masm_parser
{

class MocaAsm_parser
{
private:
    MocaAsm_lexer *mlexer = nullptr;
    struct MocaAsm_TD *token = nullptr;
    AssemblerAPI *asmAPI = nullptr;
    MocaAsm_tokenizer *masm_tokenizer;

    void next_token()
    {
        token = mlexer->get_next_token();
    }

public:
    MocaAsm_parser(MocaAsm_lexer *lex, MocaAsm_tokenizer *mtoken)
    {
        mlexer = lex;
        masm_tokenizer = mtoken;

        /* Go ahead and get the first token. */
        next_token();
    }

    void start_assembler()
    {
        if(!asmAPI) asmAPI = new AssemblerAPI;

        asmAPI->assembler_check_in_new_instruction(token, masm_tokenizer);
    }

    void parse()
    {
        switch(asmAPI->get_current_instruction())
        {
            case Instruction::Imov: {
                std::cout << "Mov" << std::endl;
                break;
            }
            case Instruction::SIdb: {
                mlexer->get_and_tokenize_imm(token);

                break;
            }
            case Instruction::SIdw: {
                std::cout << "Writing DW (2-byte value)" << std::endl;
                break;
            }
            case Instruction::SIdd: {
                std::cout << "Writing DD (4-byte value)" << std::endl;
                break;
            }
            case Instruction::SIdbarr: {
                std::cout << "Writing DB array (1-byte values)" << std::endl;
                break;
            }
            case Instruction::SIdwarr: {
                std::cout << "Writing DW array (2-byte values)" << std::endl;
                break;
            }
            case Instruction::SIddarr: {
                std::cout << "Writing DD array (4-byte values)" << std::endl;
                break;
            }
            default: break;
        }
    }

    ~MocaAsm_parser()
    {
        if(mlexer) delete mlexer;
        if(token->token_value) delete token->token_value;
        if(token) delete token;
        if(asmAPI) delete asmAPI;

        mlexer = nullptr;
        token = nullptr;
        asmAPI = nullptr;

        std::cout << "\n[DEBUG]\tDeleted parser." << std::endl;
    }
};

}

#endif
