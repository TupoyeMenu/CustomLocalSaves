#pragma once
#include "function_types.hpp"
#include "gta/enums.hpp"
#include "gta/fwddec.hpp"
#include "script/scrThread.hpp"

#include <memory/handle.hpp>
#include <player/CPlayerInfo.hpp>

namespace big
{
	class pointers
	{
	public:
		explicit pointers();
		~pointers();

	public:
		HWND m_hwnd{};

		eGameState* m_game_state{};

		CPedFactory** m_ped_factory{};
		CNetworkPlayerMgr** m_network_player_mgr{};

		rage::scrNativeRegistrationTable* m_native_registration_table{};

		rage::atArray<rage::scrThread*>* m_script_threads{};
		rage::scrProgramTable* m_script_program_table{};
		functions::run_script_threads_t m_run_script_threads{};
		std::int64_t** m_script_globals{};
		PVOID m_init_native_tables{};

		memory::handle m_skip_money_check1;
		memory::handle m_skip_money_check2;
		memory::handle m_skip_money_check3;
		memory::handle m_skip_money_check5;
		memory::handle m_skip_money_check6;
		memory::handle m_file_not_found_check;
		memory::handle m_profile_stats_skip;
		PVOID m_profile_stats_download;
		PVOID m_profile_stats_save;

		PVOID m_network_can_access_multiplayer;
		bool m_network_can_access_multiplayer_already_hooked = false;

		PVOID m_stat_ctor;
		PVOID m_stat_dtor;
		PVOID m_obf_uns64_stat_data_vtable;

		PVOID m_mp_stats_save;
		PVOID m_mp_stats_load;
		memory::handle m_mp_save_download_patch;
		memory::handle m_mp_save_decrypt;
		memory::handle m_load_check_profile_stat;
		rage::atArray<sStatArrayEntry>* m_stats;
		PVOID m_mp_save_download;
		int* m_mp_save_download_error;

		PVOID m_construct_basket;
	};

	inline pointers* g_pointers{};
}
