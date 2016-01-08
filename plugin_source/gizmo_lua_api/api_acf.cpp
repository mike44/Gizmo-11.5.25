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
 acf.getAltAgl()
 Returns: Aircraft altitude above ground level in meters.
 */
int gizmo_builtin_acf_getAltAgl(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_AGL )); return 1; }; //meters


/**
 acf.getAltMsl()
 Returns: Aircraft altitude above mean-sea-level in meters.
 */
int gizmo_builtin_acf_getAltMsl(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_MSL )); return 1; }; //meters


/**
 acf.getAuthor()
 Returns: Aircraft Author from .acf file as string value.
 */
int gizmo_builtin_acf_getAuthor(lua_State *L){

    char caAircraftAuthorString[1024];
    XPLMGetDatab( gizmo_xpdr_AcfAuthorString, caAircraftAuthorString, 0, 1024 );

    lua_pushstring( L, (const char*)caAircraftAuthorString );

	return 1;

}


/**
 acf.getDescription()
 Returns: Description string from ACF file, as set in PlaneMaker.
 */
int gizmo_builtin_acf_getAircraftDescription(lua_State *L){

    char caAircraftDescriptionString[1024];
    XPLMGetDatab( gizmo_xpdr_AcfDescriptionString, caAircraftDescriptionString, 0, 1024 );

    lua_pushstring( L, (const char*)caAircraftDescriptionString );

	return 1;

}


/**
 acf.getFilename()
 Returns: Aircraft filename, no folder info.
 Eg: "c4.acf"
 */
int gizmo_builtin_acf_getAircraftFilename(lua_State *L){
    lua_pushstring( L, (const char*)gizmo_lastPlayerAircraftName );
	return 1;
}


/**
 acf.getFolder()
 Returns: Full path to aircraft folder as string.
 Eg: "/Applications/X-Plane 9 Demo/Aircraft/General Aviation/Cirrus Jet/"
 */
int gizmo_builtin_acf_getAircraftFolder(lua_State *L){
    lua_pushstring( L, (const char*)gizmo_lastPlayerAircraftPath );
	return 1;
}


/**
 acf.getRelativeFolder()
 Returns: String containing relative path to X-Plane folder, includes trailing slash. All slashes are set to /
 Eg: "Aircraft/General Aviation/Cirrus Jet/"
 */
int gizmo_builtin_acf_getRelativeAircraftFolder(lua_State *L){
    lua_pushstring( L, (const char*)gizmo_relativePathToUsersAircraftFolder );
	return 1;
}



/**
 acf.getAlpha()
 Returns: Aircraft attitude: alpha in degrees.
 */
int gizmo_builtin_acf_getAlpha(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Alpha )); return 1; }; //deg



/**
 acf.getAStabPitch()
 Returns: Aritifial stability input: pitch: range: -1 to 1
 */
int gizmo_builtin_acf_getAStabPitch(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_AStabPitch )); return 1; }; //rat

/**
 acf.getAStabRoll()
 Returns: Aritifial stability input: roll: range: -1 to 1
 */
int gizmo_builtin_acf_getAStabRoll(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_AStabRoll )); return 1; }; //rat

/**
 acf.getAStabHeading()
 Returns: Aritifial stability input: heading(rudder): range: -1 to 1
 */
int gizmo_builtin_acf_getAStabHeading(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_AStabHeading )); return 1; }; //rat



/**
 acf.getBeta()
 Returns: Aircraft attitude: beta in degrees.
 */
int gizmo_builtin_acf_getBeta(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Beta )); return 1; }; //deg


/**
 acf.getFCPitch()
 Returns: Value of flight control pitch.
 */
int gizmo_builtin_acf_getFCPitch(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_FCPitch )); return 1; }; //rat

/**
 acf.getFCRoll()
 Returns: Value of flight control pitch.
 */
int gizmo_builtin_acf_getFCRoll(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_FCRoll )); return 1; }; //rat

/**
 acf.getFCHeading()
 Returns: Value of flight control pitch.
 */
int gizmo_builtin_acf_getFCHeading(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_FCHeading )); return 1; }; //rat



/**
 acf.getGS()
 Returns: Aircraft ground speed in meters per second.
 */
int gizmo_builtin_acf_getGS(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_GS )); return 1; }; //meters/second



