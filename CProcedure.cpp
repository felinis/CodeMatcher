#include "CProcedure.h"
#include "CSymbol.h"
#include <stdio.h>

bool CProcedure::Load(CFileStream& f)
{
	return true;
}

const char* CProcedure::GetName(CFileStream& f)
{
	const CSymbol& symbol = *(CSymbol*)f.GetDataAt(mHeader->localSymbolsOffset + (fileDesc.symbolsOffset + localSymbolsOffset) * sizeof(CSymbol));
	return symbol.GetName(fileDesc);
}

unsigned int CProcedure::GetProcedureOffset()
{
	return procedure_offset;
}

void CProcedure::Dump(CFileStream& f)
{
	printf("\tProcedure '%s' at offset 0x%x\n", GetName(f), GetProcedureOffset());
}
