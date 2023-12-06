#include "CMDebugSection.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


//===
//	Load the MDebug section from the file stream.
//===
bool CMDebugSection::Load(
	CFileStream& f,
	MDebugHeader* data,
	unsigned int text_section_offset,
	unsigned int entry_point_virtual_address)
{
	//save the mdebug data for later use
	m_data = data;

	//validate the mdebug header magic
	constexpr short MDEBUG_MAGIC = 0x7009;
	if (m_data->magic != MDEBUG_MAGIC)
	{
		printf("Invalid mdebug magic.\n");
		return false;
	}

	//parse all the object files
	for (unsigned int i = 0; i < m_data->nobjectfiles; i++)
	{
		//get the object file header
		ObjectFileHeader* object_file_header = (ObjectFileHeader*)f.GetDataAt(m_data->objectfiles_offset + i * sizeof(ObjectFileHeader));
		if (!object_file_header)
		{
			printf("Failed to get object file header.\n");
			return false;
		}

		//create a new object file instance and load it
		CObjectFile object_file(f);
		if (!object_file.Load(
			object_file_header,
			data->localsymbols_offset,
			data->procedures_offset,
			data->localstrings_offset,
			text_section_offset,
			entry_point_virtual_address))
		{
			printf("Failed to load object file.\n");
			return false;
		}

		//add it to the cache, it will be useful for later
		m_object_files.push_back(object_file);
	}

	return true;
}

//===
//	Dump the MDebug section to stdout.
//===
void CMDebugSection::Dump(CFileStream& f)
{
	//dump all the object files
#ifdef _DEBUG
	for (int i = 109; i < 110; i++)
	{
		const CObjectFile& object_file = m_object_files[i];
#else
	for (auto object_file : m_object_files)
	{
#endif
		object_file.Dump();
	}
}

const CObjectFile* CMDebugSection::FindObjectFile(const char* goal_object_file_name) const
{
	//iterate through the object files in the elf and find the one with the matching name
	for (const CObjectFile& elf_object_file : m_object_files)
	{
		// strip extension from both strings
		char* elf_object_file_name_no_ext = strdup(elf_object_file.GetName());
		char* elf_ext = strrchr(elf_object_file_name_no_ext, '.');
		if (elf_ext)
			*elf_ext = '\0';

		char* goal_object_file_name_no_ext = strdup(goal_object_file_name);
		char* goal_ext = strrchr(goal_object_file_name_no_ext, '.');
		if (goal_ext)
			*goal_ext = '\0';

#ifdef _DEBUG
		printf("Comparing '%s' with '%s'\n", elf_object_file_name_no_ext, goal_object_file_name_no_ext);
#endif
		if (strcmp(elf_object_file_name_no_ext, goal_object_file_name_no_ext) == 0)
		{
			//found it
			return &elf_object_file;
		}
	}

	//not found
	return nullptr;
}

bool CMDebugSection::MatchObjectFile(const CMDebugSection& other, const char* object_file_name) const
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
	const CObjectFile* original_object_file = FindObjectFile(object_file_name); //get the object file from the ELF
	if (!original_object_file)
	{
		printf("Could not find object file for '%s' in the ELF\n", object_file_name);
		return false;
	}

	//get the object file from the .mdebug, it's always the first one since it is the only one
	const CObjectFile& compiled_object_file = other.m_object_files[0];

	original_object_file->Compare(compiled_object_file);
#endif
	return true;
}
