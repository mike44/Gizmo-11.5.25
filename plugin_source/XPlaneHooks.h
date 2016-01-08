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

Copyright 2007,2008, Ben Russell - br@x-plugins.com.
Portions Copyright 2007, Jens Ansorg.

*/

#ifndef __GIZMO_XPLANE_HOOKS__
#define __GIZMO_XPLANE_HOOKS__


//#pragma GCC visibility push(hidden)



#include "GlobalDefs.h"


//handy stuff
#include "X_STL.h"



#include "StringTools.h"




#pragma mark Embedded Graphics

enum sys_tex_names{
	sys_tex_gizmo_logo,
	sys_tex_gizmo_ui,
	
	sys_tex_xa_logo,
	sys_tex_lua_logo

};

//texture handle storage.
int gizmo_sys_textures[64];

#include "Gizmo_Logo.h_data"
#include "xa_logo.h_data"
#include "lua_logo.h_data"
#include "ToolTrayIconsBuiltin.h_data"



// Include the C-header converted Lua IO Jail script.
// This script is injected into the Gizmo script engine during gizmo_StartLuaBindings()
#include "gfw_IO_Jail.h"





//GLee is fussy about loading before the other GL headers.
#include "GLee.h"

//OpenGL
#if APL
	//framework path differs
	#include <OpenGL/glu.h>
#else
	#include <GL/glu.h>
#endif


extern "C"{
#include "luagl.h"
#include "luaglu.h"
}




////this was for Linux/Ubuntu and data-exec protection?
//#ifdef DMALLOC
//#include "dmalloc.h"
//#endif








//begin lua
extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

lua_State *gizmo_luaState;
//end lua










//--CURL--
#include <curl/curl.h>
size_t gizmo_curl__write_data(void *buffer, size_t size, size_t nmemb, void *userp);







#include <pthread.h>

pthread_mutex_t gizmo_Partices_Mutex;
pthread_t gizmo_thread_handle_Particles;
int gizmo_Particles_TimeToDieFlag = 0;

pthread_mutex_t gizmo_Physics_Mutex;
pthread_t gizmo_thread_handle_Physics;
int gizmo_Physics_TimeToDieFlag = 0;

pthread_mutex_t gizmo_Curl_Mutex;
pthread_t gizmo_thread_handle_Curl;
int gizmo_Curl_TimeToDieFlag = 0;
	
	




//#include "AES.h"


#if IBM || LIN
	#include "md5.h"
#else
	#include <openssl/md5.h>
#endif



#if APL
	#include <Carbon/Carbon.h>
#endif
#if IBM
	#include <dir.h>        /* MS-DOS/WIN */


	#include <shellapi.h>

	#include <windows.h>

#endif


#include <sys/types.h>
#include <sys/stat.h>



//-- zlib --
//#include <zlib.h>













//X-Plane SDK
#include <XPLMCamera.h>
#include <XPLMDataAccess.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMMenus.h>
#include <XPLMPlanes.h>
#include <XPLMPlugin.h>
#include <XPLMProcessing.h>
#include <XPLMScenery.h>
#include <XPLMUtilities.h>

#include <XPStandardWidgets.h>
#include <XPUIGraphics.h>
#include <XPWidgets.h>
#include <XPWidgetDefs.h>
#include <XPWidgetUtils.h>




#include "gizmo_global_datarefs.h"
#include "XTrig.h"
#include "XDataRefCustom.h"





#include "BulletPhysicsHooks.h"
GizmoBulletHooks *gizmo_BulletPhysics;




#include "OpenALHooks.h"
GizmOpenALHooks *gizmo_OpenAL;






#pragma mark Global GL Texture Pool for Gizmo

int gizmo_GlobalTexturePoolSize = 1024;
int gizmo_GlobalTexturePool[1024];
int gizmo_GlobalTexturePoolCursor = 0;


#pragma event names names

