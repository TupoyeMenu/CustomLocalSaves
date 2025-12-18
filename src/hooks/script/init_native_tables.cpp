/**
 * @file init_native_tables.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "hooking/hooking.hpp"
#include "native_hooks/native_hooks.hpp"

namespace big
{
	bool hooks::init_native_tables(rage::scrProgram* program)
	{
		bool ret = g_hooking->get_original<hooks::init_native_tables>()(program);
		if(program->m_code_blocks && program->m_code_size)
		{
			g_native_hooks->hook_program(program);
		}

		return ret;
	}
}