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
 xp.clearReplay()
 Clears the X-Plane replay buffer.
 */
int gizmo_builtin_xp_clearReplay(lua_State *L){
	XPLMDebugString("Gizmo: xp.clearReplay()");
	XPLMLoadDataFile(
					xplm_DataFile_ReplayMovie,
					"" //loading a blank file name has the effect of reseting the replay data
					);
	return 0;
}




/**
xp.commandBeginByName( "sim/command" )
 
 Equivalent to pressing a joystick button or hotkey for the sim/command.
 
 X-Plane will call the hold down event until you call gfx.commandEndByName( "sim/command" )
 */
int gizmo_builtin_xp_commandBeginByName(lua_State *L){
	size_t szLen;
	const char *sInput = lua_tolstring( L, -1, &szLen );
	//printf("Gizmo: xp.commandBeginByName(%s);\n", sInput );
		XPLMCommandBegin(
					 XPLMFindCommand( sInput )
					 );
	return 0;
}



/**
 xp.commandEndByName( "sim/command" )
 
 Equivalent to releasing a joystick button or hotkey for the sim/command.
 */
int gizmo_builtin_xp_commandEndByName(lua_State *L){
	size_t szLen;
	const char *sInput = lua_tolstring( L, -1, &szLen );
	//printf("Gizmo: xp.commandEndByName(%s);\n", sInput );
		XPLMCommandEnd(
					 XPLMFindCommand( sInput )
					 );
	return 0;
}



/**
xp.commandOnceByName( "sim/command" )
 
 Equivalent to pressing a joystick button or hotkey for the sim/command.
 This is a momentary command.
 */
int gizmo_builtin_xp_commandOnceByName(lua_State *L){
	size_t szLen;
	const char *sInput = lua_tolstring( L, -1, &szLen );

	//printf("Gizmo: xp.commandOnceByName(%s);\n", sInput );

	XPLMCommandRef cmdRef = XPLMFindCommand( sInput );
	if( cmdRef == NULL ){
		//printf("Gizmo: xp.commandOnceByName: Unknown command string: (%s)\n", sInput);

	}else{
		XPLMCommandOnce(
					cmdRef
					);
	}

	return 0;
}





/**
xp.getCycleNumber()
 Returns: number of frames rendered since sim started
 */
int gizmo_builtin_xp_getCycleNumber(lua_State *L){
	lua_pushnumber( L, XPLMGetCycleNumber() );
	return 1;
}




/**
xp.getDataref( "sim/dataref" )
 Returns: dataref_id
 
 Does NOT accept part numbers:
 This will NOT work: xp.getDataref( "sim/dataref[99999]" )
 
 dr_parameter = xp.getDataref("sim/cockpit/switches/ice")	
 */
int gizmo_builtin_xp_getDataref(lua_State *L){

	size_t szLen;
	const char *sInput = lua_tolstring( L, -1, &szLen );
	
	
	//NOOB check, artists try and feed part numbers to this function, NAY work.
	for( unsigned int sx=0; sx < strlen( sInput ); sx++ ){
		if( sInput[sx] == '[' ){
			
			//Check to see if prefix of dataref name is sim/weather
			char caPrefixCheck[32];
			memset(caPrefixCheck, 0, 32);
			
			memcpy(caPrefixCheck, sInput, strlen("sim/weather/"));
			
			if( strcmp(caPrefixCheck, "sim/weather/") == 0 ){
				//X-Plane valid datarefs with this prefix contain part number notations.
				
			}else{
				gizmo_gui_ScriptError("xp.getDataref() does not accept part numbers.");
				return 0;
			}
		}
	}
	
	
	//If we have any existing dref handles we will search for a match first.
	if( gizmo_customDataRefs.size() > 0 ){		
		for( unsigned int drx=0; drx < gizmo_customDataRefs.size(); drx++ ){
			if( gizmo_customDataRefs[drx]->getName() == std::string( sInput ) ){
				//we found an existing match for the dref name, push the index id into Lua and return to scripted control.
				lua_pushnumber( L, drx );
				return 1;
			}
		}	
	}
	
	
	//either no handles have been accessed OR we could not find an existing copy of this handle.
	//Create a new handle, store it and pass the new index ID back to Lua.
	
	XDataRefCustom *h = new XDataRefCustom();
	h->setName( sInput, 0 );

	gizmo_customDataRefs.push_back( h );

	lua_pushnumber( L, gizmo_customDataRefs.size()-1 );
	return 1; //ret 1 result on the Lua stack, the dataref handle id.

}





