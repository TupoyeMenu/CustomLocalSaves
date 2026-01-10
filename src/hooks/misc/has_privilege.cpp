#include "hooking/hooking.hpp"
#include "gta/enums.hpp"

namespace big
{
	bool hooks::has_privilege(void* p1, uint32_t privilege)
	{
		switch ((eRosPrivilege)privilege)
		{
		case eRosPrivilege::CREATE_TICKET:
		case eRosPrivilege::MULTIPLAYER:
		case eRosPrivilege::PRIVATE_MSG:
		case eRosPrivilege::SOCIAL_CLUB:
		case eRosPrivilege::MP_TEXT_COMMUNICATION:
		case eRosPrivilege::MP_VOICE_COMMUNICATION:
		case eRosPrivilege::REPORTING:
			return true;

		case eRosPrivilege::BANNED:
		case eRosPrivilege::CHEATER:
			return false;

		default: break;
		};

		return g_hooking->get_original<hooks::has_privilege>()(p1, privilege);
	}
}
