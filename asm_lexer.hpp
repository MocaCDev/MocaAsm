#ifndef Moca_assembly_lexer
#define Moca_assembly_lexer
#include "asm_tokens.hpp"
using namespace masm_tokens;

namespace masm_lexer
{

/* Struct containing information about lexer state. */
struct lexer_state
{
	nt_BYTE		*asm_filename = nullptr;
	FILE		*asm_code = nullptr;
	ut_DWORD	filesize = 0;
	ut_DWORD	index = 0;
	ut_WORD		line = 0;
	ut_BYTE		current_value = '\0';
	ut_BYTE		*ascii_value;

	lexer_state(nt_BYTE *filename)
	{
		asm_filename = new nt_BYTE[strlen(nt_BYTE_CPTR filename)];
		memcpy(asm_filename, filename, strlen(nt_BYTE_CPTR filename));

		/* Make sure the file exists. */
		MASM_assert(access(asm_filename, F_OK) == 0,
			"\n%s[FILE ERROR]%s\tThe file `%s` does not exist.\n",
			red, white,
			asm_filename)
		
		/* Open the source code file and get its contents. */
		asm_code = fopen(asm_filename, "rb");
		MASM_assert(asm_code,
			"\n%s[FILE ERROR]%s\tThere was an error opening the files `%s`.\n",
			red, white,
			asm_filename)
		
		index = 0;
		
		/* Get the filesize. */
		fseek(asm_code, 0, SEEK_END);
		filesize = (ut_DWORD) ftell(asm_code);
		fseek(asm_code, 0, SEEK_SET);

		/* Make sure there is stuff in the file. */
		MASM_assert(filesize > 1,
			"\n%s[FILE ERROR]%s\tThe file `%s` is empty. Try writing some code.\n",
			red, white,
			asm_filename)
		
		line = 1;
		index += fread(&current_value, sizeof(ut_BYTE), 1, asm_code);
	}

	void read()
	{
		index += fread(&current_value, sizeof(ut_BYTE), 1, asm_code);
	}

	void go_back()
	{
		/* Go back by 2 so we can reread the value we had before. */
		index-=2;
		fseek(asm_code, index, SEEK_SET);

		read();
	}

	~lexer_state()
	{
		if(asm_filename) delete asm_filename;
		if(asm_code) fclose(asm_code);
		if(ascii_value) free(ascii_value);

		asm_filename = nullptr;
		asm_code = nullptr;
		ascii_value = nullptr;
	}
};

class MocaAsm_lexer
{
private:
	struct lexer_state *lstate = nullptr;
	MocaAsm_tokenizer *mtoken = nullptr;

	void seek_forward()
	{
		if(lstate->index + 1 >= lstate->filesize)
		{
			lstate->current_value = '\0';
			return;
		}

		lstate->read();
	}

	bool peek_and_test(ut_BYTE VTT, bool stay_if_matches)
	{
		seek_forward();

		if(lstate->current_value == VTT)
		{
			if(!stay_if_matches) lstate->go_back();
			return true;
		}

		lstate->go_back();
		return false;
	}

	ut_BYTE peek()
	{
		ut_BYTE peek_data = 0;
		seek_forward();
		peek_data = lstate->current_value;
		
		lstate->go_back();
		return peek_data;
	}

	void get_ascii_value()
	{
		if(lstate->ascii_value) free(lstate->ascii_value);
		lstate->ascii_value = ut_BYTE_PTR calloc(1, sizeof(*lstate->ascii_value));
		ut_BYTE index = 0;

		while(is_ascii_WE(lstate->current_value, '_') || is_number(lstate->current_value))
		{
			lstate->ascii_value[index] = lstate->current_value;
			index++;

			lstate->ascii_value = ut_BYTE_PTR realloc(
				lstate->ascii_value,
				(index + 1) * sizeof(*lstate->ascii_value)
			);
			seek_forward();
		}
	}

	void skip_whitespace()
	{
		while(lstate->current_value == ' ' || lstate->current_value == '\t')
			seek_forward();
	}

	void skip_newlines()
	{
		while(lstate->current_value == '\n')
		{
			lstate->line++;
			seek_forward();
		}
	}

public:
	MocaAsm_lexer(nt_BYTE *filename)
	{
		lstate = new struct lexer_state(filename);
		mtoken = new MocaAsm_tokenizer;
	}

	MocaAsm_tokenizer *get_instance()
	{ return mtoken; }

	struct MocaAsm_TD *get_next_token()
	{
		redo:
		if(lstate->current_value == ' ' || lstate->current_value == '\t')
		{ skip_whitespace(); goto redo; }
		
		if(lstate->current_value == '\n')
		{ skip_newlines(); goto redo; }

		if(is_ascii(lstate->current_value))
		{
			get_ascii_value();
			return mtoken->create_new_token_alone(lstate->ascii_value, lstate->line);	
		}

