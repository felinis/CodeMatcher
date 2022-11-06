#pragma once
#include "CFileStream.h"

class CProcedure
{
	unsigned int procedure_offset;
	int localSymbolsOffset;
	int lineNumbersOffset;

	int registerMask;
	int registerOffset;

	int iopt;
	int fregmask;
	int fregoffset;
	int frameoffset;
	short framereg;
	short pcreg;

	int lowerLineNumber;
	int higherLineNumber;
	int cb_line_offset;
public:
	bool Load(CFileStream& f);

	const char* GetName(CFileStream& f);
	unsigned int GetProcedureOffset();
	void Dump(CFileStream& f);
};