const char* gizmo_EventNames[] = {
	"OnAicraftUnload",
	"OnAircraftLoad",
	"OnAirportChanged",
	
	"OnBeforeReboot",
	"OnBoot",
	
	"OnCrash",
	
	"OnDraw_Airplanes",
	"OnDraw_Airports",
	"OnDraw_BeforeGauges",
	"OnDraw_BeforeGauges_3D",
	"OnDraw_DebugFlightPath",
	"OnDraw_FirstCockpit",
	"OnDraw_FirstScene",
	"OnDraw_Gauges",
	"OnDraw_Gauges_3D",
	"OnDraw_LastCockpit",
	"OnDraw_LastScene",
	"OnDraw_LocalMap2D",
	"OnDraw_LocalMapProfile",
	"OnDraw_Terrain",
	"OnDraw_Vectors",
	"OnDraw_Windows",
	"OnDraw_World",
	
	"OnFirstFrame",
	"OnForceRedraw",
	
	"OnMouseClick",
	"OnMouseWheel",
	
	"OnPluginDisable",
	"OnPluginEnable",
	"OnPluginMessage",
	"OnPluginMessageEx",
	
	"OnSceneryLoaded",
	"OnScriptWarning",
	"OnSituationLoaded",
	
	"OnTrafficCountChanged",
	
	NULL
};



#pragma mark Disabled Events Map
//This map tracks a list of function signatures that Lua does not have.
//We check it before calling Lua and populate it after if required.
std::map<std::string,int> gizmo_DisabledEventsMap;




#pragma mark Curl Globals
#include "xlua_utils_CurlInterchange.h"
std::vector<XLuaCurlInterchange*> gizmo_curl_thread_interchange_staging; //this is used by the X-Plane thread side apis - avoids interlock during http module calls.
std::vector<XLuaCurlInterchange*> gizmo_curl_thread_interchange_input; //first stage of threaded processing, requires an interlock.
std::vector<XLuaCurlInterchange*> gizmo_curl_thread_interchange_work;
std::vector<XLuaCurlInterchange*> gizmo_curl_thread_interchange_notify;





#pragma mark Particle Globals
#include "XParticleEngine.h"
std::vector<XParticleEngine*> gizmo_ParticleEngines;
std::vector<XParticle*> gizmo_svAllParticles;
std::vector<XParticle*> gizmo_svAllParticles_DrawQ;

int gizmo_ParticlePhysicsPaused;

float gizmo_Particles_fCameraGLX, gizmo_Particles_fCameraGLY, gizmo_Particles_fCameraGLZ;
float gizmo_Particles_ElapsedSimTime;





#include "XGLShader.h"
std::vector<XGLShader*> gizmo_pool_XGLShaders;






#include "XLuaWidgetWindowWrapper.h"
#include "XLuaWidgetBaseWrapper.h"

std::vector<XLuaWidgetWindowWrapper*>	gizmo_Pool_WindowWrappers;
std::vector<XLuaWidgetBaseWrapper*>		gizmo_Pool_BaseWrappers;






#include "XLuaCustomCommandWrapper.h"
int    gizmo_Callback_CustomCommandHandler(XPLMCommandRef        inCommand,
                         XPLMCommandPhase     inPhase,
                         void *               inRefcon);


std::vector<XLuaCustomCommandWrapper*>			gizmo_Pool_CommandWrappers;




//This is our script function signature message queue, we add strings to this and then process it and call into Lua for the named function.
std::vector<std::string*>	gizmo_Pool_ScriptCallbacks;


std::vector<std::string*>	gizmo_Pool_VehicleMenuItems;



#pragma mark General Globals






#include "tgaTools.h"


#include "xlua_utils_FboWrapper.h"
std::vector <XLuaFboWrapper*> gizmo_sv_FboWrappers;


//This is the color store for gfx.setColor and friends.
float gizmo_faDrawStringPenColor[] = { 1.f, 1.f, 1.f };







//This is set from lua scripts via the http.api, allows cURL proxy overrides.
int gizmo_UseCurlProxy = 0;
std::string gizmo_CurlProxyString = "";



//Custom mouse cursor state flag.
int gizmo_UseCustomCursorDrawingCallback = 0;
std::string gizmo_CustomCursorDrawingCallbackName = ""; //users script-function name to call to draw the mouse cursor using OpenGL



//Allows raising an event after a sit file has been loaded, synthesized event, not provided by SDK - Gizmo bonus.
int gizmo_OnSituationLoaded_event_trigger_armed = 0;


//this global is used to detect if the user is in a WX window or similar.
int gizmo_XPlaneFullScreenDialogOpen_Tripwire = 0;



//this is used to keep a record of how many seconds each aircraft has been run for, the map key is an md5 hash of the acf file
//please leave it in for fair use, compiling it out wont work either for good lua-drm enabled stuff.
std::map<std::string,float> gizmo_map_DemoScoreBoard;