/**
 xp.getElapsedTime()
 Returns: Number of seconds since sim started, floating point number.
 */
int gizmo_builtin_xp_getElapsedTime(lua_State *L){
	lua_pushnumber( L, XPLMGetElapsedTime() );
	return 1;
}




/**
 xp.newIntegerDataref( dataref_name, [hook_event_name] )
 Creates a new integer (no decimal allowed) dataref.
 
 hook_event_name is optional and can be used to make Gizmo notify your
 code when your dataref changes, you do not have to 'watch' or poll it.

 function hook_event_name_OnRead()
	return acf.getIAS() * 10
 end
 
 function hook_event_OnWrite( newValue )
	sound.say("dataref changed")
 end
 */
int gizmo_builtin_xp_newIntegerDataref(lua_State *L){


	//ensure we have enough args from lua
	if( lua_gettop(L) == 1 ){ 

		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );

		//int iCustomDatarefIDNumber = (int)gizmo_customDataRefs.size();

			XDataRefCustom *h = new XDataRefCustom();
			h->setName( sInput, 1 );
			h->setType( xplmType_Int );

				gizmo_customDataRefs.push_back( h );

			gizmo_NewCustomRef( h, 0 );

		lua_pushnumber( L, gizmo_customDataRefs.size()-1 );
		
		return 1;
	
	}else if( lua_gettop(L) == 2 ){ 
	
		//Toms Feature.
		//If we get two arguments to this function the user is giving us the name of a hook.
	
		size_t szLen;
		const char *sInput = lua_tolstring( L, -2, &szLen );

		size_t szLenHook;
		const char *sInputHook = lua_tolstring( L, -1, &szLenHook );

		//int iCustomDatarefIDNumber = (int)gizmo_customDataRefs.size();

			XDataRefCustom *h = new XDataRefCustom();
			h->setName( sInput, 1 );
			h->setHookName( sInputHook );
			h->setType( xplmType_Int );

				gizmo_customDataRefs.push_back( h );

			gizmo_NewCustomRef( h, 1 ); //passing a 1 as 2nd parameter changes the way the dataref is registered. Gizmo will call lua for values.

		lua_pushnumber( L, gizmo_customDataRefs.size()-1 );

		return 1;

	}


	return 0;
}





/**
 xp.newDataref( dataref_name, [hook_event_name] )
 Creates a new floating-point (decimal number) dataref.
 hook_event_name is optional and can be used to make Gizmo notify your code when your dataref changes, you do not have to 'watch' or poll it.

 function hook_event_name_OnRead()
	return acf.getIAS() * 10
 end
 
 function hook_event_OnWrite( newValue )
	sound.say("dataref changed")
 end
 */

/**
 xp.newFloatingPointDataref( dataref_name, [hook_event_name] )
 Creates a new floating-point (decimal number) dataref.
 hook_event_name is optional and can be used to make Gizmo notify your code when your dataref changes, you do not have to 'watch' or poll it.

 function hook_event_name_OnRead()
	return acf.getIAS() * 10
 end
 
 function hook_event_OnWrite( newValue )
	sound.say("dataref changed")
 end
 */
