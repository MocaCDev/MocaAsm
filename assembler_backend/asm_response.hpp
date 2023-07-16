#ifndef Moca_assembly_assembler_response
#define Moca_assembly_assembler_response

namespace MocaAsm_assembler_response
{

/* Operands for registers.
 * Example:
 *      mov ax, bx
 *      ax = reg16, bx = reg16
 *      mov bh, ah
 *      bh = reg8, ah = reg8
 * */
enum class RegisterOperands
{
    reg8,
    reg16,
    reg32,
    regNONE
};

/* All reg8 registers. */
enum class Reg8OperandRegisters
{
    ah, al,
    bh, bl,
    ch, cl,
    dh, dl
};

/* All reg16 registers. */
enum class Reg16OperandRegisters
{
    ax, bx,
    cx, dx,
    sp, bp,
    ss, es,
    ip, si,
    di, ds,
    fs, gs
};

/* Operands for immediate values.
 * Example:
 *      mov ax, 0x20
 *      ax = reg16, 0x20 = imm8 or imm16
 * */
enum class ImmediateOperands
{
    imm8,   // 8-bit (1-byte) immediate value
    imm16,  // 16-bit (2-byte) immediate value
    imm32,  // 32-bit (4-byte) immediate value
    immNONE
};

/* Operands for memory references.
 * Example:
 *      mov ax, [0x20]
 *      ax = reg16, [0x20] = mem8 or mem16
 * */
enum class MemOperands
{
    mem8,
    mem16,
    mem32,
    memNONE
};

/* Where are we in the instruction? */
enum class InstructionPlace
{
    LVAL,
    RVAL
};

/* What type of operand to expect after the lval. */
enum class OperandToExpect
{
    OPR_reg,    // reg8, reg16, reg32
    OPR_imm,    // imm8, imm16, imm32
    OPR_mem,    // mem8, mem16, mem32

    /* Other things to expect.
     * If the assembler received `KW_special` token, then it will expect either a grammatic
     * value (colon) or a datatype (db, dw, dd, dbarr, dwarr, ddarr).
     * */
    datatype,
    grammar,

    /* If `assembler_check_in_new_instruction` recieves a datatype (db, dw, dd or dbarr, dwarr, ddarr),
     * it will then expect some sort of decimal/hexadecimal value.
     * */
    common,
};

/* What instruction is it? */
enum class Instruction
{
    var_or_label,   // KW_special
    Imov,
    Imovb,
    Imovw,
    Imovd,
    Ior,
    Iand,
    Inand,
    Inor,
    Ixor,
    Ishl,
    Ishr,
    Ijmp,
    Ijne,
    Ijg,
    Ijl,
    Ijge,
    Ijle,
    Ijc,
    Ijz,
    Idiv,
    Imul,
    Idec,
    Iinc,
    Iadc,
    Iadd,
    Isub,
    Ihlt,
    Iint,
    Icall,
    Icli,
    Isti,
    SIvar_or_label, // SI - Special Instruction
    SIdb,           // assembler received a db
    SIdw,           // assembler received a dw
    SIdd,           // assembler received a dd
    SIdbarr,        // assembler received a dbarr
    SIdwarr,        // assembler received a dwarr
    SIddarr,        // assembler received a ddarr
    INONE
};

#define is_mov_instr(i)  (i == Instruction::Imov || i == Instruction::Imovb || i == Instruction::Imovd || i == Instruction::Imovw) ? true : false
#define is_bit_operation(i) (i == Instruction::Ior || i == Instruction::Iand || i == Instruction::Inand || i == Instruction::Inor || i == Instruction::Ixor || i == Instruction::Ishl || i == Instruction::Ishr) ? true : false
#define is_jmp_instr(i) (i == Instruction::Ijmp || i == Instruction::Ijne || i == Instruction::Ijg || i == Instruction::Ijl || i == Instruction::Ijge || i == Instruction::Ijle || i == Instruction::Ijc || i == Instruction::Ijz) ? true : false
#define is_arith_operation(i) (i == Instruction::Idiv || i == Instruction::Imul || i == Instruction::Idec || i == Instruction::Iinc || i == Instruction::Iadd || i == Instruction::Isub) ? true : false
#define is_cli_or_hlt(i) (i == Instruction::Icli || i == Instruction::Isti) ? true : false
#define is_int_or_call(i) (i == Instruction::Iint || i == Instruction::Icall) ? true : false

/* Data over the instruction being worked with. */
struct InstructionData
{
    Instruction     instruction;

    /* Is the lval or rval going to end up being treated as an immediate value? */
    bool            treat_lval_as_imm;
    bool            treat_rval_as_imm;

