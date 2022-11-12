#pragma once
#include "CFileStream.h"

struct ProcedureHeader
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
};

class CProcedure
{
	const CFileStream& m_f;		//direct access to the file
	ProcedureHeader* m_data;	//pointer to the procedure header

	char m_name[128];			//the procedure's name
	unsigned int m_offset;		//where does the procedures's code start
	unsigned int m_size;		//how big is the procedure's code

public:
	CProcedure(const CFileStream& f):
		m_f(f),
		m_data(nullptr),
		m_name(),
		m_offset(0),
		m_size(0)
	{}

	bool Load(ProcedureHeader* data,
		unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset);

	void SetProcedureOffset(unsigned int offset);
	void SetProcedureSize(unsigned int size);

	const char* GetName() const;
	unsigned int GetProcedureOffset() const;

	void Dump() const;

	const char* GetCode() const;
	bool Compare(const CProcedure& other) const;
};
