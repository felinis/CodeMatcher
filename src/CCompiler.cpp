#include "CCompiler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CFileStream.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

static bool Compile(CDebugElfFile& original_elf_file, const char* source_file)
{
	//form the compiled object file name like so: temp/file.o
	char object_file_name[128];
	snprintf(object_file_name, sizeof(object_file_name), "temp\\%s.o", source_file);

	//run mipscc.exe to compile the source file
	char command_line[256];
	//example of compiling a file:
	//C:\usr\local\sce\ee\gcc\bin\ee-gcc.exe -IC:\usr\local\sce\ee\gcc\include\g++-2 -g -O2 -x c++ -c mat.c -o mat.o
	snprintf(command_line, sizeof(command_line),
		"C:\\usr\\local\\sce\\ee\\gcc\\bin\\ee-gcc.exe -IC:\\usr\\local\\sce\\ee\\gcc\\include\\g++-2 -g -O2 -x c++ -c %s -o %s",
		source_file, object_file_name);
	int code = system(command_line);
	if (code != 0)
	{
		printf("Failed to compile source file '%s'\n", source_file);
		return false;
	}

	printf("Source file '%s' compiled successfully\n", source_file);

	//the object file was compiled successfully, now open it
	CDebugElfFile object_file; //the object file must also have an mdebug section
	if (!object_file.Load(object_file_name))
	{
		printf("Failed to load object file '%s'\n", object_file_name);
		return false;
	}

	//perform the actual matching
	bool is_matching = original_elf_file.MatchObjectFile(object_file, source_file);

	return true;
}

bool CCompiler::CompileAndMatch(CDebugElfFile& original_elf_file, const char* source_tree_path)
{
	//set current directory to the source tree path
	fs::current_path(source_tree_path);

	//find all of the .c source files in the given source tree path
	fs::path search_path(source_tree_path);
	search_path /= "*.c";

	//loop through every source file and compiile it
	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_iter(search_path); dir_iter != end_iter; ++dir_iter)
	{
		if (fs::is_regular_file(dir_iter->status()))
		{
			const char* source_file = dir_iter->path().filename().c_str();

			if (!Compile(original_elf_file, source_file))
			{
				return false;
			}
		}
	}

	return true;
}
