#include "asm.hpp"

int main(int args, char *argv[])
{
	/* First argument should be the file to work with. */
	MASM_assert(args > 1, 
		"\n%sInvalid Amount Of Arguments:%s\n\tMASM expects an assembly file as the first argument always.\n\tHere are two ways to pass the file as the first argument:\n\t\t%s`masm -f [file] [other arguments]`%s or %s`masm [file] [other_arguments]`%s.\n\n\t%s`[other arguments]`%s can be:\n\n\t1. -AT: AT stands for Assembly Type. Following -AT will be `bit16`, `bit32` or `bit64`.\n\t\tExample: %s`masm -f [file] -AT bit16`%s\n\n\t2. -ED: ED stands for Explicit Debug. This argument does not require anything following it like -AT does. -ED tells the assembler to display explicit debug info to the terminal.\n\t\tExample: %s`masm -f [file] -ED`%s\n\n\t3. -EDL: EDL stands for Explicit Debug Logs. This argument does not require anything following it like -AT does.\n\t   -EDL tells the assembler to write explicit debug information to a \"log\" file.\n\t\tExample: %s`masm -f [file] -EDL`%s\n\n\t4. -EFBP: EFAMP stands for Enforce Famp Boot Protocol. Moca Assember is a part of the FAMP boot protocol.\n\t   -EFBP tells the assembler to enforce needed information that the FAMP boot protocol would need. With -EFBP, you will be required to pass `mbr`, `ssboot` or `adasm`.\n\t   `-EFBP mbr` tells the assembler to enforece specification for the Master Boot Record (MBR).\n\t   `-EFBP ssboot` tells the assembler to enforece specification for second-stage bootloader.\n\t   `-EFBP adasm` tells the assembler that the assembly file being passed will be a \"add-on\" assembly library.\n\t\tExample for MBR:\t\t\t%s`masm -f [file] -EFBP mbr`%s\n\t\tExample for Second Stage Bootloader:\t%s`masm -f [file] -EFBP ssboot`%s\n\t\tExample for \"Add-On\" Assembly Library:  %s`masm -f [file] -EFBP adasm`%s\n\n\t5. -SAN: SAN stand for Store All Names. -SAN tells the assembler to take all variable/\"structure\" names and save them in another binary file for reference later on.\n\t   This will be useful if you are planning on using MocaLink, a custom linker written for MocaAsm.\n\t\tExample: %s`masm -f [file] -SAN`%s\n\n\n",
		red, white,
		green, white,
		green, white,
		yellow, white,
		green, white,
		green, white,
		green, white,
		green, white,
		green, white,
		green, white,
		green, white)
	
	ut_BYTE arg_index = 1;
	MocaAsm_Lexer *masmLexer = nullptr;
	
	reloop:
	if(arg_index == 1)
	{
		if(strcmp(argv[arg_index], "-f") == 0 || strcmp(argv[arg_index], "-F") == 0)
		{
			arg_index++;

			if(arg_index >= args)
				MASM_error("\n%sArgument Error:%s\n\tMissing `[file]` following `-f`.\n",
					red, white)
		}

		for(ut_LSIZE i = 0; i < strlen(argv[arg_index]); i++)
		{
			if(argv[arg_index][i] == '.') break;

			MASM_assert(!(i == (strlen(argv[arg_index]) - 1)),
				"\n%sArgument Error:%s\n\tMissing file extension in `[file]` following `-f`.\n\tAdd `.asm`, `.s` or any extension to `%s`.\n",
				red, white,
				argv[arg_index])
		}
		masmLexer = new MocaAsm_Lexer(nt_BYTE_CPTR argv[arg_index]);

		goto reloop;
	}

	MASM_assert(masmLexer, "\n%sInitialization Error:%s\n\tError initiating instance of Moca Assembly Lexer.\n",
		red, white)
	
	masmLexer->MocaAsm_Lexer_GNT();
	masmLexer->delete_instance<MocaAsm_Lexer> (masmLexer);
	//masmLexer->delete_instance(masmLexer);
	
	return 0;
}
