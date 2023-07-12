#ifndef moca_assembly_tokens
#define moca_assembly_tokens

enum class MASM_Keyword_Tokens
{
	/* `mov` related keywords. */
	KW_mov,
	KW_movl,

	/* Assembly binary types. */
	KW_use16,
	KW_use32,
	KW_use64,

	/* Jmp statements. */
	KW_jmp,
	KW_jmpl,
	KW_jmpf,
	KW_je,
	KW_jne,
	KW_jg,
	KW_jge,
	KW_jl,
	KW_jle,
	KW_jz,
	KW_jc,
	KW_jcxz,

	/* Bit operations. */
	KW_and,
	KW_or,
	KW_nand,
	KW_xor,
	KW_shl,
	KW_shr,
	KW_nor,

	/* Other "conditional" keywords. */
	KW_test,
	KW_cmp,

	/* Interrupt-related keywords. */
	KW_cli,
	KW_sti,
	KW_int,
	KW_clc,

	/* Arithmetic keywords. */
	KW_add, // syntax will be `add reg, val`
	KW_sub, // syntax will be `sub reg, val`
	KW_mul, // syntax will be `mul reg, val`
	KW_div, // syntax will be `div reg, val`
	KW_inc, // syntax will be `inc reg`; increments by only 1
	KW_dec, // syntax will be `dec reg`; decrements by only 1
	
	/* Other. */
	KW_org,
	KW_hlt,
	KW_pad,

	/* Although this really are not keywords, we'll consider them to be keyword because why not? */
	KW_hexadecimal,
	KW_decimal,

	/* Include another assembly source code file (incsrc).
	 * Include another assembly binary file (incbin).
	 * Both come after `%`; `%incsrc "assembly_src_code_file"` / `%incbin "assembly_binary_file"`.
	 * */
	KW_incsrc,
	KW_incbin
};

enum class MASM_Grammar_Tokens
{
	GR_comma,
	GR_lbrack,	// [
	GR_rbrack,	// ]
	GR_colon,
	GR_period,
	GR_lpara,	// (
	GR_rpara, 	// )
	GR_dolsgn,	// $
	GR_minus,
	GR_plus,
	GR_SQTE,	// '
	GR_DQTE,	// "
	GR_percent,
	GR_comment,	// ;
	GR_asm_EOF
};

/* Since hexadecimal and decimal values get tokenized as keywords, the assembler will still figure out the size (type). 
 * This will be something the assembler does itself.
 * */
enum class MASM_DataTypes_Tokens
{
	DT_db,		// byte
	DT_dw,		// word
	DT_dd,		// double-word
	
	/* "Fancy" keywords implemented by MocaAsm that tells the assembler the value will be an array of a certain data type. */
	DT_dbarr,	// tells the assembler the value will be an array of bytes (db values), added `\0`(0) to end by default; `name dbarr 'Name'
	DT_dwarr,	// tells the assembler the value will be an array of words (dw values); `ages dwarr 10, 20, 30, 40`
	DT_ddarr,	// tells the assembler the value will be an array of double words (dd values); `sizes ddarr 0xFFEE, 0xFFFF, 0xFFFF`
};

enum class MASM_Reg_Tokens
{
	/* General purpose registers. */
	REG_ax,
	REG_ah,
	REG_al,
	REG_bx,
	REG_bh,
	REG_bl,
	REG_cx,
	REG_ch,
	REG_cl,
	REG_dx,
	REG_dh,
	REG_dl,

	/* Offset registers.
	 * REG_si is offset from REG_ds; REG_di is offset from REG_es.
	 * REG_ip is the offset address of the next instruction.
	 * */
	REG_si,
	REG_di,
	REG_ip,

	/* REG_sp is the stack pointer, offset from REG_ss. 
	 * REG_bp is the stack base pointer (where the stack begins to grow).
	 * */
	REG_sp,
	REG_bp,

	/* Segment pointers.
	 * REG_cs is offset in memory of the current code segment.
	 * REG_ds is offset in memory of the current data segment.
	 * REG_ss is offset in memory of the stack.
	 * REG_es is extra segment.
	 * REG_fs is extra segment.
	 * REG_gs is extra segment.
	 * */
	REG_cs,
	REG_ds,
	REG_ss,
	REG_es,
	REG_fs,
	REG_gs,

	REG_cr0,

