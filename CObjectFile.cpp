#include "CObjectFile.h"
#include <stdio.h>

//TODO: remove
bool CObjectFile::Load(CFileStream& f)
{
	return true;
}

const char* CObjectFile::GetName(CFileStream& f, unsigned int section_local_strings_offset)
{
	return (char*)f.GetDataAt(section_local_strings_offset + nameIndex + localStringsOffset);
}

CProcedure* CObjectFile::GetProcedure(CFileStream& f, unsigned int procedures_offset, int index)
{
	return (CProcedure*)f.GetDataAt(procedures_offset + sizeof(CProcedure) * (procedures_first_index + index));
}

CSymbol* CObjectFile::GetSymbol(CFileStream& f, unsigned int section_local_symbols_offset, int index)
{
	return (CSymbol*)f.GetDataAt(section_local_symbols_offset + sizeof(CSymbol) * (symbolsOffset + index));
}

void CObjectFile::Dump(CFileStream& f, unsigned int section_local_strings_offset,
	unsigned int procedures_offset, unsigned int section_local_symbols_offset)
{
	printf("Object file '%s'\n", GetName(f, section_local_strings_offset));
	
	//print all the procedures
//	for (unsigned short i = 0; i < nprocedures; i++)
//	{
//		CProcedure* proc = GetProcedure(f, procedures_offset, i);
//		proc->Dump(f, section_local_symbols_offset, symbolsOffset, section_local_strings_offset);
//	}
	
	//print all the procedures
	for (unsigned int i = 0; i < numSymbols; i++)
	{
		CSymbol& symbol = *(CSymbol*)GetSymbol(f, section_local_symbols_offset, i);
		bool is_stab = symbol.GetIndex() > 586400; //WARNING: works for sly1 proto but untested for others...
		if (!is_stab)
		{
			bool is_procedure = (symbol.GetType() == SymbolType::PROC || symbol.GetType() == SymbolType::STATIC_PROC);
			if (is_procedure) //for now, only print procedures, later on we'll print the include files here...
				symbol.Dump(f, section_local_strings_offset);
		}
	}
}