int gizmo_builtin_xp_newFloatingPointDataref(lua_State *L){

	//ensure we have enough args from lua
	if( lua_gettop(L) == 1 ){ 

		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );

		//int iCustomDatarefIDNumber = (int)gizmo_customDataRefs.size();

			XDataRefCustom *h = new XDataRefCustom();
			h->setName( sInput, 1 );

				gizmo_customDataRefs.push_back( h );

			gizmo_NewCustomRef( h, 0 );

		lua_pushnumber( L, gizmo_customDataRefs.size()-1 );

		return 1;
	
	}else if( lua_gettop(L) == 2 ){ 
	
		//Toms Feature.
		//If we get two arguments to this function the user is giving us the name of a hook.
	
		size_t szLen;
		const char *sInput = lua_tolstring( L, -2, &szLen );

		size_t szLenHook;
		const char *sInputHook = lua_tolstring( L, -1, &szLenHook );

		//int iCustomDatarefIDNumber = (int)gizmo_customDataRefs.size();

			XDataRefCustom *h = new XDataRefCustom();
			h->setName( sInput, 1 );
			h->setHookName( sInputHook );

				gizmo_customDataRefs.push_back( h );

			gizmo_NewCustomRef( h, 1 ); //passing a 1 as 2nd parameter changes the way the dataref is registered. Gizmo will call lua for values.

		lua_pushnumber( L, gizmo_customDataRefs.size()-1 );

		return 1;

	}
	
	return 0;
}




/**
 xp.getInt( dataref_id )
 Returns: value of dataref_id Integer number
 See xp.getDataref("sim/foo") and xp.newDataref("custom/dataref") for more info.
 */
int gizmo_builtin_xp_getInt(lua_State *L){

	lua_Integer lnVIndexID = lua_tointeger( L, -1 );

	if( lnVIndexID >= 0 && lnVIndexID < (int)gizmo_customDataRefs.size() ){
		lua_pushinteger( L,
					//XPLMGetDatai( gizmo_customDataRefs[ lnVIndexID ] )
					gizmo_customDataRefs[lnVIndexID]->getInt()
					);
	}else{
		lua_pushinteger( L, 0 );
	}

	return 1;
}


/**
 xp.getFloat( dataref_id )
 Returns: value of dataref_id decimal number
 See xp.getDataref("sim/foo") and xp.newDataref("custom/dataref") for more info.
 */
int gizmo_builtin_xp_getFloat(lua_State *L){

	//dataref id number
	lua_Integer lnVIndexID = lua_tointeger( L, -1 );

	if( lnVIndexID >= 0 && lnVIndexID < (int)gizmo_customDataRefs.size() ){ 
		lua_pushnumber( L,
				   //XPLMGetDataf( gizmo_customDataRefs[ lnVIndexID ] )
				   gizmo_customDataRefs[lnVIndexID]->getFloat()
				   );
	}else{
		lua_pushnumber( L, 0 );
	}

	return 1;
}




/**
 xp.getIntV( dataref_id, start_at, number_to_fetch )
 Returns: multiple values for dataref_id as numbers
 See xp.getDataref("sim/foo") and xp.newDataref("custom/dataref") for more info.
 start_at refers to the part number in sim/dataref[99]
 
 In X-Plane, OBJ8 and C, part numbers start at 0.
 In Lua, part numbers start at 1.
 
 For this reason, the start_at parameter also starts at 1.
 
 If you want to get sim/dataref[0] use:
 value = xp.getFloatv( dataref_id, 1, 1 )
 
 We ask Gizmo to get us one value, starting at the first item.
 
 This seems a little bit annoying, but when you start using your tables of values
 with the rest of Luas functions you will understand why there is a little "shift"
 here when we push numbers in and out of X-Planes dataref systems.	
 */
int gizmo_builtin_xp_getIntV(lua_State *L){

	//dataref id number
	lua_Integer lint_DatarefIndexID = lua_tointeger( L, -3 );

	//offset and count args
	lua_Integer lint_Offset = lua_tointeger( L, -2 );
	lua_Integer lint_Count = lua_tointeger( L, -1 );
	
	if( lint_Offset == 0 ){ lint_Offset = 1; };

	std::vector<int>* tmpVector = gizmo_customDataRefs[lint_DatarefIndexID]->getIntV( lint_Offset-1, lint_Count );

	for( unsigned int x=0; x < tmpVector->size(); x++ ){

		lua_pushinteger( L,
						(*tmpVector)[x]
						);

	}

	unsigned int retSize = tmpVector->size();
	delete tmpVector;

	return retSize;
}