	/* EFLAGS registers.
	 * REG_cf is the carry flag.
	 * */
	REG_cf
};

/* Based on the token we get prior, what should we expect?
 * TokenTCBExpected - Token That Can Be (TCB) Expected.
 * */
enum class TokenTCBExpected
{
	EXP_register,
	EXP_grammar,		// any sort of grammar
	EXP_datatype,		// expecte a `db`, `dw` or `dw`; this can be a datatype value, or the keywords themselves
	EXP_hexadecimal,
	EXP_decimal,
	EXP_nothing,		// `cli`, `sti`, `hlt` will expect nothing following them
	EXP_arithmetic
};

/* This is for `MocaAsm_TDID`.
 * This describes the type of token that was tokenized.
 *  */
enum class TokenEnumType
{
	ENM_keyword,
	ENM_grammer,
	ENM_datatype,
	ENM_register
};

template<typename T>
concept IsTokenEnum = requires {
	std::is_same<T, MASM_Keyword_Tokens>::value ||
		std::is_same<T, MASM_Grammar_Tokens>::value ||
		std::is_same<T, MASM_DataTypes_Tokens>::value ||
		std::is_same<T, MASM_Reg_Tokens>::value;
};

template<typename T>
	requires IsTokenEnum<T>
struct MASM_token
{
	T		token_id;
	ut_BYTE		*token_value;
	
	struct {
		TokenTCBExpected	tokens_TTCBEXP[7]; // Tokens That Can Be Expected (TTCBEXP).
		ut_BYTE			TTCBEXP_amnt;	   // Amount of tokens that can be expected.
	} TEXP; // To Expect (TEXP).

	MASM_token(T tid, ut_BYTE *tval)
	{
		token_id = tid;
		token_value = new ut_BYTE[strlen(nt_BYTE_CPTR tval)];
		memcpy(token_value, tval, strlen(nt_BYTE_CPTR tval));	
	}

	void print_enum_type()
	{
		std::cout << typeid(token_id).name() << std::endl;
	}

	~MASM_token()
	{
		if(token_value) delete token_value;
		token_value = nullptr;
	}
};

/* MocaAsm_TDID - Token Data In Depth (TDID).
 * Return a structure that explains what data `MASM_token` is holding.
 * */
struct MocaAsm_TDID
{
	struct {
		TokenTCBExpected	tokens_TCBEXP[7]; // That Can Be Expected (TCBEXP).
		ut_BYTE			TTCBEXP_amnt;	  // Amount of tokens that can be expected.
	} TEXP; // To Expect (TEXP).
	
	TokenEnumType		token_type;
	ut_WORD			token_id;
	ut_BYTE			*token_value;
};

class MocaAsm_Token
{
private:
	struct MocaAsm_TDID	*tdataID = nullptr; // Token Data In Depth (tdataID).
	
public:
	MocaAsm_Token()
	{}

	template<typename T>
		requires IsTokenEnum<T>
	void new_token(T tid, ut_BYTE *tval)
	{
		struct MASM_token<T> *token = new struct MASM_token<T> (tid, tval);
		MASM_assert(token, "\n%sMemory Allocation Error:%s\n\tThere was an error allocating memory for the token.\n",
			red, white)

		if(tdataID)
		{ delete tdataID; tdataID = nullptr; }
	       	tdataID = new struct MocaAsm_TDID;

		if(std::is_same<T, MASM_Keyword_Tokens>::value)
			{ tdataID->token_type = TokenEnumType::ENM_keyword; goto finish; }

		finish:
		tdataID->token_id = (ut_DWORD) token->token_id;
		tdataID->token_value = new ut_BYTE[strlen(nt_BYTE_CPTR token->token_value)];
		memcpy(tdataID->token_value, token->token_value, strlen(nt_BYTE_CPTR token->token_value));
		
		delete token->token_value;
		delete token;
		token = nullptr;

	}

	struct MocaAsm_TDID *get_TDID()
	{ return tdataID; }

	template<typename T>
		requires std::is_same<T, MocaAsm_Token>::value || std::is_same<T, struct MocaAsm_TDID>::value
	void delete_instance(T *instance)
	{
		if(instance)
			delete instance;
		
		instance = nullptr;
	}

	~MocaAsm_Token()
	{
		if(tdataID->token_value) delete tdataID->token_value;
		if(tdataID) delete tdataID;
		tdataID = nullptr;	
	}
};

#endif
