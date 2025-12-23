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
#include "crossmap.hpp"
#include "fm_maintain_cloud_header_data.hpp"
#include "script_save.hpp"

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

			auto og_handler = g_native_invoker.get_native_handler(native);
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
		add_native_detour(0x3BD101471C7F9EEC, all_scripts::NETWORK_CASINO_BUY_CHIPS);
		add_native_detour(0xED44897CB336F480, all_scripts::NETWORK_CASINO_SELL_CHIPS);
		if (g_is_enhanced)
		{
			add_native_detour(0xA921DED15FDF28F5, all_scripts::NETWORK_CLEAR_CHARACTER_WALLET);
		}

		add_native_detour(0x34C9EE5986258415, all_scripts::REGISTER_INT_TO_SAVE);        // REGISTER_INT_TO_SAVE
		add_native_detour(0xA735353C77334EA0, all_scripts::REGISTER_INT64_TO_SAVE);      // REGISTER_INT64_TO_SAVE
		add_native_detour(0x10C2FA78D0E128A1, all_scripts::REGISTER_ENUM_TO_SAVE);       // REGISTER_ENUM_TO_SAVE
		add_native_detour(0x7CAEC29ECB5DFEBB, all_scripts::REGISTER_FLOAT_TO_SAVE);      // REGISTER_FLOAT_TO_SAVE
		add_native_detour(0xC8F4131414C835A1, all_scripts::REGISTER_BOOL_TO_SAVE);       // REGISTER_BOOL_TO_SAVE
		add_native_detour(0xEDB1232C5BEAE62F, all_scripts::REGISTER_TEXT_LABEL_TO_SAVE); // REGISTER_TEXT_LABEL_TO_SAVE
		add_native_detour(0x6F7794F28C6B2535, all_scripts::REGISTER_TEXT_LABEL_15_TO_SAVE); // REGISTER_TEXT_LABEL_15_TO_SAVE
		add_native_detour(0x48F069265A0E4BEC, all_scripts::REGISTER_TEXT_LABEL_23_TO_SAVE); // REGISTER_TEXT_LABEL_23_TO_SAVE
		add_native_detour(0x8269816F6CFD40F8, all_scripts::REGISTER_TEXT_LABEL_31_TO_SAVE); // REGISTER_TEXT_LABEL_31_TO_SAVE
		add_native_detour(0xFAA457EF263E8763, all_scripts::REGISTER_TEXT_LABEL_63_TO_SAVE); // REGISTER_TEXT_LABEL_63_TO_SAVE
		add_native_detour(0xA9575F812C6A7997, all_scripts::START_SAVE_DATA);                // START_SAVE_DATA
		add_native_detour(0x74E20C9145FB66FD, all_scripts::STOP_SAVE_DATA);                 // STOP_SAVE_DATA
		add_native_detour(0xBF737600CDDBEADD, all_scripts::START_SAVE_STRUCT_WITH_SIZE); // START_SAVE_STRUCT_WITH_SIZE
		add_native_detour(0xEB1774DF12BB9F12, all_scripts::STOP_SAVE_STRUCT);            // STOP_SAVE_STRUCT
		add_native_detour(0x60FE567DF1B1AF9D, all_scripts::START_SAVE_ARRAY_WITH_SIZE);  // START_SAVE_ARRAY_WITH_SIZE
		add_native_detour(0x04456F95153C6BE4, all_scripts::STOP_SAVE_ARRAY);             // STOP_SAVE_ARRAY

		if (g.no_rgs)
		{
			add_native_detour(0xC87E740D9F3872CC, fm_maintain_cloud_header_data::UGC_WAS_QUERY_FORCE_CANCELLED);
		}
		add_native_detour(0x0395CB47B022E62C, all_scripts::NET_GAMESERVER_RETRIEVE_INIT_SESSION_STATUS);

		add_native_detour(0x158C16F5E4CF41F8, all_scripts::RETURN_TRUE);  // NETWORK_CASINO_CAN_BET
		add_native_detour(0x930DE22F07B1CCE3, all_scripts::RETURN_FALSE); // SC_PROFANITY_GET_STRING_STATUS

		for (int i = 0; i < 176; i++)
		{
			rage::scrProgram* program = g_pointers->m_script_programs[i];
			if (program != nullptr && program->m_code_blocks && program->m_code_size)
			{
				hook_program(program);
			}
		}

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
