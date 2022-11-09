#include "CMDebugSection.h"
#include <stdio.h>
#include <string.h>

constexpr short MDEBUG_MAGIC = 0x7009;

bool CMDebugSection::Load(CFileStream& f)
{
	//validate
	if (magic != MDEBUG_MAGIC)
	{
		printf("Invalid .mdebug magic.\n");
		return false;
	}

	//parse all the files

	return true;
}

CObjectFile* CMDebugSection::GetObjectFile(CFileStream& f, int index)
{
	return (CObjectFile*)f.GetDataAt(objectfiles_offset + sizeof(CObjectFile) * index);
}

void CMDebugSection::Dump(CFileStream& f)
{
	//dump all
//#ifndef _DEBUG
	for (int i = 0; i < nobjectfiles; i++)
//#else
//	for (int i = 50; i < 51; i++)
//#endif
	{
		CObjectFile* obj = GetObjectFile(f, i);
		obj->Dump(f, localstrings_offset, procedures_offset, localsymbols_offset);
	}
}