//memcache api storage, could be better. pointers to heap blobs. no string data splattering.
std::map<std::string,std::string> gizmo_map_MemCache;


//This controls the logo fade when the aircraft is loaded and a script was detected.
//Minor advertising to spread the Gizmo word. Please leave this feature intact.
float gizmo_logo_alpha = 0.f;


//If this is enabled then the debugger will go off in detail more often.
bool gizmo_verbose_debug = false;


//State flag that reflects whether X-Planes flight path debug drawing is on.
//Scripters can further query the dataref: ............... for more information about what data X-Plane is showing.
//We cache the dataref in a state flag to avoid multiple calls across DLL bounds. etc. (False gains??)
bool gizmo_giDebugDrawOn = false;


//Store the last known hash for the ACF file. Bigger than we need. W/ever.
unsigned char gizmo_caAcfMd5Hash[128];



//We use a slight abstraction for the nav.api module and we need
//to track a whole heap of state flags to do it.
struct t_gizmo_bitflags_for_nav_search {
	int airport;
	int ndb;
	int vor;
	int ils;
	int localizer;
	int glideslope;
	int outermarker;
	int middlemarker;
	int innermarker;
	int fix;
	int dme;
	int latlon;
}; 
t_gizmo_bitflags_for_nav_search gizmo_bitflags_for_nav_search;



//This function will release x-planes override flags so that a script can be unloaded without any scripter effort and X-Plane stays sane.
void gizmo_ReleaseOverrides();



//Dump internal lua engine data to Log.txt
static void gizmo_LuaStackDump(lua_State *L);
static void gizmo_LuaStackDump_SizeOnly(lua_State *L);





//TAWS terrain mapping code.
#include "TerrainMap.h"






int gizmo_SimPaused; // Global store for this so we dont have to cross the DLL boundary all the time with dataref calls.
float gizmo_fFpsM; //cached value of the fps dref we all use.


char gizmo_lastPlayerAircraftName[2048];		  //		c4.acf
char gizmo_lastPlayerAircraftPath[2048];		  //		/Aircraft/General Aviation/Cirrus Jet/
char gizmo_relativePathToUsersAircraftFile[2048]; //		/Aircraft/General Aviation/Cirrus Jet/c4.acf
char gizmo_relativePathToUsersAircraftFolder[2048]; //		/Aircraft/General Aviation/Cirrus Jet/c4.acf


char gizmo_pluginFolderPath[4096];	//Complete path to plugin folder:		/foo/bar/X-Plane/Resources/plugins/
char gizmo_xplaneFolderPath[4096];	//Complete path to X-Plane folder:		/foo/bar/X-Plane/
char gizmo_scriptsFolderPath[4096];	//Complete path to plugin folder:		/foo/bar/X-Plane/Aircraft/General Aviation/Cirrus Jet/scripts/



//a call from lua to gizmo.restart()/scripts.restart() will set this to 1, it is then read by the plugin in another function
//and will restart the lua subsys if the value is 1
bool gizmo_flag_LuaRestartCalled = false;
bool gizmo_flag_CallOnFirstFrame = false;



#pragma mark Custom Widget Bounds Globals

//these are updated just prior to calling in to x-plane for a widget to draw itself
//lua may access them via gfx.getCustomWidgetSize()

	float gizmo_gui_lastCustomWidgetWidth;
	float gizmo_gui_lastCustomWidgetHeight;

	float gizmo_gui_lastCustomWidgetLeft;
	float gizmo_gui_lastCustomWidgetBottom;





bool gizmo_bUsePhysics = true;
bool gizmo_bUsePhysicsDrawingHack = false; //if default true then will crash on systems where object[0] not loaded.

bool gizmo_bPhysicsPaused = false;
bool gizmo_bPhysics_StepOnce = false; //state flag trigger that trips the physics engine into the next frame, automatically turns itself off after trigger.





//rehash the aircraft and reset all override flags
void gizmo_RehashAircraftAndReleaseOverrides();

//yeild thread so that your CPU can sleep.
void gizmo_thread_sleep( int ms );

//called when the aircraft gets changed
void gizmo_AircraftChanged();

//panic function override
int gizmo_AtPanic(lua_State *L);


//wrapper function
void gizmo_md5_hash_file( char* filename, unsigned char* hashReturn );