/**
 xp.getPaused()
 Returns: boolean
 if( not xp.getPaused() )then
 --sim is NOT paused, do stuff
 else
 --sim is paused, pause custom systems
 end
 */
int gizmo_builtin_xp_getPaused(lua_State *L){

    lua_pushboolean( L, XPLMGetDatai( gizmo_xpdr_Paused ) );

	return 1;

}




/**
 xp.getPluginsFolder()
 Returns: String containing path to plugins folder, includes trailing slash. All slashes are set to /
 */
int gizmo_builtin_xp_getPluginsFolder(lua_State *L){

    lua_pushstring( L, (const char*)gizmo_pluginFolderPath );

	return 1;

}




/**
 xp.getgetXPlaneFolder()
 Returns: String containing path to X-Plane folder, includes trailing slash. All slashes are set to /
 */
int gizmo_builtin_xp_getXPlaneFolder(lua_State *L){

    lua_pushstring( L, (const char*)gizmo_xplaneFolderPath );

    return 1;
}



/**
 xp.getLanguage()
 Returns: Language as Top Level Domain Code in a string.
 Possible return values:
 "??" - Unknown
 "us" - English
 "fr" - French
 "de" - German
 "it" - Italian
 "es" - Spanish
 "kr" - Korean
 "ru" - Russian
 "gr" - Greek
 "jp" - Japanese
 "cn" - Chinese
 "?!" - Unexpected return from XPLMGetLanuage() function. SNAFU. Note the !
 */
int gizmo_builtin_xp_getLanguage(lua_State *L){


	switch( XPLMGetLanguage() ){
		case xplm_Language_Unknown:
			lua_pushstring( L, "??" ); break;
		case xplm_Language_English:
			lua_pushstring( L, "us" ); break;
		case xplm_Language_French:
			lua_pushstring( L, "fr" ); break;
		case xplm_Language_German:
			lua_pushstring( L, "de" ); break;
		case xplm_Language_Italian:
			lua_pushstring( L, "it" ); break;
		case xplm_Language_Spanish:
			lua_pushstring( L, "es" ); break;
		case xplm_Language_Korean:
			lua_pushstring( L, "kr" ); break;
		case xplm_Language_Russian:
			lua_pushstring( L, "ru" ); break;
		case xplm_Language_Greek:
			lua_pushstring( L, "gr" ); break;
		case xplm_Language_Japanese:
			lua_pushstring( L, "jp" ); break;
		case xplm_Language_Chinese:
			lua_pushstring( L, "cn" ); break;
		default:
			lua_pushstring( L, "?!" ); break;
	}

	return 1;

}


/**
 xp.getOS()
 Returns: "WIN", "LIN" or "MAC"
 */
int gizmo_builtin_xp_getOS(lua_State *L){
	#if APL
		lua_pushstring( L, "MAC" );
	#elif IBM
		lua_pushstring( L, "WIN" );
	#elif LIN
		lua_pushstring( L, "LIN" );
	#endif

	return 1;

}




/**
 xp.getVersion()
 Returns: X-Plane version as decimal number eg: 9.41
 */
int gizmo_builtin_xp_getVersion(lua_State *L){

	int iXPVersion=0;
	int iSDKVersion=0;
	XPLMHostApplicationID xphidFoo;
	XPLMGetVersions( &iXPVersion, &iSDKVersion, &xphidFoo );

	lua_pushnumber( L, (float)iXPVersion/1000.f );

	return 1;

}



/**
 xp.getSDKVersion()
 Returns: X-Plane Plugin SDK version as decimal number eg: 2.00
 */
int gizmo_builtin_xp_getSDKVersion(lua_State *L){

	int iXPVersion=0;
	int iSDKVersion=0;
	XPLMHostApplicationID xphidFoo;
	XPLMGetVersions( &iXPVersion, &iSDKVersion, &xphidFoo );

	lua_pushnumber( L, (float)iSDKVersion/100.f );

	return 1;

}





