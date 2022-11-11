#include "CObjectFile.h"
#include <stdio.h>
#include <unordered_set>
#include <assert.h>

const char* CObjectFile::GetName(const CFileStream& f, unsigned int section_local_strings_offset) const
{
	return (char*)f.GetDataAt(section_local_strings_offset + nameIndex + localStringsOffset);
}

const CProcedure& CObjectFile::GetProcedure(const CFileStream& f, unsigned int procedures_offset, int index) const
{
	return *(CProcedure*)f.GetDataAt(procedures_offset + sizeof(CProcedure) * (procedures_first_index + index));
}

const CProcedure* CObjectFile::GetProcedure(const CFileStream& f, unsigned int procedures_offset,
	const char* procedure_name, unsigned int section_local_symbols_offset,
	unsigned int file_symbols_offset, unsigned int section_local_strings_offset) const
{
	CProcedure* procedures = (CProcedure*)f.GetDataAt(procedures_offset);

	//iterate through the procedures and find the one with the matching name
	for (int i = 0; i < nprocedures; i++)
	{
		const CProcedure& procedure = procedures[procedures_first_index + i];
		const char* current_procedure_name = procedure.GetName(f, section_local_symbols_offset, file_symbols_offset, section_local_strings_offset);
		if (strcmp(procedure_name, current_procedure_name) == 0)
		{
			//found it
			return &procedure;
		}
	}

	//not found
	return nullptr;
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

bool CObjectFile::Compare(const CFileStream& f, const CObjectFile& other, const CFileStream& other_f,
	unsigned int section_local_strings_offset, unsigned int procedures_offset, unsigned int section_local_symbols_offset,
	unsigned int other_section_local_strings_offset, unsigned int other_proceduers_offset, unsigned int other_section_local_symbols_offset) const
{
	//for every procedure of this CObjectFile
	for (unsigned short i = 0; i < nprocedures; i++)
	{
		const CProcedure& proc = GetProcedure(f, procedures_offset, i);
		
		//HACKHACK: may not exist, the actual solution is to parse the CSymbols and find the SymbolType::END for that procedure, which will contain its size.
		const CProcedure& next_proc = GetProcedure(f, procedures_offset, i + 1);
		assert(&next_proc); //TEMPORARY!!!

		const char* procedure_name = proc.GetName(f, section_local_symbols_offset, symbolsOffset, section_local_strings_offset);
		unsigned int procedure_size = next_proc.GetProcedureOffset() - proc.GetProcedureOffset();

		//try to find the procedure with the same name in the other CObjectFile
		const CProcedure* other_proc = other.GetProcedure(other_f, other_proceduers_offset, procedure_name, other_section_local_symbols_offset, other.symbolsOffset, other_section_local_strings_offset);
		if (other_proc)
			proc.Compare(f, *other_proc, procedure_size, section_local_symbols_offset, symbolsOffset, section_local_strings_offset); //if there is, compare the two procedures
		else
			printf("Procedure '%s' has not been decompiled yet\n", procedure_name); //if there is not, inform the user
	}

	return true;
}
