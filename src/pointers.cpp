/**
 * @file pointers.cpp
 * @brief All the signature scans are here.
 */

#include "pointers.hpp"

#include "common.hpp"
#include "memory/all.hpp"

namespace big
{
	pointers::pointers()
	{
		memory::pattern_batch main_batch;

		main_batch.add("Game state", "83 3D ? ? ? ? ? 75 17 8B 43 20 25", [this](memory::handle ptr) {
			m_game_state = ptr.add(2).rip().add(1).as<eGameState*>();
		});

		main_batch.add("Ped factory", "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81", [this](memory::handle ptr) {
			m_ped_factory = ptr.add(3).rip().as<CPedFactory**>();
		});

		main_batch.add("Network player manager", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", [this](memory::handle ptr) {
			m_network_player_mgr = ptr.add(3).rip().as<CNetworkPlayerMgr**>();
		});

		main_batch.add("Native handlers", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", [this](memory::handle ptr) {
			m_init_native_tables        = ptr.sub(37).as<PVOID>();
			m_native_registration_table = ptr.add(3).rip().as<rage::scrNativeRegistrationTable*>();
			m_get_native_handler        = ptr.add(12).rip().as<functions::get_native_handler_t>();
		});

		main_batch.add("Fix vectors", "83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA", [this](memory::handle ptr) {
			m_fix_vectors = ptr.as<functions::fix_vectors_t>();
		});

		main_batch.add("Script threads", "45 33 F6 8B E9 85 C9 B8", [this](memory::handle ptr) {
			m_script_threads     = ptr.sub(4).rip().sub(8).as<decltype(m_script_threads)>();
			m_run_script_threads = ptr.sub(0x1F).as<functions::run_script_threads_t>();
		});

		main_batch.add("Script programs", "48 8B 1D ? ? ? ? 41 83 F8 FF", [this](memory::handle ptr) {
			m_script_program_table = ptr.add(3).rip().as<decltype(m_script_program_table)>();
		});

		main_batch.add("Script globals", "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D", [this](memory::handle ptr) {
			m_script_globals = ptr.add(3).rip().as<std::int64_t**>();
		});

		main_batch.add("CGameScriptHandlerMgr", "48 8B 0D ? ? ? ? 4C 8B CE E8 ? ? ? ? 48 85 C0 74 05 40 32 FF", [this](memory::handle ptr) {
			m_script_handler_mgr = ptr.add(3).rip().as<CGameScriptHandlerMgr**>();
		});

		main_batch.add("SkipMoneyCheck1", "84 C0 ? ? 93 01 00 00 48", [this](memory::handle ptr) {
			m_skip_money_check1 = ptr.add(2);
		});
		main_batch.add("SkipMoneyCheck2", "84 C0 ? ? AD 01 00 00 48 8B 8D", [this](memory::handle ptr) {
			m_skip_money_check2 = ptr.add(2);
		});
		main_batch.add("SkipMoneyCheck3", "84 C0 ? ? 83 C9 FF E8 ? ? ? ? 48 85", [this](memory::handle ptr) {
			m_skip_money_check3 = ptr.add(2);
		});
		main_batch.add("SkipMoneyCheck5", "84 C0 ? ? 8B CB E8 ? ? ? ? 48 85 C0 7E", [this](memory::handle ptr) {
			m_skip_money_check5 = ptr.add(2);
		});
		main_batch.add("SkipMoneyCheck6", "84 C0 ? ? 8B CF E8 ? ? ? ? 41", [this](memory::handle ptr) {
			m_skip_money_check6 = ptr.add(2);
		});
		main_batch.add("AlwaysFileNotFound", "48 85 C0 74 52 40 88", [this](memory::handle ptr) {
			m_file_not_found_check = ptr.add(35);
		});
		main_batch.add("ProfileStatsSkip", "84 C0 ? ? ? ? ? ? 41 8A D4", [this](memory::handle ptr) {
			m_profile_stats_skip = ptr.add(2);
		});
		main_batch.add("ProfileStats Read Request", "48 8D ? ? ? 4C 8B C6 41 8B CE E8", [this](memory::handle ptr) {
			m_profile_stats_download = ptr.add(12).rip().as<PVOID>();
		});
		main_batch.add("ProfileStats Write Request", "44 8B CF 8B D6 E8", [this](memory::handle ptr) {
			m_profile_stats_save = ptr.add(6).rip().as<PVOID>();
		});

		main_batch.add("Network Can Access Multiplayer", "E9 ? 01 00 00 33 D2 8B CB", [this](memory::handle ptr) {
			m_network_can_access_multiplayer = ptr.add(10).rip().as<PVOID>();
		});

		main_batch.add("Stat Vtables", "74 6F 48 8B 16 4D 8B C6 48 8B C8 E8", [this](memory::handle ptr) {
			m_stat_ctor                  = ptr.add(12).rip().as<PVOID>();
			m_obf_uns64_stat_data_vtable = ptr.add(19).rip().as<PVOID>();
			m_stat_dtor                  = *(PVOID*)m_obf_uns64_stat_data_vtable;
		});

		main_batch.add("MP Stats Save", "48 83 EC 20 F6 81 ? ? ? ? ? 41 8B D9 45", [this](memory::handle ptr) {
			m_mp_stats_save = ptr.sub(18).as<PVOID>();
		});
		main_batch.add("MP Save Download", "85 C9 0F 84 1B 01 00 00 FF C9", [this](memory::handle ptr) {
			m_mp_save_download_patch = ptr;
			m_mp_save_download       = ptr.sub(26).as<PVOID>();
			m_mp_save_download_error = ptr.add(323).rip().as<int*>();
		});

		main_batch.add("All Stats Array", "41 B0 01 48 8D 0D ? ? ? ? E8", [this](memory::handle ptr) {
			m_stats = ptr.add(6).rip().add(8).as<rage::atArray<sStatArrayEntry>*>();
		});

		if (g.load_fsl_files)
		{
			main_batch.add("MP Save Decrypt", "84 D2 ? 34 48 8D 8C", [this](memory::handle ptr) {
				m_mp_save_decrypt = ptr;
			});
			main_batch.add("Load Check Profile Stat", "A8 01 ? ? ? ? ? ? 40 F6 C5", [this](memory::handle ptr) {
				m_load_check_profile_stat = ptr.add(2);
			});
		}

		// Source https://www.unknowncheats.me/forum/grand-theft-auto-v/442708-basket-transactions.html
		main_batch.add("Construct Basket", "48 83 C1 70 45 8B E1 41 8B", [this](memory::handle ptr) {
			m_construct_basket = ptr.sub(32).as<PVOID>();
		});

		main_batch.run(memory::module(nullptr));

		m_hwnd = FindWindowW(L"grcWindow", nullptr);
		if (!m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");

		g_pointers = this;
	}

	pointers::~pointers()
	{
		g_pointers = nullptr;
	}
}
