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

#include "XPlaneHooks.h"
#include "XGLShader.cpp"
#include "xlua_builtins.cpp"





void gizmo_AircraftChanged(){
	XPLMDebugString("Gizmo: C-Core:AircraftChanged()\n");


	//This hook is designed so scripts can perform measures to clean up after themselves.
	XPLMDebugString("Gizmo: Calling OnBeforeReboot()\n");
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnBeforeReboot") );


	char caDbg[4096];

	char playerAircraftName[2048];
	char playerAircraftPath[2048];

	
	memset(playerAircraftName, 0, 2048);
	memset(playerAircraftPath, 0, 2048);


	XPLMGetNthAircraftModel(0,
							playerAircraftName,
							playerAircraftPath
							);


	sprintf(caDbg, "Gizmo: Aircraft name: (%s)\n", playerAircraftName );
    XPLMDebugString(caDbg);




	//strip the aircraft name from the aircraft path
		int tx = (strlen(playerAircraftPath) - strlen(playerAircraftName));
		playerAircraftPath[tx] = 0; //hack, insert a null byte, early term the string.



		char fname[4096];
		memset(fname, 0, 4096);

		//platform specific file-path creation...
			#if APL
				for(unsigned int srx=0; srx < strlen(playerAircraftPath); srx++){
					if( playerAircraftPath[srx] == ':' ){ playerAircraftPath[srx] = '/'; }
				}
				sprintf(fname, "/Volumes/%s", playerAircraftPath);

			#elif IBM
				for(unsigned int srx=0; srx < strlen(playerAircraftPath); srx++){
					if( playerAircraftPath[srx] == '\\' ){ playerAircraftPath[srx] = '/'; }
				}
				sprintf(fname, "%s", playerAircraftPath);

			#elif LIN
				sprintf(fname, "%s", playerAircraftPath);

			#endif
		strcpy( playerAircraftPath, fname );

		sprintf(caDbg, "Gizmo: Aircraft path: (%s)\n", playerAircraftPath );
		XPLMDebugString(caDbg);



		//this code automatically resets the Lua script engine if a different .acf file is loaded.
		//there is also a menu to force this step

			//reset our globals
			memset(gizmo_lastPlayerAircraftName, 0, sizeof(gizmo_lastPlayerAircraftName));
			memset(gizmo_lastPlayerAircraftPath, 0, sizeof(gizmo_lastPlayerAircraftPath));

			//update them with new values
			strcpy( gizmo_lastPlayerAircraftName, playerAircraftName );
			strcpy( gizmo_lastPlayerAircraftPath, playerAircraftPath );

			gizmo_FindRelativeAircraftPath();



			//optimized jump path -calc once!
			memset(gizmo_scriptsFolderPath, 0, sizeof(gizmo_scriptsFolderPath));
			#if IBM
				sprintf( gizmo_scriptsFolderPath, "%sscripts\\", gizmo_lastPlayerAircraftPath );
			#else
				sprintf( gizmo_scriptsFolderPath, "%sscripts/", gizmo_lastPlayerAircraftPath );
			#endif


			gizmo_Restart();

}











void gizmo_thread_sleep( int ms ){
	//1000ms / 60 = 16.666666667
	//	int ms = 10; //32 still yeilds a few contentions @ 30fps

		    #if IBM
				Sleep( ms );
			#else
				usleep( ms * 1000 );
			#endif
}








#if IBM

/*
See the MSDN Article: for info on Wintendo threading.
http://msdn.microsoft.com/en-us/library/ms682516(VS.85).aspx
*/


/*
DWORD WINAPI MyThreadFunction( LPVOID lpParam )
{
    HANDLE hStdout;
    PMYDATA pDataArray;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    // Make sure there is a console to receive output results.

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if( hStdout == INVALID_HANDLE_VALUE )
        return 1;

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because
	// it was checked for NULL before the thread was created.

    pDataArray = (PMYDATA)lpParam;

    // Print the parameter values using thread-safe functions.

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"),
        pDataArray->val1, pDataArray->val2);
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    return 0;
}
*/
#endif




/**
Cheap little function to open up a 2nd log file.
*/
void thread_debug( std::string msg ){

	FILE* fp = fopen("/Users/br/Desktop/curl_thread_log.txt", "a");
	if( fp != NULL ){

		fwrite( msg.c_str(), msg.length(), 1, fp );

		fclose(fp);
	}

};




/**
This function and global are used to indicate to the particle worker thread that it's time to down tools and go home.
*/
void gizmo_Particles_TimeToDie(){
	pthread_mutex_lock( &gizmo_Partices_Mutex );

	gizmo_Particles_TimeToDieFlag = 1;

	pthread_mutex_unlock( &gizmo_Partices_Mutex );
}






/**
This function is used as the body of a worker thread that fetches data using curl.
*/
void *gizmo_pthreads_DoCurlTransfer(void *data)
{

	while(gizmo_Curl_TimeToDieFlag == 0){

		//thread_debug( "curl thread: waiting for lock..\n" );

		//get lock
		pthread_mutex_lock( &gizmo_Curl_Mutex ); //blocks until we get lock

		//thread_debug( "curl thread: locked: scan inputs..\n" );


			//loop work queue
			for( unsigned int x=0; x < gizmo_curl_thread_interchange_work.size(); x++ ){
				//copy finished items into notice queue
				if( gizmo_curl_thread_interchange_work[x]->expired == 0 ){
					if( gizmo_curl_thread_interchange_work[x]->ready == 1 ){

						//this item has been complete and should have buffer data for us.

						XLuaCurlInterchange* cid = new XLuaCurlInterchange();
							cid->cb_function_name = gizmo_curl_thread_interchange_work[x]->cb_function_name;
							cid->url = gizmo_curl_thread_interchange_work[x]->url;
							cid->user_agent = gizmo_curl_thread_interchange_work[x]->user_agent;

							memcpy( cid->rx_buffer, gizmo_curl_thread_interchange_work[x]->rx_buffer, gizmo_curl_thread_interchange_work[x]->rx_buffer_offset );
							cid->rx_buffer_offset = gizmo_curl_thread_interchange_work[x]->rx_buffer_offset;


							//thread_debug("cid->rx_buffer post memcpy: " + std::string(cid->rx_buffer) + "\n" );

							gizmo_curl_thread_interchange_notify.push_back( cid ); //push back into lua land.

						gizmo_curl_thread_interchange_work[x]->expired = 1;
						gizmo_curl_thread_interchange_work.erase(gizmo_curl_thread_interchange_work.begin() + x); //remove ourselves from the work queue.
						break; //do more next time this function runs.

					}
				}
			}
			//gizmo_curl_thread_interchange_work.resize(0);









			//loop over input queue
			for( unsigned int x=0; x < gizmo_curl_thread_interchange_input.size(); x++ ){
				//move items into work queue
				//thread_debug( "curl thread: input: " + gizmo_curl_thread_interchange_input[x]->url + "\n" );
				//gizmo_curl_thread_interchange_notify.push_back( gizmo_curl_thread_interchange_input[x] );

				XLuaCurlInterchange* cid = new XLuaCurlInterchange();
					cid->cb_function_name = gizmo_curl_thread_interchange_input[x]->cb_function_name;
					cid->url = gizmo_curl_thread_interchange_input[x]->url;
					cid->user_agent = gizmo_curl_thread_interchange_input[x]->user_agent;

				gizmo_curl_thread_interchange_work.push_back( cid );

			}
			gizmo_curl_thread_interchange_input.resize(0);




		//release lock
		pthread_mutex_unlock( &gizmo_Curl_Mutex );


		//thread_debug( "curl thread: unlocked, working...\n" );











			for( unsigned int x=0; x < gizmo_curl_thread_interchange_work.size(); x++ ){
				if( gizmo_curl_thread_interchange_work[x]->ready == 0 ){
					curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_URL, gizmo_curl_thread_interchange_work[x]->url.c_str() ); //this is only candidate for move into XLuaCurlInterhcange member func
					curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_WRITEDATA, (void*)(gizmo_curl_thread_interchange_work[x])  );
					curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_WRITEFUNCTION, gizmo_curl__write_data);


					//setup a post request if required.
					if( gizmo_curl_thread_interchange_work[x]->post_data != "" ){
						curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_POSTFIELDS, gizmo_curl_thread_interchange_work[x]->post_data.c_str() );
					}


					//setup a proxy if specified.
					if( gizmo_curl_thread_interchange_work[x]->useProxy ){
						curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_PROXY, gizmo_curl_thread_interchange_work[x]->proxyString.c_str());

					}


					curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_AUTOREFERER, 1);
					curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_FOLLOWLOCATION, 1);
					curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_MAXREDIRS, 10);

					curl_easy_setopt(gizmo_curl_thread_interchange_work[x]->curl_ezhandle, CURLOPT_USERAGENT, gizmo_curl_thread_interchange_work[x]->user_agent.c_str() );

					gizmo_curl_thread_interchange_work[x]->resetBuffer();

					//thread_debug("curl thread: starting curl_ez_perf(" + gizmo_curl_thread_interchange_work[x]->url + ")\n");

						int retVal = curl_easy_perform( gizmo_curl_thread_interchange_work[x]->curl_ezhandle ); //this will block
						if( retVal == 1 ){} //hide warning

					gizmo_curl_thread_interchange_work[x]->ready = 1; //we are now ready to pushing back to lua, but not expired.
				}

			}




		//thread_debug( "curl thread: work done, sleeping.\n" );

		gizmo_thread_sleep(1); //x-plane gets 100ms to service the thread mutex

	}//endless loop


	pthread_exit(NULL);
	return 0;

}










void *gizmo_pthreads_DoParticlesUpdate(void *data){

	while(gizmo_Particles_TimeToDieFlag == 0){

			pthread_mutex_lock( &gizmo_Partices_Mutex ); //blocks until lock


				//this code copies all the live particles into a draw buffer.

					//erase all old particles.
					gizmo_svAllParticles_DrawQ.clear();
					//for( unsigned int x=0; x < gizmo_svAllParticles_DrawQ.size(); x++ ){
					//	delete(gizmo_svAllParticles_DrawQ[x]);
					//}
					//gizmo_svAllParticles_DrawQ.resize(0); //reset before use.



					XParticle* xpNew;
					for( unsigned int x=0; x < gizmo_svAllParticles.size(); x++ ){
						xpNew = new XParticle();
							xpNew->Copy( gizmo_svAllParticles[x] );
						gizmo_svAllParticles_DrawQ.push_back( xpNew );
					}

			//the data has been copied out, we can unlock and continue onwards with the updates.
			//pthread_mutex_unlock( &gizmo_Partices_Mutex );





			if( gizmo_ParticlePhysicsPaused == 0 ){
				//run physics math updates
				for( unsigned int px=0; px < gizmo_ParticleEngines.size(); px++ ){
					gizmo_ParticleEngines[px]->update( 0.01f, gizmo_Particles_ElapsedSimTime );
				}
			}


			//collate and sort all known particles
				gizmo_svAllParticles.clear(); //reset before use.
				for( unsigned int px=0; px < gizmo_ParticleEngines.size(); px++ ){
					if( gizmo_ParticleEngines[px]->ready_for_drawing ){
						for( unsigned int sub_px=0; sub_px < gizmo_ParticleEngines[px]->particles.size(); sub_px++ ){
							/*
							gizmo_ParticleEngines[px]->particles[sub_px]->Core.off_x = gizmo_ParticleEngines[px]->zpX_Offset;
							gizmo_ParticleEngines[px]->particles[sub_px]->Core.off_y = gizmo_ParticleEngines[px]->zpY_Offset;
							gizmo_ParticleEngines[px]->particles[sub_px]->Core.off_z = gizmo_ParticleEngines[px]->zpZ_Offset;
							// */
							if( gizmo_ParticleEngines[px]->particles[sub_px]->Core.life_remaining > 0.f ){
								gizmo_svAllParticles.push_back( gizmo_ParticleEngines[px]->particles[sub_px] );
							}
						}
					}
				}


			//depth sort all particles
			//moving to point sprites, all these calcs are irrelevant.
				for( unsigned int x=0; x < gizmo_svAllParticles.size(); x++ ){

						//calculate distance from the camera
						//svAllParticles[x]->camera_distance = XTrig::distanceBetweenPoints(fCameraGLX, fCameraGLZ, svAllParticles[x]->x, svAllParticles[x]->z);
						gizmo_svAllParticles[x]->Core.camera_distance = XTrig::distanceBetweenPoints3D(gizmo_Particles_fCameraGLX, gizmo_Particles_fCameraGLY, gizmo_Particles_fCameraGLZ, gizmo_svAllParticles[x]->Core.x, gizmo_svAllParticles[x]->Core.y, gizmo_svAllParticles[x]->Core.z);

						//calculate heading to the camera
						float fRelativeToParticle_CameraGLX = gizmo_svAllParticles[x]->Core.x - gizmo_Particles_fCameraGLX;
						float fRelativeToParticle_CameraGLY = gizmo_svAllParticles[x]->Core.y - gizmo_Particles_fCameraGLY;
						float fRelativeToParticle_CameraGLZ = gizmo_svAllParticles[x]->Core.z - gizmo_Particles_fCameraGLZ;

						//svAllParticles[x]->heading_to_camera = XTrig::computeCourseToLocation( fRelativeToParticle_CameraGLX, fRelativeToParticle_CameraGLZ );

						float p,h;
						XTrig::computeCourseToLocation3D( fRelativeToParticle_CameraGLX, fRelativeToParticle_CameraGLY, fRelativeToParticle_CameraGLZ, &p, &h );
						gizmo_svAllParticles[x]->Core.heading_to_camera = h;
						gizmo_svAllParticles[x]->Core.pitch_to_camera = p;

				}
				std::sort( gizmo_svAllParticles.begin(), gizmo_svAllParticles.end(), gizmo_GreaterDistanceObjectSort );



		pthread_mutex_unlock( &gizmo_Partices_Mutex );

		gizmo_thread_sleep(10); //phys engine steps expect 10ms

	}//endless loop

	pthread_exit(NULL);
	return 0;
}









void gizmo_YUCK_DoParticlesUpdate(){

	//while(gizmo_Particles_TimeToDieFlag == 0){

			//pthread_mutex_lock( &gizmo_Partices_Mutex ); //blocks until lock


				//this code copies all the live particles into a draw buffer.


					//erase all old particles.
					gizmo_svAllParticles_DrawQ.clear();
					//for( unsigned int x=0; x < gizmo_svAllParticles_DrawQ.size(); x++ ){
					//	delete(gizmo_svAllParticles_DrawQ[x]);
					//}
					//gizmo_svAllParticles_DrawQ.resize(0); //reset before use.


					XParticle* xpNew;
					for( unsigned int x=0; x < gizmo_svAllParticles.size(); x++ ){
						xpNew = new XParticle();
							xpNew->Copy( gizmo_svAllParticles[x] );
						gizmo_svAllParticles_DrawQ.push_back( xpNew );
					}



			//the data has been copied out, we can unlock and continue onwards with the updates.
			//pthread_mutex_unlock( &gizmo_Partices_Mutex );




			if( gizmo_ParticlePhysicsPaused == 0 ){
				//run physics math updates
				for( unsigned int px=0; px < gizmo_ParticleEngines.size(); px++ ){

					//WARNING: This next call to update physics engines relies on a fixed rate assumed to be inside a worker thread.

					gizmo_ParticleEngines[px]->update( 0.01f, gizmo_Particles_ElapsedSimTime );


				}
			}
		//end crash block




			//collate and sort all known particles
				gizmo_svAllParticles.clear(); //reset before use.
				for( unsigned int px=0; px < gizmo_ParticleEngines.size(); px++ ){
					if( gizmo_ParticleEngines[px]->ready_for_drawing ){
						for( unsigned int sub_px=0; sub_px < gizmo_ParticleEngines[px]->particles.size(); sub_px++ ){
							/*
							gizmo_ParticleEngines[px]->particles[sub_px]->Core.off_x = gizmo_ParticleEngines[px]->zpX_Offset;
							gizmo_ParticleEngines[px]->particles[sub_px]->Core.off_y = gizmo_ParticleEngines[px]->zpY_Offset;
							gizmo_ParticleEngines[px]->particles[sub_px]->Core.off_z = gizmo_ParticleEngines[px]->zpZ_Offset;
							// */
							if( gizmo_ParticleEngines[px]->particles[sub_px]->Core.life_remaining > 0.f ){
								gizmo_svAllParticles.push_back( gizmo_ParticleEngines[px]->particles[sub_px] );
							}
						}
					}
				}


			//depth sort all particles
			//moving to point sprites, all these calcs are irrelevant.
				for( unsigned int x=0; x < gizmo_svAllParticles.size(); x++ ){

						//calculate distance from the camera
						//svAllParticles[x]->camera_distance = XTrig::distanceBetweenPoints(fCameraGLX, fCameraGLZ, svAllParticles[x]->x, svAllParticles[x]->z);
						gizmo_svAllParticles[x]->Core.camera_distance = XTrig::distanceBetweenPoints3D(gizmo_Particles_fCameraGLX, gizmo_Particles_fCameraGLY, gizmo_Particles_fCameraGLZ, gizmo_svAllParticles[x]->Core.x, gizmo_svAllParticles[x]->Core.y, gizmo_svAllParticles[x]->Core.z);

						//calculate heading to the camera
						float fRelativeToParticle_CameraGLX = gizmo_svAllParticles[x]->Core.x - gizmo_Particles_fCameraGLX;
						float fRelativeToParticle_CameraGLY = gizmo_svAllParticles[x]->Core.y - gizmo_Particles_fCameraGLY;
						float fRelativeToParticle_CameraGLZ = gizmo_svAllParticles[x]->Core.z - gizmo_Particles_fCameraGLZ;

						//svAllParticles[x]->heading_to_camera = XTrig::computeCourseToLocation( fRelativeToParticle_CameraGLX, fRelativeToParticle_CameraGLZ );

						float p,h;
						XTrig::computeCourseToLocation3D( fRelativeToParticle_CameraGLX, fRelativeToParticle_CameraGLY, fRelativeToParticle_CameraGLZ, &p, &h );
						gizmo_svAllParticles[x]->Core.heading_to_camera = h;
						gizmo_svAllParticles[x]->Core.pitch_to_camera = p;

				}
				std::sort( gizmo_svAllParticles.begin(), gizmo_svAllParticles.end(), gizmo_GreaterDistanceObjectSort );



		//pthread_mutex_unlock( &gizmo_Partices_Mutex );

		//gizmo_thread_sleep(10);

	//}//endless loop

	//pthread_exit(NULL);
	//return 0;
}















void *gizmo_pthreads_DoPhysicsUpdate(void *data){

	while(true){
		pthread_mutex_lock( &gizmo_Physics_Mutex ); //blocks until lock

				gizmo_BulletPhysics->Update( 0.03f );

		gizmo_thread_sleep(10);

	}//endless loop

	pthread_exit(NULL);
	return 0;
}
















GLuint gizmo_DisplayList_DefaultParticle;







//Mandatory X-Plugin Signatures

PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{



	//details for x-plane
		strcpy(outName, "Gizmo");
		strcpy(outSig, "http://github.com/benrussell/Gizmo");
		sprintf(outDesc, GIZMO_VERSION);

		char caVersionDebug[512];
		sprintf(caVersionDebug, "%s: v%s Starting up...\n", outName, outDesc);
		XPLMDebugString(caVersionDebug);

		XPLMDebugString("\t built: " __DATE__ " " __TIME__ "\n");
		XPLMDebugString("\n");


		//lookup some useful globals
		gizmo_FindXPlaneFolder();
        gizmo_FindPluginFolder();




	XPLMDebugString("Gizmo: Graphics Init..\n");
		gizmo_GraphicsInit();



    XPLMDebugString("Gizmo: Finding Common DataRefs..\n");
		gizmo_FindDatarefs();




		//there is also a thread started to render the TAWS map

		pthread_mutex_init( &gizmo_Partices_Mutex, NULL ); //this isnt used but needs to be defined properly.
		#if GIZMO_THREADED_PARTICLES
			XPLMDebugString("Gizmo: Init particles thread..\n");
			int rc = pthread_create(&gizmo_thread_handle_Particles, NULL, gizmo_pthreads_DoParticlesUpdate, NULL);
			if (rc){
				char caPThreadError[2048];
				sprintf(caPThreadError, "ERROR; return code from pthread_create() is %d\n", rc );
				XPLMDebugString( caPThreadError );
			}
		#else
			XPLMDebugString("Gizmo: Init particles..\n");
		#endif




		pthread_mutex_init( &gizmo_Curl_Mutex, NULL );
		if(true){
		XPLMDebugString("Gizmo: Init http thread..\n");
			int rc = pthread_create(&gizmo_thread_handle_Curl, NULL, gizmo_pthreads_DoCurlTransfer, NULL);
			if (rc){
				char caPThreadError[2048];
				sprintf(caPThreadError, "ERROR; return code from pthread_create() is %d\n", rc );
				XPLMDebugString( caPThreadError );
			}

		}




		pthread_mutex_init( &gizmo_Physics_Mutex, NULL );
		if(false){
		XPLMDebugString("Gizmo: Init physics thread..\n");
			int rc = pthread_create(&gizmo_thread_handle_Physics, NULL, gizmo_pthreads_DoPhysicsUpdate, NULL);
			if (rc){
				char caPThreadError[2048];
				sprintf(caPThreadError, "ERROR; return code from pthread_create() is %d\n", rc );
				XPLMDebugString( caPThreadError );
			}
		}









	// Finally register our key sniffer.
	XPLMRegisterKeySniffer(
					gizmo_Callback_KeySniffer_Raw, 				// Our callback.
					1, 							// Receive input before plugin windows.
					0);							// Refcon - not used.








	XPLMDebugString("Gizmo: Init Mapping..\n");
		gizmo_Mapping_Init();

	XPLMDebugString("Gizmo: Init cURL.\n");
		curl_global_init( CURL_GLOBAL_ALL );

    XPLMDebugString("Gizmo: Creating menus..\n");
		gizmo_Menu_CreateRoot();















	gizmo_Start(); // ------- code below this line may be routing events into Gizmo's APIs!




		XPLMDebugString("Gizmo: Register Callbacks..\n");
		//now that everything else is set to go we can
		//register callbacks that make everything tick.
			gizmo_RegisterCallbacks();




		XPLMDebugString("Gizmo: Building Mouse Trap..\n");
				int iScreenX, iScreenY;
				XPLMGetScreenSize( &iScreenX, &iScreenY );

				XPLMCreateWindow_t MouseTrapWindowInit;
					MouseTrapWindowInit.structSize = sizeof(XPLMCreateWindow_t);
					MouseTrapWindowInit.left = 0;
					MouseTrapWindowInit.top = iScreenY;
					MouseTrapWindowInit.right = iScreenX;
					MouseTrapWindowInit.bottom = 0;
					MouseTrapWindowInit.visible = 1;
					MouseTrapWindowInit.drawWindowFunc = gizmo_Callback_MouseTrap_DrawWindow;
					MouseTrapWindowInit.handleMouseClickFunc = gizmo_Callback_MouseTrap_Window_HandleMouseClick;
					MouseTrapWindowInit.handleKeyFunc = gizmo_Callback_MouseTrap_Window_HandleKeys;
					MouseTrapWindowInit.handleCursorFunc = gizmo_Callback_MouseTrap_CursorStatus;
					MouseTrapWindowInit.handleMouseWheelFunc = gizmo_Callback_MouseTrap_MouseWheel;
					MouseTrapWindowInit.refcon = NULL;

				gizmo_MouseTrapWindowHandle = XPLMCreateWindowEx( &MouseTrapWindowInit );

    XPLMDebugString("Gizmo: XPluginStart - OK.\n");

    return 1;
}




PLUGIN_API void XPluginStop(void){

	//dmalloc_shutdown();


	gizmo_Stop();



	gizmo_Mapping_TimeToDie();


	gizmo_Curl_TimeToDie();
	gizmo_Particles_TimeToDie();


	gizmo_thread_sleep(200); //gives our other threads a chance to die off properly.

	/*
	XPLMUnregisterFlightLoopCallback(
			gizmo_Callback_FlightLoop,	// Callback
			NULL);
	*/

    curl_global_cleanup();




	XPLMDebugString("Gizmo: XPluginStop() complete.\n");
}



PLUGIN_API int XPluginEnable(void){

	gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnPluginEnable") );
	gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnPluginEnable") );

    return 1;
}




PLUGIN_API void XPluginDisable(void){
	gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnPluginDisable") );
	gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnPluginDisable") );
}












inline void gizmo_handlePluginMessage_Inject_Text( char* caSig, char* caPath, char* caCString ){
	//caCString should be a null terminated text blob or we can expect a crash.

	//TODO: gitbug:31 Jump to scripts folder so dofile works? Bonus.

	char caChunkName[512];
	sprintf(caChunkName, "%s/text/xpl-ipc", caSig);

	gizmo_LoadScript_Core( gizmo_luaState, caCString, strlen(caCString), caChunkName, 0 );

	gizmo_ResetEventMap();

}





/*
void gizmo_handlePluginMessage_Inject_Blob( char* caSig, char* caPath, char* caInBlob, int inSize ){
	//caCString should be a null terminated text blob or we can expect a crash.

	//TODO: gitbug:31 Jump to scripts folder so dofile works? Bonus.

	char caChunkName[512];
	sprintf(caChunkName, "%s/text/xpl-ipc", caSig);

	gizmo_LoadScript_Core( gizmo_luaState, caInBlob, inSize, caChunkName, 0 );

}
*/








