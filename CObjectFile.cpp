#include "CObjectFile.h"
#include <stdio.h>
#include <unordered_set>

const char* CObjectFile::GetName(CFileStream& f, unsigned int section_local_strings_offset) const
{
	return (char*)f.GetDataAt(section_local_strings_offset + nameIndex + localStringsOffset);
}

const CProcedure& CObjectFile::GetProcedure(CFileStream& f, unsigned int procedures_offset, int index) const
{
	return *(CProcedure*)f.GetDataAt(procedures_offset + sizeof(CProcedure) * (procedures_first_index + index));
}

const CSymbol& CObjectFile::GetSymbol(CFileStream& f, unsigned int section_local_symbols_offset, int index) const
{
	return *(CSymbol*)f.GetDataAt(section_local_symbols_offset + sizeof(CSymbol) * (symbolsOffset + index));
}

void CObjectFile::Dump(CFileStream& f, unsigned int section_local_strings_offset,
	unsigned int procedures_offset, unsigned int section_local_symbols_offset) const
{
	printf("Object file '%s'\n", GetName(f, section_local_strings_offset));

	//create a set of include files
	//we use a set instead of an array because we want to avoid duplicates
	std::unordered_set<const char*> include_files;
	
	//print all the procedures
//	for (unsigned short i = 0; i < nprocedures; i++)
//	{
//		CProcedure* proc = GetProcedure(f, procedures_offset, i);
//		proc->Dump(f, section_local_symbols_offset, symbolsOffset, section_local_strings_offset);
//	}
	
	//print all the symbols
	for (unsigned int i = 0; i < numSymbols; i++)
	{
		const CSymbol& symbol = GetSymbol(f, section_local_symbols_offset, i);
		bool is_stab = (symbol.GetIndex() & 0xfff00) == 0x8F300; //WARNING: works for sly1 proto but untested for others...
		if (is_stab)
		{
			bool is_label = symbol.GetType() == SymbolType::LABEL;
			if (is_label)
			{
				const char* name = symbol.GetName(f, section_local_strings_offset);
				//check if there is a '.h' at the end of the name
				const char* dot = strrchr(name, '.');
				if (dot && strcmp(dot, ".h") == 0)
				{
					//we have a header file
					include_files.insert(name);
				}
			}
		}
		else
		{
			bool is_procedure = (symbol.GetType() == SymbolType::PROC || symbol.GetType() == SymbolType::STATIC_PROC);
			if (is_procedure)
				symbol.Dump(f, section_local_strings_offset);
		}
	}

	//print all the include files
	for (const char* include_file : include_files)
		printf("\t#include \"%s\"\n", include_file);
}

bool CObjectFile::Compare(CFileStream& f, const CObjectFile& other, unsigned int section_local_strings_offset,
	unsigned int procedures_offset, unsigned int section_local_symbols_offset) const
{
	//compare every procedure
	for (unsigned short i = 0; i < nprocedures; i++)
	{
		const CProcedure& original_proc = GetProcedure(f, procedures_offset, i);
		const CProcedure& other_proc = other.GetProcedure(f, procedures_offset, i);

		return original_proc.Compare(f, other_proc, section_local_symbols_offset, symbolsOffset, section_local_strings_offset);
	}
}
