#include "CProcedure.h"
#include "CSymbol.h"
#include <stdio.h>
#include <assert.h>

bool CProcedure::Load(CFileStream& f)
{
	return true;
}

const char* CProcedure::GetName(CFileStream& f, unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset)
{
	const CSymbol& symbol = *(CSymbol*)f.GetDataAt(section_local_symbols_offset + (file_symbols_offset + localSymbolsOffset) * sizeof(CSymbol));
	
	//make sure that the symbol is a procedure
	assert(symbol.GetType() == SymbolType::PROC || symbol.GetType() == SymbolType::STATIC_PROC);

	return symbol.GetName(f, section_local_strings_offset);
}

unsigned int CProcedure::GetProcedureOffset()
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

	//dump all the symbols
	
}
