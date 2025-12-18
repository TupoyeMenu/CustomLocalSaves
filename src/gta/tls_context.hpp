#pragma once
#include <intrin.h>

namespace rage_enhanced
{
	class scrThread;
	class tlsContext
	{
	public:
		char m_Pad1[0x7A8];
		rage::scrThread* m_script_thread;
		bool m_is_script_thread_active;

		static tlsContext* get()
		{
			constexpr std::uint32_t TlsIndex = 0x0;
			return *reinterpret_cast<tlsContext**>(__readgsqword(0x58) + TlsIndex);
		}
	};
	static_assert(sizeof(tlsContext) == 0x7B8);
}