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




#pragma mark dref handle

// -------------------------------------------- DATAREFS -----------------------------


XPLMDataRef gizmo_xpdr_CameraFOV;

XPLMDataRef gizmo_xpdr_AcfAuthorString;
XPLMDataRef gizmo_xpdr_AcfDescriptionString;

XPLMDataRef gizmo_xpdr_Paused;
XPLMDataRef gizmo_xpdr_FpsMultiplier;

XPLMDataRef gizmo_xpdr_ViewType;


XPLMDataRef gizmo_xpdr_ScreenWidth; // "sim/graphics/view/window_width"
XPLMDataRef gizmo_xpdr_ScreenHeight; //"sim/graphics/view/window_height"

XPLMDataRef gizmo_xpdr_PanelAreaVisible_BottomOffset; //sim/graphics/view/panel_total_pnl_b
XPLMDataRef gizmo_xpdr_PanelAreaVisible_LeftOffset; //sim/graphics/view/panel_total_pnl_l



XPLMDataRef gizmo_xpdr_Local_X;
XPLMDataRef gizmo_xpdr_Local_Y;
XPLMDataRef gizmo_xpdr_Local_Z;

XPLMDataRef gizmo_xpdr_Local_P; //pitch
XPLMDataRef gizmo_xpdr_Local_H; //heading true
XPLMDataRef gizmo_xpdr_Local_R; //roll
XPLMDataRef gizmo_xpdr_Heading;//("sim/flightmodel/position/magpsi") --magnetic heading


XPLMDataRef gizmo_xpdr_ViewX;
XPLMDataRef gizmo_xpdr_ViewY;
XPLMDataRef gizmo_xpdr_ViewZ;


XPLMDataRef gizmo_xpdr_AGL;		//("sim/flightmodel/position/y_agl") --AGL - meters
XPLMDataRef gizmo_xpdr_MSL;		//("sim/flightmodel/position/elevation") --MSL - meters
XPLMDataRef gizmo_xpdr_Alpha;	//("sim/flightmodel/position/alpha")
XPLMDataRef gizmo_xpdr_Beta;	//("sim/flightmodel/position/beta")


XPLMDataRef gizmo_xpdr_IAS;		//("sim/flightmodel/position/indicated_airspeed") --kias
XPLMDataRef gizmo_xpdr_TAS;		//("sim/flightmodel/position/true_airspeed") --mtr/sec
XPLMDataRef gizmo_xpdr_GS;		//("sim/flightmodel/position/groundspeed") --mtr/sec




XPLMDataRef gizmo_xpdr_P;//("sim/flightmodel/position/P") --roll
XPLMDataRef gizmo_xpdr_Q;//("sim/flightmodel/position/Q") --pitch
XPLMDataRef gizmo_xpdr_R;//("sim/flightmodel/position/R") --yaw

XPLMDataRef gizmo_xpdr_P_dot; //deg/second^2
XPLMDataRef gizmo_xpdr_Q_dot;
XPLMDataRef gizmo_xpdr_R_dot;

XPLMDataRef gizmo_xpdr_VS;//("sim/flightmodel/position/vh_ind") --vertical speed, meters per second

XPLMDataRef gizmo_xpdr_OnGroundAny; //("sim/flightmodel/failures/onground_any") --int bool

XPLMDataRef gizmo_xpdr_GearDeployState;//('sim/aircraft/parts/acf_gear_deploy') --float[10]
XPLMDataRef gizmo_xpdr_GearHandleState;//('sim/cockpit/switches/gear_handle_status') --int boolean


XPLMDataRef gizmo_xpdr_Lat; //("sim/flightmodel/position/latitude")
XPLMDataRef gizmo_xpdr_Lon; //("sim/flightmodel/position/longitude")
XPLMDataRef gizmo_xpdr_Track; //("sim/flightmodel/position/hpath") --actual path over ground

XPLMDataRef gizmo_xpdr_YokePitch; //("sim/joystick/yoke_pitch_ratio")
XPLMDataRef gizmo_xpdr_YokeRoll; //("sim/joystick/yoke_roll_ratio")
XPLMDataRef gizmo_xpdr_YokeHeading; //("sim/joystick/yoke_heading_ratio")





