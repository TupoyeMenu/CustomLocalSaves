#pragma once
#include "function_types.hpp"
#include "gta/enums.hpp"
#include "gta/fwddec.hpp"
#include "memory/byte_patch.hpp"
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
#ifdef ENABLE_GUI
		HWND m_hwnd{};
#endif

		CPedFactory** m_ped_factory{};
		CNetworkPlayerMgr** m_network_player_mgr{};

		rage::atArray<rage::scrThread*>* m_script_threads{};
		functions::run_script_threads_t m_run_script_threads{};
		std::int64_t** m_script_globals{};
		PVOID m_create_native{};

		memory::byte_patch* m_skip_money_check1;
		memory::byte_patch* m_skip_money_check2;
		memory::byte_patch* m_skip_money_check3;
		memory::byte_patch* m_skip_money_check4;
		memory::byte_patch* m_skip_money_check5;
		memory::byte_patch* m_skip_money_check6;
		memory::byte_patch* m_file_not_found_check;
		memory::byte_patch* m_file_not_found_check2;
		PVOID m_profile_stats_download;
		PVOID m_profile_stats_save;

		PVOID m_network_can_access_multiplayer;
		bool m_network_can_access_multiplayer_already_hooked = false;

		PVOID m_create_stat;
		PVOID m_stat_dtor;

		PVOID m_mp_stats_save;
		PVOID m_mp_stats_load;
		memory::byte_patch* m_mp_save_download_patch;
		memory::handle m_mp_save_decrypt;
		memory::handle m_load_check_profile_stat;
		rage::atArray<sStatArrayEntry>* m_stats;
		PVOID m_mp_save_download;
		int* m_mp_save_download_error;

		PVOID m_construct_basket;
		functions::spend_money m_spend_money;
		functions::earn_money m_earn_money;
	};

	inline pointers* g_pointers{};
}
