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




// -- 2011-05-18 - br@x-plugins.com
//	Using a namespace table file like this allows for aliasing of C functions across the Lua bindings.
//	Breaking the namespace table defs out into each module-header breaks this ability.



#pragma mark acf.api


//acf
const luaL_reg gizmo_ns_acf[] =
{
	{ "getAltAgl",				gizmo_builtin_acf_getAltAgl },
	{ "getAltAGL",				gizmo_builtin_acf_getAltAgl },
    { "getAltMsl",				gizmo_builtin_acf_getAltMsl },
    { "getAltMSL",				gizmo_builtin_acf_getAltMsl },
    { "getAlpha",				gizmo_builtin_acf_getAlpha },
	
	
	{ "getAuthor",	            gizmo_builtin_acf_getAuthor	},
	{ "getDescription",	        gizmo_builtin_acf_getAircraftDescription	},
	{ "getFilename",	        gizmo_builtin_acf_getAircraftFilename	},
	
	{ "getFolder",		        gizmo_builtin_acf_getAircraftFolder	},
	{ "getRelativeFolder",		gizmo_builtin_acf_getRelativeAircraftFolder	},
	
	
	{ "getAStabPitch",			gizmo_builtin_acf_getAStabPitch },
	{ "getAStabRoll",			gizmo_builtin_acf_getAStabRoll },
	{ "getAStabHeading",		gizmo_builtin_acf_getAStabHeading },
	
    { "getBeta",				gizmo_builtin_acf_getBeta },
	
	{ "getFCPitch",				gizmo_builtin_acf_getFCPitch },
	{ "getFCRoll",				gizmo_builtin_acf_getFCRoll },
	{ "getFCHeading",			gizmo_builtin_acf_getFCHeading },
	
	
    { "getGS",					gizmo_builtin_acf_getGS },
	{ "getHeading",				gizmo_builtin_acf_getHeading }, //magnetic heading by default
	{ "getHeadingTrue",			gizmo_builtin_acf_getHeadingTrue },
	{ "getIAS",					gizmo_builtin_acf_getIAS },
	{ "getKIAS",				gizmo_builtin_acf_getKIAS }, //common use
    
	{ "getLat",					gizmo_builtin_acf_getLat },
    { "getLon",					gizmo_builtin_acf_getLon },
	
	
	{ "getPosition",			gizmo_builtin_acf_getPosition },
	{ "getPositionGL",			gizmo_builtin_gfx_getAircraftPositionGL }, //ALIAS: gfx.api
	
	
	{ "getP",					gizmo_builtin_acf_getP },//roll
	{ "getP_dot",				gizmo_builtin_acf_getP_dot },
	{ "getPitch",				gizmo_builtin_acf_getPitch },
	{ "getQ",					gizmo_builtin_acf_getQ },//pitch
	{ "getQ_dot",				gizmo_builtin_acf_getQ_dot },
    { "getRoll",				gizmo_builtin_acf_getRoll },
	{ "getR",					gizmo_builtin_acf_getR },//yaw
	{ "getR_dot",				gizmo_builtin_acf_getR_dot },
	
	
	{ "getSessionTime",			gizmo_builtin_acf_getSessionTime },
	
	
	{ "getTAS",					gizmo_builtin_acf_getTAS },
	{ "getTrack",				gizmo_builtin_acf_getTAS },
	{ "getVS",					gizmo_builtin_acf_getVS },
	
	{ "getYokePitch",			gizmo_builtin_acf_getYokePitch },
	{ "getYokeRoll",			gizmo_builtin_acf_getYokeRoll },
	{ "getYokeHeading",			gizmo_builtin_acf_getYokeHeading },
	
	{ "getGearDeployState",		gizmo_builtin_acf_getGearDeployState }, //gear down state
	{ "getGearHandleState",		gizmo_builtin_acf_getGearHandleState }, //gear handle state
	
	
	{ "reset",		            gizmo_builtin_acf_reloadVehicle	},
	{ "load",					gizmo_builtin_acf_setUsersAircraft },
	
	
	//throttles
	
	{ NULL,         NULL }
};





#pragma mark airframe.api

//airframe - github issue: https://github.com/benrussell/Gizmo/issues#issue/46
const luaL_reg gizmo_ns_airframe[] =
{
	{NULL,NULL}
};