//thread notify functions
void gizmo_Curl_TimeToDie();
void gizmo_Particles_TimeToDie();




//clipboard wrapping functions
bool	gizmo_GetTextFromClipboard(std::string& outText);
bool	gizmo_SetTextToClipboard(const std::string& inText);
int		gizmo_Callback_gui_WidgetHandler_CutCopyPaste(
							  XPWidgetMessage      inMessage,
							  XPWidgetID           inWidget,
							  long                 inParam1,
								  long                 inParam2);
int		gizmo_Callback_gui_WidgetHandler_PasswordPaste(
							  XPWidgetMessage      inMessage,
							  XPWidgetID           inWidget,
							  long                 inParam1,
								  long                 inParam2);






int gizmo_Callback_gui_WidgetHandler_ErrorDialog(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);


int gizmo_Callback_gui_WidgetHandler(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);

int gizmo_Callback_gui_CustomWidgetHandler(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);

int gizmo_Callback_gui_WindowHandler(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);

int gizmo_Callback_gui_WindowHandler_ErrorDialog(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);









int gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnDismiss(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);

int gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnBuyNow(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);

int gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnActivate(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);




					
int gizmo_Callback_gui_WindowHandler_NagwareDialog(
					XPWidgetMessage      inMessage,
					XPWidgetID           inWidget,
					long                 inParam1,
					long                 inParam2
					);







int	gizmo_Callback_Draw_3D_Objects(
                                   XPLMDrawingPhase     inPhase,
                                   int                  inIsBefore,
                                   void *               inRefcon);

int	gizmo_Callback_Draw_2D_Windows(
                                   XPLMDrawingPhase     inPhase,
                                   int                  inIsBefore,
                                   void *               inRefcon);

int	gizmo_Callback_Draw_2D_Gauges(
                                   XPLMDrawingPhase     inPhase,
                                   int                  inIsBefore,
                                   void *               inRefcon);

float gizmo_Callback_FlightLoop(
                                   float                inElapsedSinceLastCall,
                                   float                inElapsedTimeSinceLastFlightLoop,
                                   int                  inCounter,
                                   void *               inRefcon);

void gizmo_Callback_core_UserTimers(
                                   float                inElapsedSinceLastCall,
                                   float                inElapsedTimeSinceLastFlightLoop,
                                   int                  inCounter,
                                   void *               inRefcon);




/**
Searches our managed widget pool.
*/
int gizmo_FindWidgetPoolIdForXPWidget( XPWidgetID inWidget );




static int gizmo_Callback_KeySniffer_Raw(
                                   char                 inChar,    
                                   XPLMKeyFlags         inFlags,    
                                   char                 inVirtualKey,    
                                   void *               inRefcon);   

void gizmo_Callback_Menu_SystemClick(
                                   void *               inMenuRef,
                                   void *               inItemRef);

void gizmo_Callback_Menu_WindowsClick(
                                   void *               inMenuRef,
                                   void *               inItemRef);

void gizmo_Callback_Menu_VehicleClick(
                                   void *               inMenuRef,
                                   void *               inItemRef);




void gizmo_DoCore();
inline void gizmo_DoCore_JumpToScriptFolder();
inline void gizmo_DoCore_RunScriptingQueue();


void gizmo_LoadScript_Core( lua_State *L, const char* buff, unsigned int buffSize, const char* chunkName, int loadFromFile );
void gizmo_LoadVehicleScript( const char* filename );
void gizmo_LoadFirmware();
//void gizmo_LoadScript_Core_Blob(lua_State *L, const char* caFilename ); //This function is designd to load encrypted files from disk.


void gizmo_FindRelativeAircraftPath();
void gizmo_FindPluginFolder();
void gizmo_FindXPlaneFolder();


/**
Load textures from the plugin binary, init some display lists, generate any built in graphics resources we rely on.
*/
void gizmo_GraphicsInit();


/**
Compares particles for distance.
*/
bool gizmo_GreaterDistanceObjectSort(const XParticle* a, const XParticle* b);


/**
Throws the Haiku box.
*/
void gizmo_gui_ScriptError( std::string sErrorMessage );


/**
Throws the nagware box.
*/
void gizmo_gui_Nagware();



/**
Initial power on.
*/
void gizmo_StartLuaBindings();