XPLMProbeRef	gizmo_terrain_probe_ref;
XPLMProbeInfo_t* gizmo_terrain_info_probe[1];

/**
 xp.getTerrainData( x,y,z )
 Returns: x,y,z, nx,ny,nz, vx,vy,vz, wet
 x,y,z, nx,ny,nz, vx,vy,vz, wet = xp.getTerrainData( x,y,z )
 */
int gizmo_builtin_xp_getTerrainData(lua_State *L){

	static int init_ref = 1;
	if( init_ref ){
		init_ref = 0;
		gizmo_terrain_probe_ref = XPLMCreateProbe( xplm_ProbeY );
		XPLMProbeInfo_t* tmpProbeInfo = new XPLMProbeInfo_t();
		tmpProbeInfo->structSize=sizeof( XPLMProbeInfo_t );
		gizmo_terrain_info_probe[0] = tmpProbeInfo;
	}

	//dataref id number
	lua_Number lnum_X = lua_tonumber( L, -3 );
	lua_Number lnum_Y = lua_tonumber( L, -2 );
	lua_Number lnum_Z = lua_tonumber( L, -1 );

		XPLMProbeTerrainXYZ(
			gizmo_terrain_probe_ref,
			(float)lnum_X,
			(float)lnum_Y,
			(float)lnum_Z,
			(gizmo_terrain_info_probe[0])
			);

	lua_pushnumber( L, gizmo_terrain_info_probe[0]->locationX );
	lua_pushnumber( L, gizmo_terrain_info_probe[0]->locationY );
	lua_pushnumber( L, gizmo_terrain_info_probe[0]->locationZ );

	lua_pushnumber( L, gizmo_terrain_info_probe[0]->normalX );
	lua_pushnumber( L, gizmo_terrain_info_probe[0]->normalY );
	lua_pushnumber( L, gizmo_terrain_info_probe[0]->normalZ );

	lua_pushnumber( L, gizmo_terrain_info_probe[0]->velocityX );
	lua_pushnumber( L, gizmo_terrain_info_probe[0]->velocityY );
	lua_pushnumber( L, gizmo_terrain_info_probe[0]->velocityZ );

	lua_pushnumber( L, gizmo_terrain_info_probe[0]->is_wet );

	return 10;
}



/**
 xp.getFloatV( dataref_id, start_at, number_to_fetch )
 Returns: multiple values for dataref_id as decimal numbers
 See xp.getDataref("sim/foo") and xp.newDataref("custom/dataref") for more info.
 start_at refers to the part number in sim/dataref[99]
 
 In X-Plane, OBJ8 and C, part numbers start at 0.
 In Lua, part numbers start at 1.
 
 For this reason, the start_at parameter also starts at 1.
 
 If you want to get sim/dataref[0] use:
 value = xp.getFloatv( dataref_id, 1, 1 )
 
 We ask Gizmo to get us one value, starting at the first item.
 
 This seems a little bit annoying, but when you start using your tables of values
 with the rest of Luas functions you will understand why there is a little "shift"
 here when we push numbers in and out of X-Planes dataref systems.	
 */
int gizmo_builtin_xp_getFloatV(lua_State *L){

	if( lua_gettop(L) != 3 ){
		XPLMDebugString("Gizmo: Syntax error: xp.getFloatV got four aguments. Expected three.\n");
		return 0;
	}

	//dataref id number
	lua_Integer lint_DatarefIndexID = lua_tointeger( L, -3 );

	//offset and count args
	lua_Integer lint_Offset = lua_tointeger( L, -2 );
	lua_Integer lint_Count = lua_tointeger( L, -1 );

	if( lint_Offset == 0 ){ lint_Offset = 1; }

	//printf("getFloatV Dataref ID: %i, offset: %i, count: %i\n", lint_DatarefIndexID, lint_Offset, lint_Count );
	//printf("Dataref name: %s\n", gizmo_customDataRefs[lint_DatarefIndexID]->getName() );
		std::vector<float>* tmpVector = gizmo_customDataRefs[lint_DatarefIndexID]->getFloatV( lint_Offset-1, lint_Count );

		//iterate vector and retrieve the values
		for( unsigned int x=0; x < tmpVector->size(); x++ ){
				
			lua_pushnumber( L,
					(*tmpVector)[x]
					);

		}
	
	unsigned int vSize = tmpVector->size();
	delete tmpVector;

	return vSize;
}




