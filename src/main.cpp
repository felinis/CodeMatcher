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

//there are 3 modes of operation of this program:
enum class ProgramMode
{
	NONE,		//no mode specified
	DUMP,		//dump the debug contents of the ELF file
	COMPILE,	//compile an existing source tree and match its code with the ELF file
	MATCH		//match an ELF against a single precompiled object file
};
static ProgramMode s_program_mode = ProgramMode::NONE;

int main(int argc, char* argv[])
{
	Console::Initialise();

	//parse commandline arguments
	const char* elf_file_name;
	elf_file_name = argv[1];

	const char* source_tree_path;
	const char* object_file_path;
	for (int i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "--dump") == 0 || strcmp(argv[i], "-d") == 0)
		{
			s_program_mode = ProgramMode::DUMP;
			break;
		}
		else if (strcmp(argv[i], "--compile") == 0 || strcmp(argv[i], "-c") == 0)
		{
			s_program_mode = ProgramMode::COMPILE;
			source_tree_path = argv[i + 1];
			break;
		}
		else if (strcmp(argv[i], "--match") == 0 || strcmp(argv[i], "-m") == 0)
		{
			s_program_mode = ProgramMode::MATCH;
			object_file_path = argv[i + 1];
			break;
		}
	}

	bool fSuccess = true;
	if (s_program_mode == ProgramMode::NONE)
	{
		printf("Invalid arguments\n");
		fSuccess = false;
	}
	else if (s_program_mode == ProgramMode::COMPILE && argc < 4)
	{
		printf("Compile mode requires a source tree to be specified\n");
		fSuccess = false;
	}
	else if (s_program_mode == ProgramMode::MATCH && argc < 4)
	{
		printf("Match mode requires an object file to be specified\n");
		fSuccess = false;
	}

	if (!fSuccess)
	{
		printf("Usage:\n");
		printf("\tCompile mode:\t %s <elf_file> [--compile <source_tree>] [-c <source_tree>]\n", argv[0]);
		printf("\tMatch mode:\t %s <elf_file> [--match <obj_file>] [-m <obj_file>]\n", argv[0]);
		printf("\tDump mode:\t %s <elf_file> [--dump] [-d]\n", argv[0]);
		return 1;
	}

#ifdef _DEBUG
	const char* source_tree_path = "C:\\SLY_TREE";
	const char* elf_file_name = "C:\\SLY_TREE\\sly_proto.elf";
#endif

	printf("=== CodeMatcher 1.2 - Dec 6 2023 ===\n");

	//load the debug ELF file
	CDebugElfFile elf_file;
	if (!elf_file.Load(elf_file_name))
	{
		printf("Failed to load ELF file: %s\n", elf_file_name);
		return 4;
	}

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
		{
			printf("Failed to compile and match the source tree: %s\n", source_tree_path);
			return 5;
		}
		break;
	case ProgramMode::MATCH:
		//match the ELF file against a single precompiled object file
		if (argc < 4)
		{
			printf("Match mode requires an object file to be specified\n");
			return 6;
		}

		CDebugElfFile object_file;
		if (!object_file.Load(object_file_path))
		{
			printf("Failed to load object file for matching: %s\n", object_file_path);
			return 7;
		}

		//strip the path and extension from the object file name
		char* object_file_name = strdup(object_file_path);
		object_file_name = basename(object_file_name);
		char* ext = strrchr(object_file_name, '.');
		if (ext)
			*ext = '\0';

		//match the object file
		if (!elf_file.MatchObjectFile(object_file, object_file_name))
		{
			printf("Failed to match object file: %s\n", object_file_name);
			return 8;
		}

#ifdef _DEBUG
		printf("Object file '%s' matched successfully.\n", object_file_name);
#endif

		break;
	}

	return 0;
}
