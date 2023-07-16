#ifndef Moca_assembly_tokens
#define Moca_assembly_tokens

namespace masm_tokens
{

enum class AsmKeywordTokens
{
    /* `mov` instructions. */
    KW_mov,     // `mov` instruction that can handle any sort of register
    KW_movw,    // `mov` instruction that handles registers that are 2-bytes in size
    KW_movd,    // `mov` instruction that handles registers that are 4-bytes in size
    KW_movb,    // `mov` instruction that handles registers that are 1-byte in size

    /* Bit operations. */
    KW_or,      // `or lval, rval`; `lval | rval`
    KW_and,     // `and lval, rval`; `lval & rval`
    KW_xor,     // `xor lval, rval`; `lval ^ rval`
    KW_nand,    // `nand lval, rval`; `!(lval && rval)`
    KW_nor,     // `nor lval, rval`; `!(lval || rval)`
    KW_shl,     // `shl lval, bits`; `lval << bits`; see `MASM KW_shl` in `NOTE`
    KW_shr,     // `shr lval, bits`; `lval >> bits`; see `MASM KW_shr` in `NOTE`
    
    /* Flag-based operations. */
    KW_clc,     // `clc` = Clear Carry (flag)
    KW_cld,     // `cld` = Clear Direction (flag)
    KW_cli,     // `cli` = Clear Interrupt (flag)
    KW_sti,     // `sti` = Set Interrupt (flag)
    KW_cmc,     // `cmc` = Complement Carry (flag)

    /* Compare operations. */
    KW_cmp,
    KW_cmpsb,
    KW_cwd,     // convert word to doubleword

    /* Arithmetic operations. */
    KW_div,
    KW_mul,
    KW_dec,
    KW_inc,
    KW_adc,     // add with carry
    KW_add,
    KW_sub,

    /* Call a "function". */
    KW_call,

    /* Interupts. */
    KW_hlt,
    KW_int,

    /* Port operations. */
    KW_in,
    KW_out,

    /* String-based operations. */
    KW_lea,
    KW_lock,
    KW_lodsb,
    KW_lodsw,

    /* Jump conditions. */
    KW_jmp,
    KW_jne,
    KW_jge,
    KW_jle,
    KW_jz,
    KW_jc,
    KW_jg,
    KW_jl,

    /* Including other binary files/assembly source code files. */
    KW_incbin,
    KW_incsrc,

    /* Label/variable name. */
    KW_special,

    /* Custom keywords for MocAsm. */
    KW_res,     // `mov ax, res ah`
};

/* All keyword token values. */
const nt_BYTE *keyword_token_values[] = {
    "mov", "movw", "movd", "movb",
    "or", "and", "xor", "nand", "nor", "shl", "shr",
    "clc", "cld", "cli", "sti", "cmc",
    "cmp", "cmpsb", "cwd",
    "div", "mul", "dec", "inc", "adc", "add", "sub",
    "call",
    "hlt", "int",
    "in", "out",
    "lea", "lock", "lodsb", "lodsw",
    "jmp", "jne", "jge", "jle", "jz", "jc", "jg", "jl",
    "incbin", "incsrc",
    0, // special
    "res"
};

/* "Common tokens" are tokens that represent an immediate value(decimal/hexadecimal),
 * or a memory reference `[hexadecimal]`.
 * */
enum class AsmCommonTokens
{
    CM_imm_hex = 0x2E,     // an immediate value represented in hexadecimal
    CM_imm_dec,     // an immediate value represented in decimal
    CM_imm_chr,     // a single 8-bit ASCII value, MocAsm will consider this an imm8
    CM_memref_hex,  // a memory reference using hexadecimal
    CM_memref_dec,  // a memory reference using decimal
};

enum class AsmGrammarTokens
{
    GR_comma,
    GR_colon,
    GR_dot,
    GR_lbrack,
    GR_rbrack,
    GR_singleQ,
    GR_doubleQ,
    GR_dollar,
    GR_lpar,
    GR_rpar,
    GR_minus,
    GR_plus,
    GR_percent,
    GR_asm_EOF
};

enum class AsmRegisterTokens
{
    /* General purpose. */
    R_ax, R_ah, R_al,
    R_bx, R_bh, R_bl,
    R_cx, R_ch, R_cl,
    R_dx, R_dh, R_dl,

    /* Pointer registers. */
    R_ip, R_sp, R_bp,

    /* Index registers. */
    R_si, R_di,

    /* Segment registers. */
    R_cs, R_ds, R_ss,

    /* Extra segment registers. */
    R_es, R_fs, R_gs
};

/* All register token values. */
const nt_BYTE *register_token_values[] = {
    "ax", "ah", "al",
    "bx", "bh", "bl",
    "cx", "ch", "cl",
    "dx", "dh", "dl",
    "ip", "sp", "bp",
    "si", "di",
    "cs", "ds", "ss",
    "es", "fs", "gs"
};

enum class AsmDataTypeTokens
{
    /* General datatypes. */
    DT_db = 0x32, // 8-bit (1-byte)
    DT_dw, // 16-bit (2-byte)
    DT_dd, // 32-bit (4-byte)

