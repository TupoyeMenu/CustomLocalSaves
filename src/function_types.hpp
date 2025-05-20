#pragma once
#include "gta/fwddec.hpp"
#include <script/scrNativeHandler.hpp>

namespace big::functions
{
	using run_script_threads_t = bool (*)(std::uint32_t ops_to_execute);
	using script_vm = rage::eThreadState (*)(uint64_t* stack, int64_t** scr_globals, rage::scrProgram* program, rage::scrThreadContext* ctx);

	using ptr_to_handle = Entity (*)(void*);
	using handle_to_ptr = rage::CDynamicEntity* (*)(Entity);
}
