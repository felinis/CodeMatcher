//================================
//	CodeMatcher
//	main.cpp
//
//	(C)2022 Alan Moczulski
//================================

#include <stdio.h>
#include "CElfFile.h"

int main(int argc, char* argv[])
{
#ifndef _DEBUG
	//make sure the user provided an ELF file name
	if (argc < 2)
	{
		printf("Usage: %s <elf_file_name>\n", argv[0]);
		return 1;
	}
	const char* elf_file_name = argv[1];
#else
	const char* elf_file_name = "C:\\Reversing\\mw_mdebug_symbols\\sly_proto.elf";
#endif
	//load the ELF file
	CElfFile elf_file;
	if (!elf_file.Load(elf_file_name))
		return 1;

	return 0;
}
