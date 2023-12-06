#pragma once

enum class ElfSectionType : int
{
	NONE = 0,		/* Section header table entry unused */
	PROGBITS = 1,		/* Program data */
	SYMTAB = 2,		/* Symbol table */
	STRTAB = 3			/* String table */
};

enum ElfSectionFlags : int
{
	WRITE = (1 << 0),	/* Writable */
	ALLOC = (1 << 1),	/* Occupies memory during execution */
	EXECINSTR = (1 << 2),	/* Executable */
	MERGE = (1 << 4),	/* Might be merged */
	STRINGS = (1 << 5),	/* Contains nul-terminated strings */
	INFO_LINK = (1 << 6),	/* `sh_info' contains SHT index */
	LINK_ORDER = (1 << 7),	/* Preserve order after combining */
	OS_NONCONFORMING = (1 << 8),	/* Non-standard OS specific handling required */
	GROUP = (1 << 9),	/* Section is member of a group.  */
	TLS = (1 << 10),/* Section hold thread-local data.  */
	COMPRESSED = (1 << 11)	/* Section with compressed data. */
};

struct ElfSectionHeader
{
	int	name_index;
	ElfSectionType type;
	ElfSectionFlags	flags;
	int	addr;		/* Section virtual addr at execution */
	int	offset;		/* Section file offset */
	int	size;		/* Section size in bytes */
	int	link;		/* Link to another section */
	int	info;		/* Additional section information */
	int	addralign;	/* Section alignment */
	int	entsize;	/* Entry size if section holds table */
};
