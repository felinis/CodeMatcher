//================================
//	CodeMatcher
//	main.cpp
//
//	(C)2022 fenik0
//================================

#include <stdio.h>
#include <string.h>
#include "CDebugElfFile.h"
#include "CCompiler.h"
#include "Console.h"

//there are 2 modes of operation of this program:
enum class ProgramMode
{
	DUMP,		//the program is used to dump the debug contents of the ELF file
	COMPILE		//the program is used to compile an existing source tree and match its code with the ELF file
};
static ProgramMode s_program_mode = ProgramMode::COMPILE; //compile by default

int main(int argc, char* argv[])
{
	Console::Initialise();

#ifndef _DEBUG
	//make sure the user provided an operation mode
	if (argc < 2)
	{
		printf("Usage:\n\tDump mode: %s <elf_file_name> -dump\n", argv[0]);
		printf("\tCompile mode: %s <elf_file_name> -compile <source_tree>\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[2], "-dump") == 0)
		s_program_mode = ProgramMode::DUMP;
	else if (strcmp(argv[2], "-compile") == 0)
		s_program_mode = ProgramMode::COMPILE;
	else
	{
		printf("Invalid operation mode: %s\n", argv[2]);
		return 2;
	}

	const char* source_tree_path = nullptr;
	if (s_program_mode == ProgramMode::COMPILE)
	{
		if (argc < 4)
		{
			printf("Compile mode requires a source tree to be specified\n");
			return 3;
		}
		
		source_tree_path = argv[3];
	}

	const char* elf_file_name = argv[1];
#else
	const char* source_tree_path = "C:\\SLY_TREE";
	const char* elf_file_name = "C:\\SLY_TREE\\sly_proto.elf";
#endif

	const char* date = "13/11/2022"; //not using the __DATA__ macro since I do not like the format
	printf("=== CodeMatcher - %s ===\n", date);

	//load the debug ELF file
	CDebugElfFile elf_file;
	if (!elf_file.Load(elf_file_name))
		return 3;

	switch (s_program_mode)
	{
	case ProgramMode::DUMP:
		//dump the debug contents of the ELF file
		elf_file.Dump();
		break;
	case ProgramMode::COMPILE:
		//compile and match the source tree
		CCompiler compiler;
		if (!compiler.CompileAndMatch(elf_file, source_tree_path))
			return 4;
		break;
	}

	return 0;
}