#pragma mark custom dataref pool
//Datarefs, lots and lots of them.
std::vector<XDataRefCustom*> gizmo_customDataRefs;




#pragma mark Error Dialog Handle
XPWidgetID gizmo_gui_handle_ErrorDialog = NULL;


#pragma mark Nagware dialog handle and global-timer-store
XPWidgetID gizmo_gui_handle_NagwareDialog = NULL;
XPWidgetID gizmo_gui_handle_NagwareDialog_lblFeedback = NULL;
XPWidgetID gizmo_gui_handle_NagwareDialog_txtSerialNumber = NULL;
float gizmo_Nagware_Timer = 0.0;
int gizmo_Nagware_Disabled = 0;
int gizmo_Nagware_RebootSupressor = 0;
int gizmo_Nagware_Armed = 0;


XPWidgetID gizmo_gui_handle_NagwareDialog_btnDismiss = NULL;


#pragma mark Menu Items
XPLMMenuID gizmo_Menu_RootItem;
XPLMMenuID gizmo_Menu_SystemItems;
XPLMMenuID gizmo_Menu_VehicleItems;
XPLMMenuID gizmo_Menu_Windows;






//worker thread function 
void gizmo_YUCK_DoParticlesUpdate();//non threaded version.

//function-pointers used to spin up threads.
void *gizmo_pthreads_DoParticlesUpdate(void *data);
void *gizmo_pthreads_DoPhysicsUpdate(void *data);
void *gizmo_pthreads_DoCurlTransfer(void *data);






#pragma mark OBJ8 Drawing SubSys

//setup a blank drawing location, so we can do our own GL rotate/translate
XPLMDrawInfo_t *gizmo_DrawLocation_Zero; //init done later..

class gizmo_DrawQueueItem{
	public:
		int ObjectID; //must remain as an int, we use -1 to indicate ghosts.
		float x,y,z;
		float p,h,r;
};


/**
Add an object to the Gizmo unified drawing Queue, this allows us to add pooling/batching in later to improve
drawing throughput.
*/
void gizmo_QDrawObject( int iObjID, float fX, float fY, float fZ, float fP, float fR, float fH );
std::vector<gizmo_DrawQueueItem*> gizmo_DrawQ;
std::vector<XPLMObjectRef> 	gizmo_XPlaneSceneryObjectHandles;





#pragma mark Terrain Probes
//XPLMObjectRef gizmo_ObjectHandle;
std::vector<XPLMProbeRef> 		gizmo_XPlaneTerrainProbeHandles;





//callbacks for custom datarefs
int		gizmo_Callback_getDatai(void * inRefcon);
void	gizmo_Callback_setDatai(void * inRefcon, int inValue);

float	gizmo_Callback_getDataf(void * inRefcon);
void	gizmo_Callback_setDataf(void * inRefcon, float inValue);

double	gizmo_Callback_getDatad(void * inRefcon);
void	gizmo_Callback_setDatad(void * inRefcon, double inValue);

long	gizmo_Callback_getDatab(void * inRefcon, void* outValues, int inOffset, long inMax);
void	gizmo_Callback_setDatab(void * inRefcon, void* inValues, int inOffset, long inCount);



//HOOKED callbacks for custom datarefs
int		gizmo_Callback_Hooked_getDatai(void * inRefcon);
void	gizmo_Callback_Hooked_setDatai(void * inRefcon, int inValue);

float	gizmo_Callback_Hooked_getDataf(void * inRefcon);
void	gizmo_Callback_Hooked_setDataf(void * inRefcon, float inValue);

double	gizmo_Callback_Hooked_getDatad(void * inRefcon);
void	gizmo_Callback_Hooked_setDatad(void * inRefcon, double inValue);

long	gizmo_Callback_Hooked_getDatab(void * inRefcon, void* outValues, int inOffset, long inMax);
void	gizmo_Callback_Hooked_setDatab(void * inRefcon, void* inValues, int inOffset, long inCount);








int		gizmo_RunLuaFunction( lua_State* L, std::string* sFunctionSig );
float	gizmo_RunLuaFunction_ReturnAsFloat( lua_State* L, std::string* sFunctionSig );
double	gizmo_RunLuaFunction_ReturnAsDouble( lua_State* L, std::string* sFunctionSig );





void gizmo_CleanupXPlaneObjectHandles();





