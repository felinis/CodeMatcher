#include "CSymbol.h"

const char* CSymbol::GetName() const
{
	
}

unsigned int CSymbol::GetValue() const
{
	return value;
}

SymbolType CSymbol::GetType() const
{
	return type;
}

StorageClass CSymbol::GetStorageClass() const
{
	return storageClass;
}

void CSymbol::Dump(CFileStream& f) const
{

}
