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
 gizmo.activate( serial_number, [write_to_disk] )
 Disables the Gizmo nag ware dialog from ever showing up if the serial number is valid.
 */
int gizmo_builtin_gizmo_activate(lua_State *L){

	//check arg count
	if( lua_gettop(L) > 0 ){

		size_t szLen;
		char *caSerialNumber;
	
		int iWriteSerialToDisk = 1;
		
		if( lua_gettop(L) == 2 ){
			//by calling gizmo.activate("serial", 0) -- we can block gizmo from saving the serial number to disk
			iWriteSerialToDisk = 0;
			caSerialNumber = (char*)lua_tolstring( L, -2, &szLen );
		}else{
			caSerialNumber = (char*)lua_tolstring( L, -1, &szLen );	
		}
		
		
		std::string sSerialNumber = std::string( caSerialNumber );
			
		
		
		//Serial numbers should be of the form:
		//<email address>:<gizmo major version>-XXXX-XXXX
		//eg: br@x-plugins.com:11-1234-5678
		
		
		//1. Split the string on colon ":"
		
		std::vector<std::string> svTokens = StringTools::explodeString( sSerialNumber, ":" );
		if( svTokens.size() == 2 ){
			

			//1.1 -- black list check
			//It is expected that people will share their serial numbers.
			//If we find this occuring the email address will be placed in this black list permanently.
			std::vector<std::string> svEmailBlackList;			
			svEmailBlackList.push_back("pirate@the.net");
			
				
				for( unsigned int bx=0; bx < svEmailBlackList.size(); bx++ ){
					if( svEmailBlackList[bx] == svTokens[0] ){
						//black list match.
						//immediate abort, consequences be damned.
						//todo: GUI dialog for blacklisted serial?
						XPLMSpeakString("Serial number has been blacklisted.");
						XPLMDebugString( std::string( std::string("Gizmo: Serial number (") + sSerialNumber + std::string(") failed - blacklisted.\n") ).c_str() );
						return 0;
					}
				}
			
			
			//1.2 -- endorsed email check
			//content producers that support Gizmo will be granted a embeddable key for use with their products.
			std::vector<std::string> svEmailEndorsed;
			svEmailEndorsed.push_back("orders@x-aviation.com");

				//1.1 -- black list check
				for( unsigned int bx=0; bx < svEmailEndorsed.size(); bx++ ){
					if( svEmailEndorsed[bx] == svTokens[0] ){
						
						iWriteSerialToDisk = 0; //force no write to disk, product script only thanks.
						
					}
				}
			
			
			//2. Run the email address through the serial number generator code.

			std::string sSerialInput = svTokens[0] + std::string(GIZMO_VERSION_MAJOR) + std::string("-") + std::string("UVXY-ABCD");
			
			unsigned char output[256];
			memset( output, 0, 256 );
			xplugins_md5( (unsigned char*)sSerialInput.c_str(), sSerialInput.length(), output );
			
			//--
			
			char extract[16];
			sprintf( extract, "%c%c%c%c-%c%c%c%c", 
									output[2], output[4], output[6], output[8],
									output[9], output[5], output[3], output[7]
					);
			
			std::string sCompareTo = std::string(GIZMO_VERSION_MAJOR) + std::string("-") + std::string(extract);
			
							
			//3. Compare the result to the serial number provided as the right hand part of the string.
			
			if( sCompareTo == svTokens[1] ){
				
				//Serial is good.
				
				//Set global flag to no more nag.
				gizmo_Nagware_Disabled = 1;		
				
				//Destroy any widget that might exist, and its children.
				if( gizmo_gui_handle_NagwareDialog ){
					XPDestroyWidget( gizmo_gui_handle_NagwareDialog, 1 );
					gizmo_gui_handle_NagwareDialog = NULL;
				}
				
				
				
				
				if( iWriteSerialToDisk ){
					//save a copy of the serial number to disk.
					std::string sSerialNumberOutputFile = std::string(gizmo_pluginFolderPath) + "Gizmo.plugin/firmware/serial_number.lua";
					
					FILE* fh = fopen( sSerialNumberOutputFile.c_str(), "w" );
					if( fh ){
						char caTmp[1024];
						sprintf(caTmp,"gizmo.activate(\"%s\",0)\n", sSerialNumber.c_str()); //2nd arg: 0: "no write to disk"

						fwrite( caTmp, strlen(caTmp), 1, fh );
					
						fclose( fh );
					}//end attempt to write serial number to disk.
					
					//XPLMSpeakString("Thank you for your support!");
				}
				
				
				
			}else{
				
				gizmo_Nagware_Disabled = 0;
				XPLMSpeakString("Bad serial number.");
				
			}

		}else{
			//malformed serial.
			XPLMSpeakString("Malformed serial number - copy and paste again?");
		}
					
	
	
	}//check to see if we got some arguments 
	
	

	return 0;
}





