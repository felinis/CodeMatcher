#include "CProcedure.h"
#include "CSymbol.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

bool CProcedure::Load(ProcedureHeader* data,
	unsigned int section_local_symbols_offset, unsigned int file_symbols_offset, unsigned int section_local_strings_offset)
{
	//save the procedure header for later use
	m_data = data;

	//parse the symbol
	SymbolHeader* symbol_header =
		(SymbolHeader*)m_f.GetDataAt(section_local_symbols_offset + (file_symbols_offset + data->localSymbolsOffset) * sizeof(SymbolHeader));
	assert(symbol_header->type == SymbolType::PROC || symbol_header->type == SymbolType::STATIC_PROC); //make sure that the symbol is a procedure
	//TODO

	//load it
	CSymbol symbol(m_f);
	if (!symbol.Load(symbol_header, section_local_strings_offset))
	{
		printf("Failed to load symbol.\n");
		return false;
	}
	
	//save the name
	strncpy(m_name, symbol.GetName(), sizeof(m_name));
	
	//we will fill the procedure offset and size later on, we do not have access to that information yet

	return true;
}

void CProcedure::SetProcedureOffset(unsigned int offset)
{
	m_offset = offset;
}

void CProcedure::SetProcedureSize(unsigned int size)
{
	m_size = size;
}

const char* CProcedure::GetName() const
{
	return m_name;
}

unsigned int CProcedure::GetProcedureOffset() const
{
	return m_offset;
}

void CProcedure::Dump() const
{
	printf(
		"\tProcedure '%s' at offset 0x%x\n",
		m_name,
		m_offset
	);
}

const char* CProcedure::GetCode() const
{
	return (char*)m_f.GetDataAt(m_offset);
}

bool CProcedure::Compare(const CProcedure& other) const
{
	//compare the two procedure's codes
	if (m_size == other.m_size)
	{
		int offset = memcmp(GetCode(), other.GetCode(), m_size);
		if (offset == 0)
		{
			printf("MATCHING - '%s'\n", m_name);
			return true;
		}
	}
	
	printf("NOT MATCHING - '%s'\n", m_name);
	return false;
}
