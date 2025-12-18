/**
 * @file all_scripts.hpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "fiber_pool.hpp"
#include "gta/enums.hpp"
#include "gta/stat.hpp"
#include "hooking/hooking.hpp"
#include "native_hooks.hpp"
#include "natives.hpp"
#include "services/stats/stats_service.hpp"
#include "util/scripts.hpp"

namespace big
{
	namespace all_scripts
	{
		inline void NET_GAMESERVER_RETRIEVE_INIT_SESSION_STATUS(rage::scrNativeCallContext* src)
		{
			*src->get_arg<int*>(0) = 3;
			src->set_return_value<BOOL>(TRUE);
		}


		inline void WITHDRAW_VC(rage::scrNativeCallContext* src)
		{
			int amount              = src->get_arg<int>(0);
			int last_character      = g_stats_service->get_stat_by_hash(RAGE_JOAAT("MPPLY_LAST_MP_CHAR"))->GetIntData();
			sStatData* BANK_BALANCE = g_stats_service->get_stat_by_hash(RAGE_JOAAT("BANK_BALANCE"));
			int64_t current_bank_balance = BANK_BALANCE->GetInt64Data();
			sStatData* WALLET_BALANCE = g_stats_service->get_stat_by_hash(rage::joaat(std::format("MP{}_WALLET_BALANCE", last_character)));
			if (WALLET_BALANCE == nullptr)
			{
				LOG(FATAL) << "WITHDRAW_VC: Failed to find WALLET_BALANCE stat";
				src->set_return_value<int>(0);
				return;
			}
			int64_t current_wallet_balance = WALLET_BALANCE->GetInt64Data();

			if (current_bank_balance < amount || amount < 0)
			{
				LOGF(WARNING, "WITHDRAW_VC: Invalid amount: {}, Wallet balance: {}, Bank balance: {}", amount, current_wallet_balance, current_bank_balance);
				src->set_return_value<int>(0);
				return;
			}

			BANK_BALANCE->SetInt64Data(current_bank_balance - amount);
			WALLET_BALANCE->SetInt64Data(current_wallet_balance + amount);
			src->set_return_value(amount);
		}

		inline void DEPOSIT_VC(rage::scrNativeCallContext* src)
		{
			int amount              = src->get_arg<int>(0);
			int last_character      = g_stats_service->get_stat_by_hash(RAGE_JOAAT("MPPLY_LAST_MP_CHAR"))->GetIntData();
			sStatData* BANK_BALANCE = g_stats_service->get_stat_by_hash(RAGE_JOAAT("BANK_BALANCE"));
			int64_t current_bank_balance = BANK_BALANCE->GetInt64Data();
			sStatData* WALLET_BALANCE = g_stats_service->get_stat_by_hash(rage::joaat(std::format("MP{}_WALLET_BALANCE", last_character)));
			if (WALLET_BALANCE == nullptr)
			{
				LOG(FATAL) << "DEPOSIT_VC: Failed to find WALLET_BALANCE stat";
				src->set_return_value<BOOL>(FALSE);
				return;
			}
			int64_t current_wallet_balance = WALLET_BALANCE->GetInt64Data();

			if (current_wallet_balance < amount || amount < 0)
			{
				LOGF(WARNING, "DEPOSIT_VC: Invalid amount: {}, Wallet balance: {}, Bank balance: {}", amount, current_wallet_balance, current_bank_balance);
				src->set_return_value<BOOL>(FALSE);
				return;
			}

			BANK_BALANCE->SetInt64Data(current_bank_balance + amount);
			WALLET_BALANCE->SetInt64Data(current_wallet_balance - amount);
			src->set_return_value<BOOL>(TRUE);
		}

		// Alternative to g_pointers->m_skip_money_check5
		inline void NETWORK_CLEAR_CHARACTER_WALLET(rage::scrNativeCallContext* src)
		{
			int char_slot = src->get_arg<int>(0);
			sStatData* WALLET_BALANCE = g_stats_service->get_stat_by_hash(rage::joaat(std::format("MP{}_WALLET_BALANCE", char_slot)));
			WALLET_BALANCE->SetInt64Data(0);
		}

		void RETURN_TRUE(rage::scrNativeCallContext* src)
		{
			src->set_return_value<BOOL>(TRUE);
		}

		void RETURN_FALSE(rage::scrNativeCallContext* src)
		{
			src->set_return_value<BOOL>(FALSE);
		}

		void DO_NOTHING(rage::scrNativeCallContext* src)
		{
		}
	}
}