XPLMDataRef gizmo_xpdr_AStabPitch; //("sim/joystick/artstab_pitch_ratio")
XPLMDataRef gizmo_xpdr_AStabRoll; //("sim/joystick/artstab_roll_ratio")
XPLMDataRef gizmo_xpdr_AStabHeading; //("sim/joystick/artstab_heading_ratio")

XPLMDataRef gizmo_xpdr_FCPitch; //("sim/joystick/FC_ptch")
XPLMDataRef gizmo_xpdr_FCRoll; //("sim/joystick/FC_roll")
XPLMDataRef gizmo_xpdr_FCHeading; //("sim/joystick/FC_hdng")





XPLMDataRef gizmo_xpdr_Throttle;				//"sim/flightmodel/engine/ENGN_thro[8]"
XPLMDataRef gizmo_xpdr_ThrottleUse;				//"sim/flightmodel/engine/ENGN_thro_use[8]"

XPLMDataRef gizmo_xpdr_AfterBurnerOn;				//"sim/flightmodel/engine/ENGN_burning[8]"
XPLMDataRef gizmo_xpdr_MachNumber;//"sim/flightmodel/misc/machno"
XPLMDataRef gizmo_xpdr_ThrustMax; //"sim/aircraft/engine/acf_tmax"
XPLMDataRef gizmo_xpdr_ThrustAtPoint; //"sim/flightmodel/engine/POINT_thrust[8]"

XPLMDataRef gizmo_xpdr_EngineMixture;

XPLMDataRef gizmo_xpdr_ShowDebugFlightPath; //("sim/cockpit/misc/show_path") --int, 0 through 8, repeating, 4 basic modes.




/*
--- common use datarefs pulled in from a lua script

xp_thermals_dr_speedbreak_actual = xp.getDataref("sim/flightmodel/controls/sbrkrat")
xp_thermals_dr_speedbreak_requested = xp.getDataref("sim/flightmodel/controls/sbrkrqst")

icon_a5_dr_wind_hdg = xp.getDataref("sim/cockpit2/gauges/indicators/wind_heading_deg_mag")
icon_a5_dr_wind_spd = xp.getDataref("sim/cockpit2/gauges/indicators/wind_speed_kts")

icon_a5_dr_G_load_normal = xp.getDataref("sim/flightmodel/forces/g_nrml")
icon_a5_dr_G_load_axial = xp.getDataref("sim/flightmodel/forces/g_axil")
icon_a5_dr_G_load_side = xp.getDataref("sim/flightmodel/forces/g_side")






dr_ground_center = xp.getDataref("sim/flightmodel/ground/plugin_ground_center") --float 3
dr_ground_slope_normal = xp.getDataref("sim/flightmodel/ground/plugin_ground_slope_normal") --float 3
dr_ground_terrain_velocity = xp.getDataref("sim/flightmodel/ground/plugin_ground_terrain_velocity") --float 3
dr_ground_surface_texture = xp.getDataref("sim/flightmodel/ground/surface_texture_type") --no work? always 0..

dr_ground_override = xp.getDataref("sim/operation/override/override_groundplane") --boolean int



--some useful constants from the ACF file.
icon_a5_Vne = xp.getFloat( xp.getDataref("sim/aircraft/view/acf_Vne") )
icon_a5_Vno = xp.getFloat( xp.getDataref("sim/aircraft/view/acf_Vno") )
icon_a5_Vs = xp.getFloat( xp.getDataref("sim/aircraft/view/acf_Vs") )

icon_a5_Gpos_max = xp.getFloat( xp.getDataref("sim/aircraft/view/acf_Gpos") )
icon_a5_Gneg_max = xp.getFloat( xp.getDataref("sim/aircraft/view/acf_Gneg") )

*/