#pragma mark base64.api
const luaL_reg gizmo_ns_base64[] =
{
	{ "encode",				gizmo_builtin_base64_encode },
	{ "decode",				gizmo_builtin_base64_decode },
	{ NULL,NULL }
};



#pragma mark camera.api
const luaL_reg gizmo_ns_camera[] =
{
	
    { "getControl",				gizmo_builtin_camera_getControl },
    { "getControlGreedy",       gizmo_builtin_camera_getControlGreedy },
	{ "releaseControl",			gizmo_builtin_camera_releaseControl },
	
    { "lookAt",					gizmo_builtin_camera_lookAt },
    { "lookAtGL",				gizmo_builtin_camera_lookAtGL },
	
    { "getAttitude",			gizmo_builtin_camera_getAttitude },
    { "getFOV",					gizmo_builtin_camera_getFOV },
    { "getPosition",			gizmo_builtin_camera_getPosition },	
    { "getPositionGL",			gizmo_builtin_camera_getPositionGL },	
    { "getZoom",				gizmo_builtin_camera_getZoom },
	
    { "setAttitude",			gizmo_builtin_camera_setAttitude },
    { "setFOV",					gizmo_builtin_camera_setFOV },
    { "setPosition",			gizmo_builtin_camera_setPosition },
    { "setPositionGL",			gizmo_builtin_camera_setPositionGL },
    { "setZoom",				gizmo_builtin_camera_setZoom },
	
	{ NULL,         NULL }
};




#pragma mark fmc.api


const luaL_reg gizmo_ns_fmc[] =
{
	{ "clear",					gizmo_builtin_fmc_clear },
	{ "clearItem",				gizmo_builtin_fmc_clearItem },//id
    { "getCount",				gizmo_builtin_fmc_getCount },//ret: int
    { "getDestination",			gizmo_builtin_fmc_getDestination },//ret: int
    { "getDisplayed",			gizmo_builtin_fmc_getDisplayed },//ret: int
	{ "getGpsNavAidType",		gizmo_builtin_fmc_getGpsNavAidType },
	{ "getGpsNavAid",			gizmo_builtin_fmc_getGpsNavAid },
    { "getInfo",				gizmo_builtin_fmc_getInfo },//id: ret lots
    { "setDestination",			gizmo_builtin_fmc_setDestination },//id
    { "setDisplayed",			gizmo_builtin_fmc_setDisplayed },//id
	{ "setInfoForItem",			gizmo_builtin_fmc_setInfoForItem },//id, nav_handle, alt
	{ "setLatLonForItem",		gizmo_builtin_fmc_setLatLonForItem }, //id, lat, lon, alt
	{NULL,NULL}
};