		if(is_number(lstate->current_value))
		{
			if(lstate->ascii_value) delete lstate->ascii_value;
			lstate->ascii_value = (ut_BYTE *) calloc(1, sizeof(*lstate->ascii_value));
			lstate->ascii_value[0] = lstate->current_value;

			seek_forward();
			ut_BYTE length = 1;
			bool is_hex = false;
			bool needs_to_be_hex = false;

			while(!(lstate->current_value == ' ' || lstate->current_value == '\t' || lstate->current_value == '\n' || lstate->current_value == '\0'))
			{
				lstate->ascii_value = (ut_BYTE *) realloc(
					lstate->ascii_value,
					(length + 1) * sizeof(*lstate->ascii_value)
				);
				
				lstate->ascii_value[length] = lstate->current_value;
				length++;

				if(lstate->current_value == 'x' || lstate->current_value == 'h')
				{
					if(lstate->current_value == 'x')
					{
						/* Make sure there are values residing after `0x`. */
						MASM_assert(is_number(peek()) || is_hex(peek()),
							"\n%s[INVALID HEXADECIMAL]%s\tThere was a hexadecimal value found on line %d, but it is invalid.\n",
							red, white,
							lstate->line)
						
						/* Make sure the first value in `lstate->ascii_value` is `0`. */
						MASM_assert(lstate->ascii_value[0] == '0',
							"\n%s[INVALID HEXADECIMAL]%s\tOn line %d, the hexadecimal value started with %s`%s`%s instead of %s`0x`%s.\n",
							red, white,
							lstate->line, 
							yellow, lstate->ascii_value, white,
							green, white)
					}
					
					is_hex = true;
					seek_forward();
					continue;
				}

				if(!is_number(lstate->current_value))
					MASM_assert(is_hex(lstate->current_value),
					"\n%s[INVALID HEXADECIMAL VALUE]%s\tOn line %d, the value %s`%c`%s is not a hexadecimal value. Hex values are %s0-9, A-F%s.\n",
					red, white,
					lstate->line, 
					yellow, lstate->current_value, white,
					green, white)

				if(!is_number(lstate->current_value) && is_hex(lstate->current_value))
					needs_to_be_hex = true;
				
				seek_forward();
			}
			
			if(needs_to_be_hex && !is_hex)
				MASM_error("\n%s[INVALID HEXADECIMAL]%s\tOn line %d, the hexadecimal value %s`%s`%s is missing an %s`h`%s at the end or %s`0x`%s at the beginning signifying it's a hexadecimal value.\n",
					red, white,
					lstate->line, 
					yellow, lstate->ascii_value, white,
					green, white,
					green, white)
			
			if(is_hex) return mtoken->new_token<AsmCommonTokens> (AsmCommonTokens::CM_imm_hex, lstate->ascii_value);
			else return mtoken->new_token<AsmCommonTokens> (AsmCommonTokens::CM_imm_dec, lstate->ascii_value);
		}

		switch(lstate->current_value)
		{
			case '[': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_lbrack, ut_BYTE_PTR "[");break;
			case ']': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_rbrack, ut_BYTE_PTR "]");break;
			case ':': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_colon, ut_BYTE_PTR ":");break;
			case ',': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_comma, ut_BYTE_PTR ",");break;
			case '.': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_dot, ut_BYTE_PTR ".");break;
			case '$': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_dollar, ut_BYTE_PTR "$");break;
			case '(': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_lpar, ut_BYTE_PTR "(");break;
			case ')': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_rpar, ut_BYTE_PTR ")");break;
			case '-': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_minus, ut_BYTE_PTR "-");break;
			case '+': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_plus, ut_BYTE_PTR "+");break;
			case '\'':seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_singleQ, ut_BYTE_PTR "'");break;
			case '"': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_doubleQ, ut_BYTE_PTR "\"");break;
			case '%': seek_forward();return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_percent, ut_BYTE_PTR "%");break;
			case '\0':goto end;break;
			case ';': {
				while(lstate->current_value != '\n' && lstate->current_value != '\0')
				{
					seek_forward();
					printf("%c", lstate->current_value);
				}

				seek_forward();
				goto redo;
			}
			default: break;
		}

		end:
		return mtoken->new_token<AsmGrammarTokens> (AsmGrammarTokens::GR_asm_EOF, ut_BYTE_PTR "\0");
	}

	void get_and_tokenize_imm(struct MocaAsm_TD *tdata)
	{
		delete tdata->token_value;
		mtoken->delete_instance<struct MocaAsm_TD> (tdata);
		
		redo:
		if(lstate->current_value == ' ' || lstate->current_value == '\t')
		{ skip_whitespace(); goto redo; }

		MASM_assert(lstate->current_value != '\n',
			"\n%s[UNEXPECTED NEWLINE]%s\tThere was an unexpected newline on line %d.\n",
			red, white,
			lstate->line)
		
		printf("%c", lstate->current_value);
	}

	template<typename T>
		requires std::is_same<T, MocaAsm_lexer>::value || std::is_same<T, struct lexer_state>::value
	void delete_instance(T *instance)
	{
		if(instance)
			delete instance;
		instance = nullptr;
	}

	~MocaAsm_lexer()
	{
		if(lstate) delete lstate;
		if(mtoken) delete mtoken;

		lstate = nullptr;
		mtoken = nullptr;
	}
};

}

#endif
