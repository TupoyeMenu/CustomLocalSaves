#pragma once
#include "common.hpp"

namespace big
{
	struct PdbInfo
	{
		DWORD Signature;
		BYTE Guid[16];
		DWORD Age;
		char PdbFileName[1];
	};

	// https://deplinenoise.wordpress.com/2013/06/14/getting-your-pdb-name-from-a-running-executable-windows/
	inline bool is_enhanced()
	{
		// Figure out where the executable is mapped in memory.
		uintptr_t base_pointer = (uintptr_t)GetModuleHandle(NULL);

		// This is where the MZ...blah header lives (the DOS header)
		IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)base_pointer;

		// We want the PE header.
		IMAGE_FILE_HEADER* file_header = (IMAGE_FILE_HEADER*)(base_pointer + dos_header->e_lfanew + 4);

		// Straight after that is the optional header (which technically is optional, but in practice always there.)
		IMAGE_OPTIONAL_HEADER* opt_header = (IMAGE_OPTIONAL_HEADER*)(((char*)file_header) + sizeof(IMAGE_FILE_HEADER));

		// Grab the debug data directory which has an indirection to its data
		IMAGE_DATA_DIRECTORY* dir = &opt_header->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];

		// Convert that data to the right type.
		IMAGE_DEBUG_DIRECTORY* dbg_dir = (IMAGE_DEBUG_DIRECTORY*)(base_pointer + dir->VirtualAddress);

		// Check to see that the data has the right type
		if (IMAGE_DEBUG_TYPE_CODEVIEW == dbg_dir->Type)
		{
			PdbInfo* pdb_info = (PdbInfo*)(base_pointer + dbg_dir->AddressOfRawData);
			if (0 == memcmp(&pdb_info->Signature, "RSDS", 4))
			{
				return strstr(pdb_info->PdbFileName, "llvm.pdb") != 0;
			}
		}
		return false;
	}

	inline eGameBranch get_game_branch()
	{
		return g_is_enhanced ? eGameBranch::Enhanced : eGameBranch::Legacy;
	}

}