#pragma mark gfx.api
const luaL_reg gizmo_ns_gfx[] =
{
	
	{ "drawArc",								gizmo_builtin_gfx_drawArc },
	{ "drawBox",								gizmo_builtin_gfx_drawBox },
	{ "drawFilledBox",							gizmo_builtin_gfx_drawFilledBox },
	{ "drawTranslucentDarkBox",					gizmo_builtin_gfx_drawTranslucentDarkBox },
	{ "drawCircle",								gizmo_builtin_gfx_drawCircle },
	{ "drawFilledCircle",						gizmo_builtin_gfx_drawFilledCircle },
	{ "drawLine",								gizmo_builtin_gfx_drawLine },
	{ "drawObject",								gizmo_builtin_gfx_drawObject  },
	{ "drawObjectGL",							gizmo_builtin_gfx_drawObjectGL  },
	{ "drawObjectEx",							gizmo_builtin_gfx_drawObjectEx  },
	{ "drawQuad",								gizmo_builtin_gfx_drawFilledBox },
	{ "drawRadial",								gizmo_builtin_gfx_drawRadial },
	{ "drawString",								gizmo_builtin_gfx_drawString },
	
	
	{ "disableReflectionDrawingCalls", 			gizmo_builtin_gfx_disableReflectionDrawingCalls },
	{ "enableReflectionDrawingCalls", 			gizmo_builtin_gfx_enableReflectionDrawingCalls },
	
	
	{ "getFPS",									gizmo_builtin_gfx_getFPS },
	{ "getFrameSpeed",							gizmo_builtin_gfx_getFrameSpeed },
	{ "getFRP",									gizmo_builtin_gfx_getM },
	{ "getM",									gizmo_builtin_gfx_getM },
	
	
	{ "getTexture_LiteMap",						gizmo_builtin_gfx_getTexture_LiteMap },
	{ "getTexture_Paint",						gizmo_builtin_gfx_getTexture_Paint },
	{ "getTexture_UI",							gizmo_builtin_gfx_getTexture_UI },
	
	{ "getTexture_GizmoLogo",					gizmo_builtin_gfx_getTexture_GizmoLogo },
	{ "getTexture_GizmoUI",						gizmo_builtin_gfx_getTexture_GizmoUI },
	{ "getTexture_XALogo",						gizmo_builtin_gfx_getTexture_XALogo },
	{ "getTexture_LuaLogo",						gizmo_builtin_gfx_getTexture_LuaLogo },
	
	
	{ "getAircraftPositionGL",					gizmo_builtin_gfx_getAircraftPositionGL },
	{ "getDrawDebugPath",						gizmo_builtin_gfx_getDrawDebugPath },
	{ "getIsViewInsideCockpit",					gizmo_builtin_gfx_isViewInsideCockpit },
    { "getScreenSize",							gizmo_builtin_gfx_getScreenSize },
	{ "matchAircraftOrientation",				gizmo_builtin_gfx_matchAircraftOrientation },
	
	
	{ "measureString",							gizmo_builtin_gfx_measureString },
	{ "getStringWidth",							gizmo_builtin_gfx_measureString },
	
	
	{ "setState",								gizmo_builtin_gfx_setGraphicsState },
	{ "setColor",								gizmo_builtin_gfx_setColor },
	
	
	{ "setMaskingTape",							gizmo_builtin_gfx_setMaskingTape },
	{ "clearMaskingTape",						gizmo_builtin_gfx_clearMaskingTape },

	
		//TODO: taws map options and functions are getting bloated; carve out new taws.api ?
		{ "bakeTerrainMap",							gizmo_builtin_gfx_bakeTerrainMap },
	
		{ "setTerrainMapMode_Predictive",			gizmo_builtin_gfx_setTerrainMapMode_Predictive },
		{ "setTerrainMapMode_Relative",				gizmo_builtin_gfx_setTerrainMapMode_Relative },
		{ "setTerrainMapMode_Synthetic",			gizmo_builtin_gfx_setTerrainMapMode_Synthetic },
		{ "setTerrainMapMode_Topographic",			gizmo_builtin_gfx_setTerrainMapMode_Topographic },
		
		{ "setTerrainMapOption_LookAhead",			gizmo_builtin_gfx_setTerrainMapOption_LookAhead },
		{ "setTerrainMapOption_NorthUp",			gizmo_builtin_gfx_setTerrainMapOption_NorthUp },
		{ "setTerrainMapOption_Range",				gizmo_builtin_gfx_setTerrainMapOption_Range },
		{ "setTerrainMapOption_RefreshRate",		gizmo_builtin_gfx_setTerrainMapOption_RefreshRate },
		{ "setTerrainMapOption_Size",				gizmo_builtin_gfx_setTerrainMapOption_Size },

	

	{ "texOff",										gizmo_builtin_gfx_texOff },
	{ "texOn",										gizmo_builtin_gfx_texOn },
	
	
	//{ "newObject",							gizmo_builtin_gfx_newObject }, //TODO
	{ "loadObject",								gizmo_builtin_gfx_loadObject },
	
	
	{ "newFBO",										gizmo_builtin_gfx_newFBO },
	{ "useFBO",										gizmo_builtin_gfx_useFBO },
	{ "releaseFBO",									gizmo_builtin_gfx_releaseFBO },
	{ "rebindFBO",								gizmo_builtin_gfx_rebindFBO },
	
	
	{ "clearTexture",							gizmo_builtin_gfx_clearTexture },
	{ "newTexture",								gizmo_builtin_gfx_newTexture },
	{ "useTexture",								gizmo_builtin_gfx_useTexture },
	{ "loadTga",								gizmo_builtin_gfx_loadTga },
	{ "loadTgaBlob",							gizmo_builtin_gfx_loadTgaBlob },
	
	
	{ "localToWorld",							gizmo_builtin_gfx_localToWorld },
	{ "worldToLocal",							gizmo_builtin_gfx_worldToLocal },
	
	
	{ NULL, NULL }
};