/**
Handles all unknown message forward into Gizmo script event "OnPluginMessageEx" - this is the expert version.
*/
void gizmo_handlePluginMessage_Expert( char* caSig, char* caPath, long inMessage, void* inParam ){

	std::string sHookFunctionName = std::string("OnPluginMessageEx"); //this is the name of the Lua event we will call
	if( ! gizmo_DisabledEventsMap[sHookFunctionName] ){

		lua_getglobal(gizmo_luaState,  sHookFunctionName.c_str() ); //prepare the function for callback.

			lua_pushstring(gizmo_luaState, caSig );
			lua_pushstring(gizmo_luaState, caPath );

			//inMessage -- defines the incoming message type - we need some msg codes - DRE uses 0x01000000 as "new dref"
			//inParam   -- defines the payload, can be a string pointer for example.

			lua_pushnumber(gizmo_luaState, inMessage );
                        lua_pushinteger(gizmo_luaState, (int)inParam );

			// do the call (4 arguments, 0 result)
			if (lua_pcall(gizmo_luaState, 4, 0, 0) != 0){  //errors handled
				//if this code runs the function did not exist inside lua

				gizmo_DisabledEventsMap[sHookFunctionName] = 1; //this function sig will not be called again
				lua_pop(gizmo_luaState,1); //pop error message off stack

			}//end check error calling func name

	}//end check dead map

}






/**
This version handles all misc events.
Lua will be called with something like: OnPluginMessage("com.x-scenery.mu2", "/plugins/mu2/foo.xpl", "some message from the moo plugin")
*/
void gizmo_handlePluginMessage_Generic_Text( char* caSig, char* caPath, void* inParam ){

	std::string sHookFunctionName = std::string("OnPluginMessage"); //this is the name of the Lua event we will call
	if( ! gizmo_DisabledEventsMap[sHookFunctionName] ){

		lua_getglobal(gizmo_luaState,  sHookFunctionName.c_str() ); //prepare the function for callback.

			lua_pushstring(gizmo_luaState, caSig );
			lua_pushstring(gizmo_luaState, caPath );

			lua_pushstring(gizmo_luaState, (char*)inParam );

			// do the call (3 arguments, 0 result)
			if (lua_pcall(gizmo_luaState, 3, 0, 0) != 0){  //errors handled
				//if this code runs the function did not exist inside lua

				gizmo_DisabledEventsMap[sHookFunctionName] = 1; //this function sig will not be called again
				lua_pop(gizmo_luaState,1); //pop error message off stack

			}//end check error calling func name

	}//end check dead map

}















PLUGIN_API void XPluginReceiveMessage(
    XPLMPluginID   inFromWho,
    long        inMessage,
    void *      inParam){



	char caDbg[1024];



		//we only listen to major messages that come from x-plane
		if( inFromWho == XPLM_PLUGIN_XPLANE ){

			switch( inMessage ){
				case XPLM_MSG_PLANE_CRASHED:
					//This message is sent to your plugin whenever the user's plane crashes.
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnCrash") ); //2011-05-25: marked as deprecated in _Events.OnCrash doc-string. br@x-plugins.com
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnCrash") );
					
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnAircraftCrash") );
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnAircraftCrash") );
					break;

				case XPLM_MSG_PLANE_LOADED:
					//This message is sent to your plugin whenever a new plane is loaded. The parameter is the number of the plane being loaded; 0 indicates the user's plane.
					if( inParam == 0 ){ //player 1
						gizmo_AircraftChanged();
					}
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnAircraftLoad") );
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnAircraftLoad") );
					break;

				case XPLM_MSG_AIRPORT_LOADED:
					//User has been placed at a new airport.
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnAirportChanged") );
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnAirportChanged") );
					break;

				case XPLM_MSG_SCENERY_LOADED:
					//This message is sent whenever new scenery is loaded. Use datarefs to determine the new scenery files that were loaded.
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnSceneryLoaded") );
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnSceneryLoaded") );
					break;

				case XPLM_MSG_AIRPLANE_COUNT_CHANGED:
					//This message is sent whenever the user adjusts the number of X-Plane aircraft models. You must use XPLMCountPlanes to find out how many planes are now available. This message will only be sent in XP7 and higher because in XP6 the number of aircraft is not user-adjustable.
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnTrafficCountChanged") );
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnTrafficCountChanged") );
					break;

				case XPLM_MSG_PLANE_UNLOADED:
					//This message is sent to your plugin whenever a plane is unloaded. The parameter is the number of the plane being unloaded; 0 indicates the user's plane.
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnAircraftUnload") ); //this exists
					gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnAircraftUnload") ); //this exists
					break;


				default:
					//Unkown message.
					break;
			}//end switch(message type)



		}else{
			//this code can handle events coming in from other plugins. we should pass this on to lua somehow.

			// Calculate a hash of the plugin
			// Translate the message into some packet types
			// Based on the packet type, cast the data and then push it into Lua as a string/number/int/blob/whatever.

			char caName[512];
			char caPath[512];
			char  caSig[512];
			char caDesc[512];

			//Get some more friendly details about who the plugin is.
			XPLMGetPluginInfo(
							inFromWho,
								caName,
								caPath,
								caSig,
								caDesc
							);

#if 0
			{
				char caTmp[2048];
				sprintf( caTmp, "Gizmo_Debug: Plugin IPC: name:%s path:%s sig:%s description:%s\n", caName, caPath, caSig, caDesc);
				XPLMDebugString( caTmp );
			}
#endif

				switch( inMessage ){

					// -- plain text script injection --
					case 0x01000000:
						sprintf(caDbg, "Gizmo: Plugin Message: %s is sending us a text chunk of Lua script.\n", caSig);
						XPLMDebugString( caDbg );
						gizmo_handlePluginMessage_Inject_Text( caSig, caPath, (char*)inParam );
						break;

						/*
					// -- blob injection --
					case 0xF2000000:
						sprintf(caDbg, "Gizmo: Plugin Message: %s is sending us a BLOB chunk size..\n", caSig);
						XPLMDebugString( caDbg );
							gizmo_smap_IPC_BlobSizes[ std::string(caSig) ] = (int)inParam; //store blob size for later.
						break;

					case 0x02000000:
						sprintf(caDbg, "Gizmo: Plugin Message: %s is sending us a BLOB chunk of Lua byte-code..\n", caSig);
						XPLMDebugString( caDbg );

							if( gizmo_smap_IPC_BlobSizes[ std::string(caSig) ] ){
								gizmo_handlePluginMessage_Inject_Blob(
																	caSig,  //whos sending us the blob
																	caPath,  //whats the file path the plugin sending it
																	(char*)inParam,  //the actual blob data
																	gizmo_smap_IPC_BlobSizes[ std::string(caSig) ]  //the size of the blob previously rx'd in another message.
																	);

								gizmo_smap_IPC_BlobSizes[ std::string(caSig) ] = NULL;

							}else{
								sprintf(caDbg, "Gizmo: Plugin Message: %s BLOB chunk failed: No size data.\n", caSig);
								XPLMDebugString( caDbg );
							}


						break;
					// -- end blob injection --
						*/




					//IPC Text Messaging - send a text packet straight into gizmo scripting.
					case 0x03000000:
						sprintf(caDbg, "Gizmo: Plugin Message: %s is sending us a text message.\n", caSig);
						XPLMDebugString( caDbg );
						gizmo_handlePluginMessage_Generic_Text( caSig, caPath, (char*)inParam );
						//OnPluginMessage( sig, path, msg )
						break;



					//Reboot the Gizmo engine.
					case 0xF5000000:
						//sprintf(caDbg, "Gizmo: Plugin Message: %s is sending us a text message.\n", caSig);
						//XPLMDebugString( caDbg );
						//gizmo_handlePluginMessage_Generic_Text( caSig, caPath, (char*)inParam );
						//OnPluginMessage( sig, path, msg )

						gizmo_Restart();

						break;




					default:
						//XPLMDebugString("Gizmo recieved an unkown plugin message-id.");
						gizmo_handlePluginMessage_Expert( caSig, caPath, inMessage, inParam );
						//OnPluginMessage( sig, path, msg_id, integer_param )
						break;


				} //end message-type case-selector



		} //end check to see if the message was from X-Plane or some other plugin

} //end of IPC message handler













void gizmo_LoadVehicleScript(const char* filename ){

	//this code doesnt run in any tight loops so doesnt need to be optimized to death.

	std::string sClearText_Filename = std::string(filename) + ".lua";

	std::string sFullPath = std::string(gizmo_lastPlayerAircraftPath) + "scripts/";



	#if IBM
		//Windows.
		//Original patch contributed by Jens Ansorg - 2007-12-14, many thanks. :)

		char caBufferCwd[2048];

		if ( getcwd(caBufferCwd, sizeof(caBufferCwd)) != NULL ) { //use getcwd() to store the location of the folder we're in now so we can come back to it later
			chdir( sFullPath.c_str() ); //switch working folder to that of lua script

					gizmo_LoadScript_Core( gizmo_luaState, "", 0, sClearText_Filename.c_str(), 1 );

			chdir( caBufferCwd ); //switch working folder back to whatever it was before we ran the lua script.

		} else {
			char caDbg[1024];
			sprintf(caDbg,"Gizmo: Core: Failed to retrieve CWD, contents of caBufferCwd: (%s)\n", caBufferCwd);
			XPLMDebugString(caDbg);
		}


	#else
		//OS X / Linux

		FILE *fp = fopen(".", "r"); //grab handle to cwd so we can come back to it later
		if( fp != NULL ){
			chdir( sFullPath.c_str() ); //switch working folder to that of lua script

					gizmo_LoadScript_Core( gizmo_luaState, "", 0, sClearText_Filename.c_str(), 1 );

			fchdir( fileno(fp) ); //return to folder we were in before we started.
			fclose( fp );
		}


	#endif


}








void gizmo_LoadFirmware(){


	XPLMDebugString( "Gizmo: Loading Firmware..\n");


	//Serial numbers are stored as a lua script which calls gizmo.activate
	std::string sSerialNumberFilename = "serial_number.lua";

	//firmware scripts live in the firmware folder to avoid any further user install confusion.
	std::string sFirmwareFolder = std::string(gizmo_pluginFolderPath) + "Gizmo.plugin/firmware";
	std::string sFirmwareFilename = "firmware_init.lua";




	#if IBM
		//Windows.
		//Original patch contributed by Jens Ansorg - 2007-12-14, many thanks. :)

		char caBufferCwd[2048];

		if ( getcwd(caBufferCwd, sizeof(caBufferCwd)) != NULL ) { //use getcwd() to store the location of the folder we're in now so we can come back to it later
			chdir( sFirmwareFolder.c_str() ); //switch working folder to that of lua script

				gizmo_LoadScript_Core( gizmo_luaState, "", 0, sSerialNumberFilename.c_str(), 1 );
				gizmo_LoadScript_Core( gizmo_luaState, "", 0, sFirmwareFilename.c_str(), 1 );

			chdir( caBufferCwd ); //switch working folder back to whatever it was before we ran the lua script.

		} else {
			char caDbg[1024];
			sprintf(caDbg,"Gizmo: Core: Failed to retrieve CWD, contents of caBufferCwd: (%s)\n", caBufferCwd);
			XPLMDebugString(caDbg);
		}


	#else
		//OS X / Linux

		FILE *fp = fopen(".", "r"); //grab handle to cwd so we can come back to it later
		if( fp != NULL ){
			chdir( sFirmwareFolder.c_str() ); //switch working folder to that of lua script

				gizmo_LoadScript_Core( gizmo_luaState, "", 0, sSerialNumberFilename.c_str(), 1 );
				gizmo_LoadScript_Core( gizmo_luaState, "", 0, sFirmwareFilename.c_str(), 1 );

			fchdir( fileno(fp) ); //return to folder we were in before we started.
			fclose( fp );
		}


	#endif


}//end gizmo_LoadFirmware









/**
This is the core function that is used to inject ALL Lua into the lua engine, either after decoding an AES chunk or by loading direct from files on disk.
*/
void gizmo_LoadScript_Core( lua_State *L, const char* buff, unsigned int buffSize, const char* chunkName, int loadFromFile ){

	char caDbg[1024];



	int nLoaderError = 0;
	if( loadFromFile ){
		if(strcmp(chunkName, "serial_number.lua") != 0){
			//shhh
			sprintf(caDbg, "Gizmo: LoadScript from File (%s)\n", chunkName);
			XPLMDebugString( caDbg );
		}


		//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );
		nLoaderError = luaL_loadfile( L, chunkName );
		//gizmo_LuaStackDump( gizmo_luaState );
		//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );

	}else{
		sprintf(caDbg, "Gizmo: LoadScript from Buffer (%s)\n", chunkName);
		XPLMDebugString( caDbg );


#if 0
#warning dumping data to disk
		XPLMDebugString("dumping c blob for debug\n");
		FILE *f = fopen("/Users/br/Desktop/c_blob2.bin", "w");
		if( f ){
			fwrite( (void*)buff, buffSize, 1, f );
			fclose(f);
		}
#endif

		//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );
		nLoaderError = luaL_loadbuffer( L, buff, buffSize, chunkName );
		//gizmo_LuaStackDump( gizmo_luaState );
		//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );
	}


	if (nLoaderError == 0) {
		//the script loaded, lets see if we can run it.



		if(
			(strcmp(chunkName, "init") == 0) ||
			(strcmp(chunkName, "init.lua") == 0 )
		){
			//found an init script, logo program on.
			gizmo_logo_alpha = 1.0;
			gizmo_Nagware_Armed = 1;

		}



		//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );
		int nScriptError = lua_pcall(L,0,0,0); //init chunk loader - errors handled.
		if( nScriptError ){
			switch (nScriptError) {
				case 0:
					//No error. yay.
					break;

				case LUA_ERRRUN:
					//std::cout << "*** Runtime error occured: " << filename << std::endl;
					sprintf(caDbg, "Gizmo: LoadScriptExec: Runtime error: %s\n", chunkName);
					XPLMDebugString(caDbg);
					break;

				case LUA_ERRMEM:
					//std::cout << "*** Memory allocation error while running the script: " << filename << std::endl;
					sprintf(caDbg, "Gizmo: LoadScriptExec: Memory Allocation Error: %s\n", chunkName);
					XPLMDebugString(caDbg);
					break;

				case LUA_ERRERR:
					//std::cout << "*** Error handler failed: " << filename << std::endl;
					sprintf(caDbg, "Gizmo: LoadScriptExec: Error handler failed: %s\n", chunkName);
					XPLMDebugString(caDbg);
					break;

				default: // not supposed to happen
					//std::cout << "*** An unknown error has occurred while running the script: " << filename << std::endl;
					sprintf(caDbg, "Gizmo: LoadScriptExec: Unknown Error No: %i\n", nScriptError);
					XPLMDebugString(caDbg);
					break;
			}//end switch


			if( (nScriptError != 0) && (lua_isstring(L,-1)) ) {
				sprintf(caDbg, "Gizmo: LoadScriptExec: Exec Debug: %s / %i / %s\n",
																	chunkName, nScriptError,
																	lua_tostring(L,-1)
																	);
				XPLMDebugString(caDbg);

				sprintf(caDbg, "%s", lua_tostring(L,-1) );
				gizmo_gui_ScriptError( std::string(caDbg) );

				//gizmo_LuaStackDump( L );
			}
			lua_pop(L,1); //pop error message off stack - exec loaded chunk error.
		}//end check for errors after exec.
		//gizmo_LuaStackDump( gizmo_luaState );
		//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );


	}else{


		//  *** File Load errors here. ***

		int iThrowDebugDialog = 1;

		switch(nLoaderError) {
			case LUA_ERRFILE:
				//Check the filename, if its a boot script we cant find we ignore the error.
				if(
					(strcmp(chunkName, "serial_number.lua") == 0) ||
					(strcmp(chunkName, "init.lua") == 0) ||
					(strcmp(chunkName, "init") == 0) ){

						iThrowDebugDialog = 0;
						//nLoaderError = 0;

				}//end check for init or init.lua file being missing, some errors are suppressed.

				if( (strcmp(chunkName, "serial_number.lua") != 0) ){
					//missing serial number files are not reported.
					sprintf(caDbg, "Gizmo: LoadScript: Loader 404: (%s) -  Are your sure your script is in the ./scripts/ folder?\n", chunkName);
					XPLMDebugString(caDbg);
				}

				//gizmo_LuaStackDump( gizmo_luaState );
				//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );


				break;

			case LUA_ERRSYNTAX:
				sprintf(caDbg, "Gizmo: LoadScript: Syntax Error: %s\n%s\n", chunkName, lua_tostring(L,-1));
				XPLMDebugString(caDbg);

				sprintf(caDbg, "%s", lua_tostring(L,-1) );
				gizmo_gui_ScriptError( std::string(caDbg) );

				break;

			case LUA_ERRMEM:
				XPLMDebugString("Gizmo: Lua Load Fail: Memory Allocation Error while loading script..\n");
				break;

			default: // not supposed to happen
				sprintf(caDbg, "Gizmo: Lua Load Fail: Unknown Parser Error No: %i\n", nLoaderError);
				XPLMDebugString(caDbg);
				break;

		}





		/*
		sprintf(caDbg, "Gizmo: file loader extended error: %i\n", nLoaderError);
		XPLMDebugString( caDbg );


		if ((nLoaderError != 0) && (lua_isstring(L,-1))) {
			sprintf(caDbg, "Gizmo: LoadScript: Loader Debug: (%s) / %s\n\n", chunkName, lua_tostring(L,-1));
			XPLMDebugString(caDbg);

			if( iThrowDebugDialog ){ // wont run for init 404's and other obvious cases of irritation.
				sprintf(caDbg, "%s", lua_tostring(L,-1) );
				//gizmo_gui_ScriptError( std::string(caDbg) );
				//gizmo_LuaStackDump( L );
			}

		}
		*/
		lua_pop(L,1); //pop error message off stack - these are the loader error messages.




	}//end run or load error


}












/**
Generates a particle quad that is stored in a display list, reserves texture blocks for artist and system.
System logo upload is doine in gizmo_Start()
*/
void gizmo_GraphicsInit(){




	//This should be replaced entirely with a GPU shader, any volunteers?
		gizmo_DisplayList_DefaultParticle = glGenLists(1);

		glNewList(gizmo_DisplayList_DefaultParticle, GL_COMPILE);
			/*
			glBegin(GL_TRIANGLES);
			glVertex3fv(v0);
			glVertex3fv(v1);
			glVertex3fv(v2);
			glEnd();
			*/

			glBegin( GL_QUADS );

					glTexCoord2f( 0.0f, 0.0f );
					//glVertex3f( fPSize * -1.0f,		fPSize * -1.0f, 0 );
					glVertex3f( -1.0f,		-1.0f, 0 );

					glTexCoord2f( 0.0f, 1.0f );
					//glVertex3f( fPSize * -1.0f,		fPSize, 0 );
					glVertex3f( -1.f,		1.f, 0 );

					glTexCoord2f( 1.0f, 1.0f );
					//glVertex3f( fPSize,				fPSize, 0 );
					glVertex3f( 1.f,		1.f, 0 );

					glTexCoord2f( 1.0f, 0.0f );
					//glVertex3f( fPSize,				fPSize * -1.0f, 0 );
					glVertex3f( 1.f,		-1.f, 0 );

				glEnd();

		glEndList();



	//Reserve a massive slab of textures for artistic purposes.
		{
		char caDbg[1024];
		sprintf( caDbg, "\t Reserving block of %i textures for artist content.\n", gizmo_GlobalTexturePoolSize);
		XPLMDebugString( caDbg );

		}
		XPLMGenerateTextureNumbers( gizmo_GlobalTexturePool, gizmo_GlobalTexturePoolSize );


	//Reserve a block of textures for system use - seperate blocks avoids clashes and eases management, in theory.
		XPLMDebugString("\t Reserving block of   64 textures for Gizmo.plugin.\n");
		XPLMGenerateTextureNumbers(gizmo_sys_textures, 64);


}









//this is used by the particle system
bool gizmo_GreaterDistanceObjectSort(const XParticle* a, const XParticle* b) {
	//printf("sort operator overload ran: %.3f:%.3f\n", a->fCameraDistance, b->fCameraDistance);
	return a->Core.camera_distance > b->Core.camera_distance;
}









void gizmo_gui_ScriptError( std::string sErrorMessage ){


	std::vector<std::string> sHaikuPool;
	sHaikuPool.push_back("Chaos reigns within.  Reflect, repent, and reboot.  Order shall return.");
	sHaikuPool.push_back("An error reduces your expensive computer to a simple stone.");
	sHaikuPool.push_back("Yesterday it worked.  Today it is not working.  Scripting is like that.");
	sHaikuPool.push_back("The sim was willing.  It considered your request,  but the scripts are wrong.");
	sHaikuPool.push_back("Errors have occurred.  We won't tell you where or why.  Good luck..");
	sHaikuPool.push_back("Wind catches lily.  Scattering petals to the wind.  Scripting fail.");
	sHaikuPool.push_back("First snow, then silence.  This thousand line script dies,  so beautifully..");
	sHaikuPool.push_back("The Tao that is seen,  is not the true Tao,  until,  You fix errors.");
	sHaikuPool.push_back("Stay the patient course.  Of little worth is your ire.  Your scripts are wrong.");
	sHaikuPool.push_back("To have no errors,  would be life without meaning.  No struggle, no joy.");
	sHaikuPool.push_back("Rather than a beep,  or a rude error message,  these words:");
	sHaikuPool.push_back("Rather than a beep,  or a rude error message,  these words:"); //bleh


	double dUniform = static_cast<double>(std::rand())/static_cast<double>(RAND_MAX);
	int iRand = dUniform * (sHaikuPool.size()-1);



		int guiWidth = 480;
		int guiHeight = 240;
		int guiLeft = ( XPLMGetDatai(gizmo_xpdr_ScreenWidth) / 2 ) - (guiWidth/2);
		int guiTop = ( XPLMGetDatai(gizmo_xpdr_ScreenHeight) / 2 ) + (guiHeight/2);

		int guiRight = guiLeft + guiWidth;
		int guiBottom = guiTop - guiHeight;


		if( gizmo_gui_handle_ErrorDialog ){
			XPDestroyWidget( gizmo_gui_handle_ErrorDialog, 1 );
		}


		gizmo_gui_handle_ErrorDialog = XPCreateWidget(
			guiLeft,
			guiTop,
			guiRight,
			guiBottom,

			1,//visible
			"Gizmo - Script Error", //descriptor

			1,//is root widget
			0, //inContainer - 0 for root always
			xpWidgetClass_MainWindow //this was our bug!
			);

		XPSetWidgetProperty( gizmo_gui_handle_ErrorDialog, xpProperty_MainWindowHasCloseBoxes, 1);
		XPAddWidgetCallback( gizmo_gui_handle_ErrorDialog, gizmo_Callback_gui_WindowHandler_ErrorDialog); //this function DESTROYS widgets, instead of just hiding them.


		XPCreateWidget(
					guiLeft+10, //left
					guiTop-25, //top
					guiRight-10, //right
					guiBottom+10, //bottom
					1,//visible
					"", //descriptor
					0,//is root widget
					gizmo_gui_handle_ErrorDialog, //inContainer - 0 for root always
					xpWidgetClass_SubWindow //this was our bug!
					);


		//this widget shows the haiku
		XPCreateWidget(
					guiLeft+25, //left
					guiTop-55, //top
					guiRight-25, //right
					guiTop-70, //bottom
					1,//visible
					sHaikuPool[iRand].c_str(), //descriptor
					0,//is root widget
					gizmo_gui_handle_ErrorDialog, //inContainer - 0 for root always
					xpWidgetClass_Caption //this was our bug!
					);


		XPWidgetID tmpWidget_ErrorMessageTextBox = XPCreateWidget(
										guiLeft+20, //left
										guiTop-110, //top
										guiRight-20, //right
										guiTop-125, //bottom
										1,//visible
										sErrorMessage.c_str(), //descriptor
										0,//is root widget
										gizmo_gui_handle_ErrorDialog, //inContainer - 0 for root always
										xpWidgetClass_TextField //this was our bug!
										);

		//attach copy and paste support!
		XPAddWidgetCallback( tmpWidget_ErrorMessageTextBox, gizmo_Callback_gui_WidgetHandler_CutCopyPaste);

		//add default callback function
		XPAddWidgetCallback( tmpWidget_ErrorMessageTextBox, gizmo_Callback_gui_WidgetHandler);



		XPWidgetID tmpWidget_ErrorMessageRebootButton = XPCreateWidget(
										guiLeft+180, //left
										guiBottom+50, //top
										guiLeft+300, //right
										guiBottom+35, //bottom
										1,//visible
										"Reboot Scripts", //descriptor
										0,//is root widget
										gizmo_gui_handle_ErrorDialog, //inContainer - 0 for root always
										xpWidgetClass_Button //this was our bug!
										);

		XPAddWidgetCallback( tmpWidget_ErrorMessageRebootButton, gizmo_Callback_gui_WidgetHandler_ErrorDialog);


}







