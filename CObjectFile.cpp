#include "CObjectFile.h"
#include <stdio.h>

//TODO: remove
bool CObjectFile::Load(CFileStream& f)
{
	return true;
}

const char* CObjectFile::GetName(CFileStream& f, unsigned int section_local_strings_offset)
{
	return (char*)f.GetDataAt(section_local_strings_offset + nameIndex + localStringsOffset);
}

CProcedure* CObjectFile::GetProcedure(CFileStream& f, unsigned int procedures_offset, int index)
{
	return (CProcedure*)f.GetDataAt(procedures_offset + sizeof(CProcedure) * (procedures_first_index + index));
}

void CObjectFile::Dump(CFileStream& f, unsigned int section_local_strings_offset, unsigned int procedures_offset)
{
	printf("Object file '%s'\n", GetName(f, section_local_strings_offset));
	
	//print all the procedures
	for (unsigned short i = 0; i < nprocedures; i++)
	{
		CProcedure* proc = GetProcedure(f, procedures_offset, i);
		proc->Dump(f);
	}
}