    /* The lval can only be reg8/reg16/reg32 or a mem8/mem16/mem32. */
    union {
        RegisterOperands    lvalreg_operand;
        MemOperands         lvalmem_operand;
    } lval;

    /* What should we expect following the instruction? (lval) */
    OperandToExpect     lvaloperand_expected[5];
    ut_BYTE             lvalamount;

    /* What should we expect after the lval? */
    OperandToExpect     rvaloperand_expected[5];
    ut_BYTE             rvalamount;

    /* The rval can be a reg8/reg16/reg32, a mem8/mem16/mem32 or an imm8, imm16 or imm32. */
    union {
        RegisterOperands    rvalreg_operand;
        MemOperands         rvalmem_operand;
        ImmediateOperands   rvalimm_operand;
    } rval;
};

class AssemblerAPI
{
private:
    struct InstructionData *idata = nullptr;

    Instruction decipher_instruction(ut_BYTE token_id)
    {
        switch(token_id)
        {
            /* Keywords. */
            case (ut_BYTE)AsmKeywordTokens::KW_mov: return Instruction::Imov;break;
            case (ut_BYTE)AsmKeywordTokens::KW_movw: return Instruction::Imovw;break;
            case (ut_BYTE)AsmKeywordTokens::KW_movd: return Instruction::Imovd;break;
            case (ut_BYTE)AsmKeywordTokens::KW_movb: return Instruction::Imovb;break;
            case (ut_BYTE)AsmKeywordTokens::KW_special: return Instruction::SIvar_or_label;break;
            /* Normal DataTypes. */
            case (ut_BYTE)AsmDataTypeTokens::DT_db: return Instruction::SIdb;break;
            case (ut_BYTE)AsmDataTypeTokens::DT_dw: return Instruction::SIdw;break;
            case (ut_BYTE)AsmDataTypeTokens::DT_dd: return Instruction::SIdd;break;
            /* MocAsm "special" datatypes. All assemble back down to db, dw or dd. */
            case (ut_BYTE)AsmDataTypeTokens::DT_dbarr: return Instruction::SIdbarr;break;
            case (ut_BYTE)AsmDataTypeTokens::DT_dwarr: return Instruction::SIdwarr;break;
            case (ut_BYTE)AsmDataTypeTokens::DT_ddarr: return Instruction::SIddarr;break;
            default: break;
        }

        return Instruction::INONE;
    }

    RegisterOperands decipher_register_operand(ut_BYTE token_id)
    {
        switch(token_id)
        {
            /* 16-bit registers (reg16). */
            case (ut_BYTE)AsmRegisterTokens::R_ax:
            case (ut_BYTE)AsmRegisterTokens::R_bx:
            case (ut_BYTE)AsmRegisterTokens::R_cx:
            case (ut_BYTE)AsmRegisterTokens::R_dx:
            case (ut_BYTE)AsmRegisterTokens::R_ds:
            case (ut_BYTE)AsmRegisterTokens::R_sp:
            case (ut_BYTE)AsmRegisterTokens::R_bp:
            case (ut_BYTE)AsmRegisterTokens::R_ss:
            case (ut_BYTE)AsmRegisterTokens::R_ip:
            case (ut_BYTE)AsmRegisterTokens::R_es:
            case (ut_BYTE)AsmRegisterTokens::R_si:
            case (ut_BYTE)AsmRegisterTokens::R_di:
            case (ut_BYTE)AsmRegisterTokens::R_cs:
            case (ut_BYTE)AsmRegisterTokens::R_fs:
            case (ut_BYTE)AsmRegisterTokens::R_gs: return RegisterOperands::reg16;break;
            default: break;
        }

        /* If we have a register and the above switch statement did not return, it's a 8-bit register.
         * Return reg8.
         * */
        return RegisterOperands::reg8;
    }

    ut_BYTE *instruction_name(Instruction instr)
    {
        switch(instr)
        {
            case Instruction::Imov: return ut_BYTE_PTR "mov";break;
            case Instruction::Imovw: return ut_BYTE_PTR "movw";break;
            case Instruction::Imovd: return ut_BYTE_PTR "movd";break;
            case Instruction::Imovb: return ut_BYTE_PTR "movb";break;
            case Instruction::SIvar_or_label: return ut_BYTE_PTR "Variable/Label";break;
            default: break;
        }

        return ut_BYTE_PTR "INONE";
    }

public:
    AssemblerAPI()
    {
        idata = new struct InstructionData;
    }

