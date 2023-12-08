#include "CCompiler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CFileStream.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#ifdef WINDOWS
#define SCE "C:\\usr\\local\\sce"
#define EE SCE"\\ee"
#define IDIR EE"\\gcc\\include\\g++-2"
#define CC EE"\\gcc\\bin\\ee-gcc.exe"
#else
#define SCE "~/.wine/drive_c/usr/local/sce"
#define EE SCE"/ee"
#define IDIR EE"/gcc/include/g++-2"
#define CC "wine " EE"/gcc/bin/ee-gcc"
#endif

static bool CompileSourceFile(CDebugElfFile& original_elf_file, const char* source_file, const char* include_dir = NULL)
{
	//buffer compiled object file name (i.e. filename.o)
	char object_file_name[128];
	snprintf(object_file_name, sizeof(object_file_name), "%s.o", source_file);

	//buffer command to compile the source file
	//	example command:
	//	C:\usr\local\sce\ee\gcc\bin\ee-gcc.exe -IC:\usr\local\sce\ee\gcc\include\g++-2 -g -O2 -x c++ -c mat.c -o mat.o
	char command_line[512];
	snprintf(command_line, sizeof(command_line),
		CC " -I" IDIR " -I %s -g -O2 -x c++ -c %s -o %s",
		include_dir, source_file, object_file_name);

	//invoke the compiler
	int cc_status_code = system(command_line);
	if (cc_status_code != 0)
	{
		printf("Failed to compile source file '%s'\n", source_file);
		return false;
	}

#ifdef _DEBUG
	printf("Source file '%s' compiled successfully\n", source_file);
#endif

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

	//loop through every source file and compiile it
	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_iter(search_path); dir_iter != end_iter; ++dir_iter)
	{
		if (fs::is_regular_file(dir_iter->status()))
		{
			// Check if the file extension is .c or .cpp
			const char* source_file = dir_iter->path().filename().c_str();
			const char* extension = strrchr(source_file, '.');
			if (extension == NULL || (strcmp(extension, ".c") != 0 && strcmp(extension, ".cpp") != 0))
			{
				continue;
			}

			if (!CompileSourceFile(original_elf_file, source_file, source_tree_path))
			{
				continue;
			}
		}
	}

	return true;
}