void gizmo_gui_Nagware(){

	//if a serial number has been used no nagware should run.
	if( gizmo_Nagware_Disabled ){
		return;
	}

	
	
	
	//calculate dead center for max irritation
	int guiWidth = int(300.0 * 1.618);
	int guiHeight = 300;
	int guiLeft = ( XPLMGetDatai(gizmo_xpdr_ScreenWidth) / 2 ) - (guiWidth/2);
	int guiTop = ( XPLMGetDatai(gizmo_xpdr_ScreenHeight) / 2 ) + (guiHeight/2);
	
	int guiRight = guiLeft + guiWidth;
	int guiBottom = guiTop - guiHeight;
	
	
	
	
	
	
	
	//check to see if we already have a nagware dialog on screen.
	if( gizmo_gui_handle_NagwareDialog ){
		
		//force nagware to front of x-plane windows/widgets
		XPBringRootWidgetToFront( gizmo_gui_handle_NagwareDialog );

		//force nagware to center of screen.
		XPSetWidgetGeometry( gizmo_gui_handle_NagwareDialog, guiLeft, guiTop, guiRight, guiBottom );
		
		 
		 float fDelta = gizmo_Nagware_Timer - XPLMGetElapsedTime();
		 
		 if( fDelta >= 1.0 ){
		 
			 char tmpFeedback[1024];
				sprintf(tmpFeedback, "You can close this message in %i seconds.", int( fDelta ) );
		 
			 XPSetWidgetDescriptor( gizmo_gui_handle_NagwareDialog_lblFeedback, tmpFeedback );
		 
		 }else{
		 
			 //User has sat through nag-duration
			 
			 gizmo_Nagware_Armed = 0;
			 
			 XPShowWidget( gizmo_gui_handle_NagwareDialog_btnDismiss );
			 XPHideWidget( gizmo_gui_handle_NagwareDialog_lblFeedback );
			 
		 }//check to see if time left on wait counter
		
		
		return; //already on screen, dont need to run this code.
	}


	
	
	if( gizmo_Nagware_Armed ){

		//this global controls how long the button dialog forces itself on screen for.

		double dUniform = static_cast<double>(std::rand())/static_cast<double>(RAND_MAX);
		float fRand = dUniform * (30);
		fRand += 30.0; //minimum 30 seconds.

		gizmo_Nagware_Timer = XPLMGetElapsedTime() + fRand; //nag timer will be between 30 and 60 seconds.


			gizmo_gui_handle_NagwareDialog = XPCreateWidget(
				guiLeft,
				guiTop,
				guiRight,
				guiBottom,

				1,//visible
				"Please Register Gizmo", //descriptor

				1,//is root widget
				0, //inContainer - 0 for root always
				xpWidgetClass_MainWindow //this was our bug!
				);

			XPSetWidgetProperty( gizmo_gui_handle_NagwareDialog, xpProperty_MainWindowHasCloseBoxes, 1);
			XPAddWidgetCallback( gizmo_gui_handle_NagwareDialog, gizmo_Callback_gui_WindowHandler_NagwareDialog); //this function DESTROYS widgets, instead of just hiding them.



			//bg framing to make the layout prettier.
			XPCreateWidget(
							guiLeft+10, //left
							guiTop-25, //top
							guiRight-10, //right
							guiBottom+10, //bottom
							1,//visible
							"", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_SubWindow //this was our bug!
							);




			int wy = 35;
			int wys = 15;



			//this widget shows the haiku
			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							"Gizmo is free to share and use as you wish.", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);
			wy += wys;

			//this widget shows the haiku
			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							"Gizmo is fully functional.", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);
			wy += wys;


			//this widget shows the haiku
			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							"Gizmo is Open Source.", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);
			wy += wys;

			/*
			//this widget shows the haiku
			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							"4. Gizmo is for the Artists.", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);
			 */
			wy += wys;

			/*
			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							"5. Gizmo is loved by the Artists.", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);
			 */
			wy += wys;
			wy += wys;




			//this widget shows the haiku
			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							"Please support Gizmo and buy a serial number to remove this message.", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);
			wy += wys;

			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							" - Serial numbers do not expire.", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);
			wy += wys;

			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							" - Serial numbers for Gizmo 11.x will not work with Gizmo 12.x", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);



			wy += (wys * 1.5);

			XPWidgetID tmpWidget_btnBuyNow = XPCreateWidget(
											guiLeft+170, //left
											guiTop-wy, //top
											guiLeft+312, //left
											guiTop-(wy+wys), //bottom
											1,//visible
											"Buy Now", //descriptor
											0,//is root widget
											gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
											xpWidgetClass_Button //this was our bug!
											);

			XPAddWidgetCallback( tmpWidget_btnBuyNow, gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnBuyNow);



			wy += wys;
			wy += wys;


			//this widget shows the haiku
			XPCreateWidget(
							guiLeft+25, //left
							guiTop-wy, //top
							guiRight-25, //right
							guiTop-(wy+wys), //bottom
							1,//visible
							"Paste your serial number here:", //descriptor
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);


			wy += (wys * 1.5);


			//text entry field to allow for serial number copy and paste.
			gizmo_gui_handle_NagwareDialog_txtSerialNumber = XPCreateWidget(
											guiLeft+20, //left
											guiTop-wy, //top
											guiRight-(20 + 100), //right
											guiTop-(wy+wys), //bottom
											1,//visible
											"", //descriptor
											0,//is root widget
											gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
											xpWidgetClass_TextField //this was our bug!
											);

			//attach copy and paste support!
			XPAddWidgetCallback( gizmo_gui_handle_NagwareDialog_txtSerialNumber, gizmo_Callback_gui_WidgetHandler_CutCopyPaste);

			//add default callback function
			XPAddWidgetCallback( gizmo_gui_handle_NagwareDialog_txtSerialNumber, gizmo_Callback_gui_WidgetHandler);






			XPWidgetID tmpWidget_btnActivate = XPCreateWidget(
											guiRight-(20 + 100), //left
											guiTop-wy, //top
											guiRight-(20), //right
											guiTop-(wy+wys), //bottom
											1,//visible
											"Activate", //descriptor
											0,//is root widget
											gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
											xpWidgetClass_Button //this was our bug!
											);

			XPAddWidgetCallback( tmpWidget_btnActivate, gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnActivate);








			//this widget shows the count down
			gizmo_gui_handle_NagwareDialog_lblFeedback = XPCreateWidget(
							guiLeft+120, //left
							guiBottom+40, //top
							guiLeft+300, //right
							guiBottom+25, //bottom
							1,//visible
							//"You can close this message in XX seconds.",
							"                                         ",
							0,//is root widget
							gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
							xpWidgetClass_Caption //this was our bug!
							);


			
			gizmo_gui_handle_NagwareDialog_btnDismiss = XPCreateWidget(
											guiLeft+180, //left
											guiBottom+40, //top
											guiLeft+300, //right
											guiBottom+25, //bottom
											0,//visible
											"Dismiss", //descriptor
											0,//is root widget
											gizmo_gui_handle_NagwareDialog, //inContainer - 0 for root always
											xpWidgetClass_Button //this was our bug!
											);

			//XPHideWidget( tmpWidget_btnDismiss );
			XPAddWidgetCallback( gizmo_gui_handle_NagwareDialog_btnDismiss, gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnDismiss);
		
	}//end if gizmo_Nagware_Armed



}// gizmo_gui_Nagware()















void gizmo_Draw_3D_Objects__Particles(){

		float fP = XPLMGetDataf( gizmo_xpdr_Local_P );
		float fH = XPLMGetDataf( gizmo_xpdr_Local_H );
		float fR = XPLMGetDataf( gizmo_xpdr_Local_R );


	//Update the Origin of all sticky-marked particle engines
		float fX = XPLMGetDataf( gizmo_xpdr_Local_X );
		float fY = XPLMGetDataf( gizmo_xpdr_Local_Y );
		float fZ = XPLMGetDataf( gizmo_xpdr_Local_Z );

		for( unsigned int x=0; x < gizmo_ParticleEngines.size(); x++ ){
			if( gizmo_ParticleEngines[x]->sticky == 1 ){
				gizmo_ParticleEngines[x]->setOrigin( fX, fY, fZ );
			}
		}




	XPLMSetGraphicsState(	1, //fog
							1, //num tex units
							0, //lighting
							1, //alpha test
							1, //alpha blend
							1, //depth test
							1  //depth write
							);


	glPushMatrix();

	/*
	//glue to the aircraft for debug
	glTranslatef(
		XPLMGetDataf( gizmo_xpdr_Local_X ),
		XPLMGetDataf( gizmo_xpdr_Local_Y ),
		XPLMGetDataf( gizmo_xpdr_Local_Z )
		);
	*/


	//using point sprites this doesnt matter
	//glDisable(GL_CULL_FACE);


	//because this is a drawing op we need the data and MUST block until we get it.
	pthread_mutex_lock( &gizmo_Partices_Mutex );

	//waiting has a tiny FPS block cost for a big win.
	//while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		//XPLMDebugString("Gizmo: Mutex Locked: gizmo_Draw_3D_Objects__Particles()..\n");


		gizmo_Particles_fCameraGLX = XPLMGetDataf( gizmo_xpdr_ViewX );
		gizmo_Particles_fCameraGLY = XPLMGetDataf( gizmo_xpdr_ViewY );
		gizmo_Particles_fCameraGLZ = XPLMGetDataf( gizmo_xpdr_ViewZ );

		gizmo_ParticlePhysicsPaused = XPLMGetDatai( gizmo_xpdr_Paused );

		gizmo_Particles_ElapsedSimTime = XPLMGetElapsedTime();




				/*
				glPointParameter(GL.GL_POINT_DISTANCE_ATTENUATION,x,y,z);

				That worked well, mission accomplished.

				For others with a similar question, I used:

				float attenuation[] = {1.0f, -0.01f, -.000001f};
				gl.glPointParameterfv(gl.GL_POINT_DISTANCE_ATTENUATION, attenuation, 0);

				I believe the actual formula used is similar to this:

				s = p * sqrt(1 / (a + b * d + c * d ^ 2))

				With default values of a = 1.0f, b = 0.0f, and c = 0.0f

				Where,
				 s is the size that the point will be drawn as.
				 p is the size of the point set by glPointSize().  The default attenuation formula is essentially s = p
				 d is the distance from the viewer.
				*/




#define GIZMO_USE_POINT_SPRITE_PARTICLES 0
#if GIZMO_USE_POINT_SPRITE_PARTICLES

	//XPLMBindTexture2d( 212, 0 );

	//glBindTexture(GL_TEXTURE_2D, objectID);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	// Do some strange stuff I don't quite understand
	float quadratic[] =  { 0.01f, 0.0f, 0.01f };
	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
	float maxSize = 0.0f;
	glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
	if( maxSize > 100.0f ) maxSize = 100.0f;
	glPointSize( maxSize );

	// Do even more strange stuff I don't quite understand
	glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );
	glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 5.0f );
	glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );



	glBegin(GL_POINTS);

		XParticle *p;
		for( unsigned int ipx = 0; ipx < gizmo_svAllParticles_DrawQ.size() ; ipx++ ){
			p = gizmo_svAllParticles_DrawQ[ipx];

			//fPSize = p->size;

					glColor4f( 1.f, 1.f, 1.f, 1.f );

						glVertex3f( p->x, p->y, p->z );


		}//end of loop particles and draw quads.

	glEnd();
	glDisable(GL_POINT_SPRITE);

	glPointSize(1.0f);


#else


	float fPSize = 1.f;
	int iCurrentTexture=-1; //local texture cache


	XParticle *p;
	for( unsigned int ipx = 0; ipx < gizmo_svAllParticles_DrawQ.size() ; ipx++ ){
		p = gizmo_svAllParticles_DrawQ[ipx];

		//local texture id caching
		if( iCurrentTexture != p->Core.texture_id ){
			XPLMBindTexture2d( p->Core.texture_id, 0 );
			iCurrentTexture = p->Core.texture_id;
		}

		fPSize = p->Core.size;

			//glColor4f( 1.f, 1.f, 1.f, (p->Core.life_remaining / p->Core.life) );

			glColor4f(	p->Core.rgba[0],
						p->Core.rgba[1],
						p->Core.rgba[2],
						(p->Core.life_remaining / p->Core.life) * p->Core.rgba[3]
						);


			glPushMatrix();

				if( p->Core.sticky == 1 ){

					glTranslatef( p->Core.x, p->Core.y, p->Core.z );
						//match the position and location of the aircraft
								glRotatef( fH,	0.0f, -1.0f, 0.0f );  //heading
								glRotatef( fP,	1.0f, 0.0f, 0.0f); //pitch
								glRotatef( fR,	0.0f, 0.0f, -1.0f);	//roll

					//jump to the wing or w/ever.
					glTranslatef( p->Core.off_x, p->Core.off_y, p->Core.off_z );


				}else{
					glTranslatef( p->Core.x, p->Core.y, p->Core.z );

				}


				glRotatef( p->Core.heading_to_camera, 0.f, -1.f, 0.f );
				glRotatef( p->Core.pitch_to_camera, 1.f, 0.f, 0.f );

				glScalef( fPSize, fPSize, fPSize );

				//glCallList( gizmo_DisplayList_DefaultParticle );

				// *
				//can we used point sprites for these?
				glBegin( GL_QUADS );

					glTexCoord2f( 0.0f, 0.0f );
					//glVertex3f( fPSize * -1.0f,		fPSize * -1.0f, 0 );
					glVertex3f( -1.0f,		-1.0f, 0 );

					glTexCoord2f( 0.0f, 1.0f );
					//glVertex3f( fPSize * -1.0f,		fPSize, 0 );
					glVertex3f( -1.f,		1.f, 0 );

					glTexCoord2f( 1.0f, 1.0f );
					//glVertex3f( fPSize,				fPSize, 0 );
					glVertex3f( 1.f,		1.f, 0 );

					glTexCoord2f( 1.0f, 0.0f );
					//glVertex3f( fPSize,				fPSize * -1.0f, 0 );
					glVertex3f( 1.f,		-1.f, 0 );

				glEnd();
				// */

			glPopMatrix();


	}//end of loop particles and draw quads.


#endif





	//XPLMDebugString("Gizmo: Mutex release: gizmo_Draw_3D_Objects__Particles()..\n");
	pthread_mutex_unlock( &gizmo_Partices_Mutex );

	//glEnable(GL_CULL_FACE);
	glPopMatrix();

}










void gizmo_Draw_3D_Objects__Physics(){

	#pragma mark Physics Origin Init Here

	static int iFlagGrabOffset=1;
	if( iFlagGrabOffset ){
		iFlagGrabOffset = 0;

			//bullet world origin set to users current aircraft position.
			//use phyics.setOrigin to change this
			gizmo_fPhysicsWorldOffsetX = XPLMGetDataf( gizmo_xpdr_Local_X );
			gizmo_fPhysicsWorldOffsetY = XPLMGetDataf( gizmo_xpdr_Local_Y );
			gizmo_fPhysicsWorldOffsetZ = XPLMGetDataf( gizmo_xpdr_Local_Z );


		char caDbg[1024];
		sprintf(caDbg,"Gizmo: OpenGL<->Physics Offset Default Init: %0.3f, %0.3f, %0.3f\n", gizmo_fPhysicsWorldOffsetX, gizmo_fPhysicsWorldOffsetY, gizmo_fPhysicsWorldOffsetZ );
		XPLMDebugString( caDbg );
	}



	//only update the physics sim when the pause is off, always draw though.
	//do computations after draw to allow for easier threading

		if( gizmo_bUsePhysics ){

			//draw objects connected to the physics lib
			gizmo_Draw_PhysicsObjects();


			if( ! gizmo_SimPaused ){

				//if not paused, or if do-one-step-mode set, then render a physics frame
				if( ! gizmo_bPhysicsPaused  ||  gizmo_bPhysics_StepOnce ){

					gizmo_bPhysics_StepOnce = false;
					gizmo_BulletPhysics->Update( gizmo_fFpsM );

				}//if( physiscs_paused || step_one_fizz_frame )

			}//if( is sim paused )

		}//end: use physics?


} // gizmo_Draw_3D_Objects__Physics













int	gizmo_Callback_Draw_3D_Objects(
								XPLMDrawingPhase	inPhase,
								int					inIsBefore,
								void				*inRefcon
								){


	if( inPhase != xplm_Phase_Objects ){
		//we almost always want to run in the genric objects phase, but its useful to get access to others as well.


			std::string* sFuncName = NULL;
			std::string* sFuncName_Sys = NULL;


				switch( inPhase ){
					case xplm_Phase_FirstScene:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeFirstScene") : new std::string("OnDraw_FirstScene");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeFirstScene") : new std::string("SYS_OnDraw_FirstScene");
						break;

					case xplm_Phase_Terrain:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeTerrain") : new std::string("OnDraw_Terrain");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeTerrain") : new std::string("SYS_OnDraw_Terrain");
						break;

					case xplm_Phase_Airports:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeAirports") : new std::string("OnDraw_Airports");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeAirports") : new std::string("SYS_OnDraw_Airports");
						break;

					case xplm_Phase_Vectors:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeVectors") : new std::string("OnDraw_Vectors");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeVectors") : new std::string("SYS_OnDraw_Vectors");
						break;

					case xplm_Phase_Airplanes:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeAirplanes") : new std::string("OnDraw_Airplanes");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeAirplanes") : new std::string("SYS_OnDraw_Airplanes");
						break;

					case xplm_Phase_LastScene:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeLastScene") : new std::string("OnDraw_LastScene");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeLastScene") : new std::string("SYS_OnDraw_LastScene");

							//lots and lots and lots of alpha goodness.
							if( ! inIsBefore ){
								gizmo_Draw_3D_Objects__Particles();
							}

						break;

					default:
						//do nothing.
						break;
				}


			if( sFuncName ){
				gizmo_RunLuaFunction(gizmo_luaState, sFuncName);
				gizmo_RunLuaFunction(gizmo_luaState, sFuncName_Sys);
			}

		
		return 1;
	}







	//This may be useful for people wishing to draw different geo depending on whether the user is inside or outside the a/c.
	//Drawing some kind of flight model visualizations for example may draw unwanted effects into the cockpit view.
	/*
		int iDrawMode = XPLMGetDatai( gizmo_xpdr_ViewType );
		//1000 - 2D mode.
		//1026 - 3D mode.
		//other - outside, etc.

		switch( iDrawMode ){
			case 1000: //2D
				break;

			case 1026: //3D
				break;

			default: //outside somewhere
				break;
		}
	*/



	gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnDraw_World"));
	gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnDraw_World"));

	float fShowDebugFlightPath = XPLMGetDataf( gizmo_xpdr_ShowDebugFlightPath );
	if( fShowDebugFlightPath != 0.0 && fShowDebugFlightPath != 4.0 ){
		gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnDraw_DebugFlightPath"));
		gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnDraw_DebugFlightPath"));
	}




	//objects to draw are queued by both the physics system and the logic flightloop,
	//this allows easy drawing access from Lua at any stage of the script, not just in a special 'drawing only' script.
	//hopefully easier for the average user.

	XPLMSetGraphicsState(
						 1, //fog
						 1, //num tex units
						 1, //lighting
						 1, //alpha test
						 1, //alpha blend
						 1, //depth test
						 1  //depth write
						 );

	//draw lua commanded scenery - these are queued using gfx.drawObject or something... defered method to match the SDK.
	gizmo_Draw_QueuedObjects();
	gizmo_Draw_3D_Objects__Physics();


	return 1;
}



















/**
Each physics object has an OBJ8 mapping.
This function walks the tree in a very naive manner and draws each physics entity using its OBJ8 brush.
TODO: Batching of OBJ8. gitbug:61
*/
void gizmo_Draw_PhysicsObjects(){


	#define GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG 0



	#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
		char caDbg[1024];
	#endif


	if( gizmo_XPlaneSceneryObjectHandles.size() == 0 ){
		static int iIssueDrawingFailureWarning = 1;
		if( iIssueDrawingFailureWarning ){
			iIssueDrawingFailureWarning = 0;
			XPLMDebugString("Gizmo: Tried to use physics engine without loading an OBJ8 file. Ignored, can't draw!\n");
		}

		return;
	} //no objects loaded. abort.
	#warning code below will still crash if the brush id is not a valid object id.



	XPLMSetGraphicsState(
							 1, //fog
							 1, //num tex units
							 1, //lighting
							 1, //alpha test
							 1, //alpha blend
							 1, //depth test
							 1  //depth write
							 );


	/*
	glMatrixMode( GL_MODELVIEW );
	GLfloat saveMatrix[16];
	glGetFloatv(  GL_MODELVIEW_MATRIX, saveMatrix );
	*/


#if 1
	glPushMatrix();



		#if GIZMO_USE_BR_DRAW
			#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
			XPLMDebugString("Gizmo: jumping to physics origin\n");
			#endif

			glTranslatef( gizmo_fPhysicsWorldOffsetX, gizmo_fPhysicsWorldOffsetY, gizmo_fPhysicsWorldOffsetZ );
		#endif



		btScalar m[16]; //this is used to recover a transform matrix from the guts of bullet to allow us to draw an object at the correct location.



		gizmo_BulletPhysics->dynamicsWorld_ObjectArray = gizmo_BulletPhysics->dynamicsWorld->getCollisionObjectArray(); //drawing code - these updates are mandatory.


			unsigned int uiNumCollisionObjects=gizmo_BulletPhysics->dynamicsWorld->getNumCollisionObjects();

			#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
			sprintf( caDbg, "Gizmo: Collision Object count = %i\n", uiNumCollisionObjects );
			XPLMDebugString( caDbg );
			#endif

				for(int j=uiNumCollisionObjects-1; j > 0 ;j--){

					//XPLMDebugString("Gizmo: loop started\n");

						#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
							sprintf( caDbg, "Gizmo: svPhysicsObjectsBrushes.size() == %i\n", (int)gizmo_svPhysicsObjectBrushes.size() );
							XPLMDebugString( caDbg );

							sprintf( caDbg, "Gizmo: Collision Object ID (j) = %i\n", j );
							XPLMDebugString( caDbg );
						#endif


						//XPLMDebugString("Gizmo: get brush handle id\n");
						int iDrawWithObjectIndex = gizmo_svPhysicsObjectBrushes[ j ];


						if( iDrawWithObjectIndex > -1 ){
							if( gizmo_XPlaneSceneryObjectHandles[ iDrawWithObjectIndex ] == NULL ){

								XPLMDebugString("invalid physics drawing brush handle..\n");
								break;

							}


							#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
							sprintf( caDbg, "Gizmo: Collision Object %i = brush: %i\n", j, iDrawWithObjectIndex );
							XPLMDebugString( caDbg );

							XPLMDebugString("Gizmo: Draw physics objects, upcast ...\n");
							#endif

							//btCollisionObject* obj = gizmo_BulletPhysics->dynamicsWorld->getCollisionObjectArray()[j];
							btCollisionObject* obj = gizmo_BulletPhysics->dynamicsWorld_ObjectArray[j];
							btRigidBody* body = btRigidBody::upcast(obj);


								if(body && body->getMotionState()){
										#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
										XPLMDebugString("Gizmo: body is valid, getting motion data\n");
										#endif

										btTransform trans;
										body->getMotionState()->getWorldTransform( trans );

										//matrix save
										//GLfloat saveMatrix[16];
										//glGetFloatv(  GL_MODELVIEW_MATRIX, saveMatrix );


										//sandys drawing code has been code-folded in Xcode using these braces.
										{
										#if GIZMO_USE_SANDY_DRAW
											#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
											XPLMDebugString("Gizmo: Drawing physics object using Sandys code..\n");
											#endif

											#warning Using this draw code.
											#define RadToDeg 57.2957795

											//sandy new code block
											btVector3 origin = trans.getOrigin();
											gizmo_DrawLocation_Zero[0].x = gizmo_fPhysicsWorldOffsetX + origin.getX();
											gizmo_DrawLocation_Zero[0].y = gizmo_fPhysicsWorldOffsetY + origin.getY();
											gizmo_DrawLocation_Zero[0].z = gizmo_fPhysicsWorldOffsetZ + origin.getZ();
											btScalar yaw, pitch, roll;
											//trans.getBasis().getEuler(yaw, pitch, roll); // Did not work as expected
											trans.getOpenGLMatrix(m);
											roll = -atan2(m[1], m[0]) * RadToDeg;
											yaw = asin(m[2]) * RadToDeg;
											pitch = atan2(m[6], m[10]) * RadToDeg;
											gizmo_DrawLocation_Zero[0].heading = yaw;
											gizmo_DrawLocation_Zero[0].pitch = pitch;
											gizmo_DrawLocation_Zero[0].roll = roll;
											XPLMDrawObjects(
															(XPLMObjectRef)gizmo_arrXPlaneSceneryObjectHandles[ iDrawWithObjectIndex ],
															1,
															gizmo_DrawLocation_Zero, //array of drawing locations

															0,	//lighting
															0   //earth relative flag; gitbug:55 - make bullet earth relative.
															);
										#endif
										}


										#if GIZMO_USE_BR_DRAW
											#warning Using GIZMO_BR_DRAW code

											#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
											XPLMDebugString("Gizmo: Drawing physics object hackishly..\n");
											#endif

												glPushMatrix();

													//use the position matrix from bullet to set our GL cursor
													trans.getOpenGLMatrix(m);
													glMultMatrixf(m);



													#if 1
														#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
														XPLMDebugString("Gizmo: Calling SDK draw function\n");
														#endif

														XPLMDrawObjects(
																		gizmo_XPlaneSceneryObjectHandles[ iDrawWithObjectIndex ],
																		1,
																		gizmo_DrawLocation_Zero, //array of drawing locations
																		0,	//lighting
																		0	//earth relative flag; gitbug:55 - make bullet earth relative.
																		);

													#endif



												glPopMatrix();


											//give x-plane back the model matrix it expects.
											//glLoadMatrixf( saveMatrix );
										#endif


								}//end; check motion state is valid and body exists


						#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
						}else{ //end check to ensure that physics object has a matching brush object

							sprintf( caDbg, "Gizmo: Collision Object %i = no brush\n", j );
							XPLMDebugString( caDbg );
						#endif

						}

				}//end; for loop to draw objects


	//pop back to location before GL<->Bullet sync offset
	glPopMatrix();


	#if GIZMO_PHYSICS_DRAWING__VERBOSE_DEBUG
	XPLMDebugString("Gizmo: death code passed.\n");
	#endif


#endif



	//------------ this is the end of the physics drawing code.
	//------------ if we have drawn some OBJ8 stuff with physics we have confused x-planes clipping code.
	//------------ by drawing one sole object far away off screen we can cheat and hack around this limitation.


	//gitbug:53 - Bullet Physics Drawing Hack
	//hopefully we can fix it by drawing one stupid object off screen somewhere -_-  <- does actually work, requires default object to be loaded though.
	if( gizmo_bUsePhysicsDrawingHack ){

		if( gizmo_XPlaneSceneryObjectHandles.size() > 0 ){

			//XPLMDebugString("Gizmo: Running Bullet Drawing Hack..\n");

			XPLMDrawObjects(
						(XPLMObjectRef)gizmo_XPlaneSceneryObjectHandles[ 0 ],
						1,
						gizmo_DrawLocation_Zero, //array of drawing locations
						0, //lighting
						0	//earth relative; gitbug:55 - make bullet earth relative.
						);

		}//end got some valid objects loaded ?

	}//end use physics drawing hack?





	// -------------------- Bullet Debug Draw Code ----------------------
	glPushMatrix();
        XPLMSetGraphicsState(
							 0, //fog
							 0, //num tex units - bullet debug
							 0, //lighting
							 1, //alpha test
							 1, //alpha blend
							 1, //depth test
							 1  //depth write
							 );

		glTranslatef( gizmo_fPhysicsWorldOffsetX, gizmo_fPhysicsWorldOffsetY, gizmo_fPhysicsWorldOffsetZ );

		gizmo_BulletPhysics->dynamicsWorld->debugDrawWorld();

	glPopMatrix();
	// -------------------- END Bullet Debug Draw Code ----------------------






} //gizmo_Draw_PhysicsObjects()


















