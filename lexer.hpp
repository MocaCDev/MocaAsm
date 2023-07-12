#ifndef moca_assembly_lexer
#define moca_assembly_lexer
#include "token.hpp"

struct MASM_lexer
{
public:
	ut_BYTE		*assembly_code = nullptr;
	nt_BYTE		*assembly_filename = nullptr;
	ut_DWORD	index;
	ut_BYTE		current_value;
	ut_DWORD	line;
	ut_DWORD	file_size;

	MASM_lexer(const nt_BYTE *filename)
	{
		assembly_filename = new nt_BYTE[strlen(filename)];
		memcpy(assembly_filename, filename, strlen(filename));

		index = 0;
		line = 1;

		FILE *asm_file = fopen(filename, "rb");
		MASM_assert(asm_file, "\n%sError:%s\n\tThe file `%s` must not exist.\n",
			red, white,
			assembly_filename)

		fseek(asm_file, 0, SEEK_END);
		file_size = ftell(asm_file);
		fseek(asm_file, 0, SEEK_SET);

		MASM_assert(file_size > 1, "\n%sFile Size Error:%s\n\tThe file `%s` tends to be empty.\n",
			red, white,
			assembly_filename)
		
		
		assembly_code = new ut_BYTE[file_size];
		MASM_assert(assembly_code, "\n%sMemory Allocation Error:%s\n\tThere was an error allocating 0x%X (%d) bytes.\n",
			red, white,
			file_size, file_size)
		
		MASM_assert(fread(assembly_code, sizeof(ut_BYTE), file_size, asm_file) == file_size,
			"\n%sRead Error:%s\n\tThere was an error reading the assembly file `%s`.\n",
			red, white,
			assembly_filename)

		fclose(asm_file);

		current_value = assembly_code[index];
		index++;

		std::cout << "\n\e[0;102m[INIT DEBUG]\e[0;37m\tInitiated Instance Of MASM Lexer." << std::endl;
		
	}

	template<typename T>
	void delete_instance(T *instance)
	{
		if(instance)
			delete instance;
		instance = nullptr;
	}

	~MASM_lexer()
	{
		if(assembly_filename) delete assembly_filename;
		if(assembly_code) delete assembly_code;
		
		assembly_filename = nullptr;
		assembly_code = nullptr;

		std::cout << "\n\e[0;102m[DEBUG]\e[0;37m\tAssembly Filename Released." << std::endl;
		std::cout << "\e[0;102m[DEBUG]\e[0;37m\tAssembly Source Code Released." << std::endl;	
		std::cout << "\e[0;102m[DEBUG]\e[0;37m\tMASM Lexer Released." << std::endl;
	}
};

class MocaAsm_Lexer
{
private:
	struct MASM_lexer	*mlexer = nullptr;
	MocaAsm_Token		*mtoken = nullptr;	
	struct MocaAsm_TDID	*mtokenID = nullptr;
	
	void MocaAsm_Lexer_move()
	{
		mlexer->index++;
		if(mlexer->index >= mlexer->file_size)
		{
			mlexer->current_value = mlexer->assembly_code[mlexer->file_size];
			return;
		}

		mlexer->current_value = mlexer->assembly_code[mlexer->index];
	}

	ut_BYTE MocaAsm_Lexer_peek()
	{
		return mlexer->assembly_code[mlexer->index + 1];
	}

	/* TAV - Test Against Value.
	 * MocaAsm_Lexer_peek_and_test will peek ahead by 1 index and test the value against `TAV`.
	 * */
	bool MocaAsm_Lexer_peek_and_test(ut_BYTE TAV, bool set_index)
	{
		if(MocaAsm_Lexer_peek() == TAV)
		{
			if(set_index)
				MocaAsm_Lexer_move();

			return true;
		}

		return false;
	}

public:
	MocaAsm_Lexer(const nt_BYTE *asmfile)
	{
		mlexer = new struct MASM_lexer(asmfile);
		mtoken = new MocaAsm_Token;

		MASM_assert(mlexer, "\n%sMemory Allocation Error:%s\n\tThere was an error allocating 0x%lX (%ld) bytes for MASM lexer.\n",
			red, white,
			sizeof(struct MASM_lexer), sizeof(struct MASM_lexer))
	}

	/* GNT - Get Next Token. */
	void MocaAsm_Lexer_GNT()
	{
		if(mtokenID)
			delete mtokenID;
		mtoken->new_token<MASM_Keyword_Tokens> (MASM_Keyword_Tokens::KW_jmp, ut_BYTE_PTR "jmp");
		mtokenID = mtoken->get_TDID();
			
	}

	template<typename T = MocaAsm_Lexer>
		requires std::is_same<T, MocaAsm_Lexer>::value || std::is_same<T, struct MASM_lexer>::value
	void delete_instance(T *instance)
	{
		if(instance)
			delete instance;
		
		instance = nullptr;
	}

	~MocaAsm_Lexer()
	{
		if(mlexer) delete mlexer;	
		if(mtoken) delete mtoken;
		if(mtokenID) delete mtokenID;

		mlexer = nullptr;
		mtoken = nullptr;
		mtokenID = nullptr;

		std::cout << "\n\e[0;102m[DEBUG]\e[0;37m\tMASM Lexer Struct Released." << std::endl;
		std::cout << "\e[0;102m[DEBUG]\e[0;37m\tMASM Lexer Class Released." << std::endl;
	}
};

#endif
