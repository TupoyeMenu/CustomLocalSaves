/**
 * @file pointers.cpp
 * @brief All the signature scans are here.
 */

#include "pointers.hpp"

#include "common.hpp"
#include "memory/all.hpp"
#include "util/profile_stats.hpp"

namespace big
{
	pointers::pointers()
	{
		memory::pattern_batch main_batch;

		main_batch.add("Game state", "83 3D ? ? ? ? ? 75 17 8B 43 20 25", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_game_state = ptr.add(2).rip().add(1).as<eGameState*>();
		});
		main_batch.add("Game state", "83 3D ? ? ? ? ? 0F 85 ? ? ? ? BA ? 00", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_game_state = ptr.add(2).rip().add(1).as<eGameState*>();
		});

		main_batch.add("Ped factory", "48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_ped_factory = ptr.add(3).rip().as<CPedFactory**>();
		});
		main_batch.add("Ped factory", "C7 40 30 03 00 00 00 48 8B 0D", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_ped_factory = ptr.add(7).add(3).rip().as<CPedFactory**>();
		});

		main_batch.add("Network player manager", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_network_player_mgr = ptr.add(3).rip().as<CNetworkPlayerMgr**>();
		});
		main_batch.add("Network player manager", "75 0E 48 8B 05 ? ? ? ? 48 8B 88 F0 00 00 00", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_network_player_mgr = ptr.add(2).add(3).rip().as<CNetworkPlayerMgr**>();
		});

		main_batch.add("Native handlers", "48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_init_native_tables        = ptr.sub(37).as<PVOID>();
			m_native_registration_table = ptr.add(3).rip().as<rage::scrNativeRegistrationTable*>();
		});
		main_batch.add("Native handlers", "EB 2A 0F 1F 40 00 48 8B 54 17 10", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_init_native_tables        = ptr.sub(0x2A).as<PVOID>();
			m_native_registration_table = ptr.sub(0xE).rip().as<rage::scrNativeRegistrationTable*>();
		});

		main_batch.add("Script threads", "45 33 F6 8B E9 85 C9 B8", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_script_threads     = ptr.sub(4).rip().sub(8).as<decltype(m_script_threads)>();
			m_run_script_threads = ptr.sub(0x1F).as<functions::run_script_threads_t>();
		});
		main_batch.add("Script threads", "BE 40 5D C6 00", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_script_threads     = ptr.add(0x1B).rip().as<decltype(m_script_threads)>();
			m_run_script_threads = ptr.sub(0xA).as<functions::run_script_threads_t>();
		});

		main_batch.add("Script globals", "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_script_globals = ptr.add(3).rip().as<std::int64_t**>();
		});
		main_batch.add("Script globals", "48 8B 8E B8 00 00 00 48 8D 15 ? ? ? ? 49 89 D8", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_script_globals = ptr.add(10).rip().as<std::int64_t**>();
		});

		// CLS signatures

		main_batch.add("SkipMoneyCheck1", "84 C0 0F 85 93 01 00 00 48", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_skip_money_check1 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x48, 0xE9}).get();
			m_earn_money = ptr.sub(0x2D).as<functions::earn_money>();
		});
		main_batch.add("SkipMoneyCheck1", "84 C0 0F 84 ? ? ? ? 80 3D ? ? ? ? 00 0F 84 ? ? ? ? E8 ? ? ? ? 84 C0 0F 84", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_skip_money_check1 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90, 0x90, 0x90, 0x90, 0x90}).get();
			m_earn_money = ptr.sub(0x3A).as<functions::earn_money>();
		});
		main_batch.add("SkipMoneyCheck2", "84 C0 ? ? AD 01 00 00 48 8B 8D", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_skip_money_check2 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x48, 0xE9}).get();
			m_spend_money = ptr.sub(0x38).as<functions::spend_money>();
		});
		main_batch.add("SkipMoneyCheck2", "84 C0 ? ? 31 F6 48 85 FF", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_skip_money_check2 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
			m_spend_money = ptr.sub(0x42).as<functions::spend_money>();
		});
		main_batch.add("SkipMoneyCheck3", "84 C0 ? ? 83 C9 FF E8 ? ? ? ? 48 85", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_skip_money_check3 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("SkipMoneyCheck3", "84 C0 ? ? B9 FF FF FF FF E8 ? ? ? ? 48 85", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_skip_money_check3 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("SkipMoneyCheck4", "84 C0 ? ? E8 ? ? ? ? 48 85 C0 75 ? B2", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_skip_money_check4 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("SkipMoneyCheck4", "74 26 E8 ? ? ? ? 48 85 C0 0F", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_skip_money_check4 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("SkipMoneyCheck5", "84 C0 ? ? 8B CB E8 ? ? ? ? 48 85 C0 7E", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_skip_money_check5 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("SkipMoneyCheck6", "84 C0 ? ? 8B CF E8 ? ? ? ? 41", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_skip_money_check6 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("SkipMoneyCheck6", "84 C0 ? ? 89 F1 E8 ? ? ? ? 48 89 C7", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_skip_money_check6 = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});

		main_batch.add("AlwaysFileNotFound", "48 85 C0 74 52 40 88", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_file_not_found_check = memory::byte_patch::make(ptr.add(35).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("AlwaysFileNotFound", "01 00 00 00 3D 98 01 00 00", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_file_not_found_check = memory::byte_patch::make(ptr.add(16).as<PVOID>(), std::vector{0x90, 0x90}).get();
			m_file_not_found_check2 = memory::byte_patch::make(ptr.add(9).as<PVOID>(), std::vector{0x90, 0x90}).get();
		});
		main_batch.add("ProfileStatsSkip", "84 C0 ? ? ? ? ? ? 41 8A D4", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			skip_profile_stats_patch::m_skip = memory::byte_patch::make(ptr.add(2).as<PVOID>(), std::vector{0x48, 0xE9}).get();
		});
		main_batch.add("ProfileStatsSkip", "84 C0 ? ? 48 8D 0D ? ? ? ? B2 01 E8 ? ? ? ? 84", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			skip_profile_stats_patch::m_skip = memory::byte_patch::make(ptr.add(2).as<uint8_t*>(), 0xEB).get();
		});
		main_batch.add("ProfileStats Read Request", "48 8D ? ? ? 4C 8B C6 41 8B CE E8", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_profile_stats_download = ptr.add(12).rip().as<PVOID>();
		});
		main_batch.add("ProfileStats Read Request", "48 8D 54 24 ? 31 C9 4D", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_profile_stats_download = ptr.add(11).rip().as<PVOID>();
		});
		main_batch.add("ProfileStats Write Request", "44 8B CF 8B D6 E8", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_profile_stats_save = ptr.add(6).rip().as<PVOID>();
		});
		main_batch.add("ProfileStats Write Request", "10 ? 89 ? 31 D2 41 89 ? E8", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_profile_stats_save = ptr.add(10).rip().as<PVOID>();
		});
		main_batch.add("Network Can Access Multiplayer", "74 E0 33 D2 8B CB", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_network_can_access_multiplayer = ptr.add(7).rip().as<PVOID>();
		});

		main_batch.add("Create Stat", "44 8A 45 70", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_create_stat = ptr.add(12).rip().as<PVOID>();
		});
		main_batch.add("Create Stat", "48 8B 4C 24 40 48 89 DA E8 ? ? ? ? 48", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_create_stat = ptr.add(9).rip().as<PVOID>();
		});

		main_batch.add("MP Stats Save", "48 83 EC 20 F6 81 ? ? ? ? ? 41 8B D9 45", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_mp_stats_save = ptr.sub(18).as<PVOID>();
		});
		main_batch.add("MP Stats Save", "48 83 EC 28 31 F6 41 83 F8 02", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_mp_stats_save = ptr.sub(12).as<PVOID>();
		});
		main_batch.add("MP Save Download", "85 C9 0F 84 1B 01 00 00 FF C9", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_mp_save_download_patch = memory::byte_patch::make(ptr.add(0x1E3).as<PVOID>(), std::vector{0x90, 0x90}).get();
			m_mp_save_download       = ptr.sub(26).as<PVOID>();
			m_mp_save_download_error = ptr.add(323).rip().as<int*>();
		});
		main_batch.add("MP Save Download", "48 89 CE 8B 41 14 48 83 F8 03", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_mp_save_download_patch = memory::byte_patch::make(ptr.add(0x174).as<PVOID>(), std::vector{0x90, 0x90, 0x90, 0x90, 0x90, 0x90}).get();
			m_mp_save_download       = ptr.sub(14).as<PVOID>();
			m_mp_save_download_error = ptr.add(0x30).rip().as<int*>();
		});

		main_batch.add("All Stats Array", "41 B0 01 48 8D 0D ? ? ? ? E8", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_stats = ptr.add(6).rip().add(8).as<rage::atArray<sStatArrayEntry>*>();
		});
		main_batch.add("All Stats Array", "45 31 C0 48 8D 3D ? ? ? ? 48 89", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_stats = ptr.add(6).rip().add(8).as<rage::atArray<sStatArrayEntry>*>();
		});

		if (g.load_fsl_files)
		{
			main_batch.add("MP Save Decrypt", "84 D2 ? 34 48 8D 8C", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
				m_mp_save_decrypt = ptr;
			});
			main_batch.add("MP Save Decrypt", "84 ? ? ? 48 8D 7C 24 ? 48 89 F9 BA", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
				m_mp_save_decrypt = ptr;
			});
			//main_batch.add("MP Save Encrypt", "44 8B 46 ? 48 8B 56 18 48 89 D9", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			//	// Unused
			//});
			main_batch.add("Load Check Profile Stat", "A8 01 ? ? ? ? ? ? 40 F6 C5", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
				m_load_check_profile_stat = ptr.add(2);
			});
			main_batch.add("Load Check Profile Stat", "F6 06 80 ? ? ? ? ? ? 4C 89 7C", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
				m_load_check_profile_stat = ptr.add(3);
			});
		}

		// Source: https://www.unknowncheats.me/forum/grand-theft-auto-v/442708-basket-transactions.html
		main_batch.add("Construct Basket", "48 83 C1 70 45 8B E1 41 8B", -1, -1, eGameBranch::Legacy, [this](memory::handle ptr) {
			m_construct_basket = ptr.sub(32).as<PVOID>();
		});
		main_batch.add("Construct Basket", "48 83 EC 20 45 89 CF 44 89 C5", -1, -1, eGameBranch::Enhanced, [this](memory::handle ptr) {
			m_construct_basket = ptr.sub(10).as<PVOID>();
		});

		main_batch.run(memory::module(nullptr));

		LPCWSTR lpClassName = g_is_enhanced ? L"sgaWindow" : L"grcWindow";
		m_hwnd              = FindWindowW(lpClassName, nullptr);
		if (!m_hwnd)
			throw std::runtime_error("Failed to find the game's window.");

		g_pointers = this;
	}

	pointers::~pointers()
	{
		g_pointers = nullptr;
	}
}