void gizmo_Draw_QueuedObjects(){

	//char caDbg[1024];

	//TODO: gitbug:61 Expand this so it can do some batching work.
	XPLMDrawInfo_t drawLocation[1];// = new XPLMDrawInfo_t[1];
					drawLocation[0].structSize = sizeof( XPLMDrawInfo_t );
					drawLocation[0].x 		= 0.f;
					drawLocation[0].y 		= 0.f;
					drawLocation[0].z 		= 0.f;
					drawLocation[0].pitch 	= 0.f;
					drawLocation[0].heading = 0.f;
					drawLocation[0].roll 	= 0.f;

	XPLMObjectRef tmpObj;
	gizmo_DrawQueueItem* drawItem = NULL;

	//we had an unsigned int here on the left, cant ever be less than 0, code always ran, boom. oops. :)
	for( int x=0; x < (int)gizmo_DrawQ.size() - 1; x++ ){
		//printf("XLua, Drawing Object...\n");

		drawItem = gizmo_DrawQ[x];


		if( drawItem == NULL ){
			XPLMDebugString("\t\t 1 2 3 BOOM.\n");
		}


		if(
			( drawItem->ObjectID > -1 )
			){

			//XPLMDebugString("\t\t object id loops valid, lets have a go...\n");

			drawLocation[0].x 		= drawItem->x;
			drawLocation[0].y 		= drawItem->y;
			drawLocation[0].z 		= drawItem->z;
			drawLocation[0].pitch 	= drawItem->p;
			drawLocation[0].heading = drawItem->h;
			drawLocation[0].roll 	= drawItem->r;

			//XPLMDebugString("\t\t location setup... getting proxy\n");
			tmpObj = gizmo_XPlaneSceneryObjectHandles[ drawItem->ObjectID ];

			//XPLMDebugString("\t\t calling for draw\n");
			XPLMDrawObjects(
							tmpObj,
							1,
							drawLocation, //gizmo_DrawQ[x].DrawInfo, //array of drawing locations
							0, //lighting
							1	//earth relative
							);

			//XPLMDebugString("\t\t drawing returned.\n");

		}else{
			XPLMDebugString("Gizmo: gfx.drawObject: Invalid object handle.\n");
		}

	}

	//XPLMDebugString("Draw_QD objects finished, reset vector to zip size.\n");
	gizmo_DrawQ.resize(0);

}
























int    gizmo_Callback_CustomCommandHandler(XPLMCommandRef        inCommand,
                         XPLMCommandPhase     inPhase,
                         void *               inRefcon){



	XLuaCustomCommandWrapper *tmpCommand = (XLuaCustomCommandWrapper*)inRefcon;


		//used to build a function sig to call.
		//value is set to a nice extreme default.
		std::string *sScriptFunction = new std::string("__FAIL_CustomCommandHandler_BUG__");


		int lua_function_retval = 0;

			switch( inPhase ){

				//start command
				case 0:
						free(sScriptFunction); //release default
						sScriptFunction = new std::string( tmpCommand->ScriptHandler + "_OnStart" );
							//printf("Gizmo: Custom Command: Calling: %s\n", sScriptFunction->c_str() );

					break;

				//is down
				case 1:
						free(sScriptFunction); //release default
						sScriptFunction = new std::string( tmpCommand->ScriptHandler + "_OnHold" );
							//we do not print feedback for this one because it can slow down the sim too much
								//printf("Gizmo: Custom Command: Calling: %s\n", sScriptFunctionName->c_str() );
					break;


				//finish command
				case 2:
						free(sScriptFunction); //release default
						sScriptFunction = new std::string( tmpCommand->ScriptHandler + "_OnStop" );
							//printf("Gizmo: Custom Command: Calling: %s\n", sScriptFunction->c_str() );
					break;

			}

			lua_function_retval = gizmo_RunLuaFunction(gizmo_luaState, sScriptFunction); //Custom Command Handler



 // Return 1 to pass the command to plugin windows and X-Plane.
 // Returning 0 disables further processing by X-Plane.

	//invert the return values from Lua before we pass them to x-plane.
	if( lua_function_retval == 1 ){
		return 0; //the script chose to absorb the event
	}else{
		return 1; //script let the event pass.
	}

}




int	gizmo_Callback_Draw_2D_Gauges(
								XPLMDrawingPhase	inPhase,
								int					inIsBefore,
								void				*inRefcon
							){


	XPLMSetGraphicsState(	1, //fog
							1, //num tex units
							1, //lighting
							1, //alpha test
							1, //alpha blend
							1, //depth test
							1  //depth write
							);



	int iDrawMode = XPLMGetDatai( gizmo_xpdr_ViewType );
	//1000 - 2D mode.
	//1026 - 3D mode.
	//other - outside, etc.

	switch( iDrawMode ){
		case 1000: //2D
				{
					//These values are used to automatically translate the GL origin in response to changes to the X-Plane panel graphics postion.
					//Custom gauges wont dissapoint the scripter when they fail to slide with the panel when this code is working correctly.
					float fPanelAreaVisible_BottomOffset = XPLMGetDataf( gizmo_xpdr_PanelAreaVisible_BottomOffset );
					float fPanelAreaVisible_LeftOffset = XPLMGetDataf( gizmo_xpdr_PanelAreaVisible_LeftOffset );

					glPushMatrix();
						glTranslatef( fPanelAreaVisible_LeftOffset, fPanelAreaVisible_BottomOffset, 0.f ); //auto slide the panel around in synch with x-planes movements.
							if( inIsBefore ){
								gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnDraw_BeforeGauges"));
								gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnDraw_BeforeGauges"));
							}else{
								gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnDraw_Gauges"));
								gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnDraw_Gauges"));
							}

					glPopMatrix();
				}
			break;



		//case 1026: //3D -- if we use this narrow approach then the code wont fire when the user is in an external view looking into the cockpit
		default:
				{
					//get acf current x,y,z
					//get camera current x,y,z
					//normalize?? (should magic this out of the way already...)
					//calc distance between points
					//check threshhold for max breakout

					static float fMaxDistance = 100.0; //100 meters max distance to bother calling the cockpit drawing stuff.
					float fDistanceFromCameraToAirframe = 10.0; //this needs to by dynamically calculated.

					if( fDistanceFromCameraToAirframe < fMaxDistance ){
						glPushMatrix();
							//It seems unlikely that these would be used during 3D work.
							//glTranslatef( fPanelAreaVisible_LeftOffset, fPanelAreaVisible_BottomOffset, 0.f );
								if( inIsBefore ){
									gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnDraw_BeforeGauges_3D"));
									gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnDraw_BeforeGauges_3D"));
								}else{
									gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnDraw_Gauges_3D"));
									gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnDraw_Gauges_3D"));
								}

						glPopMatrix();
					} //end; check distance from camera to airframe, if too far dont call these functions.
				}
			break;
	}



	gizmo_XPlaneFullScreenDialogOpen_Tripwire++;



	return 1;

}











int	gizmo_Callback_Draw_2D_Windows(
								XPLMDrawingPhase	inPhase,
								int					inIsBefore,
								void				*inRefcon
							){




	if( (inPhase == xplm_Phase_Window) && (inIsBefore == 0) ){


		if( gizmo_logo_alpha > 0.f ){

			// Draw the Gizmo Logo in the bottom right hand corner if this is a freeware item.

			XPLMSetGraphicsState(
								0, //fog
								1, //num tex units
								0, //lighting
								1, //alpha test
								1, //alpha blend
								0, //depth test
								0  //depth write
								);


			XPLMBindTexture2d( gizmo_sys_textures[sys_tex_gizmo_logo], GL_TEXTURE_2D);
			float tex_w = 128.f;
			float tex_h = 64.f;

			int w, h;
			XPLMGetScreenSize( &w, &h );


			glPushMatrix();
				glTranslatef( (float)w - tex_w, 0.f, 0.f );
				glTranslatef( 5.f, 0.f, 0.f );

				glColor4f( 1.f, 1.f, 1.f,   gizmo_logo_alpha );
				if( gizmo_Nagware_Disabled ){
					gizmo_logo_alpha = gizmo_logo_alpha - (0.3f * XPLMGetDataf(gizmo_xpdr_FpsMultiplier)); //alpha / 0.3 = 3 second fade.
				}else{
					gizmo_logo_alpha = 0.75;
				}

				glBegin(GL_QUADS);
					glTexCoord2f( 0, 0 ); glVertex3f( 0, 0, 0 );
					glTexCoord2f( 0, 1 ); glVertex3f( 0, tex_h, 0 );
					glTexCoord2f( 1, 1 ); glVertex3f( tex_w, tex_h, 0 );
					glTexCoord2f( 1, 0 ); glVertex3f( tex_w, 0, 0 );
				glEnd();

			glPopMatrix();
			
		} //end check to see if we draw the gizmo logo


	}//end RAW stuff in window phase only.




	XPLMSetGraphicsState(	1, //fog
							1, //num tex units
							1, //lighting
							1, //alpha test
							1, //alpha blend
							1, //depth test
							1  //depth write
							);



		//Handle Feeding the Lua Event Names
		{
			std::string* sFuncName = NULL;
			std::string* sFuncName_Sys = NULL;

				switch( inPhase ){
					case xplm_Phase_FirstCockpit:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeFirstCockpit") : new std::string("OnDraw_FirstCockpit");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeFirstCockpit") : new std::string("SYS_OnDraw_FirstCockpit");
						break;

					case xplm_Phase_Window:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeWindows") : new std::string("OnDraw_Windows");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeWindows") : new std::string("SYS_OnDraw_Windows");
						break;

					case xplm_Phase_LastCockpit:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeLastCockpit") : new std::string("OnDraw_LastCockpit");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeLastCockpit") : new std::string("SYS_OnDraw_LastCockpit");
						break;

					case xplm_Phase_LocalMap2D:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeLocalMap2D") : new std::string("OnDraw_LocalMap2D");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeLocalMap2D") : new std::string("SYS_OnDraw_LocalMap2D");
						break;

					case xplm_Phase_LocalMapProfile:
						sFuncName = inIsBefore ? new std::string("OnDraw_BeforeLocalMapProfile") : new std::string("OnDraw_LocalMapProfile");
						sFuncName_Sys = inIsBefore ? new std::string("SYS_OnDraw_BeforeLocalMapProfile") : new std::string("SYS_OnDraw_LocalMapProfile");
						break;

					default:
						//do nothing.
						break;
				}

			if( sFuncName != NULL ){
				gizmo_RunLuaFunction(gizmo_luaState, sFuncName);
				gizmo_RunLuaFunction(gizmo_luaState, sFuncName_Sys);

			}
			
			
			
			
			//Run the customer mouse-cursor drawing function if one has been setup.
			if( gizmo_UseCustomCursorDrawingCallback ){
				if( inPhase == xplm_Phase_Window ){
					gizmo_RunLuaFunction( gizmo_luaState, new std::string( gizmo_CustomCursorDrawingCallbackName ) ); //takes a copy of the global!
				} //draw only after windows
			} //only fire when custom cursor has been set.
			
			
		}
	
		

		return 1;


}












//this isnt actually registered as a running FL, its a sub function of the main FLCB.
void gizmo_Callback_core_UserTimers(
                                   float                inElapsedSinceLastCall,
                                   float                inElapsedTimeSinceLastFlightLoop,
                                   int                  inCounter,
                                   void *               inRefcon)
{

	#if 0
	{
		char caDbg[1024];
		sprintf( caDbg, "Gizmo: timer deltas: %0.5f   %0.5f\n", inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop );
		XPLMDebugString(caDbg);
	}
	#endif



			int iDisabledTimerCounter = 0; //track how many dead/expired timers we find

			float fMultBase = XPLMGetDataf(gizmo_xpdr_FpsMultiplier);
			float fMult = (inElapsedSinceLastCall * fMultBase) +  (1.0 * fMultBase);

			//now we've cleaned out all the dead timers, start new ones.
			for(unsigned int x=0; x<gizmo_svAllUserTimers.size(); x++ ){
				gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ x ];

				if( ip->iEnabled ){

					//2010-11-22, delta is padded.

					ip->fCounter = ip->fCounter + fMult;

					if( ip->fCounter >= ip->fInterval ){
						ip->fCounter = 0.0f; //reset the counter
							if( ip->iOneShot ){
								ip->iEnabled = 0; //turn off.
							}

							gizmo_RunLuaFunction(gizmo_luaState, new std::string(ip->sFunctionName)); //Call user timer.


					}//end interval-fill check

				}else{
					iDisabledTimerCounter++;
				}//end check enabled timer

			}//end loop all timers




			//cleanse dead timers, if any.
			if( iDisabledTimerCounter > 0 ){
				#if 0
					char caDbg[1024];
					sprintf(caDbg, "Gizmo: Cleaning dead timers. %i in pool.\n", iDisabledTimerCounter);
					XPLMDebugString( caDbg );
				#endif

				//this loop will continue until it could NOT find any items to clean.
				for(unsigned int x=0; x<gizmo_svAllUserTimers.size(); x++ ){
					gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ x ];
					if( ip->iEnabled == 0 ){
						gizmo_svAllUserTimers.erase( gizmo_svAllUserTimers.begin() + x );
						delete ip;
						break;
					}//end check for disabled.
				}//end loop looking for a dead timer
			}//end check to see if theres any to erase


}//end: gizmo_Callback_core_UserTimers




//Our main worker function, x-plane calls this to do core work.
float gizmo_Callback_FlightLoop(
                                   float                inElapsedSinceLastCall,
                                   float                inElapsedTimeSinceLastFlightLoop,
                                   int                  inCounter,
                                   void *               inRefcon)
{
	


		// -- Process Curl Staging Pool --
			if( gizmo_curl_thread_interchange_staging.size() > 0 ){
				while( pthread_mutex_trylock( &gizmo_Curl_Mutex ) > 0 ){
						/*gizmo_thread_sleep(1);*/
						XPLMDebugString("Gizmo: Curl block: Waiting to feed staging data from main flight loop.\n");
					};


					#if 1
					{
						char caDbg[1024];
						sprintf(caDbg, "Gizmo: Curl: Request staging transfer to thread-shared input queue: %i q'd\n", (int)gizmo_curl_thread_interchange_staging.size());
						XPLMDebugString(caDbg);
					}
					#endif
					for( unsigned int x=0; x < gizmo_curl_thread_interchange_staging.size(); x++ ){
						gizmo_curl_thread_interchange_input.push_back( gizmo_curl_thread_interchange_staging[x] );
					}
					gizmo_curl_thread_interchange_staging.resize(0);



				pthread_mutex_unlock( &gizmo_Curl_Mutex );
			}//only action this code if there is stuff to do.


			//pass into our timer core function for user timer updates.
			gizmo_Callback_core_UserTimers( inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon );


	//-- per-aircraft, per-sim-session, usage scoreboard. --

		//please leave this code intact and operation so that scripters have a chance to run a proper trial timer.

		std::string sMapKey = std::string((const char*)gizmo_caAcfMd5Hash);
		if( gizmo_map_DemoScoreBoard[sMapKey] ){
			
			//XPLMDebugString("scoreboard increased\n");

			gizmo_map_DemoScoreBoard[sMapKey] += ( 1.f * XPLMGetDataf(gizmo_xpdr_FpsMultiplier) );  //increase scoreboard counter.
			
			if( gizmo_map_DemoScoreBoard[sMapKey] > 30.f ){
				//throw a nagware?
				
				gizmo_gui_Nagware(); //pop the nagware dialog after N minutes of joy.
				
				
			}
			
		}else{
			//init the scoreboard for players currently loaded aircraft
			gizmo_map_DemoScoreBoard[sMapKey] = 0.1f; //init to something != 0
			
		}

	// -- end scoreboard tracking code --



	
	
	
	static int iTriggerForceRedrawCall = 0;
	
	if( gizmo_XPlaneFullScreenDialogOpen_Tripwire ){
		//our drawing code is running, WX / JS / other full-screen bullshit dialog is NOT active.
		
		if( iTriggerForceRedrawCall ){
			iTriggerForceRedrawCall = 0;
			
			gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnForceRedraw"));
			gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnForceRedraw"));
		}
		
	}else{
		//our drawing code is NOT running.
		//X-Plane has one of its dialogs open.
		
		//XPLMDebugString("Gizmo: Fullscreen X-Plane dialog open.\n");
		iTriggerForceRedrawCall = 1;
		
	}
	
	
	
	
	
	

	//Execute the core script engine loop.
        gizmo_DoCore(); //this will run all queued script events.



	//Particle Engine Call in single threaded mode
	#if GIZMO_THREADED_PARTICLES
	#else
		gizmo_YUCK_DoParticlesUpdate();
	#endif

	
	
	//This var is used to detect when X-Plane is occupied with a full screen dialog like WX or JStick Settings.
	//See also: Gauges drawing phase, which also pokes this variable.
	gizmo_XPlaneFullScreenDialogOpen_Tripwire = 0;
	
	
	
	return -1.0f;
};









float gizmo_Callback_FlightLoop_CurlThreads(
                                   float                inElapsedSinceLastCall,
                                   float                inElapsedTimeSinceLastFlightLoop,
                                   int                  inCounter,
                                   void *               inRefcon)
{





	while( pthread_mutex_trylock( &gizmo_Curl_Mutex ) != 0 ){
		XPLMDebugString("Gizmo: Curl BLOCK!\n");
		return -1.0f;
	}



		if(  gizmo_curl_thread_interchange_notify.size() > 0 ){
			#if 1
			{
				char dbg[1024];
				sprintf(dbg, "Gizmo: http: Notify queue size: %i\n", (int)gizmo_curl_thread_interchange_notify.size() );
				XPLMDebugString( dbg );
			}
			#endif

			//std::string sdbg = "";
			for( unsigned int x=0; x < gizmo_curl_thread_interchange_notify.size(); x++ ){


				#if 1
				{
					char dbg[1024];
					sprintf(dbg, "Gizmo: http: Notify processing payload: %i\n", x );
					XPLMDebugString( dbg );
				}
				#endif





				lua_State* L = gizmo_luaState;

				//attempt to fire a lua callback
				  // push functions and arguments
				  lua_getglobal(L, gizmo_curl_thread_interchange_notify[x]->cb_function_name.c_str() );  // function to be called
				  lua_pushlstring(L,  (char*)gizmo_curl_thread_interchange_notify[x]->rx_buffer, gizmo_curl_thread_interchange_notify[x]->rx_buffer_offset );   // push 1st argument
				  lua_pushstring(L,  gizmo_curl_thread_interchange_notify[x]->url.c_str() );   // push 2nd argument
				  lua_pushinteger(L, gizmo_curl_thread_interchange_notify[x]->rx_buffer_offset );


					//Attempt to run the users callback function, will throw error GUI if fails.
						int nScriptError=lua_pcall(L, 3, 1, 0); //curl callback - errors handled
						if( nScriptError ){
							if ((nScriptError != 0) && (lua_isstring(L,-1)) ){   // && (gizmo_verbose_debug)) {
								char caDbg[1024];
								sprintf(caDbg, "\nGizmo: http.call_back Error: %s / %i / %s\n", gizmo_curl_thread_interchange_notify[x]->cb_function_name.c_str(), nScriptError, lua_tostring(L,-1) );
								XPLMDebugString(caDbg);

								gizmo_gui_ScriptError( std::string(caDbg) );

								//gizmo_LuaStackDump( L );
							}

							lua_pop(L,1); //pop error message off stack - curl threads callback
						}



			}//end loop notify queue
			gizmo_curl_thread_interchange_notify.resize(0);

		}//are there items in the notify queue?





	pthread_mutex_unlock( &gizmo_Curl_Mutex );



	//return in 1 second when we have serviced some stuff
	return 0.5f; //1/10th second checks instead of 1/frame
}

















/**
This function:
- Restart the script engine in response to a trigger flag.
- Create any queud GUI windows and register their OnCreate functions for calling.
- Changes the working folder to the aircraft/scripts folder and then passes control to gizmo_DoCore_RunScriptingQueue(..)
*/
void gizmo_DoCore(){



	//restart the lua engine if flagged to do so. defered event to avoid lua engine melt down.
	if( gizmo_flag_LuaRestartCalled ){
		gizmo_flag_LuaRestartCalled = false;
		gizmo_Restart();
		//return -1.f;
	}





	//globally shared debug draw state flag. should help a bit with some of the super complex scripts.
	float fShowDebugFlightPath = XPLMGetDataf( gizmo_xpdr_ShowDebugFlightPath );
	if( fShowDebugFlightPath != 0.0 && fShowDebugFlightPath != 4.0 ){
		//true
		gizmo_giDebugDrawOn = true;
	}else{
		gizmo_giDebugDrawOn = false;
	}





	gizmo_fFpsM = XPLMGetDataf(gizmo_xpdr_FpsMultiplier);
	gizmo_SimPaused = XPLMGetDatai( gizmo_xpdr_Paused ); //this is also used by the Bullet wrapper code to automatically stop the physics sim when x-plane is paused.






	//Run some dynamically generated scripts to pump data updates into the Lua engine easily.
	//Hackish but effective, doesnt seem to cost much in speed for the ease of use.
	//Alternative is a very long winded session talking to the lua tables using assembler like C APIs.

		int x, y;

		//update the moose position automatically.
			XPLMGetMouseLocation( &x, &y );


			char caTmpLua[128];
			//memset(caTmpLua,0,512); //clear the string.
			sprintf(caTmpLua, "mouse.x=%i;mouse.y=%i;", x, y);
			if( luaL_dostring( gizmo_luaState, caTmpLua ) ){
				//handle standard lua error event as documented.
				lua_pop(gizmo_luaState,1);
			}
			if( lua_isnil( gizmo_luaState, -1 ) ){
				//handle weird "nil" stack overflow where we get left with a nil on the stack by every call to these.
				lua_pop(gizmo_luaState,1);
			}




	//Window Constructor callbacks.
		for( unsigned int iWindowInspect=0; iWindowInspect < gizmo_Pool_WindowWrappers.size(); iWindowInspect++ ){
			if( gizmo_Pool_WindowWrappers[ iWindowInspect ]->NeedInit ){
				gizmo_Pool_WindowWrappers[ iWindowInspect ]->NeedInit = 0;

				//Create dynamic event name to allow for _OnBeforeClose() Handler also
				std::string *sWindowInitScriptFunctionName = new std::string( gizmo_Pool_WindowWrappers[ iWindowInspect ]->WindowIdentifier + "_OnCreate" );
				gizmo_Pool_ScriptCallbacks.push_back( sWindowInitScriptFunctionName );

			}//check for init flag

		}//loop pool of windows - will get slow if have millions.



	//We now jump into the scripts folder so dofile() works nicely.
		gizmo_DoCore_JumpToScriptFolder();


}//end: function gizmo_DoCore







inline void gizmo_DoCore_JumpToScriptFolder(){
		//code to change folders


		#if IBM
			//Windows.
			//Original patch contributed by Jens Ansorg - 2007-12-14, many thanks. :)

			char caBufferCwd[4096];
			if ( getcwd(caBufferCwd, sizeof(caBufferCwd)) != NULL ) { //use getcwd() to store the location of the folder we're in now so we can come back to it later
				chdir( gizmo_scriptsFolderPath );

					gizmo_DoCore_RunScriptingQueue();

				chdir( caBufferCwd ); //switch working folder back to whatever it was before we ran the lua script.

			}


		#else
			//OS X / Linux

			FILE *fp = fopen(".", "r"); //grab handle to cwd so we can come back to it later
			if( fp != NULL ){
				chdir( gizmo_scriptsFolderPath );

					gizmo_DoCore_RunScriptingQueue();

				fchdir( fileno(fp) ); //return to folder we were in before we started.
				fclose( fp );
			}


		#endif

}




/**
Control is passed from here into: gizmo_RunLuaFunction( lua_State*, std::string* )
*/
inline void gizmo_DoCore_RunScriptingQueue(){


	//Call sequence OnBeforeReboot -> OnBoot -> ACF Finishes Loading -> OnFirstFrame -> main
	if( gizmo_flag_CallOnFirstFrame ){
		gizmo_flag_CallOnFirstFrame = false;
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnFirstFrame") );
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnFirstFrame") );
	}

	
	//A situation file was loaded (.sit)
	if( gizmo_OnSituationLoaded_event_trigger_armed ){
		gizmo_OnSituationLoaded_event_trigger_armed = 0;
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnSituationLoaded") );
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnSituationLoaded") );
	}

	
	
	

	//we always need to RUN Main...
	gizmo_RunLuaFunction( gizmo_luaState, new std::string("main") );
	gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_main") );


	//Run any queued events.
	for( unsigned int iScriptX=0; iScriptX < gizmo_Pool_ScriptCallbacks.size(); iScriptX++ ){

		#if 0
		{
			char caDbg[1024];
			sprintf(caDbg, "Gizmo: DoCore_RunScriptingQ: (%s)\n", gizmo_Pool_ScriptCallbacks[iScriptX]->c_str() );
			XPLMDebugString( caDbg );
		}
		#endif

		//strings will be destroyed inside RunLuaFunction
		gizmo_RunLuaFunction( gizmo_luaState, gizmo_Pool_ScriptCallbacks[iScriptX] ); //Core queue processing

	}

	gizmo_Pool_ScriptCallbacks.resize(0);


}//end: gizmo_DoCore_RunScriptQueue





#if APL
int gizmo_AppleConvertPath(const char * inPath, char * outPath, int outPathMaxLen) {

	//this will convert a path from legacy to POSIX ?

	CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, inPath ,kCFStringEncodingMacRoman);
	if (inStr == NULL)
		return -1;
	CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0);
	CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	if (!CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle))
		return -1;
	CFRelease(outStr);
	CFRelease(url);
	CFRelease(inStr);
	return 0;
}
#endif








