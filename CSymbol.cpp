#include "CSymbol.h"
#include <stdio.h>
extern "C"
{
#include "demangler/demangle.h"
}
#include <malloc.h>
#include <string.h>

const char* CSymbol::GetName(CFileStream& f, unsigned int section_local_strings_offset) const
{
	return (char*)f.GetDataAt(section_local_strings_offset + stringIndex);
}

unsigned int CSymbol::GetValue() const
{
	return value;
}

SymbolType CSymbol::GetType() const
{
	return type;
}

const char* CSymbol::GetNamedType() const
{
	switch (type)
	{
	case SymbolType::GLOBAL:
		return "GLOBAL";
	case SymbolType::STATIC:
		return "STATIC";
	case SymbolType::PARAM:
		return "PARAM";
	case SymbolType::LOCAL:
		return "LOCAL";
	case SymbolType::LABEL:
		return "LABEL";
	case SymbolType::PROC:
		return "Procedure";
	case SymbolType::BLOCK:
		return "BLOCK";
	case SymbolType::END:
		return "END";
	case SymbolType::MEMBER:
		return "MEMBER";
	case SymbolType::TYPEDEF:
		return "TYPEDEF";
	case SymbolType::FILE:
		return "FILE";
	case SymbolType::STATIC_PROC:
		return "Static Procedure";
	case SymbolType::CONSTANT:
		return "CONSTANT";
	default:
		return "UNKNOWN";
	}
}

StorageClass CSymbol::GetStorageClass() const
{
	return storageClass;
}

unsigned int CSymbol::GetIndex() const
{
	return indexSymbolOrAux;
}

//===
//	Takes a mangled function name as input and returns the demangled name.
//	Examples from the Sly1 prototype:
//	InitCoin__FP4COIN -> InitCoin(COIN*)
//	CloneDprize__FP6DPRIZET0 -> CloneDprize(DPRIZE*, DPRIZE*)
//	SetDprizeDprizes__FP6DPRIZE7DPRIZES -> SetDprizeDprizes(DPRIZE*, DPRIZES)
//	CpdprizeFindSwDprizes__FP2SW3CIDiPP6DPRIZE -> CpdprizeFindSwDprizes(SW *, CID, int, DPRIZE **)
//===
static char s_demangled_function_name[256];
static const char* DemangleName(const char* mangled_function_name)
{
	char* demangled_function_name;
	demangled_function_name = cplus_demangle(mangled_function_name, DMGL_PARAMS | DMGL_ANSI);
	if (demangled_function_name)
	{
		strncpy(s_demangled_function_name, demangled_function_name, 256);
		free(demangled_function_name);
		return s_demangled_function_name;
	}

	return mangled_function_name;
}

void CSymbol::Dump(CFileStream& f, unsigned int section_local_strings_offset) const
{
	const char* name;
//	if (type == SymbolType::PROC)
		name = DemangleName(GetName(f, section_local_strings_offset));
//	else
//		name = GetName(f, section_local_strings_offset);

	printf(
		"\t%s '%s' at offset 0x%x\n",
		GetNamedType(),
		name,
		GetValue()
	);
}