    /* Special datatypes custom to MocAsm. */
    DT_dbarr, // array of 8-bit (1-byte) values; allows the assembler to expect multiple values; `dbarr 0x10, 0x20, 0x30`
    DT_dwarr, // array of 16-bit (2-byte) values; `dwarr 0x1020, 0x3040, 0x5060`
    DT_ddarr  // array of 32-bit (4-byte) values; `ddarr 0xABCDEF, 0xABCDEF`
};

/* All datatype token values. */
const nt_BYTE *data_type_token_values[] = {
    "db", "dw", "dd",
    "dbarr", "dwarr", "ddarr"
};

enum class TypeOfTokens
{
    TT_keyword,
    TT_grammar,
    TT_datatype,
    TT_register,
    TT_common,
    /* The assembler is explicitly expecting something. */
    ETT_imm8,
    ETT_imm16,
    ETT_imm32,
    TT_NONE
};

template<typename T>
concept IsTokenEnum = requires {
    std::is_same<T, AsmKeywordTokens>::value ||
        std::is_same<T, AsmGrammarTokens>::value ||
        std::is_same<T, AsmRegisterTokens>::value ||
        std::is_same<T, AsmDataTypeTokens>::value ||
        std::is_same<T, AsmCommonTokens>::value;
};

/* Token data. */
struct MocaAsm_TD
{
    ut_BYTE         token_id;
    ut_BYTE         *token_value;
    TypeOfTokens    token_type;
};

/* This gets filled out by the assembler.
 * This structure tells the tokenization program what Token(s) To Expect (TTE).
 * */
struct MocaAsm_TTE
{
    TypeOfTokens    tokens_to_expect[5];
    ut_BYTE         amnt_of_TTE;
};

class MocaAsm_tokenizer
{
private:
    /* This will be allocated when the assembler invokes the according method. */
    struct MocaAsm_TTE *ttexp = nullptr;

public:
    MocaAsm_tokenizer()
    {}

    template<typename T>
        requires IsTokenEnum<T>
    struct MocaAsm_TD *new_token(T token_id, ut_BYTE *token_value)
    {
        struct MocaAsm_TD *token_data = new struct MocaAsm_TD;

        MASM_assert(token_data,
            "\n%s[MEM ALLOCATION ERROR]%s\tThere was an error allocating memory for the token `%s`.\n",
            red, white,
            token_value)

        token_data->token_id = (ut_BYTE) token_id;
        token_data->token_value = new ut_BYTE[strlen(nt_BYTE_CPTR token_value)];
        memcpy(token_data->token_value, token_value, strlen(nt_BYTE_CPTR token_value));

        if(std::is_same<T, AsmKeywordTokens>::value) { token_data->token_type = TypeOfTokens::TT_keyword; return token_data; }
        if(std::is_same<T, AsmGrammarTokens>::value) { token_data->token_type = TypeOfTokens::TT_grammar; return token_data; }
        if(std::is_same<T, AsmRegisterTokens>::value) { token_data->token_type = TypeOfTokens::TT_register; return token_data; }
        if(std::is_same<T, AsmCommonTokens>::value) { token_data->token_type = TypeOfTokens::TT_common; return token_data; }

        token_data->token_type = TypeOfTokens::TT_datatype;
        return token_data;
    }

    struct MocaAsm_TD *create_new_token_alone(ut_BYTE *token_value, ut_WORD line)
    {
        ut_BYTE i = 0;
        ut_BYTE i2 = 0;
        ut_BYTE i3 = 0;

        while(i < sizeof(keyword_token_values)/sizeof(keyword_token_values[0]))
        {
            if(keyword_token_values[i] == 0) { i++; continue; }
            if(strcmp(nt_BYTE_CPTR token_value, keyword_token_values[i]) == 0)
                goto keyword_found;
            i++;
        }

        /* Check if it's a datatype (db, dw, dd, dbarr, dwarr, ddarr). */
        while(i2 < sizeof(data_type_token_values)/sizeof(data_type_token_values[0]))
        {
            if(strcmp(nt_BYTE_CPTR token_value, data_type_token_values[i2]) == 0)
                return new_token<AsmDataTypeTokens> ((AsmDataTypeTokens) (i2+0x32), token_value);
            
            i2++;
        }

        /* This function should never be invoked and have to tokenize a register, error if 
         * `token_value` is a register.
         * */
        while(i3 < sizeof(register_token_values)/sizeof(register_token_values[0]))
        {
            MASM_assert(strcmp(nt_BYTE_CPTR token_value, register_token_values[i3]) != false,
                "\n%s[INVALID SYNTAX, LINE %d]%s\tThere was a unwanted register (`%s`) found on line %d without any bit operation/mov instruction found.\n",
                red, line, white,
                token_value, line)
            
            i3++;
        }
        
        return new_token<AsmKeywordTokens> (AsmKeywordTokens::KW_special, token_value);

        keyword_found:
        return new_token<AsmKeywordTokens> ((AsmKeywordTokens) i, token_value);
    }

    void assign_tokens_to_expect(TypeOfTokens TTE[5])
    {
        if(ttexp) delete ttexp;
        ttexp = new struct MocaAsm_TTE;

        for(ut_BYTE i = 0; i < 5; i++)
        {
            if(TTE[i] == TypeOfTokens::TT_NONE) { ttexp->amnt_of_TTE = i; break; }
            ttexp->tokens_to_expect[i] = TTE[i];
        }
    }

    struct MocaAsm_TTE *get_tte()
    { return ttexp; }

    TypeOfTokens *get_assemblers_TTE()
    { return ttexp->tokens_to_expect; }

    template<typename T>
        requires std::is_same<T, struct MocaAsm_TTE>::value ||
            std::is_same<T, MocaAsm_tokenizer>::value ||
            std::is_same<T, struct MocaAsm_TD>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~MocaAsm_tokenizer()
    {
        if(ttexp) delete ttexp;

        ttexp = nullptr;
    }
};

}

#endif