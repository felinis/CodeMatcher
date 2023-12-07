#include "CProcedure.h"
#include "CSymbol.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Console.h"

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
#ifdef EXPERIMENTAL_PROCEDURES_ARGUMENT_NAMES
void CProcedure::AddParameter(const char* parameter_name)
{
	m_parameters.push_back(parameter_name);
}
#endif
void CProcedure::SetProcedureOffset(unsigned int offset)
{
	m_offset = offset;
}

void CProcedure::SetProcedureVirtualOffset(unsigned int offset)
{
	m_virtual_offset = offset;
}

void CProcedure::SetProcedureSize(unsigned int size)
{
	m_size = size;
}

const char* CProcedure::GetName() const
{
	return m_name;
}

unsigned int CProcedure::GetProcedureVirtualOffset() const
{
	return m_virtual_offset;
}

void CProcedure::Dump() const
{
	printf(
		"\tProcedure '%s' at offset 0x%x",
		m_name,
		m_virtual_offset
	);
#ifdef EXPERIMENTAL_PROCEDURES_ARGUMENT_NAMES
	//print parameters
	if (m_parameters.size() > 0)
	{
		printf(" with parameters: ");
		for (unsigned int i = 0; i < m_parameters.size(); i++)
		{
			printf("%s", m_parameters[i].c_str());
			if (i < m_parameters.size() - 1)
				printf(", ");
		}
	}
#endif
}

const char* CProcedure::GetCode() const
{
	return (char*)m_f.GetDataAt(m_offset);
}

bool CProcedure::Compare(const CProcedure& other) const
{
	//the procedures in sly proto ELF start at "text_section + 8 bytes"
	constexpr int sly_procedures_offset = 8;

	//compare the two procedure's codes
	bool same_size = false;
	if (m_size == other.m_size)
	{
		same_size = true;

		int offset = memcmp(GetCode() + sly_procedures_offset, other.GetCode(), m_size);
		if (offset == 0)
		{
			Console::SetColor(Color::Green);
			printf("MATCHING -------- '%s'\n", m_name);
			Console::ResetColor();
			return true;
		}
	}

	Console::SetColor(Color::Red);
	printf("NOT MATCHING ---- '%s' ", m_name);
	if (same_size)
		printf("(same size)\n");
	else
		printf("(size %u, should be %u)\n", other.m_size, m_size);
	Console::ResetColor();

	return false;
}