/**
 xp.setPaused( integer_flag )
 Pause or Unpause the simulation.
 xp.setPaused( 1 ) --pause
 xp.setPaused( 0 ) --unpause
 */
int gizmo_builtin_xp_setPaused(lua_State *L){

	XPLMSetDatai( gizmo_xpdr_Paused,
					lua_tointeger(L, -1)
						);

	return 0;

}






/**
 xp.loadReplay( "Output/replays/replay.mov" )
 Loads a replay movie.
 NOTE: This will probably hang/block the sim while the replay file loads.
 This can take a while, it is recommended you display a message to the user -before- calling this function.
 */
int gizmo_builtin_xp_loadReplay(lua_State *L){

	size_t szLen;

	const char *sInput	= lua_tolstring( L, -1, &szLen );

		XPLMDebugString("Gizmo: xp.loadReplay(");
			XPLMDebugString( sInput );
		XPLMDebugString(")\n");

		XPLMLoadDataFile(
						xplm_DataFile_ReplayMovie,
						sInput
						);

	return 0;
}




/**
 xp.loadSituation( "Output/situations/capnsully.sit" )
 Loads a pre-saved situation file.
 NOTE: This will probably hang/block the sim while the replay file loads.
 This can take a while, it is recommended you display a message to the user -before- calling this function.
 */
int gizmo_builtin_xp_loadSituation(lua_State *L){

	size_t szLen;

	const char *sInput	= lua_tolstring( L, -1, &szLen );

		XPLMDebugString("Gizmo: xp.loadSituation(");
			XPLMDebugString( sInput );
		XPLMDebugString(")\n");

		gizmo_OnSituationLoaded_event_trigger_armed = 1;
		XPLMLoadDataFile(
						xplm_DataFile_Situation,
						sInput
						);

	return 0;
}




/**
 xp.newCommand( command_name, description, script_function_name )
 Registers a new custom command and makes it available for use inside X-Plane.
 xp.newCommand("my/custom/command", "Open the canopy.", "canopy_open" )
 
 function canopy_open_OnStart()
 --button or key press started
 end
 
 function canopy_open_OnHold()
 --button or key is being held down
 end
 
 function canopy_open_OnStop()
 --button or key was released
 end
 */
int gizmo_builtin_xp_newCommand(lua_State *L){


	size_t szLen;
	//lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_CommandName		= lua_tolstring( L, -3, &szLen );
	const char *sInput_Description		= lua_tolstring( L, -2, &szLen );
	const char *sInput_ScriptHandler	= lua_tolstring( L, -1, &szLen );

	char caDbg[1024];
	sprintf(caDbg, "Gizmo: xp.newCommand( %s, %s, %s )\n", sInput_CommandName, sInput_Description, sInput_ScriptHandler);
	XPLMDebugString(caDbg);
	
		//check for custom commands we've already hooked with this name.
		for( unsigned int x=0; x < gizmo_Pool_CommandWrappers.size(); x++ ){
			if( gizmo_Pool_CommandWrappers[x]->CommandName == std::string(sInput_CommandName) ){
				//we already have a hook for this, lets update the description and handler data. X-P won't update yet but who cares.
				
				XPLMDebugString("Gizmo: xp.newCommand: Updating existing command entry..\n");
				
				gizmo_Pool_CommandWrappers[x]->Description = std::string(sInput_Description);
				gizmo_Pool_CommandWrappers[x]->ScriptHandler = std::string(sInput_ScriptHandler);
				
				return 1;
				
			}
		}
	
	
	
	//this command has never been registered, lets grab it.

	XLuaCustomCommandWrapper *tmpCommand = new XLuaCustomCommandWrapper( sInput_CommandName, sInput_Description, sInput_ScriptHandler );

		//register - fun

		XPLMRegisterCommandHandler(tmpCommand->CommandID,              // in Command name
                                 gizmo_Callback_CustomCommandHandler,       // in Handler
                                 1,                      // Receive input before plugin windows.
                                 (void *) tmpCommand);




	int iWrapperPoolID = gizmo_Pool_CommandWrappers.size();
	gizmo_Pool_CommandWrappers.push_back( tmpCommand );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}