void gizmo_Menu_CreateRoot();
void gizmo_Menu_Reset();




void gizmo_NewCustomRef( XDataRefCustom *h, int hooked );
void gizmo_CustomRefsDestroy();




void gizmo_RegisterCallbacks();
void gizmo_FindDatarefs();




void gizmo_ResetEventMap();
void gizmo_Restart();




void gizmo_Start();
void gizmo_Stop();









#pragma mark Physics Globals and Functions

void gizmo_StartBulletPhysics();
void gizmo_StopBulletPhysics();



void gizmo_ThrowScriptWarning( std::string sWarningMessage );


//These functions are called from a 3D drawing callback.
void gizmo_Draw_QueuedObjects(); //draws all the scripter/artists objects from a vector queue that was built by the scripts cals
void gizmo_Draw_PhysicsObjects(); //draws the bullet physics world using OBJ8 brushes or invisibles


//used to sync the physics world coords with the x-plane world coords
float gizmo_fPhysicsWorldOffsetX=0.0;
float gizmo_fPhysicsWorldOffsetY=0.0;
float gizmo_fPhysicsWorldOffsetZ=0.0;


std::vector<int> gizmo_svPhysicsObjectBrushes;
int gizmo_Physics_DefaultBrushObject=-1; //defaulting to -1 is a valid brush for a physics object. it causes the physics object to become an invisible deflector/body.




//TODO: gitbug:47 MD5 and other crypto functions should be put into more robust general libs now because we are GPL
//Using internal libs and src was intentional as it provides an extra layer of obfu to unpick during DRM ops.
//A compromised crypto lib is pretty pointless to decrypt things with.

void xplugins_md5( unsigned char *input, unsigned int inputLen, unsigned char *hash );

#if APL
void xplugins_md5_Mac( unsigned char *input, unsigned int inputLen, unsigned char *hash );
#endif //end of mac code block

#if IBM
void xplugins_md5_Win( unsigned char *input, unsigned int inputLen, unsigned char *hash );
#endif

#if LIN
void xplugins_md5_Lin( unsigned char *input, unsigned int inputLen, unsigned char *hash );
#endif








//This block of hooks is used to setup a full-screen window that always maintains the same size as the x-plane window.
//We use this to trap movement, clicks and mouse wheel actions.

XPLMWindowID gizmo_MouseTrapWindowHandle;

int gizmo_Callback_MouseTrap_MouseWheel(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   int                  wheel,
                                   int                  clicks,
                                   void *               inRefcon);

XPLMCursorStatus gizmo_Callback_MouseTrap_CursorStatus(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   void *               inRefcon);


void	gizmo_Callback_MouseTrap_DrawWindow(
                                  XPLMWindowID         inWindowID,
                                   void *               inRefcon);

int		gizmo_Callback_MouseTrap_Window_HandleMouseClick(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   XPLMMouseStatus      inMouse,
                                   void *               inRefcon);

void	gizmo_Callback_MouseTrap_Window_HandleKeys(
                                   XPLMWindowID         inWindowID,
                                   char                 inKey,
                                   XPLMKeyFlags         inFlags,
                                   char                 inVirtualKey,
                                   void *               inRefcon,
                                   int                  losingFocus);








//these functions handle routing of X-Planes plugin messaging events into Gizmo scripts.
//see XPluginReceiveMessage() def for more.

std::map<std::string,int> gizmo_smap_IPC_BlobSizes;

void gizmo_handlePluginMessage_Inject_Text( char* caSig, char* caPath, char* caCString );
//void gizmo_handlePluginMessage_Inject_Blob( char* caSig, char* caPath, char* caInBlob, int inSize );
void gizmo_handlePluginMessage_Expert( char* caSig, char* caPath, long inMessage, void* inParam );
void gizmo_handlePluginMessage_Generic_Text( char* caSig, char* caPath, void* inParam );





#pragma GCC visibility pop






//Standard X-Plane plugin function hooks.
PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc);

PLUGIN_API void XPluginStop(void);

PLUGIN_API int XPluginEnable(void);

PLUGIN_API void XPluginDisable(void);

PLUGIN_API void XPluginReceiveMessage(
									  XPLMPluginID   inFromWho,
									  long        inMessage,
									  void *      inParam);










//#warning ensure this include is at the end of the file it's in.
#include "xlua_builtins.h"
#include "api_namespace_tables.h"


#endif
