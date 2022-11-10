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

	return true;
}
