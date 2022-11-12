#pragma once
#include "CFileStream.h"
#include "CProcedure.h"
#include "CSymbol.h"
#include <vector>

enum class Language : int
{
	C,
	PASCAL,
	FORTRAN,
	ASSEMBLER,
	MACHINE,
	NONE,
	STDC = 9,
	CPLUSPLUSV2
};

enum class OptimisationLevel : int
{
	O2,
	O1,
	O0,
	O3
};

struct ObjectFileHeader
{
	unsigned int adr;		/* memory address of beginning of file - adr is incorrect if there are no PDR’s for this FDR.*/
	unsigned int nameIndex;	/* file name (of source, if known) */

	unsigned int localStringsOffset;
	unsigned int stringsSize;

	unsigned int symbols_offset;
	unsigned int nsymbols;

	unsigned int ilineBase;
	unsigned int cline;

	//these two are usually empty
	unsigned int ioptBase;
	unsigned int copt;

	unsigned short procedures_first_index;
	unsigned short nprocedures;

	unsigned int iauxBase;		/* file’s auxiliary entries */
	unsigned int caux;			/* count of file’s auxiliary entries */

	unsigned int rfdBase;		/* index into the file indirect table */
	unsigned int crfd;			/* count file indirect entries */

	Language lang : 5;
	int fMerge : 1;
	int fReadin : 1;
	int wasCompiledOnBigEndianMachine : 1;
	OptimisationLevel optimisationLevel : 2;
	int signedchar : 1;			/* whether files was compiled with char being signed */
	int ipdFirstMSBits : 4;
	int cpdMSBits : 4;
	int reserved : 13;
	int cbLineOffset;
	int cbLine;
};

class CObjectFile
{
	CFileStream& m_f;
	ObjectFileHeader* m_data;				//pointer to the data in the file
	char m_name[128];						//the object file's name (actually the name of the source file)
	std::vector<CProcedure> m_procedures;	//the procedures contained in this object file
	std::vector<CSymbol> m_symbols;			//the symbols contained in this object file
	
public:
	CObjectFile(CFileStream& f):
		m_f(f),
		m_data(nullptr),
		m_name(),
		m_procedures(),
		m_symbols()
	{}

	bool Load(
		ObjectFileHeader* data,
		unsigned int section_local_symbols_offset,
		unsigned int procedures_offset,
		unsigned int section_local_strings_offset,
		unsigned int text_section_offset,
		unsigned int entry_point_virtual_address);

	const char* GetName() const;
	const CProcedure* FindProcedure(const char* name) const;

	void Dump() const;

	bool Compare(const CObjectFile& other) const;
};