#pragma mark gizmo.api
const luaL_reg gizmo_ns_gizmo[] =
{
	
	{ "activate",			gizmo_builtin_gizmo_activate },
	{ "getActivated",			gizmo_builtin_gizmo_getActivated },
	
	
	// Event map access.....
	{ "getPossibleEventNames",	gizmo_builtin_gizmo_getPossibleEventNames },
	{ "getDisabledEventNames",	gizmo_builtin_gizmo_getDisabledEventNames },	
	{ "resetEventMap",      gizmo_builtin_gizmo_resetEventMap },
	
	{ "resetFirmware",      gizmo_builtin_gizmo_resetFirmware },
	
	{ "setDebugVerbose",	gizmo_builtin_gizmo_setDebugVerbose },
	
	{ "getVersion",			gizmo_builtin_gizmo_getVersion },
	{ "getVersionFull",		gizmo_builtin_gizmo_getVersionFull },
	
	{ "reboot",		        gizmo_builtin_gizmo_reboot },
	{ "sleep",              gizmo_builtin_gizmo_sleep }, //aliased in xp.api - it makes the main thread yeild and X-P gets laggy. (CPU gets cooler, OS gets responsive)
	
	{ NULL, NULL }
};





#pragma mark gui.api
const luaL_reg gizmo_ns_gui[] =
{
	{ "newButton",						gizmo_builtin_gui_newButton },
	{ "newCheckbox",					gizmo_builtin_gui_newCheckbox },
	{ "newCustomWidget",				gizmo_builtin_gui_newCustomWidget },
	{ "newLabel",						gizmo_builtin_gui_newLabel },
	{ "newListView",					gizmo_builtin_gui_newListView },
	{ "newPasswordBox",					gizmo_builtin_gui_newPasswordBox },
	
	{ "newScreen",						gizmo_builtin_gui_newScreen },
	{ "newSubWindow",					gizmo_builtin_gui_newSubWindow },
	
	{ "newRadioButton",					gizmo_builtin_gui_newRadioButton },
	{ "newTextBox",						gizmo_builtin_gui_newTextBox },
	
	{ "newWindow",						gizmo_builtin_gui_newWindow },
	
	//general purpose
	{ "hideWidget",						gizmo_builtin_gui_hideWidget },
	{ "showWidget",						gizmo_builtin_gui_showWidget },
	
	//root windows only
	{ "hideWindow",						gizmo_builtin_gui_hideWindow },
	{ "showWindow",						gizmo_builtin_gui_showWindow },
	{ "setWindowCaption",				gizmo_builtin_gui_setWindowCaption },
	{ "setWindowSize",					gizmo_builtin_gui_setWindowSize },
	
	{ "getCustomWidgetPosition",		gizmo_builtin_gui_getCustomWidgetPosition },	//ret: l,b
	{ "getCustomWidgetSize",			gizmo_builtin_gui_getCustomWidgetSize },		//ret: w,h
	{ "getCustomWidgetMouseXY",			gizmo_builtin_gui_getCustomWidgetMouseXY },		//ret: x,y
	
	{ "getWindowList",					gizmo_builtin_gui_getWindowList },
	
	{ "getWidgetValue",					gizmo_builtin_gui_getWidgetValue },
	{ "setWidgetValue",					gizmo_builtin_gui_setWidgetValue },
	
	{ "setPasswordMode",				gizmo_builtin_gui_setPasswordMode },
	
	{ NULL,NULL }
};





#pragma mark hash.api
const luaL_reg gizmo_ns_hash[] =
{
	
	{ "md5",		gizmo_builtin_hash_md5String }, //alias; hash.md5String
	
	{ "md5File",		gizmo_builtin_hash_md5File },
	{ "md5String",		gizmo_builtin_hash_md5String },
	
	
	{ NULL,         NULL }
};




#pragma mark http.api
const luaL_reg gizmo_ns_http[] =
{
	{ "get",            gizmo_builtin_http_get },
	{ "post",           gizmo_builtin_http_post },
	
	{ "urlEncode",      gizmo_builtin_http_urlEncode },
	{ "urlDecode",      gizmo_builtin_http_urlDecode },
	
	{ "resetProxy",		gizmo_builtin_http_resetProxy },	//clear any user set proxy options
	{ "setProxy",		gizmo_builtin_http_setProxy },		//accept a string to setup a proxy!
	//{ "setNoProxyList", gizmo_builtin_http_setNoProxyList },	//a list of hosts not to proxy to.
	
	{ NULL,NULL }
};