/**
 acf.getHeading()
 Returns: Aircraft attitude: magnetic heading in degrees.
 */
int gizmo_builtin_acf_getHeading(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Heading )); return 1; }; //deg

/**
 acf.getHeadingTrue()
 Returns: Aircraft attitude: true heading in degrees.
 */
int gizmo_builtin_acf_getHeadingTrue(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Local_H )); return 1; }; //deg



/**
 acf.getIAS()
 Returns: Indicated Air Speed in Meters/Second.
 */
int gizmo_builtin_acf_getIAS(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_IAS ) * 0.514444444f  ); return 1; }; //kias, auto converted to meters per second.

/**
 acf.getKIAS()
 Returns: Indicated Air Speed in Knots.
 */
int gizmo_builtin_acf_getKIAS(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_IAS ) ); return 1; }; //kias - common use



/**
acf.getLat()
 Returns: latitude of aircraft as decimal number.
 */
int gizmo_builtin_acf_getLat(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Lat )); return 1; }; //deg

/**
acf.getLon()
 Returns: longitude of aircraft as decimal number.
 */
int gizmo_builtin_acf_getLon(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Lon )); return 1; }; //deg



/**
 acf.getPosition()
 Returns: lat,lon,alt_msl
 
 API: >= 11.5.18
 */
int gizmo_builtin_acf_getPosition(lua_State *L){ 
	lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Lat )); 
	lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Lon )); 
	lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_MSL ));
	return 3; 
}; //deg

/**
 acf.getPositionGL()
 See: gfx.getAircraftPositionGL()
 
 API: >= 11.5.18
 */





/**
acf.getP()
 */
int gizmo_builtin_acf_getP(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_P )); return 1; };
/**
acf.getP_dot()
 */
int gizmo_builtin_acf_getP_dot(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_P_dot )); return 1; };
/**
acf.getPitch()
 Returns: pitch of aircraft as decimal number.
 */
int gizmo_builtin_acf_getPitch(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Local_P )); return 1; };

/**
acf.getQ()
 */
int gizmo_builtin_acf_getQ(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Q )); return 1; };
/**
acf.getQ_dot()
 */
int gizmo_builtin_acf_getQ_dot(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Q_dot )); return 1; };
/**
acf.getRoll()
 Returns: latitude of aircraft as decimal number.
 */
int gizmo_builtin_acf_getRoll(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Local_R )); return 1; };

/**
acf.getR()
 */
int gizmo_builtin_acf_getR(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_R )); return 1; };
/**
acf.getR_dot()
 */
int gizmo_builtin_acf_getR_dot(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_R_dot )); return 1; };



/**
acf.getSessionTime()
 Returns: Number of decimal seconds that the airframe has been flown for this sim-session.
 
 API: 11.4+
*/
int gizmo_builtin_acf_getSessionTime(lua_State *L){ 
	
	//lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_R_dot )); 
	
	std::string sMapKey = std::string((const char*)gizmo_caAcfMd5Hash);
	
	/*
	XPLMDebugString( "acf.getSessionTime Debug:");
	XPLMDebugString( sMapKey.c_str() );
	XPLMDebugString( ":\n");
	 */
	
	if( gizmo_map_DemoScoreBoard[sMapKey] ){
		//Score exists, return it.
		lua_pushnumber(L, gizmo_map_DemoScoreBoard[sMapKey] );
		
		//XPLMDebugString("\tkey exists.\n");
		
	}else{
		//value does not exist, create it so that the rest of the function will work.
		gizmo_map_DemoScoreBoard[sMapKey] = 0.1f; //init to something != 0
		lua_pushnumber(L, 0.f );
		
		//XPLMDebugString("\tkey NOT exist: return default val 0.\n");
	}
	
	
	return 1; 

};






/**
acf.getTAS()
 Returns: Aircraft true airspeed in meters per second.
 */
int gizmo_builtin_acf_getTAS(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_TAS )); return 1; }; //mtrs/sec
/**
acf.getTrack()
 Returns: Aircraft track-heading across ground in degrees.
 */
int gizmo_builtin_acf_getTrack(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_Track )); return 1; }; //mtrs/sec
/**
acf.getVS()
 Returns: Aircraft vertical speed in meters per second.
 */
int gizmo_builtin_acf_getVS(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_VS )); return 1; }; //mtrs/sec