XPLMDataRef gizmo_xpdr_Override_Joystick;//						= XPLMFindDataRef("sim/operation/override/override_joystick");
XPLMDataRef gizmo_xpdr_Override_ArtStab;//						= XPLMFindDataRef("sim/operation/override/override_artstab");
XPLMDataRef gizmo_xpdr_Override_FlightControls;//					= XPLMFindDataRef("sim/operation/override/override_flightcontro");
XPLMDataRef gizmo_xpdr_Override_GearAndBrakes;//					= XPLMFindDataRef("sim/operation/override/override_gearbrake");
XPLMDataRef gizmo_xpdr_Override_AI_PlanePath;//					= XPLMFindDataRef("sim/operation/override/override_planepath"); //arr 20
XPLMDataRef gizmo_xpdr_Override_AI_AutoPilot;//					= XPLMFindDataRef("sim/operation/override/override_plane_ai_autopilot"); //arr 20
XPLMDataRef gizmo_xpdr_Override_Nav_Needles;//						= XPLMFindDataRef("sim/operation/override/override_navneedles");
XPLMDataRef gizmo_xpdr_Override_Nav1_Needles;//					= XPLMFindDataRef("sim/operation/override/override_nav1_needles");
XPLMDataRef gizmo_xpdr_Override_Nav2_Needles;//					= XPLMFindDataRef("sim/operation/override/override_nav2_needles");
XPLMDataRef gizmo_xpdr_Override_ADF;//;//								= XPLMFindDataRef("sim/operation/override/override_adf");
XPLMDataRef gizmo_xpdr_Override_DME;//								= XPLMFindDataRef("sim/operation/override/override_dme");
XPLMDataRef gizmo_xpdr_Override_GPS;//;//								= XPLMFindDataRef("sim/operation/override/override_gps");
XPLMDataRef gizmo_xpdr_Override_Nav_Heading;//						= XPLMFindDataRef("sim/operation/override/override_nav_heading");
XPLMDataRef gizmo_xpdr_Override_FlightDirector;//					= XPLMFindDataRef("sim/operation/override/override_flightdir");
XPLMDataRef gizmo_xpdr_Override_FlightDirector_Pitch;//			= XPLMFindDataRef("sim/operation/override/override_flightdir_ptch");
XPLMDataRef gizmo_xpdr_Override_FlightDirector_Roll;//				= XPLMFindDataRef("sim/operation/override/override_flightdir_roll");
//XPLMDataRef gizmo_xpdr_Override_Camera;//							= XPLMFindDataRef("sim/operation/override/override_camera");
XPLMDataRef gizmo_xpdr_Override_Annunciators;//					= XPLMFindDataRef("sim/operation/override/override_annunciators");
XPLMDataRef gizmo_xpdr_Override_AutoPilot;//						= XPLMFindDataRef("sim/operation/override/override_autopilot");
XPLMDataRef gizmo_xpdr_Override_PFC_AutoPilot_Lights;//			= XPLMFindDataRef("sim/operation/override/override_pfc_autopilot_lites");
XPLMDataRef gizmo_xpdr_Override_Joystick_Heading;//				= XPLMFindDataRef("sim/operation/override/override_joystick_heading");
XPLMDataRef gizmo_xpdr_Override_Joystick_Pitch;//					= XPLMFindDataRef("sim/operation/override/override_joystick_pitch");
XPLMDataRef gizmo_xpdr_Override_Joystick_Roll;//					= XPLMFindDataRef("sim/operation/override/override_joystick_roll");
XPLMDataRef gizmo_xpdr_Override_Throttles;//						= XPLMFindDataRef("sim/operation/override/override_throttles");
XPLMDataRef gizmo_xpdr_Override_PropPitch;//						= XPLMFindDataRef("sim/operation/override/override_prop_pitch");
XPLMDataRef gizmo_xpdr_Override_Mixture;//							= XPLMFindDataRef("sim/operation/override/override_mixture");
XPLMDataRef gizmo_xpdr_Override_GroundPlane;//						= XPLMFindDataRef("sim/operation/override/override_groundplane");
XPLMDataRef gizmo_xpdr_Override_Disable_CockpitObject;//			= XPLMFindDataRef("sim/operation/override/disable_cockpit_object");
XPLMDataRef gizmo_xpdr_Override_Disable_FuselageObject;//			= XPLMFindDataRef("sim/operation/override/disable_twosided_fuselage");
XPLMDataRef gizmo_xpdr_Override_FMS_Advance;//						= XPLMFindDataRef("sim/operation/override/override_fms_advance");
XPLMDataRef gizmo_xpdr_Override_FuelFlow;//						= XPLMFindDataRef("sim/operation/override/override_fuel_flow");
XPLMDataRef gizmo_xpdr_Override_ITT_EGT;//							= XPLMFindDataRef("sim/operation/override/override_itt_egt");
XPLMDataRef gizmo_xpdr_Override_Control_Surfaces;//				= XPLMFindDataRef("sim/operation/override/override_control_surfaces");