#pragma mark logging.api
const luaL_reg gizmo_ns_logging[] =
{
	{ "debug",					gizmo_builtin_logging_debug },
	
	{ NULL,         NULL }
};




#pragma mark matrix.api
const luaL_reg gizmo_ns_matrix[] =
{
	{ "transform3D",					gizmo_builtin_matrix_transform3D },
	
	{ NULL,								NULL }
};




#pragma mark memcache.api
const luaL_reg gizmo_ns_memcache[] =
{
	{ "get",		gizmo_builtin_memcache_get },
	{ "set",		gizmo_builtin_memcache_set },
	{ NULL,         NULL }
};





#pragma mark menu.api
const luaL_reg gizmo_ns_menu[] =
{
	{ "newItem",					gizmo_builtin_menu_newItem },
	{ NULL,         NULL }
};




#pragma mark mouse.api
const luaL_reg gizmo_ns_mouse[] =
{
    { "getXY",            gizmo_builtin_mouse_getXY }, //this isn't used, mouse.x and mouse.y are auto updated.
	{ "setCursor",        gizmo_builtin_mouse_setCursor },
	
	{ NULL,         NULL }
};






#pragma mark nav.api
const luaL_reg gizmo_ns_nav[] =
{
	{ "findNavAid",						gizmo_builtin_nav_findNavAid },
    { "getFirstNavAid",					gizmo_builtin_nav_getFirstNavAid },
	{ "getFirstNavAidOfType",			gizmo_builtin_nav_getFirstNavAidOfType },
	{ "getLastNavAidOfType",			gizmo_builtin_nav_getLastNavAidOfType },
	{ "getNavAidInfo",					gizmo_builtin_nav_getNavAidInfo },
	{ "getNextNavAid",					gizmo_builtin_nav_getNextNavAid },
	{ "getTypeName",					gizmo_builtin_nav_getTypeName },
	{ "setSearchGroups",				gizmo_builtin_nav_setSearchGroups },
	{NULL,NULL}
};







#pragma mark particles.api
const luaL_reg gizmo_ns_particles[] =
{
	{ "new",							gizmo_builtin_particles_newParticles },
	
	{ "setBirthRate",					gizmo_builtin_particles_setBirthRate },
	
	{ "setBounce",						gizmo_builtin_particles_setBounce },
	
	{ "setColor",						gizmo_builtin_particles_setColor },
	{ "setColour",						gizmo_builtin_particles_setColor },
	
	{ "setCount",						gizmo_builtin_particles_setCount },
	
	{ "setDrag",						gizmo_builtin_particles_setDrag },
	
	{ "setDisabled",     		    	gizmo_builtin_particles_setDisabled },
	{ "setEnabled",     		    	gizmo_builtin_particles_setEnabled },
	
	{ "setEnergyMax",     		    	gizmo_builtin_particles_setEnergyMax },
	{ "setEnergyMin",     		    	gizmo_builtin_particles_setEnergyMin },
	
	{ "setGravity",     		    	gizmo_builtin_particles_setGravity },
	{ "setHardDeck",     		    	gizmo_builtin_particles_setHardDeck },
	
	{ "setLife",						gizmo_builtin_particles_setLife },
	//{ "setLifeNoise",					gizmo_builtin_particles_setLifeNoise },
	
	{ "setOrigin",						gizmo_builtin_particles_setOrigin },
	{ "setOrigin_Offset",				gizmo_builtin_particles_setOrigin_Offset },
	
	{ "setScaleRate",     		    	gizmo_builtin_particles_setScaleRate },
	{ "setSize",						gizmo_builtin_particles_setSize },
	
	{ "setStuckToAcf",					gizmo_builtin_particles_setStuckToAcf },
	{ "setStreamPitch",     	    	gizmo_builtin_particles_setStreamPitch },
	{ "setStreamHeading",     	    	gizmo_builtin_particles_setStreamHeading },
	//{ "setStreamNoise",				gizmo_builtin_particles_setStreamNoise },
	
	{ "setTexture",     		    	gizmo_builtin_particles_setTexture },
	{ "setTextureSpriteCount",	    	gizmo_builtin_particles_setTextureSpriteCount },
	
	{ "setType_Cubic",     		    	gizmo_builtin_particles_setType_Cubic },
	//{ "setType_Conical",     	    	gizmo_builtin_particles_setType_Conical },
	{ "setType_Spherical",  	    	gizmo_builtin_particles_setType_Spherical },
	{ "setType_Stream",     	    	gizmo_builtin_particles_setType_Stream },
	
	{ "start",							gizmo_builtin_particles_start },
	
	{ NULL, NULL }
};





