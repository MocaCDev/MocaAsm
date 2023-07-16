#ifndef Moca_assembly
#define Moca_assembly
#include "common.hpp"

#include "asm_lexer.hpp"
using namespace masm_lexer;

#include "asm_parser.hpp"
using namespace masm_parser;

namespace moca_assembler
{

class masm_assembler
{
private:
	MocaAsm_lexer *mlex = nullptr;
	MocaAsm_parser *mpars = nullptr;

public:
	masm_assembler(nt_BYTE *filename)
	{
		mlex = new MocaAsm_lexer(filename);
		mpars = new MocaAsm_parser(mlex, mlex->get_instance());
		mpars->start_assembler();
		mpars->parse();
	}

	template<typename T>
		requires std::is_same<T, MocaAsm_lexer>::value || 
			std::is_same<T, MocaAsm_parser>::value ||
			std::is_same<T, masm_assembler>::value
	void delete_instance(T *instance)
	{
		if(instance)
			delete instance;
		instance = nullptr;
	}

	~masm_assembler()
	{
		if(mlex) delete mlex;
		if(mpars) delete mpars;

		mlex = nullptr;
		mpars = nullptr;

		/* Debug. */
		std::cout << "\n[DEBUG]\tDeleted `MocaAsm_parser` instance." << std::endl;
		std::cout << "[DEBUG]\tDeleted `MocaAsm_lexer` instance." << std::endl;
		std::cout << "[DEBUG]\tDeleted `masm_assembler` instance." << std::endl;
	}
};

}

#endif
