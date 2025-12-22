/**
 * @file main.cpp
 * @brief File with the DllMain function, used for initialization.
 */

#include "backend/backend.hpp"
#include "byte_patch_manager.hpp"
#include "common.hpp"
#include "fiber_pool.hpp"
#include "file_manager.hpp"
#include "hooking/hooking.hpp"
#include "native_hooks/native_hooks.hpp"
#include "pointers.hpp"
#include "script_mgr.hpp"
#include "services/stats/stats_service.hpp"
#include "thread_pool.hpp"
#include "util/is_enhanced.hpp"

#ifdef ENABLE_EXCEPTION_HANDLER
	#include "logger/exception_handler.hpp"
#endif

#include <rage/gameSkeleton.hpp>


BOOL APIENTRY DllMain(HMODULE hmod, DWORD reason, PVOID)
{
	using namespace big;
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hmod);

		g_is_enhanced = big::is_enhanced();

		g_hmodule     = hmod;
		g_main_thread = CreateThread(
		    nullptr,
		    0,
		    [](PVOID) -> DWORD {
#ifdef ENABLE_EXCEPTION_HANDLER
			    auto handler = exception_handler();
#endif
			    std::srand(std::chrono::system_clock::now().time_since_epoch().count());

			    std::filesystem::path base_dir = std::getenv("appdata");
			    base_dir /= PROJECT_NAME;
			    g_file_manager.init(base_dir);
			    g_log.initialize(PROJECT_NAME,
			        g_file_manager.get_project_file("./cout.log")
#ifndef ENABLE_GUI
			            ,
			        false // Disable log window when GUI is disabled.
#endif
			    );
			    try
			    {
				    auto thread_pool_instance = std::make_unique<thread_pool>();
				    LOGF(INFO, "Thread pool initialized.");

				    g.init(g_file_manager.get_project_file("./settings.json"));
				    LOGIF(INFO, g.enable_debug_logs, "Settings Loaded.");

				    auto pointers_instance = std::make_unique<pointers>();
				    LOGIF(INFO, g.enable_debug_logs, "Pointers initialized.");

				    auto byte_patch_manager_instance = std::make_unique<byte_patch_manager>();
				    LOGIF(INFO, g.enable_debug_logs, "Byte Patch Manager initialized.");

				    auto fiber_pool_instance = std::make_unique<fiber_pool>(10);
				    LOGIF(INFO, g.enable_debug_logs, "Fiber pool initialized.");

				    auto hooking_instance = std::make_unique<hooking>();
				    LOGIF(INFO, g.enable_debug_logs, "Hooking initialized.");

				    auto stats_service_instance = std::make_unique<stats_service>();
				    LOGIF(INFO, g.enable_debug_logs, "Stats service initialized.");

				    g_script_mgr.add_script(std::make_unique<script>(&backend::loop));
				    LOGIF(INFO, g.enable_debug_logs, "Scripts registered.");

				    g_hooking->enable();
				    LOGIF(INFO, g.enable_debug_logs, "Hooking enabled.");

				    auto native_hooks_instance = std::make_unique<native_hooks>();
				    LOGIF(INFO, g.enable_debug_logs, "Dynamic native hooker initialized.");

				    while (g_running)
					    std::this_thread::sleep_for(500ms);

				    g_hooking->disable();
				    LOGIF(INFO, g.enable_debug_logs, "Hooking disabled.");

				    g_script_mgr.remove_all_scripts();
				    LOGIF(INFO, g.enable_debug_logs, "Scripts unregistered.");

				    // cleans up the thread responsible for saving settings
				    g.destroy();

				    // Make sure that all threads created don't have any blocking loops
				    // otherwise make sure that they have stopped executing
				    thread_pool_instance->destroy();
				    LOGIF(INFO, g.enable_debug_logs, "Destroyed thread pool.");

				    stats_service_instance.reset();
				    LOGIF(INFO, g.enable_debug_logs, "Stats service uninitialized.");

				    hooking_instance.reset();
				    LOGIF(INFO, g.enable_debug_logs, "Hooking uninitialized.");

				    native_hooks_instance.reset();
				    LOGIF(INFO, g.enable_debug_logs, "Dynamic native hooker uninitialized.");

				    fiber_pool_instance.reset();
				    LOGIF(INFO, g.enable_debug_logs, "Fiber pool uninitialized.");

				    byte_patch_manager_instance.reset();
				    LOGIF(INFO, g.enable_debug_logs, "Byte Patch Manager uninitialized.");

				    pointers_instance.reset();
				    LOGIF(INFO, g.enable_debug_logs, "Pointers uninitialized.");
			    }
			    catch (std::exception const& ex)
			    {
				    LOG(WARNING) << ex.what();
				    MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
			    }

			    LOG(INFO) << "Farewell!";
			    g_log.destroy();

			    CloseHandle(g_main_thread);
			    FreeLibraryAndExitThread(g_hmodule, 0);
		    },
		    nullptr,
		    0,
		    &g_main_thread_id);
	}

	return true;
}
