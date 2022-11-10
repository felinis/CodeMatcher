#pragma once
#include "CDebugElfFile.h"

class CCompiler
{
public:
	bool CompileAndMatch(CDebugElfFile& original_elf_file, const char* source_tree_path);
};
