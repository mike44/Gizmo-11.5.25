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





/**
 API: 10.11.06+
 These are not high precision timers. They will improve as time allows.
 Timers faster than the users frame-rate are not currently possible. 
 
 A timer clock-source thread OR OTHER multi-platform high precision portable timer function would allow for this.
 
 Timer intervals:
 Positive timers are measured in seconds.
 Negative timers are measured in frames.
 */






/**
 timer.newOneShot( "timer_function_name", interval )
 Returns: timer_id
 Create a timer that will fire once and die.
 */
int gizmo_builtin_timer_newOneShot(lua_State *L){ 

	size_t szLen;
	const char *caPayload = lua_tolstring( L, -2, &szLen );
	float fInterval = lua_tonumber( L, -1 );

	
		std::string sSig = caPayload;
		for( unsigned int x=0; x < gizmo_svAllUserTimers.size(); x++ ){
			if(gizmo_svAllUserTimers[x]->sFunctionName == sSig){
				if( gizmo_svAllUserTimers[x]->iEnabled ){
					//A timer exists with this name already. Fail.
					
					{
					char caDbg[1024];
					sprintf(caDbg, "Gizmo: FAIL: timer.newOneShot(\"%s\", %0.3f) - function already registered for timers.\n", caPayload, fInterval);
					XPLMDebugString(caDbg);
					}
					
					return 0;
				}
			}
		}
	
	
	
		gizmo_user_timer_info* ip = new gizmo_user_timer_info();
			ip->sFunctionName = caPayload;
			ip->fInterval = fInterval;
			ip->iOneShot = 1;
		
		int ret = gizmo_svAllUserTimers.size();
		gizmo_svAllUserTimers.push_back( ip );
		
		//timer init is defered and done from the pool
		
		#if 0
			char caDbg[2048];
			sprintf(caDbg, "Gizmo: timer.newOneShot('%s',%0.3f)\n", caPayload, fInterval);
			XPLMDebugString(caDbg);
		#endif

	lua_pushinteger(L, ret);
	return 1; 

}



/**
 timer.newTimer( "timer_function_name", interval )
 Returns: timer_id
 Create a timer that will fire repeatedly forever until stopped.
 */
int gizmo_builtin_timer_newTimer(lua_State *L){ 

	size_t szLen;
	const char *caPayload = lua_tolstring( L, -2, &szLen );
	float fInterval = lua_tonumber( L, -1 );
	
	
		std::string sSig = caPayload;
		for( unsigned int x=0; x < gizmo_svAllUserTimers.size(); x++ ){
			if(gizmo_svAllUserTimers[x]->sFunctionName == sSig){
				if( gizmo_svAllUserTimers[x]->iEnabled ){
					//A timer exists with this name already. Fail.
					
					{
					char caDbg[1024];
					sprintf(caDbg, "Gizmo: FAIL: timer.newTimer(\"%s\", %0.3f) - function already registered for timers.\n", caPayload, fInterval);
					XPLMDebugString(caDbg);
					}
					
					return 0;
				}
			}
		}
	
	
	

		gizmo_user_timer_info* ip = new gizmo_user_timer_info();
			ip->sFunctionName = caPayload;
			ip->fInterval = fInterval;
			
		int ret = gizmo_svAllUserTimers.size();
		gizmo_svAllUserTimers.push_back( ip );

		//timer init is defered and done from the pool

		char caDbg[2048];
		sprintf(caDbg, "Gizmo: timer.newTimer('%s',%0.3f)\n", caPayload, fInterval);
		XPLMDebugString(caDbg);

	lua_pushinteger(L, ret);
	return 1; 

}



/**
 timer.stop( timer_id )
 Stops a timer. The function for timer_id will not be called again.
 */
int gizmo_builtin_timer_stop(lua_State *L){ 

	//This function marks a timer for destruction.
	//Actual destruction is done at the head of the core flight loop.

	
	int iSlotNumber = lua_tointeger( L, -1 );
	
	
	//basic protection, stop sim from crashing
	if(( iSlotNumber > (int)gizmo_svAllUserTimers.size()-1 ) || ( iSlotNumber < 0 )){
		XPLMDebugString("Gizmo: timer.stop() called on invalid timer handle. Ignored.\n");
		return 0;
	}
	
		gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ iSlotNumber ];	
		ip->iEnabled = 0;

		char caDbg[2048];
		sprintf(caDbg, "Gizmo: timer.stop(%i)\n", iSlotNumber);
		XPLMDebugString(caDbg);

	return 0; 
	
}



/**
 timer.reset( timer_id )
 Reset the internal value of a timer so that it restarts.
 This function will restart a timer.stop()ed timer_id.
 */
int gizmo_builtin_timer_reset(lua_State *L){ 
	
	int iSlotNumber = lua_tointeger( L, -1 );
	
	//basic protection, stop sim from crashing
	if(( iSlotNumber > (int)gizmo_svAllUserTimers.size()-1 ) || ( iSlotNumber < 0 )){
		XPLMDebugString("Gizmo: timer.reset() called on invalid timer handle. Ignored.\n");
		return 0;
	}
	
	gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ iSlotNumber ];	
	ip->fCounter = 0.f; //restart timer.
	ip->iEnabled = 1;

	//char caDbg[2048];
	//sprintf(caDbg, "Gizmo: timer.stop(%i)\n", iSlotNumber);
	//XPLMDebugString(caDbg);

	
	return 0; 
	
}





/**
 timer.getValue( timer_id )
 Returns: numeric timber value, the current value of a timer, can be used to detect current fill %.
 */
int gizmo_builtin_timer_getValue(lua_State *L){ 

	int iSlotNumber = lua_tointeger( L, -1 );
	
	//basic protection, stop sim from crashing
	if(( iSlotNumber > (int)gizmo_svAllUserTimers.size()-1 ) || ( iSlotNumber < 0 )){
		XPLMDebugString("Gizmo: timer.getValue() called on invalid timer handle. Ignored.\n");
		return 0;
	}
	
		gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ iSlotNumber ];	
		
		lua_pushnumber( L, ip->fCounter );


	return 1; 
	
}





/**
 timer.getInterval( timer_id )
 Returns: numeric timer interval.
 */
int gizmo_builtin_timer_getInterval(lua_State *L){ 

	
	int iSlotNumber = lua_tointeger( L, -1 );
	
	
	//basic protection, stop sim from crashing
	if(( iSlotNumber > (int)gizmo_svAllUserTimers.size()-1 ) || ( iSlotNumber < 0 )){
		XPLMDebugString("Gizmo: timer.getValue() called on invalid timer handle. Ignored.\n");
		return 0;
	}
	
		gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ iSlotNumber ];	
		
		lua_pushnumber( L, ip->fInterval );


	return 1; 
	
}



/**
 timer.setInterval( timer_id, interval )
 Adjust a timers firing rate.
 */
int gizmo_builtin_timer_setInterval(lua_State *L){ 

	
	int iSlotNumber = lua_tointeger( L, -2 );
	float fValue = lua_tonumber( L, -1 );
	
	
	//basic protection, stop sim from crashing
	if(( iSlotNumber > (int)gizmo_svAllUserTimers.size()-1 ) || ( iSlotNumber < 0 )){
		XPLMDebugString("Gizmo: timer.getValue() called on invalid timer handle. Ignored.\n");
		return 0;
	}
	
		gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ iSlotNumber ];	
		
		ip->fInterval = fValue;


	return 0; 
	
}