/**
 xp.reloadScenery()
 Calls for scenery system reload. Thrown in for completeness.
 */
int gizmo_builtin_xp_reloadScenery(lua_State *L){

	XPLMReloadScenery();

	return 0;
}








/**
 xp.saveReplay( "Output/replays/filename.mov" )
 Save a replave movie to disk.
 This is NOT a quicktime movie.
 */
int gizmo_builtin_xp_saveReplay(lua_State *L){

	size_t szLen;

	const char *sInput	= lua_tolstring( L, -1, &szLen );

		XPLMDebugString("Gizmo: xp.saveReplay(");
			XPLMDebugString( sInput );
		XPLMDebugString(")\n");

		XPLMSaveDataFile(
						xplm_DataFile_ReplayMovie,
						sInput
						);

	return 0;
}




/**
 xp.saveSituation( "Output/situations/filename.sit" )
 Save a situation file to disk.
 */
int gizmo_builtin_xp_saveSituation(lua_State *L){

	size_t szLen;

	const char *sInput	= lua_tolstring( L, -1, &szLen );

		XPLMDebugString("Gizmo: xp.saveSituation(");
			XPLMDebugString( sInput );
		XPLMDebugString(")\n");

		XPLMSaveDataFile(
						xplm_DataFile_Situation,
						sInput
						);

	return 0;
}




/**
 xp.setFloat( dataref_id, value )
 Push a new value into a dataref. Value is a decimal number.
 vvi = vvi * 2 
 xp.setFloat( dref_VVI, vvi )
 */
int gizmo_builtin_xp_setFloat(lua_State *L){

	lua_Integer lnVIndexID = lua_tointeger( L, -2 );
	lua_Number lnV = lua_tonumber( L, -1 );

	//printf("gizmo_builtin_setFloat called: ID: %i Value: %0.3f\n", (int)lnVIndexID, (float)lnV);

		gizmo_customDataRefs[lnVIndexID]->setFloat( (float)lnV );

	//lua_pushnumber(L, 1);

	return 0;
}




/**
 xp.setFloatV( dataref_id, start_at, value1, value2, value3, ...., value10 )
 OR
 tmp = { value1, value2, value3, ...., value10 }
 xp.setFloatV( dataref_id, start_at, tmp )
 
 Please also read the notes for xp.getFloatV() and xp.getIntV() functions.
 
 xp.setFloatArray(..) does the same things.
 */
int gizmo_builtin_xp_setFloatV(lua_State *L)
{

	/* get number of arguments */
	int n = lua_gettop(L);
	int iStackDepth = n * -1;

	lua_Integer lnVIndexID = lua_tointeger( L, iStackDepth ); //tells us which dref handle we're going to write into eventually.
	lua_Integer lint_Offset = lua_tointeger( L, iStackDepth+1 ); //weird use of n is for stack popping ease.
	
	
	if( n >= 3 ){
	
		int t = iStackDepth+2;
		
		//check to see if argument 3 was a table of values
		if( lua_istable( L, t )   &&   t == -1 ){
						
				#if 1
				const int nUserValues = lua_objlen( L, -1 );
				float* tmpStore = new float[ nUserValues ];
				
						//extract values from Lua into our storage array.
						for( int i = 1; i <= nUserValues; i++ ){
							lua_pushinteger( L, i );
							lua_gettable( L, -2 );
							tmpStore[i-1] = lua_tonumber( L, -1 ); 
							lua_pop( L, 1 );
							
						}
				
					//push the values into x-plane.	
					XPLMSetDatavf( gizmo_customDataRefs[lnVIndexID]->dataRefHandle, tmpStore, (int)lint_Offset-1, nUserValues );
									
				delete[] tmpStore;
				#endif
		
				
								
												
		}else{ // argument 3 is NOT a table, the user is passing in data item by item.
			
			float* tmpStore = new float[n-2]; // n-2 becuase we have the dref handle and the offset argument on the stack

				int iStackTarget = 0;
				for( int i=0; i < n-2; i++ ){
					iStackTarget = 2 + i;
					tmpStore[i] = lua_tonumber( L, (n * -1)+iStackTarget );
				}

				XPLMSetDatavf( gizmo_customDataRefs[lnVIndexID]->dataRefHandle, tmpStore, (int)lint_Offset-1, n-2 );

			delete[] tmpStore;
			
		}//end check to see if arg 3 was a table or not
	
	
	
	
	}//end of check to ensure we have 3 args or more, if less do nothing

	return 0;
}