#pragma mark physics.api
const luaL_reg gizmo_ns_physics[] =
{
	
	//primitives
	{ "newSphere",					gizmo_builtin_physics_newSphere },
	{ "newBox",     		    	gizmo_builtin_physics_newBox },
	
	{ "newCylinder",     		   	gizmo_builtin_physics_newCylinder },
	{ "newCapsule",     		   	gizmo_builtin_physics_newCapsule },
	{ "newCone",					gizmo_builtin_physics_newCone },
	//{ "newMultiSphere",				gizmo_builtin_physics_newMultiSphere },
	
	
	//mesh prims - hulls etc
	
	
	//rigid body controls - force application / query
	//rigid body control will be the lions share of physics engine interaction and
	//has been granted the cleaner API function signatures.
	{ "applyForce",		                gizmo_builtin_physics_rb_applyForce },
	{ "applyTorque",		             gizmo_builtin_physics_rb_applyTorque },
	
	{ "applyImpulse",		            gizmo_builtin_physics_rb_applyImpulse },
	{ "applyTorqueImpulse",		        gizmo_builtin_physics_rb_applyTorqueImpulse },
	
	{ "applyOffsetImpulse",		        gizmo_builtin_physics_rb_applyOffsetImpulse },
	{ "applyOffsetForce",		        gizmo_builtin_physics_rb_applyOffsetForce },
	
	{ "clearForces",		                gizmo_builtin_physics_rb_clearForces },
	
	{ "getOrientationQ",		            gizmo_builtin_physics_rb_getOrientationQ }, //quaternion query
	
	{ "getLinearVelocity",		        gizmo_builtin_physics_rb_getLinearVelocity },
	{ "getAngularVelocity",		        gizmo_builtin_physics_rb_getAngularVelocity },
	
	{ "setLinearVelocity",		        gizmo_builtin_physics_rb_setLinearVelocity },
	{ "setAngularVelocity",		        gizmo_builtin_physics_rb_setAngularVelocity },
	
	{ "translate",		                gizmo_builtin_physics_rb_translate },
	
	
	//soft body control
	//soft body usage will be somewhat less common than rigid bodies.
	//the API contains "sb_" prefix to denote this, while still keeping
	//commonality with the rigid body systems.
	
	
	
	
	{ "reset",		                gizmo_builtin_physics_restart },	//alias
	{ "restart",	                gizmo_builtin_physics_restart },
	
	//{ "loadCollada",		        gizmo_builtin_physics_loadCollada },
	//{ "saveCollada",		        gizmo_builtin_physics_saveCollada },
	
	{ "setBrush",				    gizmo_builtin_physics_setBrush }, //bind an x-plane object (obj8) to a physics entity
	{ "setBrushByID",			    gizmo_builtin_physics_setBrushByID },
	{ "setCursor",				    gizmo_builtin_physics_setCursor },
	
	
	{ "setDebugMode",			    gizmo_builtin_physics_setDebugMode }, //FIXME: NAY WORKD
	
	
	{ "setDrawingHackEnabled",		gizmo_builtin_physics_setPhysicsDrawingHack },
	
	
	
	{ "setEnabled",		            gizmo_builtin_physics_setEnabled },
	
	{ "setGravityVector",		    gizmo_builtin_physics_setGravityVector },
	{ "setInertia",				    gizmo_builtin_physics_setInertia },
	{ "setMass",				    gizmo_builtin_physics_setMass },
	{ "setOrigin",				    gizmo_builtin_physics_setOrigin }, //controls the x-plane origin?
	
	{ "setPaused",		            gizmo_builtin_physics_setPaused },
	{ "stepOnce",		            gizmo_builtin_physics_stepOnce },
	
	{ NULL,						NULL }
};






#pragma mark plugins.api
const luaL_reg gizmo_ns_plugins[] =
{
	{ "findPlugin",			gizmo_builtin_plugins_findPlugin },	
	{ "sendMessage",		gizmo_builtin_plugins_sendMessage },
	
		{ "getFolder",		        gizmo_builtin_xp_getPluginsFolder	}, //TODO: Document new alias; 2011.5.25
	
	{ NULL, NULL }
};








