#include "CMDebugSection.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

constexpr short MDEBUG_MAGIC = 0x7009;

bool CMDebugSection::Load(CFileStream& f)
{
	//validate
	if (magic != MDEBUG_MAGIC)
	{
		printf("Invalid .mdebug magic.\n");
		return false;
	}

	//parse all the files

	return true;
}

void CMDebugSection::Dump(CFileStream& f)
{
	//dump all
#ifndef _DEBUG
	for (int i = 0; i < nobjectfiles; i++)
#else
	for (int i = 70; i < 75; i++)
#endif
	{
		const CObjectFile& object_file = GetObjectFile(f, i);
		object_file.Dump(f, localstrings_offset, procedures_offset, localsymbols_offset);
	}
}

const CObjectFile& CMDebugSection::GetObjectFile(const CFileStream& f, int index) const
{
	return *(CObjectFile*)f.GetDataAt(objectfiles_offset + sizeof(CObjectFile) * index);
}

const CObjectFile* CMDebugSection::GetObjectFile(const CFileStream& f, const char* source_file_name) const
{
	CObjectFile* object_files = (CObjectFile*)f.GetDataAt(objectfiles_offset);

	//iterate through the object files and find the one with the matching source file name
	for (int i = 0; i < nobjectfiles; i++)
	{
		const CObjectFile& object_file = object_files[i];
		const char* object_file_name = object_file.GetName(f, localstrings_offset);
		if (strcmp(object_file_name, source_file_name) == 0)
		{
			//found it
			return &object_file;
		}
	}
	
	//not found
	return nullptr;
}

bool CMDebugSection::Compare(const CFileStream& f, const CMDebugSection& other, const CFileStream& other_f) const
{
#if 0
	for (int i = 0; i < nobjectfiles; i++)
	{
		//find the corresponding object file
		const CObjectFile& original_object_file = GetObjectFile(f, i);
		const char* original_object_file_name = original_object_file.GetName(f, localstrings_offset);
		
		//try to find the other object file, it must have the same name
		const CObjectFile* candidate_object_file = other.GetObjectFile(other_f, original_object_file_name);
		if (candidate_object_file)
		{
			original_object_file.Compare(f, *candidate_object_file, localstrings_offset, procedures_offset, localsymbols_offset);
			break;
		}

		printf("Warning: Object file '%s' missing from the source tree. Skipping.\n", original_object_file_name);
	}
#else
	const char* object_file_name = "mat.c"; //TEMP! pass the name from CCompiler as a parameter!
	const CObjectFile* original_object_file = GetObjectFile(f, object_file_name); //get the mat.c object file from the ELF
	assert(original_object_file);
	const CObjectFile& compiled_object_file = other.GetObjectFile(other_f, 0); //get the mat.c object file from the .mdebug

	original_object_file->Compare(f, compiled_object_file, other_f, localstrings_offset, procedures_offset, localsymbols_offset,
		other.localstrings_offset, other.procedures_offset, other.localsymbols_offset);
#endif
	return true;
}
