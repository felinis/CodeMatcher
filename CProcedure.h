#pragma once
#include "CFileStream.h"
#ifdef EXPERIMENTAL_PROCEDURES_ARGUMENT_NAMES
#include <vector>
#include <string>
#endif

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
	unsigned int m_offset;		//where does the procedures's code start, from the beginning of the ELF file
	unsigned int m_virtual_offset;	//where does the procedure's code start, in Ghidra
	unsigned int m_size;		//how big is the procedure's code
#ifdef EXPERIMENTAL_PROCEDURES_ARGUMENT_NAMES
	std::vector<std::string> m_parameters;	//this procedure's parameters/arguments
#endif

public:
	CProcedure(const CFileStream& f):
		m_f(f),
		m_data(nullptr),
		m_name(),
		m_offset(0),
		m_virtual_offset(0),
		m_size(0)
#ifdef EXPERIMENTAL_PROCEDURES_ARGUMENT_NAMES
		m_parameters()
#endif
	{}

	bool Load(ProcedureHeader* data,
		unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset);

#ifdef EXPERIMENTAL_PROCEDURES_ARGUMENT_NAMES
	void AddParameter(const char* parameter_name);
#endif
	void SetProcedureOffset(unsigned int offset);
	void SetProcedureVirtualOffset(unsigned int offset);
	void SetProcedureSize(unsigned int size);

	const char* GetName() const;
	unsigned int GetProcedureVirtualOffset() const;

	void Dump() const;

	const char* GetCode() const;
	bool Compare(const CProcedure& other) const;
};
