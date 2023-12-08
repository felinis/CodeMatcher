#include "CObjectFile.h"
#include <stdio.h>
#include <unordered_set>
#include "Console.h"

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
			m_procedures[last_procedure_symbol_index].SetProcedureVirtualOffset(symbol_header->value);
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

#ifdef EXPERIMENTAL_PROCEDURES_ARGUMENT_NAMES
			//skip one symbol
			i++;

			const char* name = m_procedures[last_procedure_symbol_index].GetName();

			//count the number of parameters
			unsigned int nparameters = 0;

			//a procedure does not have parameters if it end with "()" or "(void)"
			const char* open_parentesis = strchr(name, '(');
			if (open_parentesis)
			{
				bool has_no_parameters = strcmp(open_parentesis, "()") == 0 || strcmp(open_parentesis, "(void)") == 0;
				if (!has_no_parameters)
				{
					//count the number of commas
					for (unsigned int i = 0; i < strlen(name); i++)
					{
						if (name[i] == ',')
							nparameters++;
					}

					//add 1 to the number of commas to get the number of parameters
					nparameters++;
				}

				printf("Procedure '%s'", name);

				if (!has_no_parameters && symbol_header->value != 0xFFFFFFFF)
				{
					printf(" with parameters ");

					//read the arguments' names, these are the symbols that follow the procedure symbol
					for (int j = i + 1; j <= i + nparameters; j++)
					{
						SymbolHeader* ssymbol_header =
							(SymbolHeader*)m_f.GetDataAt(section_local_symbols_offset + (data->symbols_offset + j) * sizeof(SymbolHeader));
						CSymbol ssymbol(m_f);
						if (!ssymbol.Load(ssymbol_header, section_local_strings_offset))
						{
							printf("Failed to load symbol.\n");
							return false;
						}

						const char* parameter_name = ssymbol.GetName();

						//put in param_name until ':'
						char param_name[32] = {};
						for (unsigned int i = 0; i < strlen(parameter_name); i++)
						{
							if (parameter_name[i] == ':')
								break;
							param_name[i] = parameter_name[i];
						}

						//add the parameter name to the procedure
						m_procedures[last_procedure_symbol_index].AddParameter(param_name);
					}
					i += nparameters;
				}
			}
#endif

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
	for (const CProcedure& procedure : m_procedures)
	{
		procedure.Dump();
		printf("\n");
	}

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
			{
//				const char* name = symbol.GetName();

//				printf("Procedure '%s'", name);
			}
		}
	}

	//print all the include files
	for (const char* include_file : include_files)
		printf("\t#include \"%s\"\n", include_file);
}

bool CObjectFile::Compare(const CObjectFile& other) const
{
	// print the name of the object file
	Console::SetColor(Color::White);
	printf("----------------------------------------------------------------\n");
	printf("+ Comparing '%s'\n", m_name);
	printf("----------------------------------------------------------------\n");
	Console::ResetColor();
	//for every procedure of this CObjectFile
	for (unsigned int i = 0; i < m_procedures.size(); i++)
	{
		const CProcedure& procedure = m_procedures[i];

		//immediately skip procedures which do not exist in the ELF by default (they have an offset of 0xFFFFFFFF)
		if (procedure.GetProcedureVirtualOffset() == 0xFFFFFFFF)
			continue;

		const char* procedure_name = procedure.GetName();

		//try to find the procedure with the same name in the other CObjectFile
		//if there is, compare the two procedures, if not then inform the user
		const CProcedure* other_proc = other.FindProcedure(procedure_name);
		if (other_proc)
			procedure.Compare(*other_proc);
		else
		{
			Console::SetColor(Color::Yellow);
			printf("PLEASE DECOMP --- '%s' at 0x%x\n", procedure_name, procedure.GetProcedureVirtualOffset());
			Console::ResetColor();
		}
	}

	return true;
}
