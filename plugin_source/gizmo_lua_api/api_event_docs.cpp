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

/*
 *  api_event_docs.cpp
 *  Gizmo
 *
 *  Created by Ben Russell on 19/03/11.
 *  Copyright 2011 X-Plugins.com. All rights reserved.
 *
 */



// The documentation syntax is being abused in this file to cope with the _Events stuff.





/**
_Events.main()
 
 function main()
	--this is your basic "every frame" function call, do most of your work here.
 end
*/





/**
_Events.OnAircraftCrash()
 
 function OnAircraftCrash()
 
	--The user crashed the vehicle.
 
 end
 */


/**
_Events.OnCrash()

DEPRECATED - Use OnAircraftCrash 
 
function OnCrash()

	--The user crashed the vehicle.

end
*/


/**
 _Events.OnAircraftLoad()
 
 function OnAircraftLoad()
 --This function needs to accept an index number telling us which aircraft changed, currently it does not.
 --You know -an- aircraft was loaded, you don't know which one, it's kind of irrelevant anyway due to engine issues.(??? - br)
 end
 */


/**
 _Events.OnAircraftUnload()
 
 function OnAicraftUnload()
 --This function needs to accept an index number telling us which aircraft changed, currently it does not.
 --You know an aircraft was unloaded, but not which one.
 end
 */


/**
 _Events.OnAirportChanged()
 
 function OnAirportChanged()
 
 --A new airport has been selected
 end
 */


/**
 _Events.OnBeforeReboot()
 
 function OnBeforeReboot()
 --This hook has been created to give you a chance to cleanup x-planes state.
 --This may include releasing overrides, restoring the autopilot/fmc, etc.
 end
 */


/**
 _Events.OnBoot()
 
 function OnBoot()
 
	--Gizmo has rebooted, init your code.
	--This event is also called when the user first loads your ACF file.
 
 end
 */



/**
 _Events.OnFirstFrame()
 
	 function OnFirstFrame()
		--OnBoot -> ACF Finishes Loading -> OnFirstFrame
		--set any "default acf state" datarefs here.
	 end
 
 API: >= 11.5.12
 */



/**
 _Events.OnForceRedraw()
 
	 function OnForceRedraw()
	 --The user has exited a full screen X-Plane dialog: Eg: WX/Joystick/Rendering options.
	 
	 --Does not work for: Load ACF / Change ACF Livery. Checked most others. Please report any that dont work.
	 
	 end
	 
 API: >= 11.5.12
 */




/**
_Events.OnMouseClick()
 
 function OnMouseClick()
	--mouse.click.x, mouse.click.y and mouse.click.e 
	--are automatically updated by Gizmo before this function is called.
	--You may use them immediately.
 
	sound.say("click: " .. mouse.click.x .. " / " .. mouse.click.y .. 
				" / " .. mouse.click.e )
 
	--See the mouse.api module documentation for more details.
 
 end

 */



/**
_Events.OnMouseWheel( x,y, wheel,clicks )
 
 function OnMouseWheel( x,y, wheel,clicks )
	--x,y in screen coordinates
	--wheel and clicks are int values, unkown details.
 
 end
 
 */




/**
_Events.OnSceneryLoaded()
 
 function OnSceneryLoaded()
 
	--New scenery tiles were loaded
 
 end
 */


/**
_Events.OnSituationLoaded()
 
 function OnSituationLoaded()
 
	--A .sit file completed loading.
 
 end
 */


/**
_Events.OnTrafficCountChanged()
 
 function OnTrafficCountChanged()
 
	--The number of traffic aircraft has been changed
 
 end
 */



/**
 _Events.OnPluginMessage( sig, path, message, integer_param )
 Calls for inter plugin comms.
 
 function OnPluginMessage( sig, path, message ) 
	 if( sig == "com.example.plug" )then
	 --if we return 1 then Gizmo will ensure that only this function sees this event.
	 return 1
 end
 
 --if we do not return any value Gizmo will continue looking for an event handler for this plugin message type.
 
 end
 */



