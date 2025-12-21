/**
 * @file byte_patch_manager.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "byte_patch_manager.hpp"

#include "common.hpp"
#include "memory/byte_patch.hpp"
#include "pointers.hpp"
#include "util/decrypt_save.hpp"
#include "util/profile_stats.hpp"

namespace big
{
	static void init()
	{
		g_pointers->m_skip_money_check1->apply();
		g_pointers->m_skip_money_check2->apply();
		g_pointers->m_skip_money_check3->apply();
		g_pointers->m_skip_money_check3->apply();
		g_pointers->m_skip_money_check4->apply();
		g_pointers->m_skip_money_check6->apply();
		if(!g_is_enhanced)
		{
			g_pointers->m_skip_money_check5->apply();
		}
		else
		{
			g_pointers->m_file_not_found_check2->apply();
		}
		g_pointers->m_file_not_found_check->apply(); // When a cloud file failes to load, create a new one

		skip_profile_stats_patch::apply(); // Skip by default
		g_pointers->m_mp_save_download_patch->apply();

		if(g.load_fsl_files)
		{
			memory::byte_patch::make(g_pointers->m_load_check_profile_stat.as<PVOID>(), std::vector{0x90, 0x90, 0x90, 0x90, 0x90, 0x90})->apply();
			decrypt_save_patch::m_check_enc_param = memory::byte_patch::make(g_pointers->m_mp_save_decrypt.add(2).as<PVOID>(), std::vector{0xEB}).get();
		}
	}

	byte_patch_manager::byte_patch_manager()
	{
		init();

		g_byte_patch_manager = this;
	}

	byte_patch_manager::~byte_patch_manager()
	{
		memory::byte_patch::restore_all();

		g_byte_patch_manager = nullptr;
	}
}