#pragma once
#include "common.hpp"
#include "detour_hook.hpp"
#include "gta/fwddec.hpp"
#include "vmt_hook.hpp"

#include <script/scrNativeHandler.hpp>
#include <MinHook.h>

namespace big
{
	struct hooks
	{
		static bool run_script_threads(std::uint32_t ops_to_execute);

		static uint32_t network_can_access_multiplayer(uint32_t a1, uint64_t* a2);
		static void create_native(void* a1, rage::scrNativeHash native_hash, rage::scrNativeHandler native_func);

		static void create_stat(void* p1, const char** name);
		static bool mp_stats_save(void* _this, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6);
		static bool mp_save_download(CSavegameQueuedOperation_MPStats_Load* _this);
		static bool construct_basket(void* _this, int *transaction_id, int category_type, int action_type, int target);
		static bool profile_stats_download(int p0, void* p1, void* p2);
		static bool profile_stats_save(void* p0, void* p1, uint32_t* p2, uint32_t p3);
	};

	class minhook_keepalive
	{
	public:
		minhook_keepalive()
		{
			MH_Initialize();
		}
		~minhook_keepalive()
		{
			MH_Uninitialize();
		}
	};

	class hooking
	{
		friend hooks;

	public:
		explicit hooking();
		~hooking();

		void enable();
		void disable();

		class detour_hook_helper
		{
			friend hooking;

			using ret_ptr_fn = std::function<void*()>;

			ret_ptr_fn m_on_hooking_available = nullptr;

			detour_hook* m_detour_hook = nullptr;

			~detour_hook_helper();

			void enable_hook_if_hooking_is_already_running();

			template<auto detour_function>
			struct hook_to_detour_hook_helper
			{
				static inline detour_hook* m_detour_hook;
			};

			template<auto detour_function>
			static detour_hook_helper* add_internal(detour_hook* dh)
			{
				auto d           = new detour_hook_helper();
				d->m_detour_hook = dh;

				m_detour_hook_helpers.push_back(d);
				hook_to_detour_hook_helper<detour_function>::m_detour_hook = dh;

				return d;
			}

		public:
			template<auto detour_function>
			static void add(const std::string& name, void* target)
			{
				auto d = add_internal<detour_function>(new detour_hook(name, target, (void*)detour_function));

				d->enable_hook_if_hooking_is_already_running();
			}

			template<auto detour_function>
			static void* add_lazy(const std::string& name, detour_hook_helper::ret_ptr_fn on_hooking_available)
			{
				auto d                    = add_internal<detour_function>(new detour_hook(name, detour_function));
				d->m_on_hooking_available = on_hooking_available;

				d->enable_hook_if_hooking_is_already_running();

				return nullptr;
			}
		};

		template<auto detour_function>
		static auto get_original()
		{
			return detour_hook_helper::hook_to_detour_hook_helper<detour_function>::m_detour_hook->template get_original<decltype(detour_function)>();
		}

	private:
		bool m_enabled{};
		minhook_keepalive m_minhook_keepalive;

#ifdef ENABLE_GUI
		WNDPROC m_og_wndproc = nullptr;
#endif

		static inline std::vector<detour_hook_helper*> m_detour_hook_helpers;
	};

	inline hooking* g_hooking{};
}