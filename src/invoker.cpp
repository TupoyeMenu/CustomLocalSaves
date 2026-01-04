/**
 * @file invoker.cpp
 */

#include "invoker.hpp"

#include "crossmap.hpp"
#include "hooking/hooking.hpp"
#include "pointers.hpp"
#include "script/scrNativeHandler.hpp"
#include "script/scrProgram.hpp"

namespace big
{
	native_call_context::native_call_context()
	{
		m_return_value = &m_return_stack[0];
		m_args         = &m_arg_stack[0];
	}

	void native_invoker::add_native_handler(rage::scrNativeHash hash, rage::scrNativeHandler handler)
	{
		m_handler_cache[hash] = handler;
	}

	rage::scrNativeHandler native_invoker::get_native_handler(rage::scrNativeHash hash)
	{
		return m_handler_cache[hash];
	}

	void fix_vectors(native_call_context& call_context)
	{
		for (; call_context.m_data_count;
		    call_context.m_orig[call_context.m_data_count][2].Float = call_context.m_buffer[call_context.m_data_count].z)
		{
			--call_context.m_data_count;
			call_context.m_orig[call_context.m_data_count]->Float   = call_context.m_buffer[call_context.m_data_count].x;
			call_context.m_orig[call_context.m_data_count][1].Float = call_context.m_buffer[call_context.m_data_count].y;
		}
		--call_context.m_data_count;
	}

	void native_invoker::begin_call()
	{
		m_call_context.reset();
	}

	void native_invoker::end_call(rage::scrNativeHash hash)
	{
		try
		{
			rage::scrNativeHandler handler = m_handler_cache.at(hash);

			// return address checks are no longer a thing
			handler(&m_call_context);
			fix_vectors(m_call_context);
		}
		catch(const std::out_of_range& ex)
		{
			LOG(WARNING) << "Failed to find " << HEX_TO_UPPER(hash) << " native's handler.";
		}
	}
}