    void assembler_check_in_new_instruction(struct MocaAsm_TD *tdata, MocaAsm_tokenizer *masm_tokenizer)
    {
        idata->instruction = decipher_instruction(tdata->token_id);

        switch(tdata->token_type)
        {
            case TypeOfTokens::TT_keyword: {
                if(is_mov_instr(idata->instruction))
                {
                    idata->lvaloperand_expected[0] = OperandToExpect::OPR_reg;
                    idata->lvaloperand_expected[1] = OperandToExpect::OPR_mem;
                    idata->lvalamount = 2;

                    TypeOfTokens TTE[5] = {TypeOfTokens::TT_register, TypeOfTokens::TT_common, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE};
                    masm_tokenizer->assign_tokens_to_expect(TTE);
                    return;
                }

                if(is_bit_operation(idata->instruction))
                {
                    idata->lvaloperand_expected[0] = OperandToExpect::OPR_reg;
                    idata->lvalamount = 1;

                    TypeOfTokens TTE[5] = {TypeOfTokens::TT_register, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE};
                    masm_tokenizer->assign_tokens_to_expect(TTE);
                    return;
                }

                if(is_arith_operation(idata->instruction))
                {
                    switch(tdata->token_id)
                    {
                        case (ut_BYTE)AsmKeywordTokens::KW_dec:
                        case (ut_BYTE)AsmKeywordTokens::KW_inc:
                        case (ut_BYTE)AsmKeywordTokens::KW_div:
                        case (ut_BYTE)AsmKeywordTokens::KW_mul: {
                            idata->lvaloperand_expected[0] = OperandToExpect::OPR_reg;
                            idata->lvalamount = 1;

                            TypeOfTokens TTE[5] = {TypeOfTokens::TT_register, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE};
                            masm_tokenizer->assign_tokens_to_expect(TTE);

                            return;
                        }
                        case (ut_BYTE)AsmKeywordTokens::KW_add:
                        case (ut_BYTE)AsmKeywordTokens::KW_sub: {
                            idata->lvaloperand_expected[0] = OperandToExpect::OPR_reg;
                            idata->lvalamount = 1;

                            TypeOfTokens TTE[5] = {TypeOfTokens::TT_register, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE};
                            masm_tokenizer->assign_tokens_to_expect(TTE);

                            return;
                        }
                        default: {
                            MASM_error("\n%s[UNKNOWN ERROR]%s\tAn unknown error occurred when checking in a new instruction.\n",
                                red, white)
                        }
                    }
                }

                if(tdata->token_id == (ut_BYTE) AsmKeywordTokens::KW_special)
                {
                    /* Variable/Label (function). */
                    idata->rvaloperand_expected[0] = OperandToExpect::datatype;
                    idata->rvaloperand_expected[1] = OperandToExpect::grammar;
                    idata->rvalamount = 2;

                    TypeOfTokens TTE[5] = {TypeOfTokens::TT_datatype, TypeOfTokens::TT_grammar, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE};
                    masm_tokenizer->assign_tokens_to_expect(TTE);
                }

                return;
            }
            case TypeOfTokens::TT_datatype: {
                switch(tdata->token_id)
                {
                    case (ut_BYTE)AsmDataTypeTokens::DT_db: {
                        idata->rvaloperand_expected[0] = OperandToExpect::OPR_imm;
                        idata->rvalamount = 1;

                        TypeOfTokens TTE[5] = {TypeOfTokens::ETT_imm8, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE, TypeOfTokens::TT_NONE};
                        masm_tokenizer->assign_tokens_to_expect(TTE);
                        return;
                    }
                    case (ut_BYTE)AsmDataTypeTokens::DT_dw: {
                        std::cout << "DW" << std::endl;
                        return;
                    }
                    case (ut_BYTE)AsmDataTypeTokens::DT_dd: {
                        std::cout << "DD" << std::endl;
                        return;
                    }
                    case (ut_BYTE)AsmDataTypeTokens::DT_dbarr: {
                        std::cout << "DBARR" << std::endl;
                        return;
                    }
                    case (ut_BYTE)AsmDataTypeTokens::DT_dwarr: {
                        std::cout << "DWARR" << std::endl;
                        return;
                    }
                    case (ut_BYTE)AsmDataTypeTokens::DT_ddarr: {
                        std::cout << "DDARR" << std::endl;
                        return;
                    }
                    default: break;
                }
                return;
            }
            case TypeOfTokens::TT_common: printf("Common");break;
            default: break;
        }
    }

    void assembler_done_with_instruction(MocaAsm_tokenizer *masm_tokenizer)
    {
        masm_tokenizer->delete_instance<struct MocaAsm_TTE> (masm_tokenizer->get_tte());
    }

    Instruction get_current_instruction()
    { return idata->instruction; }

    template<typename T>
        requires std::is_same<T, AssemblerAPI>::value ||
            std::is_same<T, struct InstructionData>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~AssemblerAPI()
    {
        if(idata) delete idata;

        idata = nullptr;
    }
};

}

#endif