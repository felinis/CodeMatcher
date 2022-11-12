#include "CElfFile.h"
#include <stdio.h>
#include <string.h>

ElfSectionHeader* CElfFile::GetSectionHeaders()
{
	//get a pointer to the section headers
	return (ElfSectionHeader*)m_f.GetDataAt(m_header->section_headers_offset);
}

const char* CElfFile::GetSectionNamesOffset()
{
	ElfSectionHeader* sections = GetSectionHeaders();

	//get the strings section (it contains the names of all the other sections)
	ElfSectionHeader* strings_section = &sections[m_header->string_section_index];
	return (char*)m_f.GetDataAt(strings_section->offset);
}

bool CElfFile::Load(const char* file_name)
{
	if (!CFileStream::DoesFileExist(file_name))
	{
		printf("File '%s' does not exist.\n", file_name);
		return false;
	}

	if (!m_f.Open(file_name))
	{
		printf("Failed to open file '%s'.\n", file_name);
		return false;
	}

	//get the ELF header
	m_header = (ElfHeader*)m_f.GetDataAt(0);

	//validate it
	if (m_header->magic != 'FLE\177' ||
//		m_header->fileType != ElfFileType::EXECUTABLE ||
		m_header->architecture != ElfArchitecture::MIPS ||
		m_header->size != sizeof(ElfHeader) ||
		m_header->section_size != sizeof(ElfSectionHeader) ||
		m_header->nsections > 60)
	{
		printf("This is not a valid PlayStation 2 ELF.\n");
		return false;
	}

	ElfSectionHeader* sections = GetSectionHeaders();
	const char* section_names = GetSectionNamesOffset();

	//find the .text section
	int text_section_index = -1;
	for (int i = 0; i < m_header->nsections; i++)
	{
		ElfSectionHeader& section = sections[i];

		//compare the names
		const char* name = section_names + section.name_index;
		if (strcmp(name, ".text") == 0)
		{
			//found it
			text_section_index = i;
			break;
		}
	}

	if (text_section_index == -1)
	{
		printf("Could not find the .text section.\n");
		return false;
	}

	ElfSectionHeader* text_header = (ElfSectionHeader*)m_f.GetDataAt(
		m_header->section_headers_offset + text_section_index * sizeof(ElfSectionHeader)
	);

	m_text_section_offset = text_header->offset;
	m_entry_point_virtual_offset = m_header->entryPointVirtualAddress;

	return true;
}