void gizmo_FindPluginFolder(){


    //const char *sep = XPLMGetDirectorySeparator();
    const char sep = '/';
    char SysPath[512]; //i think x-plane might have a path length cap...
    char tmp[4096];


    XPLMGetSystemPath(SysPath);

    //sprintf(tmp, "%sResources%cplugins%c", SysPath, *sep, *sep);
    sprintf(tmp, "%sResources%cplugins%c", SysPath, sep, sep);


		#if APL

			char tmp2[4096];
			sprintf(tmp2, "/Volumes/%s", tmp);
			strcpy( tmp, tmp2 );

			//replace all occurences of the colon seperator with /'s

			int x=0;
			for(x=0; x<(int)strlen(tmp); x++){
				if( tmp[x] == ':' ){
					tmp[x] = '/';
				}
			}

		#endif


		#if IBM
			//replace all \ occurences with /
			int x=0;
			for(x=0; x<(int)strlen(tmp); x++){
				if( tmp[x] == '\\' ){
					tmp[x] = '/';
				}
			}

		#endif


    strcpy( gizmo_pluginFolderPath, tmp );

}



void gizmo_FindRelativeAircraftPath(){

	//we want a copy of the string data starting from fairly far inside the actual string.
	//this operation baseically chops off the x-plane folder part
	memset( gizmo_relativePathToUsersAircraftFile, 0, 2048 );
	strcpy( gizmo_relativePathToUsersAircraftFile, gizmo_lastPlayerAircraftPath + (strlen(gizmo_xplaneFolderPath)) );


	//we have the full x-plane path in a variable
	//copy out a sub section of it, just the relative folder is all we want.
	memset( gizmo_relativePathToUsersAircraftFolder, 0, 2048 );
	strcpy( gizmo_relativePathToUsersAircraftFolder, gizmo_relativePathToUsersAircraftFile );



		char tmp[4096];
		sprintf( tmp, "%s%s", gizmo_relativePathToUsersAircraftFile, gizmo_lastPlayerAircraftName );
		//if( strcmp( tmp, gizmo_relativePathToUsersAircraftFile ) != 0 ){
		strcpy( gizmo_relativePathToUsersAircraftFile, tmp );



	//sprintf( gizmo_relativePathToUsersAircraftFile, "%s%s", gizmo_relativePathToUsersAircraftFile, gizmo_lastPlayerAircraftName );
	XPLMDebugString( std::string(std::string("Gizmo: X-Plane Relative Folder Path: (") + std::string( gizmo_relativePathToUsersAircraftFolder ) + std::string(")\n")).c_str() );

	//sprintf( gizmo_relativePathToUsersAircraftFile, "%s%s", gizmo_relativePathToUsersAircraftFile, gizmo_lastPlayerAircraftName );
	XPLMDebugString( std::string(std::string("Gizmo: X-Plane Relative File Path:   (") + std::string( gizmo_relativePathToUsersAircraftFile ) + std::string(")\n")).c_str() );


}















void gizmo_StartLuaBindings(){


	gizmo_luaState = lua_open();
	lua_atpanic( gizmo_luaState, gizmo_AtPanic);


		//open the standard suite of Lua libraries.
		luaL_openlibs( gizmo_luaState );
			//the call above opens

				//coroutine
				//table
				//io
				//os
				//string
				//math
				//debug
				//package


		luaL_register( gizmo_luaState, "acf",		 gizmo_ns_acf);
		//luaL_register( gizmo_luaState, "airframe",	 gizmo_ns_airframe);

		//luaL_register( gizmo_luaState, "base64",	gizmo_ns_base64);

		luaL_register( gizmo_luaState, "camera",	 gizmo_ns_camera);

		luaL_register( gizmo_luaState, "fmc",        gizmo_ns_fmc);			//See also: nav module for lua constants this module will need.

		luaL_register( gizmo_luaState, "gfx",        gizmo_ns_gfx);
		luaL_register( gizmo_luaState, "gizmo",		 gizmo_ns_gizmo);
		luaL_register( gizmo_luaState, "gui",        gizmo_ns_gui);

		luaopen_luagl( gizmo_luaState );									//Lua GL
		luaopen_luaglu( gizmo_luaState );									//Lua GLU! - 11.5.25

		luaL_register( gizmo_luaState, "hash",       gizmo_ns_hash);
		luaL_register( gizmo_luaState, "http",       gizmo_ns_http);
		luaL_register( gizmo_luaState, "logging",    gizmo_ns_logging);
		luaL_register( gizmo_luaState, "matrix",     gizmo_ns_matrix);
		luaL_register( gizmo_luaState, "memcache",   gizmo_ns_memcache);
		luaL_register( gizmo_luaState, "menu",       gizmo_ns_menu);
		luaL_register( gizmo_luaState, "mouse",      gizmo_ns_mouse);


		luaL_register( gizmo_luaState, "nav",		gizmo_ns_nav);
			//setup some pre-def namespace vars that can be used with the nav lib
				char caTmpLua[1024];
				memset(caTmpLua,0,1024); //clear the string.
				sprintf(caTmpLua, "nav.unknown=%i; nav.airport=%i; nav.ndb=%i; nav.vor=%i; nav.ils=%i; nav.localizer=%i; nav.glideslope=%i; nav.outermarker=%i; nav.middlemarker=%i; nav.innermarker=%i; nav.fix=%i; nav.dme=%i; nav.latlon=%i; ",
						xplm_Nav_Unknown,
						xplm_Nav_Airport,
						xplm_Nav_NDB,
						xplm_Nav_VOR,
						xplm_Nav_ILS,
						xplm_Nav_Localizer,
						xplm_Nav_GlideSlope,
						xplm_Nav_OuterMarker,
						xplm_Nav_MiddleMarker,
						xplm_Nav_InnerMarker,
						xplm_Nav_Fix,
						xplm_Nav_DME,
						xplm_Nav_LatLon

						);

				XPLMDebugString("Gizmo: Loading NAV constants..\n");
				//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );
				if( luaL_dostring( gizmo_luaState, caTmpLua ) ){
					lua_pop(gizmo_luaState,1); //pop error message off stack - nav constants setup
				}
				//gizmo_LuaStackDump( gizmo_luaState );
				//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );


		luaL_register( gizmo_luaState, "particles",  gizmo_ns_particles);
		luaL_register( gizmo_luaState, "physics",    gizmo_ns_physics);
		luaL_register( gizmo_luaState, "plugins",    gizmo_ns_plugins);
		luaL_register( gizmo_luaState, "shaders",    gizmo_ns_shaders);
		luaL_register( gizmo_luaState, "sound",      gizmo_ns_sound);
		luaL_register( gizmo_luaState, "timer",		gizmo_ns_timer);

			//luaL_register( gizmo_luaState, "traffic",	gizmo_ns_traffic);

		luaL_register( gizmo_luaState, "trig",		gizmo_ns_trig);
		luaL_register( gizmo_luaState, "utils",		gizmo_ns_utils);
		luaL_register( gizmo_luaState, "xp",         gizmo_ns_xplane);


		//IO Jail.
			XPLMDebugString("Gizmo: Init IO Jail..\n");
			//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );
			if( luaL_dostring( gizmo_luaState, (const char*)gfw_IO_Jail_lua ) ){
				XPLMDebugString("Gizmo: IO Jail failed.\n");
				lua_pop(gizmo_luaState,1); //pop error message off stack - IO Jail is in effect.

			}





		gizmo_LoadFirmware();



}






int gizmo_AtPanic(lua_State *L){
	
	size_t szLen;
	const char *sErrorMessage		= lua_tolstring( L, -1, &szLen );

	std::string sDbg = std::string("Gizmo Panic Error: (") + std::string( sErrorMessage ) + std::string( ")\n" );
	
	XPLMDebugString( sDbg.c_str() );
	
	//return 0;
}



void gizmo_ResetEventMap(){
	gizmo_DisabledEventsMap.clear();
	XPLMDebugString("Gizmo: Event Map cleared.\n");
}



void gizmo_Restart(){

	XPLMDebugString("\nGizmo: Entered gizmo_Restart(..)\n");

		//destroy any existing error dialog.
		if( gizmo_gui_handle_ErrorDialog ){
			XPDestroyWidget( gizmo_gui_handle_ErrorDialog, 1 );
			gizmo_gui_handle_ErrorDialog = NULL;
		}

		gizmo_Stop(); //all other variable-reset code goes in here.
		gizmo_Start();


		gizmo_SetTerrainMode_Defaults();


		//-- Why is this block not inside _Start() ?
		// --- gizmo_Start(); is also called from XPluginStart() and this code is not suitable for use 
		// --- there as no ACF file is loaded yet.
			gizmo_RehashAircraftAndReleaseOverrides();
	
			//FIXME: we should keep a flag to see if its us that has camera control
			if( XPLMIsCameraBeingControlled(NULL) ){
				XPLMDontControlCamera();
			}


			XPLMDebugString("Gizmo: Loading init...\n");
			gizmo_LoadVehicleScript("init"); //call for the .lua version -plaintext, first.


			gizmo_ResetEventMap();
		//--

		
		// -- 2011-05-06 - Immediate direct call of OnBoot instead of waiting for DoCore to pick it up.
		//gizmo_DoCore();
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnBoot") );
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("SYS_OnBoot") );

	
		XPLMDebugString("Gizmo: Completed gizmo_Restart(..)\n\n");

}//end gizmo_Restart()






/**
This function runs at the start of every reboot.
*/
void gizmo_Start(){


	XPLMDebugString("\nGizmo: Starting up..\n");

	//setup a blank drawing location, we do our own GL rotate/translate
	gizmo_DrawLocation_Zero = new XPLMDrawInfo_t[1];
		gizmo_DrawLocation_Zero[0].structSize = sizeof( XPLMDrawInfo_t );
		gizmo_DrawLocation_Zero[0].x 		= 0.f;
		gizmo_DrawLocation_Zero[0].y 		= 0.f;
		gizmo_DrawLocation_Zero[0].z 		= 0.f;
		gizmo_DrawLocation_Zero[0].pitch 	= 0.f;
		gizmo_DrawLocation_Zero[0].heading 	= 0.f;
		gizmo_DrawLocation_Zero[0].roll 		= 0.f;

	gizmo_OpenAL = new GizmOpenALHooks();

	gizmo_StartBulletPhysics(); //this is used by the physics.module





	gizmo_bitflags_for_nav_search.airport				= 0;
	gizmo_bitflags_for_nav_search.ndb					= 0;
	gizmo_bitflags_for_nav_search.vor					= 0;
	gizmo_bitflags_for_nav_search.ils					= 0;
	gizmo_bitflags_for_nav_search.localizer				= 0;
	gizmo_bitflags_for_nav_search.glideslope			= 0;
	gizmo_bitflags_for_nav_search.outermarker			= 0;
	gizmo_bitflags_for_nav_search.middlemarker			= 0;
	gizmo_bitflags_for_nav_search.innermarker			= 0;
	gizmo_bitflags_for_nav_search.fix					= 0;
	gizmo_bitflags_for_nav_search.dme					= 0;
	gizmo_bitflags_for_nav_search.latlon				= 0;





	gizmo_StartLuaBindings();


	//We do NOT reset the proxy settings.

	gizmo_UseCustomCursorDrawingCallback = 0;
	gizmo_CustomCursorDrawingCallbackName = "";

	gizmo_logo_alpha = 0.f; //splash screen logo fader.
	gizmo_GlobalTexturePoolCursor = 0; //reset texture pool offset.

	//this is setup by xp.loadSituation() script calls and allows us to create an addditional scripting event hook.
	gizmo_OnSituationLoaded_event_trigger_armed = 0;

	
	gizmo_flag_CallOnFirstFrame = true; //this will call the OnFirstFrame event next time we run through DoCore


	gizmo_Nagware_Timer = 0.0;



	//Upload our logos.
		gizmo_LoadTGA_DecodeHeaders( gizmo_sys_textures[sys_tex_gizmo_logo],		Gizmo_Logo_tga, Gizmo_Logo_tga_len);
		gizmo_LoadTGA_DecodeHeaders( gizmo_sys_textures[sys_tex_xa_logo],			gizmo_xa_logo_tga, gizmo_xa_logo_tga_len);
		gizmo_LoadTGA_DecodeHeaders( gizmo_sys_textures[sys_tex_lua_logo],			gizmo_lua_logo_tga, gizmo_lua_logo_tga_len);
		gizmo_LoadTGA_DecodeHeaders( gizmo_sys_textures[sys_tex_gizmo_ui],			ToolTrayIconsBuiltin_tga, ToolTrayIconsBuiltin_tga_len);


}




void gizmo_Stop(){

	XPLMDebugString("Gizmo: Cleaning up..\n");
	//reverse order of start up


		XPLMDebugString("\t Reset timers..\n");
			//gizmo_svAllUserTimers
			for(unsigned int x=0; x<gizmo_svAllUserTimers.size(); x++ ){
				gizmo_user_timer_info* ip = gizmo_svAllUserTimers[ x ];
				delete(ip);
			}
			gizmo_svAllUserTimers.resize(0); //zap


		XPLMDebugString("\t Reset menus..\n");
			for(unsigned int x=0; x<gizmo_Pool_VehicleMenuItems.size(); x++ ){
				std::string* ip = gizmo_Pool_VehicleMenuItems[ x ];
				delete(ip);
			}
			gizmo_Pool_VehicleMenuItems.resize(0); //zap
			gizmo_Menu_Reset();


		XPLMDebugString("\t Reset GUI Widgets..\n");
			for( unsigned int x=0; x < gizmo_Pool_BaseWrappers.size(); x++ ){
				XPDestroyWidget( gizmo_Pool_BaseWrappers[x]->WidgetID, 1);
				delete( (gizmo_Pool_BaseWrappers[x]) );
			}
			gizmo_Pool_BaseWrappers.resize(0);


		XPLMDebugString("\t Reset GUI Windows..\n");
			for( unsigned int x=0; x < gizmo_Pool_WindowWrappers.size(); x++ ){
				XPDestroyWidget( gizmo_Pool_WindowWrappers[x]->WidgetID, 1);
				delete( (gizmo_Pool_WindowWrappers[x]) );
			}
			gizmo_Pool_WindowWrappers.resize(0);


		XPLMDebugString("\t Reset OBJ8 handles..\n");
			gizmo_CleanupXPlaneObjectHandles();


		XPLMDebugString("\t Reset datarefs..\n");
			gizmo_CustomRefsDestroy();


		XPLMDebugString("\t Reset Bullet Physics..\n");
			delete[] gizmo_DrawLocation_Zero;
			gizmo_StopBulletPhysics();


		XPLMDebugString("\t Reset OpenAL..\n");
			delete gizmo_OpenAL;


		XPLMDebugString("\t Reset Particle Engines..\n");
			for( unsigned int x=0; x < gizmo_ParticleEngines.size(); x++ ){
				delete( (gizmo_ParticleEngines[x]) );
			}
			gizmo_ParticleEngines.resize(0);


		XPLMDebugString("\t Reset Lua..\n");
			//kill the lua engine instance.
			lua_close( gizmo_luaState );


		gizmo_ReleaseOverrides();


	XPLMDebugString("Gizmo: Cleanup Finished.\n");

}








/**
This function recalculates the stored MD5 hash of the ACF file and resets all of X-Planes override datarefs to 0. (releases them)
It does this to ensure that when the user changes to another aircraft X-Plane behaves normally and scripters don't need to care. (and can't get it wrong!)
*/
void gizmo_RehashAircraftAndReleaseOverrides(){


        //------ attempt to calculate an MD5 hash of the aircraft file.. NOT a drm function.
            char* caAircraftFileCompletePath = new char[ strlen( gizmo_lastPlayerAircraftPath ) * 2 ];
            sprintf(caAircraftFileCompletePath, "%s%s", gizmo_lastPlayerAircraftPath, gizmo_lastPlayerAircraftName );

				memset(gizmo_caAcfMd5Hash, 0, 128);
				gizmo_md5_hash_file( caAircraftFileCompletePath, gizmo_caAcfMd5Hash ); // <- written to global

            delete( caAircraftFileCompletePath );
        //----- end hash of aircraft


		//this function releases all override-datarefs. not a drm functione either.
		gizmo_ReleaseOverrides();

}










void gizmo_md5_hash_file( char* filename, unsigned char* hashReturn ){

	struct stat fsize;

	if( !stat(filename, &fsize) ){
		//return fsize.st_size;
	}

	//printf("Gizmo: MD5 Hash, File size: %i\n", (int)fsize.st_size);

	unsigned char* caFileBuffer = (unsigned char*)malloc( (unsigned int)fsize.st_size );
	memset( caFileBuffer, 0, (unsigned int)fsize.st_size );


	FILE* fp = fopen( filename, "rb" );
	if( fp != NULL ){
		fread( caFileBuffer, 1, (unsigned int)fsize.st_size, fp );
		fclose( fp );
	}else{
		XPLMDebugString("Gizmo MD5 file hash failure. No such file:");
		XPLMDebugString( filename );
		XPLMDebugString("\n");

	}


	//do hash!
	//unsigned char caMd5Hash[128];
	//memset( caMd5Hash, 0, 128 );
        xplugins_md5( caFileBuffer, (unsigned int)fsize.st_size, hashReturn );

	//printf("Gizmo: MD5 Filename: %s\n", filename);
	//printf("Gizmo: MD5 Hash: %s\n", hashReturn);

	{
	char dbg[4096];
	sprintf(dbg, "Gizmo: MD5: %s: %s\n", filename, hashReturn);
	XPLMDebugString(dbg);
	}


	free( caFileBuffer );
	//delete( caAircraftFileCompletePath );

}

















void gizmo_FindXPlaneFolder(){



    char SysPath[512];
    XPLMGetSystemPath(SysPath);

    char tmp[4096];
	memset(tmp,0,4096);

	#if APL

		sprintf(tmp, "/Volumes/%s", SysPath);

		tmp[ strlen(tmp) ] = 0; //truncate string by one char

		//replace all occurences of the colon seperator with /'s
		for(unsigned int x=0; x < strlen(tmp); x++){
			if( tmp[x] == ':' ){
				tmp[x] = '/';
			}
		}

	#elif IBM
		sprintf(tmp, "%s", SysPath);
		//kill all backslashes
		for(unsigned int x=0; x < strlen(tmp); x++){
			if( tmp[x] == '\\' ){
				tmp[x] = '/';
			}
		}
	#else
		//Linux
		sprintf(tmp, "%s", SysPath);
	#endif


	strcpy(gizmo_xplaneFolderPath, tmp);

	{
	char caDbg[4096];
	sprintf( caDbg, "Gizmo: X-Plane Folder: (%s)\n", gizmo_xplaneFolderPath);
	XPLMDebugString( caDbg );
	}

}





bool	gizmo_GetTextFromClipboard(std::string& outText)
{
#if IBM
	HGLOBAL   	hglb;
	LPTSTR    	lptstr;
	bool		retVal = false;
	static XPLMDataRef hwndDataRef = XPLMFindDataRef("sim/operation/windows/system_window");
	HWND hwndMain = (HWND) XPLMGetDatai(hwndDataRef);

	if (!IsClipboardFormatAvailable(CF_TEXT))
		return false;

	if (!OpenClipboard(hwndMain))
		return false;

	hglb = GetClipboardData(CF_TEXT);
	if (hglb != NULL)
	{
		lptstr = (LPSTR)GlobalLock(hglb);
		if (lptstr != NULL)
		{
			outText = lptstr;
			GlobalUnlock(hglb);
  			retVal = true;
		}
	}

	CloseClipboard();

	return retVal;
#endif
#if APL
	ScrapRef	scrap;
	if (::GetCurrentScrap(&scrap) != noErr)
		return false;

	SInt32		byteCount = 0;
	OSStatus	status = ::GetScrapFlavorSize(scrap, kScrapFlavorTypeText, &byteCount);
	if (status != noErr)
		return false;

	outText.resize(byteCount);

	return (::GetScrapFlavorData(scrap, kScrapFlavorTypeText, &byteCount, &*outText.begin() ) == noErr);
#endif
}




bool	gizmo_SetTextToClipboard(const std::string& inText)
{
#if IBM
	LPTSTR  lptstrCopy;
	HGLOBAL hglbCopy;
	static XPLMDataRef hwndDataRef = XPLMFindDataRef("sim/operation/windows/system_window");
	HWND hwndMain = (HWND) XPLMGetDatai(hwndDataRef);

	if (!OpenClipboard(hwndMain))
		return false;
	EmptyClipboard();

	hglbCopy = GlobalAlloc(GMEM_MOVEABLE, sizeof(TCHAR) * (inText.length() + 1));
	if (hglbCopy == NULL)
	{
		CloseClipboard();
		return false;
	}

	lptstrCopy = (LPSTR)GlobalLock(hglbCopy);
	strcpy(lptstrCopy, inText.c_str());
	GlobalUnlock(hglbCopy);

	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();
	return true;
#endif
#if APL
	ScrapRef	scrap;
	if (::ClearCurrentScrap() != noErr) return false;
	if (::GetCurrentScrap(&scrap) != noErr) return false;

	return ::PutScrapFlavor( scrap, kScrapFlavorTypeText, kScrapFlavorMaskNone, inText.size(), &*inText.begin()) == noErr;

#endif
}

int	gizmo_Callback_gui_WidgetHandler_CutCopyPaste(
									XPWidgetMessage      inMessage,
									XPWidgetID           inWidget,
									long                 inParam1,
									long                 inParam2)
{
	if (inMessage == xpMsg_KeyPress)
	{
		char			theChar = KEY_VKEY(inParam1);
		XPLMKeyFlags	flags = KEY_FLAGS(inParam1);

		if ((flags & (xplm_DownFlag + xplm_ControlFlag)) == (xplm_DownFlag + xplm_ControlFlag))
		{
			long	selStart = XPGetWidgetProperty(inWidget, xpProperty_EditFieldSelStart, NULL);
			long	selEnd = XPGetWidgetProperty(inWidget, xpProperty_EditFieldSelEnd, NULL);
			long	strLen = XPGetWidgetDescriptor(inWidget, NULL, 0);
			std::string	txt;
			txt.resize(strLen);
			XPGetWidgetDescriptor(inWidget, &*txt.begin(), txt.size()+1);
			if (theChar == XPLM_VK_V)
			{
				std::string	scrap;
				if (gizmo_GetTextFromClipboard(scrap) && !scrap.empty())
				{
					if ((selEnd > selStart) && (selStart >= 0) && (selEnd <= strLen))
					{
						txt.replace(selStart, selEnd - selStart, scrap);
						XPSetWidgetDescriptor(inWidget, txt.c_str());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelStart, selStart + scrap.size());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelEnd, selStart + scrap.size());
					} else if ((selStart >= 0) && (selStart <= strLen)) {
						txt.insert(selStart, scrap);
						XPSetWidgetDescriptor(inWidget, txt.c_str());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelStart, selStart + scrap.size());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelEnd, selStart + scrap.size());
					}
				}
				return 1;
			}//end check for paste
			else if ((theChar == XPLM_VK_C) || (theChar == XPLM_VK_X))
			{
				if ((selStart >= 0) && (selStart < selEnd) && (selEnd <= strLen))
				{
					std::string	scrap = txt.substr(selStart, selEnd - selStart);
					if (gizmo_SetTextToClipboard(scrap) && (theChar == XPLM_VK_X))
					{
						txt.erase(selStart, selEnd - selStart);
						XPSetWidgetDescriptor(inWidget, txt.c_str());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelEnd, selStart);
					}
				}
				return 1;
			}//end check for copy or cut

		}//end check for control key held down




		// ------ Scriptable Key Sniffer Hook below here ----------





		/*
		xplm_ShiftFlag		1	The shift key is down
		xplm_OptionAltFlag	2	The option or alt key is down
		xplm_ControlFlag 	4	The control key is down*
		xplm_DownFlag		8	The key is being pressed down
		xplm_UpFlag			16	The key is being released
		*/


		int iFlagShift	= 0;
		if( flags & xplm_ShiftFlag ){ iFlagShift = 1; }

		int iFlagAlt	= 0;
		if( flags & xplm_OptionAltFlag ){ iFlagAlt = 1; }

		int iFlagCtrl	= 0;
		if( flags & xplm_ControlFlag ){ iFlagCtrl = 1; }

		int iFlagDown	= 0;
		if( flags & xplm_DownFlag ){ iFlagDown = 1; }

		int iFlagUp		= 0;
		if( flags & xplm_UpFlag ){ iFlagUp = 1; }





		int iWidgetPoolId = gizmo_FindWidgetPoolIdForXPWidget(inWidget);

		if( iWidgetPoolId > -1 ){
			std::string sTargetFunction = gizmo_Pool_BaseWrappers[ iWidgetPoolId ]->ScriptHandler;
			if( iFlagDown ){
					sTargetFunction += "_OnKeyDown";
			}else if( iFlagUp ){
					sTargetFunction += "_OnKeyUp";
			}else{
					sTargetFunction += "_OnKeyHold"; //TODO: Need an additional event, OnKeyPress, which fires for Up's and Holds.
													 //			This requires a small function queue here which is bleh.
			}


			//std::string sDbg = "Gizmo: WDG Call Check: " + sTargetFunction + "\n";
			//XPLMDebugString( sDbg.c_str() );

			if( sTargetFunction != gizmo_Pool_BaseWrappers[ iWidgetPoolId ]->ScriptHandler ){

				//attempt to fire a lua callback
				// push functions and arguments
				int ret = 0;
				lua_getglobal(gizmo_luaState, sTargetFunction.c_str() );  // function to be called

				char theRawChar = theChar;

				if( ! iFlagShift ){
					//If the user is not pressing shift we force the input to lower case.
					if (theChar >= 'A' && theChar <= 'Z') {
						theChar = theChar + 'a' - 'A';
					}
				}

				char caConverter[4];
				sprintf(caConverter, "%c", theChar);
				lua_pushstring(gizmo_luaState, caConverter);
				lua_pushinteger(gizmo_luaState, theRawChar);

				lua_pushboolean(gizmo_luaState, iFlagShift);
				lua_pushboolean(gizmo_luaState, iFlagAlt);
				lua_pushboolean(gizmo_luaState, iFlagCtrl);

				#if 0
					{
					char caDbg[1024];
					sprintf(caDbg, "Gizmo: Wid Key Sniffer: char:%c keyCode:%i shift:%i alt:%i ctrl:%i dwn:%i up:%i\n",
								theChar,
								theRawChar,
								iFlagShift,
								iFlagAlt,
								iFlagCtrl,
								iFlagDown,
								iFlagUp
						);
					XPLMDebugString( caDbg );
					}
				#endif



				// do the call (4 arguments, 1 result)
				if( lua_pcall(gizmo_luaState, 5, 1, 0) != 0){  //key press hook - errors handled
					//Error messages for failed key press hooks: suppressed.

					lua_pop(gizmo_luaState,1); //pop error message off stack - widget key press handler

				}else{
					ret = (int)lua_tointeger( gizmo_luaState, -1 );
					lua_pop(gizmo_luaState,1);
				}


				return ret;

			}//check to ensure we have a valid event to call, ensures Fully-Qual'd event name.

		}//couldnt find it in the widget pool, wtf.

	}//end check for key press event
	return 0;
}



