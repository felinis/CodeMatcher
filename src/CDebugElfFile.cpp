#include "CDebugElfFile.h"
#include <stdio.h>
#include <string.h>

bool CDebugElfFile::Load(const char* file_name)
{
	if (!CElfFile::Load(file_name)) //base class load
		return false;
	
	ElfSectionHeader* sections = GetSectionHeaders();
	const char* section_names = GetSectionNamesOffset();

	//find the mdebug section
	int mdebug_section_index = -1;
	for (int i = 0; i < m_header->nsections; i++)
	{
		ElfSectionHeader& section = sections[i];

		//compare the names
		const char* name = section_names + section.name_index;
		if (strcmp(name, ".mdebug") == 0)
		{
			//found it
			mdebug_section_index = i;
			break;
		}
	}

	if (mdebug_section_index == -1)
	{
		printf("Could not find the .mdebug section.\n");
		return false;
	}

	ElfSectionHeader* mdebug_header = (ElfSectionHeader*)m_f.GetDataAt(
		m_header->section_headers_offset + mdebug_section_index * sizeof(ElfSectionHeader)
	);
	if (!m_mdebug_section.Load(
		m_f,
		(MDebugHeader*)m_f.GetDataAt(mdebug_header->offset),
		m_text_section_offset,
		m_entry_point_virtual_offset))
	{
		printf("Failed to load the .mdebug section.\n");
		return false;
	}

	return true;
}

void CDebugElfFile::Dump()
{
	m_mdebug_section.Dump(m_f);
}

//===
//	Compares two ELF files' procedures.
//===
bool CDebugElfFile::MatchObjectFile(const CDebugElfFile& object_file, const char* object_file_name) const
{
	return m_mdebug_section.MatchObjectFile(object_file.m_mdebug_section, object_file_name);
}
