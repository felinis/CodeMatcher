#include "CSymbol.h"
#include <stdio.h>
extern "C"
{
#include "demangler/demangle.h"
}
#include <malloc.h>
#include <string.h>

//===
//	Takes a mangled function name as input and returns the demangled name.
//	Examples from the Sly1 prototype:
//	InitCoin__FP4COIN -> InitCoin(COIN*)
//	CpdprizeFindSwDprizes__FP2SW3CIDiPP6DPRIZE -> CpdprizeFindSwDprizes(SW *, CID, int, DPRIZE **)
//===
static char s_demangled_function_name[256];
static const char* DemangleName(const char* mangled_function_name)
{
	char* demangled_function_name;
	demangled_function_name = cplus_demangle(mangled_function_name, DMGL_PARAMS | DMGL_ANSI);
	if (demangled_function_name)
	{
		strncpy(s_demangled_function_name, demangled_function_name, sizeof(s_demangled_function_name));
		free(demangled_function_name);
		return s_demangled_function_name;
	}

	return mangled_function_name; //could not demangle, just return the original name
}

bool CSymbol::Load(SymbolHeader* data, unsigned int section_local_strings_offset)
{
	//save the symbol header for later use
	m_data = data;

	//save the name
	const char* mangled_name = (char*)m_f.GetDataAt(section_local_strings_offset + data->stringIndex);
	strncpy(m_name, DemangleName(mangled_name), sizeof(m_name));

	return true;
}

unsigned int CSymbol::GetValue() const
{
	return m_data->value;
}

SymbolType CSymbol::GetType() const
{
	return m_data->type;
}

const char* CSymbol::GetNamedType() const
{
	switch (m_data->type)
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

const char* CSymbol::GetName() const
{
	return m_name;
}

unsigned int CSymbol::GetIndex() const
{
	return m_data->indexSymbolOrAux;
}

void CSymbol::Dump() const
{
	printf(
		"\t%s '%s' at offset 0x%x\n",
		GetNamedType(),
		m_name,
		m_data->value
	);
}