#pragma mark shaders.api
const luaL_reg gizmo_ns_shaders[] =
{
	
	{ "newShader",	        gizmo_builtin_shaders_newShader },
	{ "compile",	        gizmo_builtin_shaders_compileShaders },
	{ "load",				gizmo_builtin_shaders_compileShaders },
	{ "use",				gizmo_builtin_shaders_useShader },
	{ "release",	        gizmo_builtin_shaders_release },
	{ "setParam",	        gizmo_builtin_shaders_setParam },
	
	{ NULL,         NULL }
};







#pragma mark sound.api
const luaL_reg gizmo_ns_sound[] =
{
	{ "newSound",				gizmo_builtin_sound_newSound },
    { "load",                   gizmo_builtin_sound_load },
	{ "pause",		            gizmo_builtin_sound_pause },
	{ "play",	                gizmo_builtin_sound_play },
	{ "rewind",		            gizmo_builtin_sound_rewind },
    { "stop",		            gizmo_builtin_sound_rewind },
	{ "say",       			    gizmo_builtin_sound_say }, //text to speech wrapper (XPLMSpeakString)
	{ "setDirection",			gizmo_builtin_sound_setDirection },
	
	{ "setGain",				gizmo_builtin_sound_setGain },
	
	{ "setLoop",		    	gizmo_builtin_sound_setLoop },
	{ "setPitch",		   	 	gizmo_builtin_sound_setPitch },
	{ "setPosition",			gizmo_builtin_sound_setPosition },
	
	{ NULL, NULL }
};








#pragma mark timer.api
const luaL_reg gizmo_ns_timer[] =
{
    { "getValue",				gizmo_builtin_timer_getValue },
    { "getInterval",			gizmo_builtin_timer_getInterval },
    { "newTimer",				gizmo_builtin_timer_newTimer },
    { "newOneShot",				gizmo_builtin_timer_newOneShot },
	{ "reset",					gizmo_builtin_timer_reset },
    { "setInterval",			gizmo_builtin_timer_setInterval },
	{ "stop",					gizmo_builtin_timer_stop },
	{NULL,NULL}
};







#pragma mark traffic.api
const luaL_reg gizmo_ns_traffic[] =
{
    //{ "getControl",				gizmo_builtin_traffic_getControl },
	{NULL,NULL}
};







#pragma mark trig.api
const luaL_reg gizmo_ns_trig[] =
{
	{ "courseToLocation",		            gizmo_builtin_trig_courseToLocation },
	{ "courseToLocation3D",	                gizmo_builtin_trig_courseToLocation3D },
	{ "distanceBetweenPoints",				gizmo_builtin_trig_distanceBetweenPoints },
    { "distanceBetweenWorldPoints",         gizmo_builtin_trig_distanceBetweenWorldPoints },
	
	{ "interpolator",						gizmo_builtin_trig_interpolator },
	
	{ "newPositionOffset",					gizmo_builtin_trig_newPositionOffset },
    { "newPositionOffset3D",				gizmo_builtin_trig_newPositionOffset3D },
	{ NULL,         NULL }
};







#pragma mark utils.api
const luaL_reg gizmo_ns_utils[] =
{
	{ "expScale",		        gizmo_builtin_utils_expScale },
	{ "getStringFromCPointer",	gizmo_builtin_utils_getStringFromCPointer },
	
	{ "popWebBrowser",			gizmo_builtin_utils_popWebBrowser },
	
	{ NULL,         NULL }
};






