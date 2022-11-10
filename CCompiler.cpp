#include "CCompiler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <Windows.h>
#include "CFileStream.h"

static bool Compile(CDebugElfFile& original_elf_file, const char* source_file)
{
	//form the compiled object file name like so: temp/file.o
	char object_file_name[128];
	snprintf(object_file_name, sizeof(object_file_name), "temp/%s.o", source_file);

	//run mipscc.exe to compile the source file
	char command_line[128];
	snprintf(command_line, sizeof(command_line), "mipscc.exe -c -o %s %s", object_file_name, source_file);
	int code = system(command_line);
	if (code != 0)
	{
		printf("Failed to compile source file '%s'\n", source_file);
		return false;
	}

	//the object file was compiled successfully, now open it
	CDebugElfFile object_file; //the object file must also have an mdebug section
	if (!object_file.Load(object_file_name))
	{
		printf("Failed to load object file '%s'\n", object_file_name);
		return false;
	}

	//read the compiled code's opcodes
//	const char* compiled_opcodes = (const char*)object_file.GetDataAt(0); //TODO: wrong offset

	//get the same procedure's opcodes from the original ELF
//	const char* original_opcodes = original_elf_file.GetProcedureCode(procedure_name);

	//compare them, if they are the same, we have a MATCH!
//	if (memcmp(compiled_opcodes, original_opcodes, 0x100) == 0)
//	{
//		printf("MATCH: %s\n", source_file);
//		return true;
//	}

	//for every procedure in the original ELF, find the same procedure in the compiled object file
	bool is_matching = (original_elf_file == object_file);

	return true;
}

bool CCompiler::CompileAndMatch(CDebugElfFile& original_elf_file, const char* source_tree_path)
{
	//find all of the .c source files in the given source tree path
	char search_path[128];
	snprintf(search_path, sizeof(search_path), "%s\\*.c", source_tree_path);

	WIN32_FIND_DATA find_data;
	HANDLE find_handle = FindFirstFile(search_path, &find_data);
	if (find_handle == INVALID_HANDLE_VALUE)
		return false;
	
	//loop through every source file and compile it
	do
	{
		const char* source_file = find_data.cFileName;
		
		if (!Compile(original_elf_file, source_file))
		{
			FindClose(find_handle);
			return false;
		}
	} while (FindNextFile(find_handle, &find_data));
	FindClose(find_handle);

	return true;
}
