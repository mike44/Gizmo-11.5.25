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


//Dont put anything but macro defs in here.


//version information down to the minute
#define GIZMO_UPDATER_VERSION 1105251945
#define GIZMO_VERSION_COMMON "11.5.25+JIT"
#define GIZMO_VERSION_MAJOR  "11"

	#if APL
		#if __POWERPC__
			#define GIZMO_OS_PLATFORM "Mac.PPC"
		#else
			#define GIZMO_OS_PLATFORM "Mac.Intel"
		#endif
	#elif IBM
		#define GIZMO_OS_PLATFORM "Win"
	#elif LIN
		#define GIZMO_OS_PLATFORM "Lin"
	#endif


	//Makes: "10.11.22+JIT Mac.Intel"
	#define GIZMO_VERSION GIZMO_VERSION_COMMON " " GIZMO_OS_PLATFORM




//These two flags are used to control various apporachs to drawing the Bullet state.
#define GIZMO_USE_SANDY_DRAW				0
#define GIZMO_USE_BR_DRAW					1



//Used to debug the particle engine, ultimately we want to ship with this set to 1.
#define GIZMO_THREADED_PARTICLES			0



