#include "native_hooks.hpp"

#include "all_scripts.hpp"
#include "fm_maintain_cloud_header_data.hpp"
#include "script_save.hpp"

#include <optional>
#include <script/scrProgram.hpp>
#include <script/scrProgramTable.hpp>

namespace big
{
	native_hooks::native_hooks()
	{
		add_native_detour(0x7D2708796355B20B, all_scripts::RETURN_FALSE); // NET_GAMESERVER_USE_SERVER_TRANSACTIONS
		add_native_detour(0x84B418E93894AC1C, all_scripts::RETURN_FALSE); // SAVEMIGRATION_IS_MP_ENABLED
		add_native_detour(0x7F2C4CDF2E82DF4C, all_scripts::RETURN_FALSE); // STAT_CLOUD_SLOT_LOAD_FAILED
		add_native_detour(0xE496A53BA5F50A56, all_scripts::RETURN_FALSE); // STAT_CLOUD_SLOT_LOAD_FAILED_CODE
		add_native_detour(0xECB41AC6AB754401, all_scripts::RETURN_FALSE); // STAT_LOAD_DIRTY_READ_DETECTED
		add_native_detour(0xC0E0D686DDFC6EAE, all_scripts::RETURN_TRUE); // STAT_GET_LOAD_SAFE_TO_PROGRESS_TO_MP_FROM_SP

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

		g_native_hooks = this;
	}

	native_hooks::~native_hooks()
	{
		g_native_hooks = nullptr;
	}

	void native_hooks::add_native_detour(rage::scrNativeHash hash, rage::scrNativeHandler detour)
	{
		m_native_detours[hash] = detour;
	}

	std::optional<rage::scrNativeHandler> native_hooks::get_hooked_handler(rage::scrNativeHash hash)
	{
		try
		{
			return m_native_detours.at(hash);
		}
		catch (const std::out_of_range& ex)
		{
			return std::nullopt;
		}
	}
}