int	gizmo_Callback_gui_WidgetHandler_PasswordPaste(
									XPWidgetMessage      inMessage,
									XPWidgetID           inWidget,
									long                 inParam1,
									long                 inParam2)
{
	if (inMessage == xpMsg_KeyPress)
	{
		char			theChar = KEY_VKEY(inParam1);
		XPLMKeyFlags	flags = KEY_FLAGS(inParam1);

		if ((flags & (xplm_DownFlag + xplm_ControlFlag)) == (xplm_DownFlag + xplm_ControlFlag))
		{
			long	selStart = XPGetWidgetProperty(inWidget, xpProperty_EditFieldSelStart, NULL);
			long	selEnd = XPGetWidgetProperty(inWidget, xpProperty_EditFieldSelEnd, NULL);
			long	strLen = XPGetWidgetDescriptor(inWidget, NULL, 0);
			std::string	txt;
			txt.resize(strLen);
			XPGetWidgetDescriptor(inWidget, &*txt.begin(), txt.size()+1);
			if (theChar == XPLM_VK_V)
			{
				std::string	scrap;
				if (gizmo_GetTextFromClipboard(scrap) && !scrap.empty())
				{
					if ((selEnd > selStart) && (selStart >= 0) && (selEnd <= strLen))
					{
						txt.replace(selStart, selEnd - selStart, scrap);
						XPSetWidgetDescriptor(inWidget, txt.c_str());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelStart, selStart + scrap.size());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelEnd, selStart + scrap.size());
					} else if ((selStart >= 0) && (selStart <= strLen)) {
						txt.insert(selStart, scrap);
						XPSetWidgetDescriptor(inWidget, txt.c_str());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelStart, selStart + scrap.size());
						XPSetWidgetProperty(inWidget, xpProperty_EditFieldSelEnd, selStart + scrap.size());
					}
				}
				return 1;
			}
		}
	}
	return 0;
}




int gizmo_FindWidgetPoolIdForXPWidget( XPWidgetID inWidget ){
		for( unsigned int iBaseX=0; iBaseX < gizmo_Pool_BaseWrappers.size(); iBaseX++ ){

			if( gizmo_Pool_BaseWrappers[ iBaseX ]->WidgetID == inWidget ){

				return iBaseX;

			}//end located our widget

		}//end: loop all widgets

	return -1;
}





int gizmo_Callback_gui_WidgetHandler(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){




		int r=0; //assume we dont want this event

		for( unsigned int iBaseX=0; iBaseX < gizmo_Pool_BaseWrappers.size(); iBaseX++ ){

			if( gizmo_Pool_BaseWrappers[ iBaseX ]->WidgetID == inWidget ){

				if( inMessage == xpMsg_PushButtonPressed ){

					//teh button has been pressed!

								gizmo_RunLuaFunction(gizmo_luaState, new std::string( gizmo_Pool_BaseWrappers[ iBaseX ]->ScriptHandler + "_OnClick" ));

							r = 1;


				}//end: button pushed

			}//end located our widget

		}//end: loop all widgets


	return r;

}








int gizmo_Callback_gui_WindowHandler(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){




	int r = 0; // the return value, assume failure - returning 0 lets x-plane handle the event.



		for( unsigned int iBaseX=0; iBaseX < gizmo_Pool_WindowWrappers.size(); iBaseX++ ){

			if( gizmo_Pool_WindowWrappers[ iBaseX ]->WidgetID == inWidget ){

				if( inMessage == xpMessage_CloseButtonPushed ){

					//teh button has been pressed!

							int cancel_close = gizmo_RunLuaFunction(gizmo_luaState, new std::string(gizmo_Pool_WindowWrappers[ iBaseX ]->WindowIdentifier + "_OnBeforeClose") );

							//check to see if the script canceled closing us.
							if( cancel_close == 0 ){ //user does NOT want to cancel
								XPHideWidget( inWidget );
								r=1;
							}


				}//end: button pushed

			}//end: located our window

		}//end: loop all widgets


	return r;
}














int gizmo_Callback_gui_WidgetHandler_ErrorDialog(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){


		if( inMessage == xpMsg_PushButtonPressed ){

			gizmo_flag_LuaRestartCalled = true;
			return 1;

		}//end: button pushed


	return 0;

}

int gizmo_Callback_gui_WindowHandler_ErrorDialog(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){


	int r = 0; // the return value, assume failure - returning 0 lets x-plane handle the event.

		if( inMessage == xpMessage_CloseButtonPushed ){
			//XPHideWidget( inWidget );
			XPDestroyWidget( gizmo_gui_handle_ErrorDialog, 1 );
			gizmo_gui_handle_ErrorDialog = NULL;
			r=1;
		}


	return r;
}












int gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnDismiss(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){


		if( inMessage == xpMsg_PushButtonPressed ){

			if( XPLMGetElapsedTime() >= gizmo_Nagware_Timer ){

				XPDestroyWidget( gizmo_gui_handle_NagwareDialog, 1 );
				gizmo_gui_handle_NagwareDialog = NULL;

				gizmo_Nagware_RebootSupressor = 5; //suppress the dialog for 5 reboots - if no serial number.

				return 1;

			} //end check to see if nag timer has expired.

		}//end: button pushed


	return 0;

}

int gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnActivate(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){


		if( inMessage == xpMsg_PushButtonPressed ){

			char tmpUserSerial[128];
			memset( tmpUserSerial, 0, 128 );

			XPGetWidgetDescriptor( gizmo_gui_handle_NagwareDialog_txtSerialNumber, tmpUserSerial, 64 );


					char caTmpLua[1024];

					sprintf(caTmpLua, "gizmo.activate(\"%s\")\n", tmpUserSerial);
					if( luaL_dostring( gizmo_luaState, caTmpLua ) ){
						//handle standard lua error event as documented.
						XPLMDebugString("Gizmo: Error: Couldn't call gizmo.activate function.\n");
						XPLMSpeakString("Gizmo: Error: Couldn't call gizmo.activate function.");
						lua_pop(gizmo_luaState,1);
					}
					if( lua_isnil( gizmo_luaState, -1 ) ){
						//handle weird "nil" stack overflow where we get left with a nil on the stack by every call to these.
						lua_pop(gizmo_luaState,1);
					}


			return 1;

		}//end: button pushed


	return 0;

}




/*
 http://msdn.microsoft.com/en-us/library/bb762153(v=vs.85).aspx

 Example of ShellExecute
 //example of ShellExecute Function
 #include <shellapi.h>
 #pragma comment(lib,"shell32.lib")
 static HWND hBut;

 switch (message) {
	 case WM_CREATE:
		 hBut = CreateWindow(TEXT("button"),TEXT("button"),WS_VISIBLE|WS_CHILD,100,100,100,100,hWnd,(HMENU)1,NULL,NULL);
		 break;
	 case WM_COMMAND:
		switch(LOWORD(wParam)){
			case 1:
				LoadLibrary(TEXT("shell32.dll"));
				ShellExecute(hWnd,TEXT("open"), TEXT("http://www.rsdn.ru"), TEXT(""),NULL,SW_SHOWNORMAL);
				break;
		break;
		}
*/

int gizmo_Callback_gui_WidgetHandler_NagwareDialog_btnBuyNow(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){


		if( inMessage == xpMsg_PushButtonPressed ){

			//Jump to browser.


				char* caProductUrl = "http://www.store.x-plugins.com/servlet/the-13/Gizmo-" GIZMO_VERSION_MAJOR ".x-Serial-Number/Detail";


				// ------------- This is example code that shows how to pop a web browser. ------------

				#if IBM

					//  http://support.microsoft.com/kb/224816

					//LONG r =
					ShellExecute(NULL, "open", caProductUrl, NULL, NULL, SW_SHOWNORMAL);

				#elif APL
					char tmp[1024];
					sprintf(tmp, "open %s", caProductUrl);
					system(tmp);

				#elif LIN

					//see: http://portland.freedesktop.org/wiki/
					char tmp[1024];
					sprintf(tmp, "xdg-open %s", caProductUrl);
					system(tmp);

				#endif



			return 1;

		}//end: button pushed


	return 0;

}

int gizmo_Callback_gui_WindowHandler_NagwareDialog(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){


	int r = 0; // the return value, assume failure - returning 0 lets x-plane handle the event.

		if( inMessage == xpMessage_CloseButtonPushed ){
			//XPHideWidget( inWidget );

			/*
			if( XPLMGetElapsedTime() >= gizmo_Nagware_Timer ){

				XPDestroyWidget( gizmo_gui_handle_NagwareDialog, 1 );
				gizmo_gui_handle_NagwareDialog = NULL;

				gizmo_Nagware_RebootSupressor = 5; //suppress the dialog for 5 reboots - if no serial number.

				r=1;

			} //end check to see if nag timer has expired.
			 */
			
			
			XPDestroyWidget( gizmo_gui_handle_NagwareDialog, 1 );
			gizmo_gui_handle_NagwareDialog = NULL;
			
			//gizmo_Nagware_RebootSupressor = 5; //suppress the dialog for 5 reboots - if no serial number.
			
			r=1;
			

		}//end check to see if user wants to click the X button to close the gui


	return r;
}











int gizmo_Callback_gui_CustomWidgetHandler(
                   XPWidgetMessage      inMessage,
                   XPWidgetID           inWidget,
                   long                 inParam1,
                   long                 inParam2
                   ){


	if (XPUSelectIfNeeded(inMessage, inWidget, inParam1, inParam2, 1/*eat*/))	return 1;

	int l, t, r, b;
	//XPGetWidgetExposedGeometry(inWidget, &l, &t, &r, &b);
	XPGetWidgetGeometry(inWidget, &l, &t, &r, &b);

	//Adjust these so they fall inside the clipped boundary.
		l++;
		b++;




	char caDescriptor[1024];
	XPGetWidgetDescriptor(inWidget, caDescriptor, 1024);

	/*
	if(0){
		XPLMDebugString("Gizmo: Custom Widget CB: ( ");
		XPLMDebugString( caDescriptor );
		XPLMDebugString( " )\n" );
	}
	*/




	std::string sDescriptor = std::string(caDescriptor);
	std::string sTargetFunction = sDescriptor;

	int bIsDrawCall = 0;

	int ret = 0; // the return value, assume failure - returning 0 lets x-plane handle the event.
		switch( inMessage ){
			case xpMsg_None:
				{
				XPLMDebugString("Gizmo: Custom widget rx xpMsg_None - very strange. SDK Error.\n");
				}
				break;

			/*
			case xpMsg_Create:
				{
				//XPLMDebugString("Gizmo: Custom widget rx xpMsg_Create\n");
				sTargetFunction += std::string("_OnCreate");
				//ret=1;
				}
				break;

			case xpMsg_Destroy:
				{
				XPLMDebugString("Gizmo: Custom widget rx xpMsg_Destroy\n");
				}
				break;

			case xpMsg_Paint:
				{
				XPLMDebugString("Gizmo: Custom widget rx xpMsg_Paint\n");
				}
				break;
			*/

			case xpMsg_Draw:
				{
				sTargetFunction += std::string("_OnDraw");
				bIsDrawCall=1;
				}
				break;



			case xpMsg_KeyPress:
				{
				XPLMDebugString("Gizmo: Custom widget key press.\n");
				sTargetFunction += std::string("_OnKeyPress");
				}
				break;
			case xpMsg_KeyTakeFocus:
				{
				XPLMDebugString("Gizmo: Custom widget key focus.\n");
				//sTargetFunction += std::string("_OnKeyFocus");
				return 1; //we do not give a choice on this event, just accept it!!
				}
				break;
			case xpMsg_KeyLoseFocus:
				{
				XPLMDebugString("Gizmo: Custom widget key blur.\n");
				sTargetFunction += std::string("_OnKeyBlur");
				}
				break;


			//Use all phases of the mouse action rather than just click, the user is attempting to creat a custom widget, assume needs power.
			case xpMsg_MouseDown:
				{
				//XPLMDebugString("Gizmo: Custom widget rx xpMsg_MouseDown\n");
				sTargetFunction += std::string("_OnMouseDown");
				}
				break;


			case xpMsg_MouseDrag:
				{
				//XPLMDebugString("Gizmo: Custom widget rx xpMsg_MouseDrag\n");
				sTargetFunction += std::string("_OnMouseDrag");
				}
				break;


			case xpMsg_MouseUp:
				{
				//XPLMDebugString("Gizmo: Custom widget rx xpMsg_MouseUp\n");
				sTargetFunction += std::string("_OnMouseUp");
				}
				break;

			/*
			case xpMsg_Reshape:
				break;
			case xpMsg_ExposedChanged:
				break;

			case xpMsg_AcceptChild:
				break;
			case xpMsg_LoseChild:
				break;


			case xpMsg_AcceptParent:
				{
				XPLMDebugString("Gizmo: Custom widget rx xpMsg_AcceptParent\n");
				//ret = 1;
				}
				break;


			case xpMsg_Shown:
				break;

			case xpMsg_Hidden:
				break;
			*/

			default:
				if( 0 ){
					//Use this for intensive boring debug.
					char caDbg[1024];
					sprintf(caDbg, "Gizmo: Custom Widget: RX Unknown Message ID: %i\n", (int)inMessage);
					XPLMDebugString(caDbg);
				}
				break;


		}





		//If we were able to locate a valid Event to call Lua with we will have the name of it appened to sTargetFunction
		//Eg: Foo becomes Foo_OnMouseDown

		if( sTargetFunction != sDescriptor ){

			#if 0
				{
				char caDbg[1024];
				sprintf( caDbg, "Gizmo: Custom widget CB target function: %s\n", sTargetFunction.c_str());
				XPLMDebugString( caDbg );
				}
			#endif


			gizmo_gui_lastCustomWidgetLeft = l;
			gizmo_gui_lastCustomWidgetBottom = b;
			gizmo_gui_lastCustomWidgetWidth = r - l;
			gizmo_gui_lastCustomWidgetHeight = t - b;


			if( bIsDrawCall ){
				//automatically jump to the bottom left corner of the widget for OpenGL drawing.
				glPushMatrix();
				glTranslatef( l, b, 0 );
					if( ! gizmo_DisabledEventsMap[sTargetFunction] ){
						ret = gizmo_RunLuaFunction(gizmo_luaState, new std::string(sTargetFunction)); //Custom widget draw function
					}else{
						//this is an extra layer of deferal that lets us catch the fact that there is no custom widget handler yet
						//we can now draw a filler widget so the user sees -something-


						XPLMSetGraphicsState(
							 0, //(int)lnV_Fog,//1, //fog
							 0, //num tex units - custom widget - no draw function.
							 0, //(int)lnV_Lighting,//1, //lighting
							 0, //(int)lnV_AlphaTest,//1, //alpha test
							 0, //(int)lnV_AlphaBlend,//1, //alpha blend
							 0, //(int)lnV_DepthTest,//1, //depth test
							 0 //(int)lnV_DepthWrite//1  //depth write
							 ); //very important!


						glColor4f( 0.5,0.5,0.5, 1 );
						glLineWidth(3);
						glBegin( GL_LINE_LOOP );
							//clockwise winding..
							glVertex2f( 0, 0 ); //l,b
								glVertex2f( 0, gizmo_gui_lastCustomWidgetHeight ); //l,t
								glVertex2f( gizmo_gui_lastCustomWidgetWidth, gizmo_gui_lastCustomWidgetHeight ); //r,t
							glVertex2f( gizmo_gui_lastCustomWidgetWidth, 0 ); //r,b


							glVertex2f( 0, 0 ); //l,b
							glVertex2f( gizmo_gui_lastCustomWidgetWidth, gizmo_gui_lastCustomWidgetHeight ); //r,t


							glVertex2f( 0, gizmo_gui_lastCustomWidgetHeight ); //l,t
							glVertex2f( gizmo_gui_lastCustomWidgetWidth, 0 ); //r,b


						glEnd();
						glLineWidth(1);


					}

				glPopMatrix();

			}else{

				ret = gizmo_RunLuaFunction(gizmo_luaState, new std::string(sTargetFunction)); //Custom widget handler function - mouse, keys, etc.
			}


		}



	//XPLMDebugString("Gizmo: Custom widget CB returning..\n");
	return ret;
}


















// Custom dataref support functions.


int	gizmo_Callback_getDatai(void * inRefcon)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	//printf("getDataF! (%f)\n", xcdrp->getFloat());
	return xcdrp->getInt();
}

void	gizmo_Callback_setDatai(void * inRefcon, int inValue)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	xcdrp->setInt( inValue );

	//printf("setDataD!\n");
	//gOwnedData = inValue;
}




float	gizmo_Callback_getDataf(void * inRefcon)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	//printf("getDataF! (%f)\n", xcdrp->getFloat());
	return xcdrp->getFloat();
}

void	gizmo_Callback_setDataf(void * inRefcon, float inValue)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	xcdrp->setFloat( inValue );

	//printf("setDataD!\n");
	//gOwnedData = inValue;
}





double	gizmo_Callback_getDatad(void * inRefcon)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	//printf("getDataD! (%f)\n", xcdrp->getDouble());
	return xcdrp->getDouble();
}

void	gizmo_Callback_setDatad(void * inRefcon, double inValue)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	xcdrp->setDouble( inValue );

	//printf("setDataD!\n");
	//gOwnedData = inValue;
}



long gizmo_Callback_getDataVi(
                                   void *               inRefcon,
                                   int *                outValues,    /* Can be NULL */
                                   int                  inOffset,
                                   int                  inMax){

	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	if( outValues == NULL ){

		return xcdrp->getElementCount();

	}else{
		std::vector<int>* ivValues = xcdrp->getIntV( inOffset, inMax );

		int y = 0;
		for( int x=inOffset; x<inMax+inOffset; x++ ){
				outValues[y] = (*ivValues)[x];
				y++;
		}

		delete ivValues;

	}


	return xcdrp->getElementCount();

}



long gizmo_Callback_getDataVf(
                                   void *               inRefcon,
                                   float *                outValues,    /* Can be NULL */
                                   int                  inOffset,
                                   int                  inMax){

	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	if( outValues == NULL ){

		//printf("gizmo_getDataVfCallback size query: %s: %i\n", xcdrp->getName().c_str(), xcdrp->getElementCount() );
		return xcdrp->getElementCount();

	}else{
		std::vector<float>* fvValues = xcdrp->getFloatV(inOffset, inMax);

		int y = 0;
		for( int x=inOffset; x<inMax+inOffset; x++ ){
				outValues[y] = (*fvValues)[x];
				y++;
		}


		delete fvValues;

	}


	return xcdrp->getElementCount();

}





long gizmo_Callback_getDataVdCallback(
                                   void *               inRefcon,
                                   double *                outValues,    /* Can be NULL */
                                   int                  inOffset,
                                   int                  inMax){

	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	if( outValues == NULL ){

		return xcdrp->getElementCount();

	}else{
		std::vector<double> dvValues = xcdrp->getDoubleV();

		int y = 0;
		for( int x=inOffset; x<inMax+inOffset; x++ ){
				outValues[y] = dvValues[x];
				y++;
		}

	}


	return xcdrp->getElementCount();

}








// -- Byte Array Datarefs --
long gizmo_Callback_getDatab(
								void* inRefcon,
								void* outValues, /* Can be NULL */
								int inOffset,
								long inMax
								){


	//gitbug:39 - Copy N bytes into outValues

	if( outValues ){
		sprintf( (char*)outValues, "getDatab" );
	}

	return 9; // 8 char string + terminator.

}





void gizmo_Callback_setDatab(
								void* inRefcon,
								void* inValues, /* Can be NULL */
								int inOffset,
								long inCount
								){

	//gitbug:39 - Store incoming byte data.

}








// Custom --hooked-- dataref support functions.


int	gizmo_Callback_Hooked_getDatai(void * inRefcon)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;
	return gizmo_RunLuaFunction( gizmo_luaState, new std::string(xcdrp->getHookName() + "_OnRead") ); //hooked datarefs

}

void	gizmo_Callback_Hooked_setDatai(void * inRefcon, int inValue)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

		std::string sHookFunctionName = std::string(xcdrp->getHookName() + "_OnWrite");
		if( ! gizmo_DisabledEventsMap[sHookFunctionName] ){

			lua_getglobal(gizmo_luaState,  sHookFunctionName.c_str() ); //hooked datarefs
			lua_pushinteger(gizmo_luaState, inValue);


			// do the call (1 arguments, 1 result)
			if( lua_pcall(gizmo_luaState, 1, 1, 0) != 0 ){ //errors handled

				gizmo_DisabledEventsMap[sHookFunctionName] = 1; //this function sig will not be called again

				lua_pop(gizmo_luaState,1); //pop error message off stack -- hooked setDatad

			}//end check error calling func name
		}//end check dead map
}




float	gizmo_Callback_Hooked_getDataf(void * inRefcon)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;
	return gizmo_RunLuaFunction_ReturnAsFloat( gizmo_luaState, new std::string(xcdrp->getHookName() + "_OnRead") ); //hooked datarefs
}

void	gizmo_Callback_Hooked_setDataf(void * inRefcon, float inValue)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

		std::string sHookFunctionName = std::string(xcdrp->getHookName() + "_OnWrite");
		if( ! gizmo_DisabledEventsMap[sHookFunctionName] ){

			lua_getglobal(gizmo_luaState,  sHookFunctionName.c_str() ); //hooked datarefs
			lua_pushnumber(gizmo_luaState, inValue);


			// do the call (1 arguments, 1 result)
			if (lua_pcall(gizmo_luaState, 1, 1, 0) != 0){  //errors handled

				gizmo_DisabledEventsMap[sHookFunctionName] = 1; //this function sig will not be called again

				lua_pop(gizmo_luaState,1); //pop error message off stack -- hooked setDatad

			}//end check error calling func name
		}//end check dead map

}





double	gizmo_Callback_Hooked_getDatad(void * inRefcon)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;
	return gizmo_RunLuaFunction_ReturnAsDouble( gizmo_luaState, new std::string(xcdrp->getHookName() + "_OnRead") ); //hooked datarefs
}

void	gizmo_Callback_Hooked_setDatad(void * inRefcon, double inValue)
{
	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

		std::string sHookFunctionName = std::string(xcdrp->getHookName() + "_OnWrite");
		if( ! gizmo_DisabledEventsMap[sHookFunctionName] ){

			lua_getglobal(gizmo_luaState,  sHookFunctionName.c_str() ); //hooked datarefs
			lua_pushnumber(gizmo_luaState, inValue);


			// do the call (1 arguments, 1 result)
			if (lua_pcall(gizmo_luaState, 1, 1, 0) != 0){  //errors handled

				gizmo_DisabledEventsMap[sHookFunctionName] = 1; //this function sig will not be called again

				lua_pop(gizmo_luaState,1); //pop error message off stack -- hooked setDatad

			}//end check error calling func name
		}//end check dead map
}//end hook dataref-doubles









long gizmo_Callback_Hooked_getDataVi(
                                   void *               inRefcon,
                                   int *                outValues,    /* Can be NULL */
                                   int                  inOffset,
                                   int                  inMax){

	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	if( outValues == NULL ){

		return xcdrp->getElementCount();

	}else{
		std::vector<int>* ivValues = xcdrp->getIntV( inOffset, inMax );

		int y = 0;
		for( int x=inOffset; x<inMax+inOffset; x++ ){
				outValues[y] = (*ivValues)[x];
				y++;
		}


		delete ivValues;

	}


	return xcdrp->getElementCount();

}



long gizmo_Callback_Hooked_getDataVf(
                                   void *               inRefcon,
                                   float *                outValues,    /* Can be NULL */
                                   int                  inOffset,
                                   int                  inMax){

	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	if( outValues == NULL ){

		//printf("gizmo_getDataVfCallback size query: %s: %i\n", xcdrp->getName().c_str(), xcdrp->getElementCount() );
		return xcdrp->getElementCount();

	}else{
		std::vector<float>* fvValues = xcdrp->getFloatV(inOffset, inMax);

		int y = 0;
		for( int x=inOffset; x<inMax+inOffset; x++ ){
				outValues[y] = (*fvValues)[x];
				y++;
		}

		delete fvValues;

	}


	return xcdrp->getElementCount();

}





long gizmo_Callback_Hooked_getDataVdCallback(
                                   void *               inRefcon,
                                   double *                outValues,    /* Can be NULL */
                                   int                  inOffset,
                                   int                  inMax){

	XDataRefCustom *xcdrp = (XDataRefCustom *)inRefcon;

	if( outValues == NULL ){

		return xcdrp->getElementCount();

	}else{
		std::vector<double> dvValues = xcdrp->getDoubleV();

		int y = 0;
		for( int x=inOffset; x<inMax+inOffset; x++ ){
				outValues[y] = dvValues[x];
				y++;
		}

	}


	return xcdrp->getElementCount();

}




// -- Byte Array Datarefs --
long gizmo_Callback_Hooked_getDatab(
								void* inRefcon,
								void* outValues, /* Can be NULL */
								int inOffset,
								long inMax
								){


	//gitbug:39 - CALL LUA (Copy N bytes into outValues)


	if( outValues ){
		sprintf( (char*)outValues, "getDatab" );
	}

	return 9; // 8 char string + terminator.

}



