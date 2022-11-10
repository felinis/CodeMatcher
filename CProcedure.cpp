#include "CProcedure.h"
#include "CSymbol.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

const char* CProcedure::GetName(CFileStream& f, unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset) const
{
	const CSymbol& symbol = *(CSymbol*)f.GetDataAt(section_local_symbols_offset + (file_symbols_offset + localSymbolsOffset) * sizeof(CSymbol));
	
	//make sure that the symbol is a procedure
	assert(symbol.GetType() == SymbolType::PROC || symbol.GetType() == SymbolType::STATIC_PROC);

	return symbol.GetName(f, section_local_strings_offset);
}

unsigned int CProcedure::GetProcedureOffset() const
{
	return procedure_offset;
}

void CProcedure::Dump(CFileStream& f, unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset)
{
	printf(
		"\tProcedure '%s' at offset 0x%x\n",
		GetName(f, section_local_symbols_offset, file_symbols_offset, section_local_strings_offset),
		GetProcedureOffset()
	);
}

bool CProcedure::Compare(CFileStream& f, const CProcedure& other, unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset) const
{
	//compare the names
	const char* original_name = GetName(f, section_local_symbols_offset, file_symbols_offset, section_local_strings_offset);
	const char* other_name = other.GetName(f, section_local_symbols_offset, file_symbols_offset, section_local_strings_offset);
	if (strcmp(original_name, other_name) != 0)
	{
		printf("Procedure name mismatch: '%s' vs '%s'\n", original_name, other_name);
		return false;
	}
}
