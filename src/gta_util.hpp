#pragma once
#include "gta/script_thread.hpp"
#include "gta/tls_context.hpp"
#include "pointers.hpp"
#include "script/scrThread.hpp"

#include <network/CNetworkPlayerMgr.hpp>
#include <ped/CPedFactory.hpp>
#include <script/tlsContext.hpp>


#define CROSS_CLASS_ACCESS(legacy_type, enhanced_type, variable, expression) \
	(g_is_enhanced ? ((enhanced_type*)variable)expression : ((legacy_type*)variable)expression)

namespace big::gta_util
{
	inline CPed* get_local_ped()
	{
		if (auto ped_factory = *g_pointers->m_ped_factory)
		{
			return ped_factory->m_local_ped;
		}

		return nullptr;
	}

	inline CVehicle* get_local_vehicle()
	{
		if (const auto ped = get_local_ped(); ped)
		{
			if (const auto veh = ped->m_vehicle; veh)
			{
				return veh;
			}
		}
		return nullptr;
	}


	inline CPlayerInfo* get_local_playerinfo()
	{
		if (auto ped_factory = *g_pointers->m_ped_factory)
		{
			if (auto ped = ped_factory->m_local_ped)
			{
				return ped->m_player_info;
			}
		}

		return nullptr;
	}

	template<typename F, typename... Args>
	void execute_as_script(rage::joaat_t script_hash, F&& callback, Args&&... args)
	{
		auto tls_ctx = rage::tlsContext::get();
		for (auto thread : *g_pointers->m_script_threads)
		{
			if (!thread || !CROSS_CLASS_ACCESS(rage::scrThread, rage_enhanced::scrThread, thread, ->m_context.m_thread_id)
			    || CROSS_CLASS_ACCESS(rage::scrThread, rage_enhanced::scrThread, thread, ->m_context.m_script_hash) != script_hash)
				continue;

			auto og_thread = CROSS_CLASS_ACCESS(rage::tlsContext, rage_enhanced::tlsContext, tls_ctx, ->m_script_thread);

			CROSS_CLASS_ACCESS(rage::tlsContext, rage_enhanced::tlsContext, tls_ctx, ->m_script_thread) = thread;
			CROSS_CLASS_ACCESS(rage::tlsContext, rage_enhanced::tlsContext, tls_ctx, ->m_is_script_thread_active) = true;

			std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

			CROSS_CLASS_ACCESS(rage::tlsContext, rage_enhanced::tlsContext, tls_ctx, ->m_script_thread) = og_thread;
			CROSS_CLASS_ACCESS(rage::tlsContext, rage_enhanced::tlsContext, tls_ctx, ->m_is_script_thread_active) = og_thread != nullptr;

			return;
		}
	}


	inline rage::scrThread* find_script_thread(rage::joaat_t hash)
	{
		for (auto thread : *g_pointers->m_script_threads)
		{
			if (thread && CROSS_CLASS_ACCESS(rage::scrThread, rage_enhanced::scrThread, thread, ->m_context.m_thread_id)
			    && CROSS_CLASS_ACCESS(rage::scrThread, rage_enhanced::scrThread, thread, ->m_context.m_script_hash) == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}

	inline rage::scrThread* find_script_thread_by_id(std::uint32_t id)
	{
		for (auto thread : *g_pointers->m_script_threads)
		{
			if (thread && CROSS_CLASS_ACCESS(rage::scrThread, rage_enhanced::scrThread, thread, ->m_context.m_thread_id) == id)
			{
				return thread;
			}
		}

		return nullptr;
	}
}
