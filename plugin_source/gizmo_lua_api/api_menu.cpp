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
 menu.newItem( "Menu Label", "MenuEventName" )
 Menu will be created at: Plugins > Gizmo > Scripts > "Menu Label"
 When clicked Gizmo will call the function: MenuEventName_OnClick()
 */
int gizmo_builtin_menu_newItem(lua_State *L){

	//ensure we have enough args from lua
	if( lua_gettop(L) != 2 ){ return 0; }


	size_t szLen;
	const char *sInput_Caption			= lua_tolstring( L, -2, &szLen );
	const char *sInput_ScriptHandler	= lua_tolstring( L, -1, &szLen );


	std::string* sScriptHandler = new std::string( sInput_ScriptHandler );

	int iFoundItem = 0;
	for( int x=0; x<gizmo_Pool_VehicleMenuItems.size(); x++ ){
		if( *(gizmo_Pool_VehicleMenuItems[x]) == *sScriptHandler ){
			iFoundItem = 1;
		}
	}
	
	
	if( iFoundItem ){
		//ignore action, return now.
		XPLMDebugString("Gizmo: menu.newItem() - handler already registered, ignored.\n");
		return 0;
	}
	
	
	gizmo_Pool_VehicleMenuItems.push_back(sScriptHandler);
	


	//turn the scripts menu item back on because we're going to put something in it!
	XPLMEnableMenuItem(gizmo_Menu_RootItem, 2, 1);

	//printf("Gizmo: newMenuItem( %s, %s )\n", sInput_Caption, sScriptHandler_Leaked );

		XPLMAppendMenuItem(
							gizmo_Menu_VehicleItems,
							sInput_Caption,
							//(void *)sScriptHandler_Leaked, //item ref
							(void*)sScriptHandler,
							1 //force english
							);

	return 0;
}




