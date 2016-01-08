/*
 *  api_utils.cpp
 *  Gizmo
 *
 *  Created by Ben Russell on 22/03/11.
 *  Copyright 2011 X-Plugins.com. All rights reserved.
 *
 */






// -- this is a helper function, not glue code.
inline float gizmo_expScale( float fInput, float fScale ){
	
	float fSMax = (float)exp(fScale);
	if( fInput < 0.f ){
		//fScale input automatically converted to positive for transform then auto converted back to negative for return.
		return -(float)(exp( fInput * (-fScale)) / fSMax) * fInput;
	}else{
		return (float)(exp( fInput * fScale) / fSMax) * fInput;
	}        
	
	return 0.f; //we never see this.
}


/**
utils.expScale( input, scale_factor )
Returns: exponentially scaled value.
*/
int gizmo_builtin_utils_expScale(lua_State *L){
	
	lua_pushnumber( L, gizmo_expScale( lua_tonumber(L,-2), lua_tonumber(L,-1) ) );
	
    return 1;
	
}




/**
utils.getStringFromCPointer( pointer_address, [length] )
Returns: String value at pointer address.
 
 WARNING: Experts only, will crash X-Plane to desktop if not used correctly.
 
 API: 11.4+
 */
int gizmo_builtin_utils_getStringFromCPointer(lua_State *L){
	
	int argc = lua_gettop( L );
	
	if( argc == 1 ){		
		lua_pushstring( L, (char*)lua_tointeger(L,-1) );
		return 1;

	}else if( argc == 2 ){
		XPLMDebugString("Gizmo: Attempting return of binary StringFromCPointer...\n");
		lua_pushlstring( L, (char*)lua_tointeger(L,-2), lua_tointeger(L,-1) );
		return 1;
		
		
		
	}else{
		return 0;
	}
}







/**
utils.popWebBrowser( url )
 Opens the system default web browser and sends user to specified URL.
  
 API: >= 11.5.6
 */
int gizmo_builtin_utils_popWebBrowser(lua_State *L){

	if( lua_gettop( L ) == 1 ){
	
	
		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );
		

		// ------------- This is example code that shows how to pop a web browser. ------------

		#if IBM
		//  http://support.microsoft.com/kb/224816
		//LONG r = 
		ShellExecute(NULL, "open", sInput, NULL, NULL, SW_SHOWNORMAL);

		#elif APL

		system( std::string(std::string("open ") + std::string(sInput)).c_str() );

		#elif LIN

		//see: http://portland.freedesktop.org/wiki/
		//system("xdg-open http://foo"); //installed on ubuntu by default.
		system( std::string(std::string("xdg-open ") + std::string(sInput)).c_str() );

		#endif

	}else{
		XPLMDebugString("Gizmo: utils.popWebBrowser - syntax error, wrong number of args.\n");
	}
	
	
	return 0;

}











