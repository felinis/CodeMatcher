#pragma once
#include "CFileStream.h"
#include "CProcedure.h"

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

class CObjectFile
{
	unsigned int adr;		/* memory address of beginning of file - adr is incorrect if there are no PDR’s for this FDR.*/
	unsigned int nameIndex;	/* file name (of source, if known) */

	unsigned int localStringsOffset;
	unsigned int stringsSize;

	unsigned int symbolsOffset;
	unsigned int numSymbols;

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
	
public:
	//TODO: remove
	bool Load(CFileStream& f);

	const char* GetName(CFileStream& f, unsigned int section_local_strings_offset);
	CProcedure* GetProcedure(CFileStream& f, unsigned int procedures_offset, int index);
	void Dump(CFileStream& f, unsigned int section_local_strings_offset, unsigned int procedures_offset);
};
