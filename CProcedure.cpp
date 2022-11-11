#include "CProcedure.h"
#include "CSymbol.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

const char* CProcedure::GetName(const CFileStream& f, unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset) const
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

bool CProcedure::Compare(const CFileStream& f, const CProcedure& other, unsigned int procedure_size, unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset) const
{
	//compare the opcodes
	int offset = memcmp(f.GetDataAt(procedure_offset), f.GetDataAt(other.procedure_offset), procedure_size);
	if (offset == 0)
	{
		printf("Procedure '%s' IS MATCHING :)\n", GetName(f, section_local_symbols_offset, file_symbols_offset, section_local_strings_offset));
		return true;
	}
	else
	{
		printf("Procedure '%s' IS NOT MATCHING :(, different code at offset %d\n", GetName(f, section_local_symbols_offset, file_symbols_offset, section_local_strings_offset), offset);
		return false;
	}
}