void gizmo_Callback_Hooked_setDatab(
								void* inRefcon,
								void* inValues, /* Can be NULL */
								int inOffset,
								long inCount
								){

	//gitbug:39 - CALL LUA (Store incoming byte data.)

}








/**
Calls X-Plane SDK function to register dataref appropriately.
*/
void gizmo_NewCustomRef( XDataRefCustom *h, int hooked=0 ){

	if( hooked ){
		//Toms Function - 2010-11-16

		h->dataRefHandle =
			XPLMRegisterDataAccessor(
				h->getName().c_str(),
				h->getType(),
				1,
				gizmo_Callback_Hooked_getDatai, gizmo_Callback_Hooked_setDatai,
				gizmo_Callback_Hooked_getDataf, gizmo_Callback_Hooked_setDataf,
				gizmo_Callback_Hooked_getDatad, gizmo_Callback_Hooked_setDatad,
				gizmo_Callback_Hooked_getDataVi, NULL, //We do not support creating custom datarefs that are vectors of data. Leave these as NULL until we do.
				gizmo_Callback_Hooked_getDataVf, NULL, //We do not support creating custom datarefs that are vectors of data. Leave these as NULL until we do.
				gizmo_Callback_Hooked_getDatab, gizmo_Callback_Hooked_setDatab,
				(void *)h, (void *)h);

	}else{

		h->dataRefHandle =
			XPLMRegisterDataAccessor(
				h->getName().c_str(),
				h->getType(),
				1,
				gizmo_Callback_getDatai, gizmo_Callback_setDatai,
				gizmo_Callback_getDataf, gizmo_Callback_setDataf,
				gizmo_Callback_getDatad, gizmo_Callback_setDatad,
				gizmo_Callback_getDataVi, NULL, //We do not support creating custom datarefs that are vectors of data. Leave these as NULL until we do.
				gizmo_Callback_getDataVf, NULL, //We do not support creating custom datarefs that are vectors of data. Leave these as NULL until we do.
				gizmo_Callback_getDatab, gizmo_Callback_setDatab,
				(void *)h, (void *)h);

	}//register dref, decide hooked or not.


	//Dataref Editor Calls should be made _AFTER_ the dataref has been fully registered above.
	//----- publish datarefs for inspector -------
	 XPLMPluginID PluginID = XPLMFindPluginBySignature("xplanesdk.examples.DataRefEditor");
	 if (PluginID != XPLM_NO_PLUGIN_ID){
		long MSG_ADD_DATAREF = 0x01000000;
		XPLMSendMessageToPlugin(PluginID, MSG_ADD_DATAREF, (void*)h->getName().c_str() );
		//printf("Gizmo: Registering new dataref with Dref editor:(%s)\n", h->getName().c_str() );
	 }
	 //------- end publish -------

}







/**
Destroy all known custom datarefs.
*/
void gizmo_CustomRefsDestroy(){

	//printf( "Custom Dataref Vector.size = %i\n", (int)gizmo_customDataRefs.size() );

	for( int x=0; x< (int)gizmo_customDataRefs.size(); x++ ){
		XDataRefCustom *h = gizmo_customDataRefs[x];
		if( h->iCustomMode ){
			XPLMUnregisterDataAccessor( h->dataRefHandle );
			//printf( "Destroyed: %s\n", h->getName().c_str() );
		}
		delete h;
	}//end custom dataref vector loop

	gizmo_customDataRefs.resize(0);

}//end gizmo_CustomRefsDestroy






/**
Set all of X-Planes override datarefs to 0, releasing control of all possible items.
This is crucial when a user loads an aircraft that is NOT Gizmo enabled immediately after flying a Gizmo aircraft that used an override.
Gizmo will handle the release automatically, the artist cant get it wrong.
*/
void gizmo_ReleaseOverrides(){

	//This is NOT a drm function.

	//This is a scripter convenience babysitting function.

	//By automatically releasing all overrides the scripter doesn't have to worry about when the user decides to load a different aircraft file.


	XPLMDebugString("\t Releasing all X-Plane overrides.\n");


	XPLMSetDatai(gizmo_xpdr_Override_Joystick,0);
	XPLMSetDatai(gizmo_xpdr_Override_ArtStab,0);
	XPLMSetDatai(gizmo_xpdr_Override_FlightControls,0);
	XPLMSetDatai(gizmo_xpdr_Override_GearAndBrakes,0);


	//reset flag
	int flags[] = {
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0,
					0,0,0,0,0
					};

	XPLMSetDatavi(gizmo_xpdr_Override_AI_PlanePath,flags,0,20);
	XPLMSetDatavi(gizmo_xpdr_Override_AI_AutoPilot,flags,0,20);


	XPLMSetDatai(gizmo_xpdr_Override_Nav_Needles,0);
	XPLMSetDatai(gizmo_xpdr_Override_Nav1_Needles,0);
	XPLMSetDatai(gizmo_xpdr_Override_Nav2_Needles,0);
	XPLMSetDatai(gizmo_xpdr_Override_ADF,0);
	XPLMSetDatai(gizmo_xpdr_Override_DME,0);
	XPLMSetDatai(gizmo_xpdr_Override_GPS,0);
	XPLMSetDatai(gizmo_xpdr_Override_Nav_Heading,0);
	XPLMSetDatai(gizmo_xpdr_Override_FlightDirector,0);
	XPLMSetDatai(gizmo_xpdr_Override_FlightDirector_Pitch,0);
	XPLMSetDatai(gizmo_xpdr_Override_FlightDirector_Roll,0);

	XPLMSetDatai(gizmo_xpdr_Override_Annunciators,0);
	XPLMSetDatai(gizmo_xpdr_Override_AutoPilot,0);
	XPLMSetDatai(gizmo_xpdr_Override_PFC_AutoPilot_Lights,0);
	XPLMSetDatai(gizmo_xpdr_Override_Joystick_Heading,0);
	XPLMSetDatai(gizmo_xpdr_Override_Joystick_Pitch,0);
	XPLMSetDatai(gizmo_xpdr_Override_Joystick_Roll,0);
	XPLMSetDatai(gizmo_xpdr_Override_Throttles,0);
	XPLMSetDatai(gizmo_xpdr_Override_PropPitch,0);
	XPLMSetDatai(gizmo_xpdr_Override_Mixture,0);
	XPLMSetDatai(gizmo_xpdr_Override_GroundPlane,0);
	XPLMSetDatai(gizmo_xpdr_Override_Disable_CockpitObject,0);
	XPLMSetDatai(gizmo_xpdr_Override_Disable_FuselageObject,0);
	XPLMSetDatai(gizmo_xpdr_Override_FMS_Advance,0);
	XPLMSetDatai(gizmo_xpdr_Override_FuelFlow,0);
	XPLMSetDatai(gizmo_xpdr_Override_ITT_EGT,0);
	XPLMSetDatai(gizmo_xpdr_Override_Control_Surfaces,0);

}





/**
We let artists make drawing calls from anywhere.
There requests are placed in a magic queue.
This function contains that magic.
*/
void gizmo_QDrawObject( int iObjID, float fX, float fY, float fZ, float fP, float fR, float fH ){

	gizmo_DrawQueueItem* drawItem = new gizmo_DrawQueueItem();

		drawItem->ObjectID = iObjID;
		drawItem->x = fX;
		drawItem->y = fY;
		drawItem->z = fZ;
		drawItem->p = fP;
		drawItem->r = fR;
		drawItem->h = fH;

	gizmo_DrawQ.push_back( drawItem );

}






/**
Nail some hooks to the walls.
*/
void gizmo_RegisterCallbacks(){


	//register flight loop callback for main processing and lua script calls.
		XPLMRegisterFlightLoopCallback(
			gizmo_Callback_FlightLoop,	// Callback
			-1.0f,					// Interval
			NULL);

	//register callback to check on progress of curl downloads
		XPLMRegisterFlightLoopCallback(
			gizmo_Callback_FlightLoop_CurlThreads,	// Callback
			0.1f,					// Interval
			NULL);



	//this is purely example code
	//DO NOT REGISTER THIS FLCB by default
	//This FLCB is registered during API calls and relies on the refcon parameter to do complex multiplexed yuck.
	/*
		XPLMRegisterFlightLoopCallback(
			gizmo_Callback_FlightLoop_UserTimers,
			-1.0f,
			NULL);
	*/




		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_FirstScene,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_FirstScene,
								 	0, //wants before?
								 	0 //param pointer
								 );


		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Terrain,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Terrain,
								 	0, //wants before?
								 	0 //param pointer
								 );


		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Airports,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Airports,
								 	0, //wants before?
								 	0 //param pointer
								 );


		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Vectors,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Vectors,
								 	0, //wants before?
								 	0 //param pointer
								 );


		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Objects,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Objects,
								 	0, //wants before?
								 	0 //param pointer
								 );


		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Airplanes,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_Airplanes,
								 	0, //wants before?
								 	0 //param pointer
								 );


		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_LastScene,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_LastScene,
								 	0, //wants before?
								 	0 //param pointer
								 );



		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_LocalMap3D,
								 	1, //wants before?
								 	0 //param pointer
								 );
		XPLMRegisterDrawCallback(
								 	gizmo_Callback_Draw_3D_Objects,
								 	xplm_Phase_LocalMap3D,
								 	0, //wants before?
								 	0 //param pointer
								 );








	//-------- Register 2D Drawing modes

		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_FirstCockpit,
		 	 						1,						// wants before ?
		  							NULL
								 );
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_FirstCockpit,
		 	 						0,						// wants before ?
		  							NULL
								 );


/*
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Gauges,
		  							xplm_Phase_Panel,
		 	 						1,						// wants before ?
		  							NULL
								 );
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Gauges,
		  							xplm_Phase_Panel,
		 	 						0,						// wants before ?
		  							NULL
								 );
*/


		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Gauges,
		  							xplm_Phase_Gauges,
		 	 						1,						// wants before ?
		  							NULL
								 );
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Gauges,
		  							xplm_Phase_Gauges,
		 	 						0,						// wants before ?
		  							NULL
								 );




		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_Window,
		 	 						1,						// wants before ?
		  							NULL
								 );
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_Window,
		 	 						0,						// wants before ?
		  							NULL
								 );


		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_LastCockpit,
		 	 						1,						// wants before ?
		  							NULL
								 );
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_LastCockpit,
		 	 						0,						// wants before ?
		  							NULL
								 );


		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_LocalMap2D,
		 	 						1,						// wants before ?
		  							NULL
								 );
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_LocalMap2D,
		 	 						0,						// wants before ?
		  							NULL
								 );


		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_LocalMapProfile,
		 	 						1,						// wants before ?
		  							NULL
								 );
		XPLMRegisterDrawCallback(
		  							gizmo_Callback_Draw_2D_Windows,
		  							xplm_Phase_LocalMapProfile,
		 	 						0,						// wants before ?
		  							NULL
								 );



}//end register callbacks






/**
Runs a lot of XPLMFindDataref calls.
*/
void gizmo_FindDatarefs(){

    gizmo_xpdr_AcfAuthorString						= XPLMFindDataRef("sim/aircraft/view/acf_author" );
    gizmo_xpdr_AcfDescriptionString					= XPLMFindDataRef("sim/aircraft/view/acf_descrip" );

	gizmo_xpdr_CameraFOV							= XPLMFindDataRef("sim/graphics/view/field_of_view_deg");

	gizmo_xpdr_Paused								= XPLMFindDataRef("sim/time/paused"); //this is one of a few datarefs we pre-bind for convenience.
	gizmo_xpdr_FpsMultiplier						= XPLMFindDataRef("sim/operation/misc/frame_rate_period" );

	gizmo_xpdr_ViewType								= XPLMFindDataRef("sim/graphics/view/view_type");

	gizmo_xpdr_Local_X								= XPLMFindDataRef("sim/flightmodel/position/local_x" );
	gizmo_xpdr_Local_Y								= XPLMFindDataRef("sim/flightmodel/position/local_y" );
	gizmo_xpdr_Local_Z								= XPLMFindDataRef("sim/flightmodel/position/local_z" );


	gizmo_xpdr_Local_P								= XPLMFindDataRef("sim/flightmodel/position/theta" );
	gizmo_xpdr_Local_H								= XPLMFindDataRef("sim/flightmodel/position/psi" );
	gizmo_xpdr_Local_R								= XPLMFindDataRef("sim/flightmodel/position/phi" );
	gizmo_xpdr_Heading								= XPLMFindDataRef("sim/flightmodel/position/magpsi"); //--magnetic heading

	gizmo_xpdr_ScreenWidth							= XPLMFindDataRef("sim/graphics/view/window_width"); //int
	gizmo_xpdr_ScreenHeight							= XPLMFindDataRef("sim/graphics/view/window_height"); //int

	gizmo_xpdr_PanelAreaVisible_BottomOffset		= XPLMFindDataRef("sim/graphics/view/panel_total_pnl_b");
	gizmo_xpdr_PanelAreaVisible_LeftOffset			= XPLMFindDataRef("sim/graphics/view/panel_total_pnl_l");

	gizmo_xpdr_ViewX								= XPLMFindDataRef( "sim/graphics/view/view_x" );
	gizmo_xpdr_ViewY								= XPLMFindDataRef( "sim/graphics/view/view_y" );
	gizmo_xpdr_ViewZ								= XPLMFindDataRef( "sim/graphics/view/view_z" );

	gizmo_xpdr_AGL									= XPLMFindDataRef("sim/flightmodel/position/y_agl"); //--AGL - meters
	gizmo_xpdr_MSL									= XPLMFindDataRef("sim/flightmodel/position/elevation"); //--MSL - meters
	gizmo_xpdr_Alpha								= XPLMFindDataRef("sim/flightmodel/position/alpha");
	gizmo_xpdr_Beta									= XPLMFindDataRef("sim/flightmodel/position/beta");

	gizmo_xpdr_IAS									= XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed"); //--kias
	gizmo_xpdr_TAS									= XPLMFindDataRef("sim/flightmodel/position/true_airspeed"); //--mtr/sec
	gizmo_xpdr_GS									= XPLMFindDataRef("sim/flightmodel/position/groundspeed"); //--mtr/sec

	gizmo_xpdr_P									= XPLMFindDataRef("sim/flightmodel/position/P"); //--roll
	gizmo_xpdr_Q									= XPLMFindDataRef("sim/flightmodel/position/Q"); //--pitch
	gizmo_xpdr_R									= XPLMFindDataRef("sim/flightmodel/position/R"); //--yaw

	gizmo_xpdr_P_dot								= XPLMFindDataRef("sim/flightmodel/position/P_dot");
	gizmo_xpdr_Q_dot								= XPLMFindDataRef("sim/flightmodel/position/Q_dot");
	gizmo_xpdr_R_dot								= XPLMFindDataRef("sim/flightmodel/position/R_dot");

	gizmo_xpdr_VS									= XPLMFindDataRef("sim/flightmodel/position/vh_ind"); //--vertical speed, meters per second

	gizmo_xpdr_OnGroundAny							= XPLMFindDataRef("sim/flightmodel/failures/onground_any"); //--int bool

	gizmo_xpdr_GearDeployState						= XPLMFindDataRef("sim/aircraft/parts/acf_gear_deploy"); //--float[10]
	gizmo_xpdr_GearHandleState						= XPLMFindDataRef("sim/cockpit/switches/gear_handle_status"); //--int boolean

	gizmo_xpdr_Lat									= XPLMFindDataRef("sim/flightmodel/position/latitude");
	gizmo_xpdr_Lon									= XPLMFindDataRef("sim/flightmodel/position/longitude");
	gizmo_xpdr_Track								= XPLMFindDataRef("sim/flightmodel/position/hpath"); //--actual path over ground

	gizmo_xpdr_YokePitch							= XPLMFindDataRef("sim/joystick/yoke_pitch_ratio");
	gizmo_xpdr_YokeRoll								= XPLMFindDataRef("sim/joystick/yoke_roll_ratio");
	gizmo_xpdr_YokeHeading							= XPLMFindDataRef("sim/joystick/yoke_heading_ratio");

	gizmo_xpdr_AStabPitch							= XPLMFindDataRef("sim/joystick/artstab_pitch_ratio");
	gizmo_xpdr_AStabRoll							= XPLMFindDataRef("sim/joystick/artstab_roll_ratio");
	gizmo_xpdr_AStabHeading							= XPLMFindDataRef("sim/joystick/artstab_heading_ratio");

	gizmo_xpdr_FCPitch								= XPLMFindDataRef("sim/joystick/FC_ptch");
	gizmo_xpdr_FCRoll								= XPLMFindDataRef("sim/joystick/FC_roll");
	gizmo_xpdr_FCHeading							= XPLMFindDataRef("sim/joystick/FC_hdng");


	//these do not have Lua accesor functions just yet
	gizmo_xpdr_Throttle								= XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro" ); //[8] element array
	gizmo_xpdr_ThrottleUse							= XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use" ); //[8] element array
	gizmo_xpdr_AfterBurnerOn						= XPLMFindDataRef("sim/flightmodel/engine/ENGN_burning"); //8 elements
	gizmo_xpdr_MachNumber							= XPLMFindDataRef("sim/flightmodel/misc/machno");
	gizmo_xpdr_ThrustMax							= XPLMFindDataRef("sim/aircraft/engine/acf_tmax");
	gizmo_xpdr_ThrustAtPoint						= XPLMFindDataRef("sim/flightmodel/engine/POINT_thrust");


	gizmo_xpdr_EngineMixture						= XPLMFindDataRef("sim/flightmodel/engine/ENGN_mixt"); //float 8


	//debug path
	gizmo_xpdr_ShowDebugFlightPath					= XPLMFindDataRef("sim/cockpit/misc/show_path");// --int, 0 through 8, repeating, 4 basic modes.



	gizmo_xpdr_Override_Joystick						= XPLMFindDataRef("sim/operation/override/override_joystick");
	gizmo_xpdr_Override_ArtStab							= XPLMFindDataRef("sim/operation/override/override_artstab");
	gizmo_xpdr_Override_FlightControls					= XPLMFindDataRef("sim/operation/override/override_flightcontrol");
	gizmo_xpdr_Override_GearAndBrakes					= XPLMFindDataRef("sim/operation/override/override_gearbrake");
	gizmo_xpdr_Override_AI_PlanePath					= XPLMFindDataRef("sim/operation/override/override_planepath"); //arr 20
	gizmo_xpdr_Override_AI_AutoPilot					= XPLMFindDataRef("sim/operation/override/override_plane_ai_autopilot"); //arr 20
	gizmo_xpdr_Override_Nav_Needles						= XPLMFindDataRef("sim/operation/override/override_navneedles");
	gizmo_xpdr_Override_Nav1_Needles					= XPLMFindDataRef("sim/operation/override/override_nav1_needles");
	gizmo_xpdr_Override_Nav2_Needles					= XPLMFindDataRef("sim/operation/override/override_nav2_needles");
	gizmo_xpdr_Override_ADF								= XPLMFindDataRef("sim/operation/override/override_adf");
	gizmo_xpdr_Override_DME								= XPLMFindDataRef("sim/operation/override/override_dme");
	gizmo_xpdr_Override_GPS								= XPLMFindDataRef("sim/operation/override/override_gps");
	gizmo_xpdr_Override_Nav_Heading						= XPLMFindDataRef("sim/operation/override/override_nav_heading");
	gizmo_xpdr_Override_FlightDirector					= XPLMFindDataRef("sim/operation/override/override_flightdir");
	gizmo_xpdr_Override_FlightDirector_Pitch			= XPLMFindDataRef("sim/operation/override/override_flightdir_ptch");
	gizmo_xpdr_Override_FlightDirector_Roll				= XPLMFindDataRef("sim/operation/override/override_flightdir_roll");
	//gizmo_xpdr_Override_Camera							= XPLMFindDataRef("sim/operation/override/override_camera");
	gizmo_xpdr_Override_Annunciators					= XPLMFindDataRef("sim/operation/override/override_annunciators");
	gizmo_xpdr_Override_AutoPilot						= XPLMFindDataRef("sim/operation/override/override_autopilot");
	gizmo_xpdr_Override_PFC_AutoPilot_Lights			= XPLMFindDataRef("sim/operation/override/override_pfc_autopilot_lites");
	gizmo_xpdr_Override_Joystick_Heading				= XPLMFindDataRef("sim/operation/override/override_joystick_heading");
	gizmo_xpdr_Override_Joystick_Pitch					= XPLMFindDataRef("sim/operation/override/override_joystick_pitch");
	gizmo_xpdr_Override_Joystick_Roll					= XPLMFindDataRef("sim/operation/override/override_joystick_roll");
	gizmo_xpdr_Override_Throttles						= XPLMFindDataRef("sim/operation/override/override_throttles");
	gizmo_xpdr_Override_PropPitch						= XPLMFindDataRef("sim/operation/override/override_prop_pitch");
	gizmo_xpdr_Override_Mixture							= XPLMFindDataRef("sim/operation/override/override_mixture");
	gizmo_xpdr_Override_GroundPlane						= XPLMFindDataRef("sim/operation/override/override_groundplane");
	gizmo_xpdr_Override_Disable_CockpitObject			= XPLMFindDataRef("sim/operation/override/disable_cockpit_object");
	gizmo_xpdr_Override_Disable_FuselageObject			= XPLMFindDataRef("sim/operation/override/disable_twosided_fuselage");
	gizmo_xpdr_Override_FMS_Advance						= XPLMFindDataRef("sim/operation/override/override_fms_advance");
	gizmo_xpdr_Override_FuelFlow						= XPLMFindDataRef("sim/operation/override/override_fuel_flow");
	gizmo_xpdr_Override_ITT_EGT							= XPLMFindDataRef("sim/operation/override/override_itt_egt");
	gizmo_xpdr_Override_Control_Surfaces				= XPLMFindDataRef("sim/operation/override/override_control_surfaces");


}//end gizmo_FindDatarefs()





/**
Handle built in plugin menu clicks, everything under: Plugins > Gizmo > ...
*/
void gizmo_Callback_Menu_SystemClick(
                                   void *               inMenuRef,
                                   void *               inItemRef){


	char *inMenuString = (char *)inItemRef;

	//printf("Gizmo: System Menu clicked: (%s)\n", inMenuString);

	if( strcmp(inMenuString, "mnu_restart") == 0 ){
		gizmo_flag_LuaRestartCalled = true;
		return;
	}


	if( strcmp(inMenuString, "mnu_reset_firmware") == 0 ){
		gizmo_LoadFirmware();
		return;
	}


	if( strcmp(inMenuString, "mnu_lua_stack_dump") == 0 ){

		gizmo_LuaStackDump( gizmo_luaState );
		gizmo_LuaStackDump_SizeOnly( gizmo_luaState );

		return;
	}
	
	
	if( strcmp( inMenuString, "mnu_register_gizmo") == 0 ){
		
		gizmo_Nagware_Armed = 1;
		gizmo_gui_Nagware();
		
		return;
	}

}




/**
Handle clicks of Plugins > Gizmo > Scripts > menu_item
*/
void gizmo_Callback_Menu_VehicleClick(
                                   void *               inMenuRef,
                                   void *               inItemRef){

	//char *inMenuString = (char *)inItemRef;
	std::string* sScriptHandler = (std::string*)inItemRef;

	//std::string *sScriptFunctionName = new std::string( std::string(inMenuString) + "_OnClick" );
	//gizmo_Pool_ScriptCallbacks.push_back( sScriptFunctionName );
	
	gizmo_Pool_ScriptCallbacks.push_back( new std::string( *sScriptHandler + "_OnClick" ) );

}




/**
Handle clicks in the Plugins > Gizmo > Windows > items   menus.
*/
void gizmo_Callback_Menu_WindowsClick(
                                   void *               inMenuRef,
                                   void *               inItemRef){

	char *inMenuString = (char *)inItemRef;

	//Search the pool of Gizmo managed GUI Windows for the correct one to display
	for( unsigned int iMenuX=0; iMenuX < gizmo_Pool_WindowWrappers.size(); iMenuX++ ){
		if( gizmo_Pool_WindowWrappers[iMenuX]->WindowIdentifier == inMenuString ){

			//show this window - we have uncovered the correct name
			gizmo_Pool_WindowWrappers[iMenuX]->Show();

		}
	}

}






/**
Crude stack dump function copied from online examples.
*/
static void gizmo_LuaStackDump (lua_State *L) {
	  int i=lua_gettop(L);
	  XPLMDebugString("----------------  Stack Dump ----------------\n" );

	  char caDbg[1024];

	  while(  i   ) {
		int t = lua_type(L, i);
		switch (t) {
		  case LUA_TSTRING:
			sprintf(caDbg, "\t%02d: string: '%s'\n", i, lua_tostring(L, i));
			XPLMDebugString( caDbg );
		  break;
		  case LUA_TBOOLEAN:
			sprintf(caDbg,"\t%02d: boolean: %s\n",i,lua_toboolean(L, i) ? "true" : "false");
			XPLMDebugString( caDbg );
		  break;
		  case LUA_TNUMBER:
			sprintf(caDbg,"\t%02d:  number: %g\n",  i, lua_tonumber(L, i));
			XPLMDebugString( caDbg );
		 break;
		 default:
			sprintf(caDbg,"\t%02d:   other: %s\n", i, lua_typename(L, t));
			XPLMDebugString( caDbg );
			break;
		}
	   i--;
	  }
	 XPLMDebugString("--------------- Stack Dump Finished ----------\n" );
}



/**
Sometimes it's useful just to know how big the stack is to watch for a leak.
*/
static void gizmo_LuaStackDump_SizeOnly (lua_State *L) {
	  int i=lua_gettop(L);

	  char caDbg[1024];
	  sprintf(caDbg, "Gizmo: Lua stack size: %i\n", i);
		XPLMDebugString( caDbg );

}











