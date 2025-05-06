/**
 * @file native_hooks.cpp
 * @brief Hooks to native functions.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "native_hooks.hpp"

#include "all_scripts.hpp"
#include "fm_maintain_cloud_header_data.hpp"
#include "crossmap.hpp"
#include "gta_util.hpp"

#include <script/scrProgram.hpp>
#include <script/scrProgramTable.hpp>

namespace big
{
	static bool map_native(rage::scrNativeHash* hash)
	{
		for (auto const& mapping : g_crossmap)
		{
			if (mapping.first == *hash)
			{
				*hash = mapping.second;
				return true;
			}
		}

		return false;
	}

	native_hook::native_hook(rage::scrProgram* program, const std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler>& native_replacements)
	{
		hook_instance(program, native_replacements);
	}

	native_hook::~native_hook()
	{
		if (m_handler_hook)
		{
			m_handler_hook->disable();
			m_handler_hook.reset();
		}

		if (m_vmt_hook)
		{
			m_vmt_hook->disable();
			m_vmt_hook.reset();
		}
	}

	void native_hook::hook_instance(rage::scrProgram* program, const std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler>& native_replacements)
	{
		m_program  = program;
		m_vmt_hook = std::make_unique<vmt_hook>(m_program, 9);
		m_vmt_hook->hook(6, (void*)&scrprogram_dtor);
		m_vmt_hook->enable();

		m_handler_hook = std::make_unique<vmt_hook>(&m_program->m_native_entrypoints, m_program->m_native_count);
		m_handler_hook->enable();

		std::unordered_map<rage::scrNativeHandler, rage::scrNativeHandler> handler_replacements;

		for (auto& [replacement_hash, replacement_handler] : native_replacements)
		{
			auto native = replacement_hash;
			map_native(&native);

			auto og_handler = g_pointers->m_get_native_handler(g_pointers->m_native_registration_table, native);
			if (!og_handler)
				continue;

			handler_replacements[og_handler] = replacement_handler;
		}

		for (int i = 0; i < m_program->m_native_count; i++)
		{
			if (auto it = handler_replacements.find((rage::scrNativeHandler)program->m_native_entrypoints[i]);
			    it != handler_replacements.end())
			{
				m_handler_hook->hook(i, (void*)it->second);
			}
		}
	}

	void native_hook::scrprogram_dtor(rage::scrProgram* this_, char free_memory)
	{
		if (auto it = g_native_hooks->m_native_hooks.find(this_); it != g_native_hooks->m_native_hooks.end())
		{
			auto og_func = it->second->m_vmt_hook->get_original<decltype(&native_hook::scrprogram_dtor)>(6);
			it->second->m_vmt_hook->disable();
			it->second->m_vmt_hook.reset();
			it->second->m_handler_hook->disable();
			it->second->m_handler_hook.reset();
			g_native_hooks->m_native_hooks.erase(it);
			og_func(this_, free_memory);
		}
		else
		{
			LOG(FATAL) << "Cannot find hook for program";
		}
	}

	constexpr auto ALL_SCRIPT_HASH = RAGE_JOAAT("ALL_SCRIPTS");

	native_hooks::native_hooks()
	{
		add_native_detour(0x7D2708796355B20B, all_scripts::RETURN_FALSE); // NET_GAMESERVER_USE_SERVER_TRANSACTIONS
		add_native_detour(0x84B418E93894AC1C, all_scripts::RETURN_FALSE); // SAVEMIGRATION_IS_MP_ENABLED
		add_native_detour(0x7F2C4CDF2E82DF4C, all_scripts::RETURN_FALSE); // STAT_CLOUD_SLOT_LOAD_FAILED
		add_native_detour(0xE496A53BA5F50A56, all_scripts::RETURN_FALSE); // STAT_CLOUD_SLOT_LOAD_FAILED_CODE
		add_native_detour(0xC0E0D686DDFC6EAE, all_scripts::RETURN_TRUE); // STAT_GET_LOAD_SAFE_TO_PROGRESS_TO_MP_FROM_SP
		add_native_detour(0xD9719341663C385F, all_scripts::RETURN_TRUE); // _NETWORK_HAS_ROS_PRIVILEGE_MP_TEXT_COMMUNICATION
		add_native_detour(0x8956A309BE90057C, all_scripts::RETURN_TRUE); // _NETWORK_HAS_ROS_PRIVILEGE_MP_VOICE_COMMUNICATION
		add_native_detour(0xE1E02509169C124E, all_scripts::RETURN_TRUE); // _NETWORK_HAVE_PLATFORM_COMMUNICATION_PRIVILEGES

		add_native_detour(0xF70EFA14FE091429, all_scripts::WITHDRAW_VC);
		add_native_detour(0xE260E0BB9CD995AC, all_scripts::DEPOSIT_VC);

		add_native_detour(0xC87E740D9F3872CC, fm_maintain_cloud_header_data::UGC_WAS_QUERY_FORCE_CANCELLED);
		/*
		add_native_detour(0x158EC424F35EC469, all_scripts::RETURN_TRUE); // UGC_QUERY_BY_CONTENT_ID
		add_native_detour(0x815E5E3073DA1D67, all_scripts::RETURN_TRUE); // UGC_GET_GET_BY_CONTENT_ID
		add_native_detour(0x02ADA21EA2F6918F, all_scripts::RETURN_TRUE); // UGC_HAS_GET_FINISHED
		add_native_detour(0x941E5306BCD7C2C7, all_scripts::RETURN_FALSE); // UGC_DID_GET_SUCCEED
		add_native_detour(RAGE_JOAAT("fm_maintain_cloud_header_data"), 0xE0A6138401BCB837, all_scripts::RETURN_FALSE); // UGC_GET_CONTENT_NUM
		add_native_detour(RAGE_JOAAT("fm_maintain_cloud_header_data"), 0x4C61B39930D045DA, all_scripts::RETURN_TRUE); // CLOUD_HAS_REQUEST_COMPLETED
		add_native_detour(RAGE_JOAAT("fm_maintain_cloud_header_data"), 0x3A3D5568AF297CD5, all_scripts::RETURN_TRUE); // CLOUD_DID_REQUEST_SUCCEED
		add_native_detour(0xFD75DABC0957BF33, all_scripts::UGC_SET_USING_OFFLINE_CONTENT);
		add_native_detour(0xF98DDE0A8ED09323, all_scripts::UGC_SET_QUERY_DATA_FROM_OFFLINE);
		*/

		add_native_detour(0x0395CB47B022E62C, all_scripts::NET_GAMESERVER_RETRIEVE_INIT_SESSION_STATUS);

		for (auto& entry : *g_pointers->m_script_program_table)
			if (entry.m_program)
				hook_program(entry.m_program);

		g_native_hooks = this;
	}

	native_hooks::~native_hooks()
	{
		m_native_hooks.clear();
		g_native_hooks = nullptr;
	}

	void native_hooks::add_native_detour(rage::scrNativeHash hash, rage::scrNativeHandler detour)
	{
		add_native_detour(ALL_SCRIPT_HASH, hash, detour);
	}

	void native_hooks::add_native_detour(rage::joaat_t script_hash, rage::scrNativeHash hash, rage::scrNativeHandler detour)
	{
		if (const auto& it = m_native_registrations.find(script_hash); it != m_native_registrations.end())
		{
			it->second.emplace_back(hash, detour);
			return;
		}

		m_native_registrations.emplace(script_hash, std::vector<native_detour>({{hash, detour}}));
	}

	void native_hooks::hook_program(rage::scrProgram* program)
	{
		std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> native_replacements;
		const auto script_hash = program->m_name_hash;

		// Functions that need to be detoured for all scripts
		if (const auto& pair = m_native_registrations.find(ALL_SCRIPT_HASH); pair != m_native_registrations.end())
			for (const auto& native_hook_reg : pair->second)
				native_replacements.insert(native_hook_reg);

		// Functions that only need to be detoured for a specific script
		if (const auto& pair = m_native_registrations.find(script_hash); pair != m_native_registrations.end())
			for (const auto& native_hook_reg : pair->second)
				native_replacements.insert(native_hook_reg);

		if (!native_replacements.empty())
		{
			m_native_hooks.emplace(program, std::make_unique<native_hook>(program, native_replacements));
		}
	}
}