/**
acf.getYokePitch()
 Returns: pilot pitch input as decimal number.
 */
int gizmo_builtin_acf_getYokePitch(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_YokePitch )); return 1; }; //rat

/**
acf.getYokeRoll()
 Returns: pilot roll input as decimal number.
 */
int gizmo_builtin_acf_getYokeRoll(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_YokeRoll )); return 1; }; //rat

/**
acf.getYokeHeading()
 Returns: pilot heading input as decimal number.
 */
int gizmo_builtin_acf_getYokeHeading(lua_State *L){ lua_pushnumber(L, XPLMGetDataf( gizmo_xpdr_YokeHeading )); return 1; }; //rat


/**
acf.getGearDeployState()
 Returns: Aircraft landing gear: actual state. Lua table of values as ratios.

	Eg: gear_state = { acf.getGearDeployState() }
	gear_state is now somthing like [ 1, 0.5, 0.75,  1,1,1,  1,1,1, 1 ]
 */
int gizmo_builtin_acf_getGearDeployState(lua_State *L){ 
	//10 is the number of landing gear dref slots we have available as of 2010-07-08
	float tmp[10];
	XPLMGetDatavf( gizmo_xpdr_GearDeployState, tmp, 0, 10 );
	for( int x=0; x<10; x++ ){
		lua_pushnumber(L, tmp[x]);
	}
	return 10; 
}; //rat[] //this is an array



/**
acf.getGearHandleState()
 Returns: Aircraft landing gear handle selection request value. 0 = gear up, 1 = gear down.
 */
int gizmo_builtin_acf_getGearHandleState(lua_State *L){ lua_pushboolean(L, XPLMGetDatai( gizmo_xpdr_GearHandleState )); return 1; }; //int






/**
acf.reloadVehicle()

 Attempts to reload the users currnetly selected aircraft.
 This can be performed to make certain content changes take effect.
 It also has the effect of rebinding any objects to new datarefs that
 were created between the time that the user first loaded the aircraft
 and invoked the ./scripts/init.lua file and when this function is called.
 */
int gizmo_builtin_acf_reloadVehicle(lua_State *L){
	

	//This function looks and feels like: xp.setUsersAircraft() but it's different enough to be it's own copy.

		//Please refer to the example code from Ben Supnik in function gizmo_builtin_xp_setUsersAircraft for more info on fixing this bug.

	std::string sPath = "./" + std::string(gizmo_relativePathToUsersAircraftFile);
	
	std::string sDbg = "Gizmo: xp.reloadVehicle(" + sPath + ")\n";
	XPLMDebugString( sDbg.c_str() );
	
		XPLMDebugString("Gizmo: ** This API is disabled as it crashed X-Plane as of 2010-07\n");
		//XPLMSetUsersAircraft( sPath.c_str() );
	
		//FIXME: gitbug:62 Defered execution and careful path checks to see if file actually exists. xp.reloadVehicle()

	return 0;
}







/**
 acf.setUsersAircraft( xp_relative_filename ) FIXME: gitbug:63 this function is broken
 Intention: Allow scripts to call for a change of currently loaded aircraft content.
 Filename should be of the form: "Aircraft/Heavy Metal/B747-400 United:United-Air.acf"
 (see C source code for reference example in C.)
 */
int gizmo_builtin_acf_setUsersAircraft(lua_State *L){


	//This function looks and feels like: xp.setUsersAircraft() but it's different enough to be it's own copy.


	/*

	 Reference Code for loading an aircraft on OS-X with an X-Plane relative path.

	static int iCheckAcfLoadBoom=1;

	if( iCheckAcfLoadBoom ){
		iCheckAcfLoadBoom = 0;

		char	buf[1024];
		XPLMGetSystemPath(buf);
		strcat(buf, "Aircraft:Heavy Metal:B747-400 United:United-Air.acf");
		XPLMDebugString(buf);

		XPLMSetUsersAircraft(buf);

	}
	*/


	size_t szLen;
	const char *sInput	= lua_tolstring( L, -1, &szLen );

		XPLMDebugString("Gizmo: acf.setUsersAircraft: (");
			XPLMDebugString( sInput );
		XPLMDebugString(")\n");

		XPLMDebugString("Gizmo: ** This API is disabled as it crashed X-Plane as of 2010-07\n");
		//XPLMSetUsersAircraft( sInput );
	

	return 0;
}










