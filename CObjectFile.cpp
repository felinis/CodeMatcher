#include "CObjectFile.h"
#include <stdio.h>
#include <unordered_set>

bool CObjectFile::Load(
	ObjectFileHeader* data,
	unsigned int section_local_symbols_offset,
	unsigned int procedures_offset,
	unsigned int section_local_strings_offset,
	unsigned int text_section_offset,
	unsigned int entry_point_virtual_address)
{
	//save the object file header for later use
	m_data = data;

	//get the name
	strncpy(m_name, (char*)m_f.GetDataAt(section_local_strings_offset + data->nameIndex + data->localStringsOffset), sizeof(m_name));

	//parse all the procedures
	for (int i = 0; i < data->nprocedures; i++)
	{
		//get the procedure header
		ProcedureHeader* procedure_header =
			(ProcedureHeader*)m_f.GetDataAt(procedures_offset + (data->procedures_first_index + i) * sizeof(ProcedureHeader));
		if (!procedure_header)
		{
			printf("Failed to get procedure header.\n");
			return false;
		}

		//create a new procedure instance and load it
		CProcedure procedure(m_f);
		if (!procedure.Load(procedure_header, section_local_symbols_offset, data->symbols_offset, section_local_strings_offset))
		{
			printf("Failed to load procedure.\n");
			return false;
		}

		//add it to the cache, it will be useful for later
		m_procedures.push_back(procedure);
	}

	//parse all the symbols
	unsigned int last_procedure_symbol_index = 0;
	unsigned int last_procedure_string_index = 0;
	for (unsigned int i = 0; i < data->nsymbols; i++)
	{
		//get the symbol header
		SymbolHeader* symbol_header =
			(SymbolHeader*)m_f.GetDataAt(section_local_symbols_offset + (data->symbols_offset + i) * sizeof(SymbolHeader));
		
		//if this is a procedure symbol, get the procedure offset and size from it
		if ((symbol_header->type == SymbolType::PROC || symbol_header->type == SymbolType::STATIC_PROC) &&
			symbol_header->storageClass == StorageClass::TEXT)
		{
			//get the procedure offset, also make it start from the beginning of the .text section
			unsigned int procedure_offset = text_section_offset + symbol_header->value - entry_point_virtual_address;
			m_procedures[last_procedure_symbol_index].SetProcedureOffset(procedure_offset);
			last_procedure_string_index = symbol_header->stringIndex;
		}
		else if (symbol_header->type == SymbolType::END &&
			symbol_header->storageClass == StorageClass::TEXT &&
			symbol_header->stringIndex == last_procedure_string_index)
		{
			//this is a procedure's end, get the size from it
			m_procedures[last_procedure_symbol_index].SetProcedureSize(symbol_header->value);
			last_procedure_symbol_index++;
		}

		//create a new symbol instance and load it
		CSymbol symbol(m_f);
		if (!symbol.Load(symbol_header, section_local_strings_offset))
		{
			printf("Failed to load symbol.\n");
			return false;
		}

		//add it to the cache, it will be useful for later
		m_symbols.push_back(symbol);
	}

	return true;
}

const char* CObjectFile::GetName() const
{
	return m_name;
}

const CProcedure* CObjectFile::FindProcedure(const char* name) const
{
	//iterate through the procedures and find the one with the matching name
	for (const CProcedure& procedure : m_procedures)
	{
		if (strcmp(procedure.GetName(), name) == 0)
			return &procedure;
	}
	
	return nullptr; //not found
}

void CObjectFile::Dump() const
{
	printf("Object file '%s'\n", m_name);

	//create a set of include files
	//we use a set instead of an array because we want to avoid duplicates
	std::unordered_set<const char*> include_files;
	
	//print all the procedures
//	for (unsigned short i = 0; i < nprocedures; i++)
//	{
//		CProcedure* proc = GetProcedure(f, procedures_offset, i);
//		proc->Dump(f, section_local_symbols_offset, symbolsOffset, section_local_strings_offset);
//	}
	
	//print all the symbols
	for (const CSymbol& symbol : m_symbols)
	{
		bool is_stab = (symbol.GetIndex() & 0xfff00) == 0x8F300; //WARNING: works for sly1 proto but untested for others...
		if (is_stab)
		{
			bool is_label = symbol.GetType() == SymbolType::LABEL;
			if (is_label)
			{
				const char* name = symbol.GetName();
				//check if there is a '.h' at the end of the name
				const char* dot = strrchr(name, '.');
				if (dot && strcmp(dot, ".h") == 0)
				{
					//we found a header file, add it to the set
					include_files.insert(name);
				}
			}
		}
		else
		{
			bool is_procedure = (symbol.GetType() == SymbolType::PROC || symbol.GetType() == SymbolType::STATIC_PROC);
			if (is_procedure)
				symbol.Dump();
		}
	}

	//print all the include files
	for (const char* include_file : include_files)
		printf("\t#include \"%s\"\n", include_file);
}

bool CObjectFile::Compare(const CObjectFile& other) const
{
	//for every procedure of this CObjectFile
	for (unsigned int i = 0; i < m_procedures.size(); i++)
	{
		const CProcedure& procedure = m_procedures[i];
		const char* procedure_name = procedure.GetName();

		//try to find the procedure with the same name in the other CObjectFile
		//if there is, compare the two procedures, if not then inform the user
		const CProcedure* other_proc = other.FindProcedure(procedure_name);
		if (other_proc)
			procedure.Compare(*other_proc);
		else
			printf("PLEASE DECOMP - '%s'\n", procedure_name);
	}

	return true;
}