/**
This function is responsible for interfacing with the Lua engine and requesting function and event calls.

It will bubble up any int value returned from a Lua function it calls. If no value is available from Lua we return a default of 0.

If the function could not be found inside the Lua instance an error report will be collected/reported and the function signature
will be marked as disabled in a global std::map<std::string> table, this allows caching of functions that don't exist in Lua and optimal
number of calls to the Lua engine over time relative to what the user is actually doing in script.
*/
int gizmo_RunLuaFunction( lua_State* L, std::string* sFunctionSig ){

	int ret = 0;

		if( ! gizmo_DisabledEventsMap[*sFunctionSig] ){

			lua_getglobal(L, sFunctionSig->c_str() );  // function to be called

				if( lua_pcall(L,0,1,0) ){ //gizmo_RunLuaFunction: errors handled

						gizmo_DisabledEventsMap[*sFunctionSig] = 1; //this function sig will not be called again

						#if 0
						//we almost never want to run this code.
						if( lua_isstring(L,-1) ) && (gizmo_verbose_debug) ){
							char caDbg[1024];
							sprintf(caDbg, "\nGizmo: RunLuaFunction Error: %s / %s\n", sFunctionSig->c_str(), lua_tostring(L,-1) );
							XPLMDebugString(caDbg);

							gizmo_gui_ScriptError( std::string(caDbg) );

							//gizmo_LuaStackDump( L );
						}
						#endif


					lua_pop(L,1); //pop error message off stack -- RunLuaFunction


				}else{
						//bubble-up the value that our Lua function returned, IF it returned anything.
						//if it didn't we will default to returning 0.
						ret = (int)lua_tointeger( L, -1 );
						lua_pop(gizmo_luaState,1);
				}


				//gizmo_LuaStackDump( gizmo_luaState );
				//gizmo_LuaStackDump_SizeOnly( gizmo_luaState );



		}//end event map check

	delete( sFunctionSig );
	return ret; //defaults to 0

} //end; gizmo_RunLuaFunction






float gizmo_RunLuaFunction_ReturnAsFloat( lua_State* L, std::string* sFunctionSig ){

	float ret = 0;

		if( ! gizmo_DisabledEventsMap[*sFunctionSig] ){

			lua_getglobal(L, sFunctionSig->c_str() );  // function to be called

				if( lua_pcall(L,0,1,0) ){  //errors handled

						gizmo_DisabledEventsMap[*sFunctionSig] = 1; //this function sig will not be called again

						if((lua_isstring(L,-1)) && (gizmo_verbose_debug)) {
							char caDbg[1024];
							sprintf(caDbg, "\nGizmo: RunLuaFunction_ReturnAsFloat Error: %s / %s\n", sFunctionSig->c_str(), lua_tostring(L,-1) );
							XPLMDebugString(caDbg);

							gizmo_gui_ScriptError( std::string(caDbg) );

							//gizmo_LuaStackDump( L );
						}


					lua_pop(L,1); //pop error message off stack -- RunLuaFunction_ReturnAsFloat


				}else{
						//bubble-up the value that our Lua function returned, IF it returned anything.
						//if it didn't we will default to returning 0.
						ret = (float)lua_tonumber( L, -1 );
						lua_pop(gizmo_luaState,1);
				}

		}//end event map check

	delete( sFunctionSig );
	return ret; //defaults to 0

} //end; gizmo_RunLuaFunction_ReturnAsFloat





double gizmo_RunLuaFunction_ReturnAsDouble( lua_State* L, std::string* sFunctionSig ){

	double ret = 0;

		if( ! gizmo_DisabledEventsMap[*sFunctionSig] ){

			lua_getglobal(L, sFunctionSig->c_str() );  // function to be called

				if( lua_pcall(L,0,1,0) ){ //gizmo_RunLuaFunction_ReturnAsDouble: errors handled

						gizmo_DisabledEventsMap[*sFunctionSig] = 1; //this function sig will not be called again

						if((lua_isstring(L,-1)) && (gizmo_verbose_debug)) {
							char caDbg[1024];
							sprintf(caDbg, "\nGizmo: RunLuaFunction_ReturnAsDouble Error: %s / %s\n", sFunctionSig->c_str(), lua_tostring(L,-1) );
							XPLMDebugString(caDbg);

							gizmo_gui_ScriptError( std::string(caDbg) );

							//gizmo_LuaStackDump( L );
						}


					lua_pop(L,1); //pop error message off stack -- RunLuaFunction_ReturnAsDouble


				}else{
					//bubble-up the value that our Lua function returned, IF it returned anything.
					//if it didn't we will default to returning 0.
					ret = (double)lua_tonumber( L, -1 );
					lua_pop(gizmo_luaState,1);
				}


		} //end event map check

	delete( sFunctionSig );
	return ret; //defaults to 0

} //end; gizmo_RunLuaFunction_ReturnAsDouble




/*
 
 //This function is difficult to add due to the threaded nature of the terrain baking code.


float gizmo_RunLuaFunction_TawsCrayon( lua_State* L, std::string* sFunctionSig ){
	
	float ret = 0;
	
	if( ! gizmo_DisabledEventsMap[*sFunctionSig] ){
		
		lua_getglobal(L, sFunctionSig->c_str() );  // function to be called
		
		if( lua_pcall(L,0,1,0) ){  //errors handled
			
			gizmo_DisabledEventsMap[*sFunctionSig] = 1; //this function sig will not be called again
			
			if((lua_isstring(L,-1)) && (gizmo_verbose_debug)) {
				char caDbg[1024];
				sprintf(caDbg, "\nGizmo: RunLuaFunction_ReturnAsFloat Error: %s / %s\n", sFunctionSig->c_str(), lua_tostring(L,-1) );
				XPLMDebugString(caDbg);
				
				gizmo_gui_ScriptError( std::string(caDbg) );
				
				//gizmo_LuaStackDump( L );
			}
			
			
			lua_pop(L,1); //pop error message off stack -- RunLuaFunction_ReturnAsFloat
			
			
		}else{
			//bubble-up the value that our Lua function returned, IF it returned anything.
			//if it didn't we will default to returning 0.
			ret = (float)lua_tonumber( L, -1 );
			lua_pop(gizmo_luaState,1);
		}
		
	}//end event map check
	
	delete( sFunctionSig );
	return ret; //defaults to 0
	
} //end; gizmo_RunLuaFunction_TawsCrayon


*/












/**
Cleanup all the OBJ8 handles and helper objects we loaded.
*/
void gizmo_CleanupXPlaneObjectHandles(){

	//printf("Gizmo: Releasing loaded objects: %i\n", (int)gizmo_XPlaneSceneryObjectHandles.size());
	char caDbg[512];
	sprintf(caDbg, "Gizmo: Releasing loaded objects in array, count: %i\n", (int)gizmo_XPlaneSceneryObjectHandles.size());
	XPLMDebugString( caDbg );

	for( int x=0; x< (int)gizmo_XPlaneSceneryObjectHandles.size()-1; x++ ){

		XPLMUnloadObject( gizmo_XPlaneSceneryObjectHandles[ x ] );

		char caDbg[512];
		sprintf(caDbg, "\tGizmo: Object released: %i\n", x);
		XPLMDebugString( caDbg );
	}
	gizmo_XPlaneSceneryObjectHandles.resize(0);

}





/**
Maps X-Plane key presses with modifier key stats into Lua event calls.
*/
int gizmo_Callback_KeySniffer_Raw(
                                   char                 inChar,
                                   XPLMKeyFlags         flags,
                                   char                 inVirtualKey,
                                   void *               inRefcon)
{



	/* First record the key data. */
	//gVirtualKey = inVirtualKey;
	//gFlags = inFlags;
	//gChar = inChar;


		char theChar = inVirtualKey;



		// ------ Scriptable Key Sniffer Hook below here ----------


		int ret = 0;



		/*
		xplm_ShiftFlag		1	The shift key is down
		xplm_OptionAltFlag	2	The option or alt key is down
		xplm_ControlFlag 	4	The control key is down*
		xplm_DownFlag		8	The key is being pressed down
		xplm_UpFlag			16	The key is being released
		*/


		int iFlagShift	= 0;
		if( flags & xplm_ShiftFlag ){ iFlagShift = 1; }

		int iFlagAlt	= 0;
		if( flags & xplm_OptionAltFlag ){ iFlagAlt = 1; }

		int iFlagCtrl	= 0;
		if( flags & xplm_ControlFlag ){ iFlagCtrl = 1; }

		int iFlagDown	= 0;
		if( flags & xplm_DownFlag ){ iFlagDown = 1; }

		int iFlagUp		= 0;
		if( flags & xplm_UpFlag ){ iFlagUp = 1; }





		std::string sBaseFunctionSig = "RawKeys";


		//int iWidgetPoolId = gizmo_FindWidgetPoolIdForXPWidget(inWidget);

			std::string sTargetFunction = sBaseFunctionSig;
			if( iFlagDown ){
					sTargetFunction += "_OnKeyDown";
			}else if( iFlagUp ){
					sTargetFunction += "_OnKeyUp";
			}else{
					sTargetFunction += "_OnKeyHold"; //TODO: Need an additional event, OnKeyPress, which fires for Up's and Holds.
													 //			This requires a small function queue here which is bleh. Non essential.
			}


			//std::string sDbg = "Gizmo: WDG Call Check: " + sTargetFunction + "\n";
			//XPLMDebugString( sDbg.c_str() );

			if( sTargetFunction != sBaseFunctionSig ){

				//attempt to fire a lua callback
				// push functions and arguments
				int ret = 0;
				lua_getglobal(gizmo_luaState, sTargetFunction.c_str() );  // function to be called

				char theRawChar = theChar;

				if( ! iFlagShift ){
					//If the user is not pressing shift we force the input to lower case.
					if (theChar >= 'A' && theChar <= 'Z') {
						theChar = theChar + 'a' - 'A';
					}
				}

				char caConverter[4];
				sprintf(caConverter, "%c", theChar);
				lua_pushstring(gizmo_luaState, caConverter);
				lua_pushinteger(gizmo_luaState, theRawChar);

				lua_pushboolean(gizmo_luaState, iFlagShift);
				lua_pushboolean(gizmo_luaState, iFlagAlt);
				lua_pushboolean(gizmo_luaState, iFlagCtrl);

				#if 0
					{
					char caDbg[1024];
					sprintf(caDbg, "Gizmo: Raw Key Sniffer: char:%c keyCode:%i shift:%i alt:%i ctrl:%i dwn:%i up:%i\n",
								theChar,
								theRawChar,
								iFlagShift,
								iFlagAlt,
								iFlagCtrl,
								iFlagDown,
								iFlagUp
						);
					XPLMDebugString( caDbg );
					}
				#endif



				// do the call (4 arguments, 1 result)
				if (lua_pcall(gizmo_luaState, 5, 1, 0) != 0){ //Raw Key Sniffer: failed key press hooks: suppressed.

					lua_pop(gizmo_luaState,1); //pop error message off stack -- Raw Key Sniffer

				}else{
					ret = (int)lua_tointeger( gizmo_luaState, -1 );
					lua_pop(gizmo_luaState,1);
				}

			}


			//we need to flip the result from Lua before we return it to x-plane.
			if( ret ){
				return 0; //lua script wanted to absorb this event
			}else{
				return 1; //lua script did NOT want this event, return it to x-plane
			}

	/* Return 1 to pass the keystroke to plugin windows and X-Plane.
	 * Returning 0 would consume the keystroke. */
	return 1;
}





/**
Creates our standard tree of ever persistent system menu items.
*/
void gizmo_Menu_CreateRoot(){


		gizmo_Menu_RootItem = XPLMCreateMenu(
                                "Gizmo",
                                NULL, //parent menu item - plugins menu
								1, //index value - auto assigned.
                                gizmo_Callback_Menu_SystemClick,
                                0
                                );




						//id:0
						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"Reboot Scripts..",
											(void *)"mnu_restart", //item ref
											1 //force english
											);

						//id:1
						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"---",
											(void *)"spacer", //item ref
											1 //force english
											);

						//id:2
						gizmo_Menu_VehicleItems = XPLMCreateMenu(
												"Vehicle Menu Items",
												gizmo_Menu_RootItem,
														XPLMAppendMenuItem(
															gizmo_Menu_RootItem,
															"Scripts",
															(void *)"mnu_root_vehicle",
															1
														  ),
												gizmo_Callback_Menu_VehicleClick,
												0
												);


						//disable the scripts menu item until theres something placed inside
						XPLMEnableMenuItem(gizmo_Menu_RootItem, 2, 0);



						//id:3
						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"---",
											(void *)"spacer", //item ref
											1 //force english
											);
						//id:4
						gizmo_Menu_Windows = XPLMCreateMenu(
												"Window Menu",
												gizmo_Menu_RootItem,
														XPLMAppendMenuItem(
															gizmo_Menu_RootItem,
															"Windows",
															(void *)"mnu_root_windows",
															1
														  ),
												gizmo_Callback_Menu_WindowsClick,
												0
												);

						//disable the windows menu item until theres something placed inside
						XPLMEnableMenuItem(gizmo_Menu_RootItem, 4, 0);




						//id: 5
						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"---",
											(void *)"spacer", //item ref
											1 //force english
											);

						//id:6
						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"Reset Firmware..",
											(void *)"mnu_reset_firmware", //item ref
											1 //force english
											);

						//id:7
						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"Lua Stack Dump..",
											(void *)"mnu_lua_stack_dump", //item ref
											1 //force english
											);

	
						// -------------
						//id:8
						XPLMAppendMenuItem(
										   gizmo_Menu_RootItem,
										   "---",
										   (void *)"spacer", //item ref
										   1 //force english
										   );

						
						//id:9
						XPLMAppendMenuItem(
										   gizmo_Menu_RootItem,
										   "Register Gizmo..",
										   (void *)"mnu_register_gizmo", //item ref
										   1 //force english
										   );
	

	
	
/*


						//id:
						gizmo_Menu_Windows = XPLMCreateMenu(
												"Enable ___ module.",
												gizmo_Menu_RootItem,
														XPLMAppendMenuItem(
															gizmo_Menu_RootItem,
															"Windows",
															(void *)"mnu_root_windows",
															1
														  ),
												gizmo_Callback_Menu_WindowsClick,
												0
												);

						//disable the windows menu item until theres something placed inside
						XPLMEnableMenuItem(gizmo_Menu_RootItem, 4, 0);
*/




/*

						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"---",
											(void *)"spacer", //item ref
											1 //force english
											);

						XPLMAppendMenuItem(
											gizmo_Menu_RootItem,
											"About Gizmo..",
											(void *)"mnu_about", //item ref
											1 //force english
											);
											//XPLMCheckMenuItem( gMenu, menu_udp, xplm_Menu_Checked );


*/


} //end of menu create function






/**
Clears all Plugins > Gizmo > Window/Scripts menu items, resets menu active status to disabled.
*/
void gizmo_Menu_Reset(){
		XPLMClearAllMenuItems( gizmo_Menu_VehicleItems );
		XPLMClearAllMenuItems( gizmo_Menu_Windows );

		//disable our Window and Script menu items
		XPLMEnableMenuItem(gizmo_Menu_RootItem, 2, 0);
		XPLMEnableMenuItem(gizmo_Menu_RootItem, 4, 0);

}





void gizmo_StartBulletPhysics(){
	gizmo_BulletPhysics = new GizmoBulletHooks();
}

void gizmo_StopBulletPhysics(){
	delete gizmo_BulletPhysics;
	gizmo_svPhysicsObjectBrushes.clear();
}




/**
This function allows you to call the Lua function OnScriptWarning with a string argument.
What the script does with it is anyones guess.
*/
void gizmo_ThrowScriptWarning( std::string sWarningMessage ){

	std::string sHookFunctionName = std::string("OnScriptWarning"); //this is the name of the Lua event we will call
	if( ! gizmo_DisabledEventsMap[sHookFunctionName] ){

		lua_getglobal(gizmo_luaState,  sHookFunctionName.c_str() ); //prepare the function for callback.
		lua_pushstring(gizmo_luaState, sWarningMessage.c_str() ); //push the message into Lua

		// do the call (1 arguments, 0 result)
		if (lua_pcall(gizmo_luaState, 1, 0, 0) != 0){  //errors handled
			//if this code runs the function did not exist inside lua

			gizmo_DisabledEventsMap[sHookFunctionName] = 1; //this function sig will not be called again
			lua_pop(gizmo_luaState,1); //pop error message off stack

		}//end check error calling func name
	}//end check dead map

}




/**
Sets a thread-locked flag var to say that we want the curl thread to die off as soon as possible.
Mutex protected.
*/
void gizmo_Curl_TimeToDie(){
	pthread_mutex_lock( &gizmo_Curl_Mutex );

	gizmo_Curl_TimeToDieFlag = 1;

	pthread_mutex_unlock( &gizmo_Curl_Mutex );
}




/**
Core cURL function that handles accepting all the incoming data from the lib.
*/
size_t gizmo_curl__write_data(void *buffer, size_t size, size_t nmemb, void *userp){

	//WARNING: This function is called from inside a worker thread. Do not use SDK functions.

	//see docs: http://curl.haxx.se/libcurl/c/curl_easy_setopt.html
		//buffer = new data
		//size = block size
		//nmemb = number of blocks
		//userp = user pointer as specified by CURLOPT_WRITEDATA

	//CURL calls this function when it wants to give us new data that it has read from the network call we last made.


	XLuaCurlInterchange* interchangeData = (XLuaCurlInterchange*)userp;


	interchangeData->eatCurlChunk( buffer, nmemb * size );


	#if 0
	{
		char dbg[4096];
		sprintf(dbg, "curl thread: eat data: %i bytes.\n", (nmemb * size));
		thread_debug( dbg );
	}
	#endif



    return nmemb * size;


}; //end curl write cb func






/**
We kept a complete set of md5 functions "compiled in" so we could trust them.
*/
void xplugins_md5( unsigned char *input, unsigned int inputLen, unsigned char *hash ){
	#if IBM
		xplugins_md5_Win( input, inputLen, hash );
	#endif

	#if APL
		xplugins_md5_Mac( input, inputLen, hash );
	#endif

	#if LIN
		xplugins_md5_Lin( input, inputLen, hash );
	#endif
};

#if APL
void xplugins_md5_Mac( unsigned char *input, unsigned int inputLen, unsigned char *hash ){

	//mac MD5 - OpenSSL

	//char keyQuery[2048];
	//sprintf(keyQuery, "%s", input);

	unsigned char ucMD5Hash[MD5_DIGEST_LENGTH];
	//MD5( (const unsigned char*)keyQuery, strlen(keyQuery), ucMD5Hash );
	MD5( (const unsigned char*)input, inputLen, ucMD5Hash );


	sprintf( (char*)hash,
			//xplugins_gca_FormatString_MD5,
			"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
				ucMD5Hash[0], ucMD5Hash[1], ucMD5Hash[2], ucMD5Hash[3],
				ucMD5Hash[4], ucMD5Hash[5], ucMD5Hash[6], ucMD5Hash[7],
				ucMD5Hash[8], ucMD5Hash[9], ucMD5Hash[10], ucMD5Hash[11],
				ucMD5Hash[12], ucMD5Hash[13], ucMD5Hash[14], ucMD5Hash[15]
				);


	//char hash[64];
	//xplugins_make_hex(ucMD5Hash, (unsigned char*)hash);

};
#endif

#if IBM
void xplugins_md5_Win( unsigned char *input, unsigned int inputLen, unsigned char *hash ){

    unsigned char signature[16];

    //this MD5 function is NOT 64bit compatible! see md5_main.c for a bunch of protection cruft.
    struct xplugins_MD5Context md5c;
    xplugins_MD5Init(&md5c);
        xplugins_MD5Update(&md5c, (unsigned char *)input, inputLen);
    xplugins_MD5Final(signature, &md5c);


    sprintf( (char*)hash,
					//xplugins_gca_FormatString_MD5,
					"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                        signature[0], signature[1], signature[2], signature[3],
                        signature[4], signature[5], signature[6], signature[7],
                        signature[8], signature[9], signature[10], signature[11],
                        signature[12], signature[13], signature[14], signature[15]
                        );


    //use these for debug builds ONLY
    //XPLMDebugString( "MD5: " );
    //XPLMDebugString( hash );
    //XPLMDebugString( "\n" );

};
#endif

#if LIN
void xplugins_md5_Lin( unsigned char *input, unsigned int inputLen, unsigned char *hash ){

    unsigned char signature[16];

    //this MD5 function is NOT 64bit compatible! see md5_main.c for a bunch of protection cruft.
    struct xplugins_MD5Context md5c;
    xplugins_MD5Init(&md5c);
        xplugins_MD5Update(&md5c, (unsigned char *)input, inputLen);
    xplugins_MD5Final(signature, &md5c);


    sprintf( (char*)hash,
					//xplugins_gca_FormatString_MD5,
					"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                        signature[0], signature[1], signature[2], signature[3],
                        signature[4], signature[5], signature[6], signature[7],
                        signature[8], signature[9], signature[10], signature[11],
                        signature[12], signature[13], signature[14], signature[15]
                        );


};
#endif





/**
Listen for mouse wheel events on our big magic window surface.
*/
int gizmo_Callback_MouseTrap_MouseWheel(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   int                  wheel,
                                   int                  clicks,
                                   void *               inRefcon){
	/*
	char caDbg[1024];
	sprintf(caDbg, "Gizmo: Mouse Weel: %i/%i  %i %i\n", x,y, wheel,clicks );
	XPLMDebugString( caDbg );
	// */

	
		//SDK Reference: http://www.xsquawkbox.net/xpsdk/mediawiki/XPLMHandleMouseWheel_f
	
		int ret = 0; //value we will return to X-Plane; 1 - eat this event. 0 - ignore this event.

	
		std::string sHookFunctionName = std::string("OnMouseWheel"); //this is the name of the Lua event we will call
		if( ! gizmo_DisabledEventsMap[sHookFunctionName] ){

			lua_getglobal(gizmo_luaState,  sHookFunctionName.c_str() ); //prepare the function for callback.
				//push arguments, x,y,wheel,clicks
				lua_pushnumber(gizmo_luaState, x );
				lua_pushnumber(gizmo_luaState, y );

				lua_pushnumber(gizmo_luaState, wheel );
				lua_pushnumber(gizmo_luaState, clicks );


			// do the call (4 arguments, 1 result)
			if( lua_pcall(gizmo_luaState, 4, 1, 0) != 0 ){  //errors handled
				//if this code runs the function did not exist inside lua

				gizmo_DisabledEventsMap[sHookFunctionName] = 1; //this function sig will not be called again
				lua_pop(gizmo_luaState,1); //pop error message off stack

			}else{
				//bubble-up the value that our Lua function returned, IF it returned anything.
				//if it didn't we will default to returning 0.
				ret = lua_tointeger( gizmo_luaState, -1 );
				lua_pop(gizmo_luaState,1);	
			}
		}//end check dead map


	return ret;
}





/**
Returns a value to X-Plane indicating whether the user has elected to draw their own mouse cursor.
Actual drawing of the mouse cursor is handled at the end of: gizmo_Callback_Draw_2D_Windows
*/
XPLMCursorStatus gizmo_Callback_MouseTrap_CursorStatus(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   void *               inRefcon){

	if( gizmo_UseCustomCursorDrawingCallback ){
		return xplm_CursorHidden; //cursor will be drawn during 2D windows phase as per SDK recommendation.
	}else{
		return xplm_CursorDefault; //let x-plane manage the cursor
	}

}




/**
Drawing callback for the Mouse-Trap window that listens for mouse action.
Function exists to get the current screen size and stretch the window we listen for to the full size.
This ensures that we get a chance to see all mouse movement, scroll wheel action or clicks.
*/
void gizmo_Callback_MouseTrap_DrawWindow(
                                  XPLMWindowID         inWindowID,
                                   void *               inRefcon){



	//auto adjust window size so it always tracks the users x-plane window size
		int l,t,r,b;
		XPLMGetWindowGeometry( inWindowID, &l, &t, &r, &b );

		int iScreenX, iScreenY;
		XPLMGetScreenSize( &iScreenX, &iScreenY );

		if( r != iScreenX  ||  t != iScreenY ){
			//XPLMSpeakString("Window size changed.");
			XPLMSetWindowGeometry( inWindowID, 0, iScreenY, iScreenX, 0 );
		}


		//ST3400 code elected to draw the mouse cursor here. Hmm.

}



/**
Trap mouse clicks anywhere on the screen and raise a Lua event.
*/
int		gizmo_Callback_MouseTrap_Window_HandleMouseClick(
                                   XPLMWindowID         inWindowID,
                                   int                  x,
                                   int                  y,
                                   XPLMMouseStatus      inMouse,
                                   void *               inRefcon){

	//iterate callback function handles sending in mouse clicks


	char caMouseEvent[4096];
	sprintf( caMouseEvent, "mouse.click = {x=%i, y=%i, e=%i};", x, y, (int)inMouse );
	//XPLMDebugString( caMouseEvent );


	if( luaL_dostring(gizmo_luaState, caMouseEvent ) ){
		//handle standard lua error event as documented.
		lua_pop(gizmo_luaState,1);
	}
	if( lua_isnil( gizmo_luaState, -1 ) ){
		//handle weird "nil" stack overflow where we get left with a nil on the stack by every call to these.
		lua_pop(gizmo_luaState,1);
	}


	int ret = gizmo_RunLuaFunction(gizmo_luaState, new std::string("SYS_OnMouseClick"));
	if( ret == 0 ){
		ret = gizmo_RunLuaFunction(gizmo_luaState, new std::string("OnMouseClick"));
	}
	return ret;

}




/**
Redundant: See: gizmo_Callback_KeySniffer_Raw
*/
void	gizmo_Callback_MouseTrap_Window_HandleKeys(
                                   XPLMWindowID         inWindowID,
                                   char                 inKey,
                                   XPLMKeyFlags         inFlags,
                                   char                 inVirtualKey,
                                   void *               inRefcon,
                                   int                  losingFocus){


	/*
	char caDbg[1024];
	sprintf(caDbg, "Gizmo: Raw Key: %x(%c)   Virt Key: %x(%c)\n", inKey,inKey, inVirtualKey,inVirtualKey );
	XPLMDebugString( caDbg );
	*/

}




//Meh.

