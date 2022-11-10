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
	const char* GetName(CFileStream& f, unsigned int section_local_symbols_offset,
		unsigned int file_symbols_offset, unsigned int section_local_strings_offset) const;
	unsigned int GetProcedureOffset() const;
	void Dump(CFileStream& f, unsigned int section_local_symbols_offset,
		unsigned int file_symbols_offset, unsigned int section_local_strings_offset);
	bool Compare(CFileStream& f, const CProcedure& other, unsigned int section_local_symbols_offset,
		unsigned int file_symbols_offset, unsigned int section_local_strings_offset) const;
};
