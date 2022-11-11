#pragma once
#include "CElfFile.h"
#include "CMDebugSection.h"

//===
//	A CDebugElfFile is a CElfFile with MDebug information.
//===
class CDebugElfFile final: public CElfFile
{
	CMDebugSection* m_mdebug_section;
public:
	bool Load(const char* file_name);
	void Dump();

	bool operator==(const CDebugElfFile& other) const;
};