/**
 gizmo.getDisabledEventNames()
 Returns: Table of strings containing all disabled event names.
 Eg:
 disabled_events = { gizmo.getDisabledEventNames() }
 */
int gizmo_builtin_gizmo_getDisabledEventNames(lua_State *L){

	XPLMDebugString("Gizmo: gizmo.getDisabledEventNames()\n");	

	unsigned int iEventCounter=0;

		typedef std::map <std::string, int> MapType;		

		// Iterate over the map and print out all key/value pairs.
		// Using a const_iterator since we are not going to change the values.
		MapType::const_iterator end = gizmo_DisabledEventsMap.end(); 
		for (MapType::const_iterator it = gizmo_DisabledEventsMap.begin(); it != end; ++it)
		{
			//std::cout << "Who(key = first): " << it->first;
			//std::cout << " Score(value = second): " << it->second << '\n';
			
			lua_pushstring( L, it->first.c_str() );
			iEventCounter++;
			
		}

	return iEventCounter;
}


/**
 gizmo.getPossibleEventNames()
 Returns: Table of strings containing all possible event names.
 Eg:
 possible_events = { gizmo.getPossibleEventNames() }
 */
int gizmo_builtin_gizmo_getPossibleEventNames(lua_State *L){

	XPLMDebugString("Gizmo: gizmo.getPossibleEventNames()\n");	
	
	int push_counter = 0;
	
		const char **i = gizmo_EventNames;
		while(*i) {
			//printf("%s ", *i);
			lua_pushstring( L, *i );
			push_counter++;
			i++;
		}
	
	return push_counter;
}


/**
 gizmo.getActivated()
 
 Returns: boolean
 
 True if Gizmo has seen a valid serial number in this session.
	
 API: >= 11.5.18
 
 */
int gizmo_builtin_gizmo_getActivated(lua_State *L){

	lua_pushboolean(L, gizmo_Nagware_Disabled);
	return 1;
	
}



/**
 gizmo.setDebugVerbose( flag )
 Increase debugging messages that get in your face with dialog boxes etc.
 Good for development, not so good for shipping products sometimes.
 */
int gizmo_builtin_gizmo_setDebugVerbose(lua_State *L){

	int tmp = (int)lua_tointeger( L, -1 );
	
	if(tmp == 0){
		XPLMDebugString("Gizmo: Verbose debugging OFF ------------------------------------\n");
		gizmo_verbose_debug = false;
	}else{
		XPLMDebugString("Gizmo: Verbose debugging ON -------------------------------------\n");
		gizmo_verbose_debug = true;
	}
	
	return 0;
}



/**
 gizmo.resetEventMap()
 Reset the table of disabled events. Causes all events to be allowed another try.
 */
int gizmo_builtin_gizmo_resetEventMap(lua_State *L){
	
	gizmo_ResetEventMap();
	
	return 0;
}



/**
 gizmo.resetFirmware()
 Reset the firmware to a copy from disk. 
 (Gizmo.plugin/scripts/firmware.lua)
 */
int gizmo_builtin_gizmo_resetFirmware(lua_State *L){
	
	XPLMDebugString("Gizmo: gizmo.resetFirmware()\n");
	
		gizmo_LoadFirmware();
	
	return 0;
}




/**
 gizmo.sleep( duration_in_milliseconds )
 Causes the main X-Plane thread to sleep for N ms.
 */
int gizmo_builtin_gizmo_sleep(lua_State *L){

	//ensure we have enough args from lua
	if( lua_gettop(L) != 1 ){ return 0; }

	int sleep_val = (int)lua_tointeger( L, -1 );


		#if IBM
		Sleep( sleep_val );
		#endif

		#if APL
		usleep( sleep_val * 1000 );
		#endif

		#if LIN
		usleep( sleep_val * 1000 );
		#endif


    return 0;
}


/**
 gizmo.getVersion()
 Returns: "Human" friendly version string.
 */
int gizmo_builtin_gizmo_getVersion(lua_State *L){

	lua_pushstring( L, GIZMO_VERSION );

    return 1;
}


/**
 gizmo.getVersionFull()
 Returns: Machine friendly version number as Integer.
 Eg: 201103141234 - 2011-03-14-1234
 */
int gizmo_builtin_gizmo_getVersionFull(lua_State *L){

	lua_pushnumber( L, GIZMO_UPDATER_VERSION );

    return 1;
}


/**
 gizmo.reboot()
 Reboot the Gizmo script engine, same effect as the menu item.
 */
int gizmo_builtin_gizmo_reboot(lua_State *L){

	//a flag is set and the operation of restarting the lua runtime defered otherwise if we do it now, mid lua function stack, we blow up.
	gizmo_flag_LuaRestartCalled = true;

	return 0;
}