/**
 xp.setInt( dataref_id, value )
 Push a new value into a dataref. Value is a decimal number.
 vvi = vvi * 2 
 xp.setFloat( dref_VVI, vvi )
 */
int gizmo_builtin_xp_setInt(lua_State *L){

	lua_Integer lnVIndexID = lua_tointeger( L, -2 );
	lua_Integer lnV = lua_tointeger( L, -1 );

	//XPLMSetDatai( gizmo_customDataRefs[lnVIndexID], (int)lnV );
	gizmo_customDataRefs[lnVIndexID]->setInt( (int)lnV );

	return 0;
}






/**
 xp.setIntV( dataref_id, start_at, value1, value2, value3, ...., value10 )
 OR
 tmp = { value1, value2, value3, ...., value10 }
 xp.setIntV( dataref_id, start_at, tmp )
 
 Please also read the notes for xp.getFloatV() and xp.getIntV() functions.
 
 xp.setIntArray(..) does the same things.
 */
int gizmo_builtin_xp_setIntV(lua_State *L)
{

	/* get number of arguments */
	int n = lua_gettop(L);
	int iStackDepth = n * -1;

	lua_Integer lnVIndexID = lua_tointeger( L, iStackDepth ); //tells us which dref handle we're going to write into eventually.
	lua_Integer lint_Offset = lua_tointeger( L, iStackDepth+1 ); //weird use of n is for stack popping ease.
	
	
	if( n >= 3 ){
	
		int t = iStackDepth+2;
		
		//check to see if argument 3 was a table of values
		if( lua_istable( L, t )   &&   t == -1 ){
						
				#if 1
				const int nUserValues = lua_objlen( L, -1 );
				int* tmpStore = new int[ nUserValues ];
				
						//extract values from Lua into our storage array.
						for( int i = 1; i <= nUserValues; i++ ){
							lua_pushinteger( L, i );
							lua_gettable( L, -2 );
							tmpStore[i-1] = lua_tointeger( L, -1 ); 
							lua_pop( L, 1 );
							
						}
				
					//push the values into x-plane.	
					XPLMSetDatavi( gizmo_customDataRefs[lnVIndexID]->dataRefHandle, tmpStore, (int)lint_Offset-1, nUserValues );
									
				delete[] tmpStore;
				#endif
		
				
						
								
												
		}else{ // argument 3 is NOT a table, the user is passing in data item by item.
			
			int* tmpStore = new int[n-2]; // n-2 becuase we have the dref handle and the offset argument on the stack

				int iStackTarget = 0;
				for( int i=0; i < n-2; i++ ){
					iStackTarget = 2 + i;
					tmpStore[i] = lua_tointeger( L, (n * -1)+iStackTarget );
				}

				XPLMSetDatavi( gizmo_customDataRefs[lnVIndexID]->dataRefHandle, tmpStore, (int)lint_Offset-1, n-2 );

			delete[] tmpStore;
			
		}//end check to see if arg 3 was a table or not
	
	
	
	
	}//end of check to ensure we have 3 args or more, if less do nothing

	return 0;
}