#pragma mark xp.api
const luaL_reg gizmo_ns_xplane[] =
{
	{ "clearReplay",					gizmo_builtin_xp_clearReplay		},
	
	{ "commandBeginByName",				gizmo_builtin_xp_commandBeginByName  },
	{ "commandEndByName",				gizmo_builtin_xp_commandEndByName  },
	{ "commandOnceByName",				gizmo_builtin_xp_commandOnceByName  },
	
	
	
	
		//aliased from acf.api
		{ "getAircraftAuthor",							gizmo_builtin_acf_getAuthor	},					//+acf.getAuthor
		{ "getAircraftDescription",						gizmo_builtin_acf_getAircraftDescription	},	//+acf.getDescription
		
		{ "getAircraftFilename",						gizmo_builtin_acf_getAircraftFilename	},		//+acf.getFilename
		{ "getAircraftFolder",							gizmo_builtin_acf_getAircraftFolder	},			//+acf.getFolder

	
		{ "reloadVehicle",								gizmo_builtin_acf_reloadVehicle	},				//+acf.reset
		{ "setUsersAircraft",							gizmo_builtin_acf_setUsersAircraft },			//+acf.....
		

	
	
	{ "getCycleNumber",			        gizmo_builtin_xp_getCycleNumber },
	
    { "getDataref",			            gizmo_builtin_xp_getDataref }, //find and cache a dataref handle for existing datarefs.
    { "getDataRef",			            gizmo_builtin_xp_getDataref }, //find and cache a dataref handle for existing datarefs.
	
    { "getElapsedTime",			        gizmo_builtin_xp_getElapsedTime },
	
	{ "getInt",				            gizmo_builtin_xp_getInt },
	{ "getIntV",			            gizmo_builtin_xp_getIntV },
	{ "getIntArray",		            gizmo_builtin_xp_getIntV },
	
	{ "getFloat",			            gizmo_builtin_xp_getFloat },
	{ "getFloatV",			            gizmo_builtin_xp_getFloatV },
	{ "getFloatArray",		            gizmo_builtin_xp_getFloatV },
	
	{ "getLanguage",					gizmo_builtin_xp_getLanguage	},
	
	{ "getPaused",		        		gizmo_builtin_xp_getPaused	},
	
	
		//TODO: additional alias in plugins.api
		{ "getPluginsFolder",		        gizmo_builtin_xp_getPluginsFolder	},
	
	
	
	{ "getVersion",						gizmo_builtin_xp_getVersion	},
	{ "getSDKVersion",					gizmo_builtin_xp_getSDKVersion	},
	{ "getTerrainData",					gizmo_builtin_xp_getTerrainData },
	
	
		{ "getFolder",					gizmo_builtin_xp_getXPlaneFolder	}, //TODO: Document new Alias: 2011.5.25
		{ "getXPlaneFolder",		    gizmo_builtin_xp_getXPlaneFolder	},
	
	
	{ "loadReplay",						gizmo_builtin_xp_loadReplay		},
	{ "loadSituation",					gizmo_builtin_xp_loadSituation	},
	
	{ "newCommand",				        gizmo_builtin_xp_newCommand  },
	
	
	
	{ "newDataref",					    gizmo_builtin_xp_newFloatingPointDataref }, //create a new fp dataref by default.
	{ "newDataRef",					    gizmo_builtin_xp_newFloatingPointDataref }, //create a new fp dataref by default.
	
	{ "newFloatingPointDataref",	    gizmo_builtin_xp_newFloatingPointDataref }, //create a new fp dataref explicitly.
	{ "newFloatingPointDataRef",	    gizmo_builtin_xp_newFloatingPointDataref }, //create a new fp dataref explicitly.
	
	{ "newFloatPtDataref",				gizmo_builtin_xp_newFloatingPointDataref }, //length matched to Integer for nice code formating
	{ "newFloatPtDataRef",				gizmo_builtin_xp_newFloatingPointDataref }, //length matched to Integer for nice code formating
	{ "newIntegerDataref",			    gizmo_builtin_xp_newIntegerDataref }, 		//create a new int dataref explicitly.
	{ "newIntegerDataRef",			    gizmo_builtin_xp_newIntegerDataref }, 		//create a new int dataref explicitly.
	
	
	
	
	{ "reloadScenery",		            gizmo_builtin_xp_reloadScenery	},
	
	{ "saveReplay",						gizmo_builtin_xp_saveReplay		},
	{ "saveSituation",					gizmo_builtin_xp_saveSituation	},
	
	{ "setFloat",			            gizmo_builtin_xp_setFloat },
	{ "setFloatV",			            gizmo_builtin_xp_setFloatV },
	{ "setFloatArray",		            gizmo_builtin_xp_setFloatV },
	
	{ "setInt",				            gizmo_builtin_xp_setInt },
	{ "setIntV",				        gizmo_builtin_xp_setIntV },
	{ "setIntArray",				    gizmo_builtin_xp_setIntV },
	
	{ "setPaused",						gizmo_builtin_xp_setPaused },
	
		{ "sleep",							gizmo_builtin_gizmo_sleep }, //calls into gizmo module, makes cpu sleep.
	
	{ NULL, NULL }
};






