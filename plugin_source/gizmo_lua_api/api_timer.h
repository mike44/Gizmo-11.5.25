/*
	Gizmo - A Lua based scripting engine for X-Plane.
    Copyright (C) 2005-2011  Ben Russell, br@x-plugins.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/





int gizmo_builtin_timer_getInterval(lua_State *L);
int gizmo_builtin_timer_getValue(lua_State *L);

int gizmo_builtin_timer_newOneShot(lua_State *L);
int gizmo_builtin_timer_newTimer(lua_State *L);
int gizmo_builtin_timer_reset(lua_State *L);


int gizmo_builtin_timer_setInterval(lua_State *L);
int gizmo_builtin_timer_stop(lua_State *L);



// ----- Support Code Below -----


class gizmo_user_timer_info{
	public:
	
	gizmo_user_timer_info(){
		this->sFunctionName = "default_undefined_timer_function";
		
		this->fInterval = 1.0;
		this->fCounter = 0.0f;
		
		this->iEnabled = 1;
		
		this->iOneShot = 0;
		
	};
	
	std::string sFunctionName;
	float fInterval;
	float fCounter;
	int iEnabled;
	int iOneShot;
};

std::vector<gizmo_user_timer_info*> gizmo_svAllUserTimers;