/**
_Events.OnPluginMessageEx( sig, path, message, integer_param )
 Expert calls for inter plugin comms, allows for fine grained control of message handlers.
 
 function OnPluginMessageEx( sig, path, message, integer_param )
 
	if( sig == "com.example.plug" )then
		--if we return 1 then Gizmo will ensure that only this function sees this event.
		return 1
	end
 
	
	--if we do not return any value Gizmo will continue looking for an event handler for this plugin message type.
 
 end
*/


 
 
 



//------------ These are 2D orthographic projection drawing modes. ------------


/**
_2D_Events.OnDraw_FirstCockpit()
 
	function OnDraw_BeforeFirstCockpit()
	end
	function OnDraw_FirstCockpit()
	end
 */


/**
_2D_Events.OnDraw_Gauges()
 
	function OnDraw_BeforeGauges()
		--Using this function can be more suitable for mixing Avionics with X-Planes built in items.
		--Combined with the next function you should be able to achieve most tasks.
	end
	function OnDraw_Gauges()
		--You should draw your Panel Gauges / Avionics displays in this function.
	end
 */


/**
_2D_Events.OnDraw_Gauges_3D()
 
	function OnDraw_BeforeGauges_3D()
		--3d Panel Phase, gauge background, bezels etc
		--The function names have 3D in them but the drawing modes are purely 2D GL.
	end
	function OnDraw_Gauges_3D()
		--3D panel phase
	end
 */


/**
_2D_Events.OnDraw_LastCockpit()
 
	function OnDraw_BeforeLastCockpit()

	end
	function OnDraw_LastCockpit()

	end
*/


/*
_2D_Events.OnDraw_Panel() 
 --These functions are not available yet.
 --function OnDraw_BeforePanel()
 --end
 --function OnDraw_Panel()
 --end
 */


/**
_2D_Events.OnDraw_LocalMap2D()
 
	--WARNING: These functions have ambiguous names that will be changed.
	--They have been added "for completeness" and don't fit in with the Gizmo naming conventions as they are.
	function OnDraw_BeforeLocalMap2D()

	end
	function OnDraw_LocalMap2D()

	end
 */


/**
_2D_Events.OnDraw_LocalMapProfile() 
	function OnDraw_BeforeLocalMapProfile()

	end
	function OnDraw_LocalMapProfile()

	end
 */


/**
_2D_Events.OnDraw_Windows() 
 
	function OnDraw_BeforeWindows()

	--Draw your own 2d popups here
	end
	function OnDraw_Windows()

	--Draw your own 2d popups here
	end
*/









/**
_3D_Events.OnDraw_FirstScene()

	------------ These are 3D perspective projection drawing modes. ------------
	function OnDraw_BeforeFirstScene()
	
	end
	function OnDraw_FirstScene()
	
	end
 */


/**
_3D_Events.OnDraw_Terrain()
 
	function OnDraw_BeforeTerrain()
	
	end
	function OnDraw_Terrain()
	
	end
*/

/**
_3D_Events.OnDraw_Airports()
 
	function OnDraw_BeforeAirports()
	
	end
	function OnDraw_Airports()
	
	end
 */

/**
_3D_Events.OnDraw_Vectors()
 
	function OnDraw_BeforeVectors()
	
	end
	function OnDraw_Vectors()
	
	end
 */

/**
_3D_Events.OnDraw_World()
 
	function OnDraw_BeforeWorld()
	
	end
	function OnDraw_World()
	
	--Draw objects in the world next to your aircraft, your aircraft will be drawn after them so effects should integrate well
	--Called during: xplm_Phase_Objects
	end
 */

/**
_3D_Events.OnDraw_DebugFlightPath() 
 
	function OnDraw_DebugFlightPath()
	
	--Use this to draw graphics when x-planes purple flight path is turned on
	--The actual SDK phase is currently xplm_Phase_Objects
	end
 */

/**
_3D_Events.OnDraw_Airplanes()

	function OnDraw_BeforeAirplanes()
	
	end
	function OnDraw_Airplanes()
	
	end
 */

/**
_3D_Events.OnDraw_LastScene()
 
	function OnDraw_BeforeLastScene()
	
	end
	function OnDraw_LastScene()
	
	end
*/

/**
_3D_Events.OnDraw_LocalMap3D()	--WARNING: This function needs a name tweak to match the panel drawing signature convetion.

	function OnDraw_BeforeLocalMap3D()
	
	end
	function OnDraw_LocalMap3D()
	
	end

*/




