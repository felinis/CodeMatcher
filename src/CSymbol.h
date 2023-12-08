#pragma once
#include "CFileStream.h"

enum class SymbolType : int
{
	NONE,
	GLOBAL,				//NOTE: this is used only in AUXs table in sly1 prototype
	STATIC,
	PARAM,				//NOTE: this is never used in sly1 prototype
	LOCAL,				//NOTE: this is never used in sly1 prototype
	LABEL,
	PROC,
	BLOCK,				//NOTE: this is never used in sly1 prototype
	END,
	MEMBER,				//NOTE: this is never used in sly1 prototype
	TYPEDEF,			//NOTE: this is never used in sly1 prototype
	FILE,
	STATIC_PROC = 14,
	CONSTANT			//NOTE: this is never used in sly1 prototype
};

enum class StorageClass : int
{
	NONE,
	TEXT,
	DATA,
	BSS,
	REGISTER,
	ABS,
	UNDEFINED,
	CDB_LOCAL,
	BITS,
	DBX,
	REG_IMAGE,
	INFO,
	USER_STRUCT,
	SDATA,
	SBSS,
	RDATA,
	VAR,
	COMMON,
	SCOMMON,
	VAR_REGISTER,
	VARIANT,
	SUNDEFINED,
	INIT,
	BASED_VAR,
	XDATA,
	PDATA,
	FINI,
	NON_GP,
//	VIRTUAL,
//	PURE,
//	FRIEND,
//	CLASS,
//	STATIC,
//	VIRTUAL_STATIC,
//	MAX = 32
};

struct SymbolHeader
{
	unsigned int stringIndex;
	unsigned int value;
	SymbolType type : 6;
	StorageClass storageClass : 5;
	unsigned int reserved : 1;
	unsigned int indexSymbolOrAux : 20;
};

class CSymbol
{
	const CFileStream& m_f;
	SymbolHeader* m_data;
	char m_name[128];

public:
	CSymbol(const CFileStream& f):
		m_f(f),
		m_data(nullptr),
		m_name()
	{}
	
	bool Load(SymbolHeader* data, unsigned int section_local_strings_offset);

	const char* GetName() const;
	unsigned int GetValue() const;
	SymbolType GetType() const;
	const char* GetNamedType() const;
	StorageClass GetStorageClass() const;
	unsigned int GetIndex() const;
	void Dump() const;
};
