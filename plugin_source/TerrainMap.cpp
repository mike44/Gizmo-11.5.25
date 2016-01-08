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


#include "TerrainMap.h"
//#include "gizmo_lua_api/api_xp.h"





//Unify these with the other Datarefs that we use.

XPLMDataRef	gizmo_xpdrAcfX;
XPLMDataRef	gizmo_xpdrAcfY;
XPLMDataRef	gizmo_xpdrAcfZ;

XPLMDataRef	gizmo_xpdrGS;

XPLMDataRef	gizmo_xpdrAcfH;
XPLMDataRef	gizmo_xpdrAcfH_Track;
XPLMDataRef	gizmo_xpdrAcfLat;
XPLMDataRef	gizmo_xpdrAcfLon;

XPLMDataRef	gizmo_xpdr_TerrainMap_AcfAltMsl;
XPLMDataRef gizmo_xpdr_TerrainMap_AcfVerticalSpeed;










pthread_mutex_t gizmo_Taws_Bake_Mutex;
pthread_t gizmo_thread_handle_taws_baker;




float gizmo_TerrainMap_StepSize; //controls range... step size * size = range across edge
int gizmo_TerrainMap_Size;
int gizmo_TerrainMap_RefreshRate;
int gizmo_TerrainMap_ColorMode;

int gizmo_TerrainMap_NorthUp;
int gizmo_TerrainMap_LookAhead;

int gizmo_TerrainMap_TimeDoDie;



void gizmo_SetTerrainMode_Defaults(){
	
	gizmo_TerrainMap_StepSize = 20.0f; //controls range... step size * size = range across edge
	gizmo_TerrainMap_Size = 128;
	gizmo_TerrainMap_RefreshRate = 30;
	gizmo_TerrainMap_ColorMode = gizmo_mapColorMode_Topographic;
	
	gizmo_TerrainMap_NorthUp = 0;
	gizmo_TerrainMap_LookAhead = 1;
	
	gizmo_TerrainMap_TimeDoDie = 0;
	
}






void gizmo_SetTerrainMode_Topographic(){	gizmo_TerrainMap_ColorMode = gizmo_mapColorMode_Topographic; }
void gizmo_SetTerrainMode_Relative(){		gizmo_TerrainMap_ColorMode = gizmo_mapColorMode_Relative; }
void gizmo_SetTerrainMode_Synthetic(){		gizmo_TerrainMap_ColorMode = gizmo_mapColorMode_Synthetic; }
void gizmo_SetTerrainMode_Predictive(){		gizmo_TerrainMap_ColorMode = gizmo_mapColorMode_Predictive; }


void gizmo_SetTerrainOption_Range( float r ){
	gizmo_TerrainMap_StepSize = r;
}

void gizmo_SetTerrainOption_Size( int s ){
	gizmo_TerrainMap_Size = s;
}

void gizmo_SetTerrainOption_RefreshRate( int r ){
	gizmo_TerrainMap_RefreshRate = r;
}

void gizmo_SetTerrainOption_LookAhead( int b ){
	gizmo_TerrainMap_LookAhead = b;
}

void gizmo_SetTerrainOption_NorthUp( int b ){
	gizmo_TerrainMap_NorthUp = b;
}

void gizmo_Mapping_TimeToDie(){
	pthread_mutex_lock( &gizmo_Taws_Bake_Mutex );

		gizmo_TerrainMap_TimeDoDie = 1;

	pthread_mutex_unlock( &gizmo_Taws_Bake_Mutex );
}




//Max map size is 800x800
#define INSIGHT_PROBE_STORAGE_SIZE 640000
#define INSIGHT_MAX_MAP_RESOLUTION 800


//we only use one of these
std::vector<XPLMProbeRef*>			gizmo_XPSceneryProbeHandles;		//probe handles for finding ground location

bool gizmo_ActiveTawsBuffer_Flip = true; //0: A, 1: B.

std::vector<XPLMProbeInfo_t*>	gizmo_XPSceneryProbeResult;
std::vector<XPLMProbeInfo_t*>	gizmo_XPSceneryProbeResult_B;

std::vector<gizmo_DrawBufferEntry_t*> gizmo_DrawBuffer;
std::vector<gizmo_DrawBufferEntry_t*> gizmo_DrawBuffer_B;


void* gizmo_TerrainMap_TextureBlob;// = (unsigned char*)void_blob;   //calloc( (gizmo_TerrainMap_Size*gizmo_TerrainMap_Size)*3, 1 );



void gizmo_Mapping_Init(){

	gizmo_SetTerrainMode_Defaults();

	
	
	//XPLMDebugString("Gizmo: TAWS: Init pthread mutex..\n");
		pthread_mutex_init( &gizmo_Taws_Bake_Mutex, NULL );


	//XPLMDebugString("Gizmo: Terrain Bake: Init Blob.\n");
		gizmo_TerrainMap_TextureBlob = calloc( (gizmo_TerrainMap_Size*gizmo_TerrainMap_Size)*3, 1 );



	


	gizmo_xpdrGS = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
	
	gizmo_xpdrAcfX = XPLMFindDataRef("sim/flightmodel/position/local_x");
	gizmo_xpdrAcfY = XPLMFindDataRef("sim/flightmodel/position/local_y");
	gizmo_xpdrAcfZ = XPLMFindDataRef("sim/flightmodel/position/local_z");

	gizmo_xpdrAcfH = XPLMFindDataRef("sim/flightmodel/position/psi");
	gizmo_xpdrAcfH_Track = XPLMFindDataRef("sim/flightmodel/position/hpath");

	gizmo_xpdr_TerrainMap_AcfAltMsl = XPLMFindDataRef("sim/flightmodel/position/elevation");
	gizmo_xpdr_TerrainMap_AcfVerticalSpeed = XPLMFindDataRef("sim/flightmodel/position/vh_ind");





	//we only use one of these.
	XPLMProbeRef* tmpProbe = new XPLMProbeRef();
		*tmpProbe = XPLMCreateProbe( xplm_ProbeY );
		gizmo_XPSceneryProbeHandles.push_back( tmpProbe );


		for( int initX = 0; initX < (INSIGHT_PROBE_STORAGE_SIZE); initX++){ //obscene amounts of storage space for sample data.
			XPLMProbeInfo_t* tmpProbeInfo = new XPLMProbeInfo_t();
				tmpProbeInfo->structSize = sizeof( XPLMProbeInfo_t );
				gizmo_XPSceneryProbeResult.push_back( tmpProbeInfo );

			gizmo_DrawBufferEntry_t* tmpBufferEntry = new gizmo_DrawBufferEntry_t();
				gizmo_DrawBuffer.push_back(tmpBufferEntry);
		}


		for( int initX = 0; initX < (INSIGHT_PROBE_STORAGE_SIZE); initX++){
			XPLMProbeInfo_t* tmpProbeInfo = new XPLMProbeInfo_t();
				tmpProbeInfo->structSize = sizeof( XPLMProbeInfo_t );
				gizmo_XPSceneryProbeResult_B.push_back( tmpProbeInfo );

			gizmo_DrawBufferEntry_t* tmpBufferEntry = new gizmo_DrawBufferEntry_t();
				gizmo_DrawBuffer_B.push_back(tmpBufferEntry);
		}


		if(true){
			XPLMDebugString("\t Init oven thread..\n");
			int rc = pthread_create(&gizmo_thread_handle_taws_baker, NULL, gizmo_pthreads_Mapping_TerrainBake, gizmo_TerrainMap_TextureBlob);
			if (rc){
				char caPThreadError[2048];
				sprintf(caPThreadError, "\t ERROR; return code from pthread_create() is %d\n", rc );
				XPLMDebugString( caPThreadError );
			}
		}


}








//core function! :D


void gizmo_ProbeTerrain( int iForceFullUpdate ){

		//this is our zero point.
			static float fAcfX = XPLMGetDataf( gizmo_xpdrAcfX );
			static float fAcfZ = XPLMGetDataf( gizmo_xpdrAcfZ );

		//using track has some interesting issues with ground alignment, it behaves like a real GPS.
			static float fAcfH = XPLMGetDataf( gizmo_xpdrAcfH_Track );

		//Number of pixels along one edge
			static int iMapResolution = gizmo_TerrainMap_Size;

		//map zoom level control - map range
			static float fStepSize = gizmo_TerrainMap_StepSize;

		//these are calculated inside the bCalcFixes guard glag below.
			static float fStartX;
			static float fStartZ;

		//these control stepping the vertical scan beam i think.
			static float fZOffX, fZOffZ;
			static float fXOffX, fXOffZ;

		//guard flag to cut out jitter
			static bool sbCalcFixes = true;

		//storage for world probe positions, they're tricky to calc correctly and best pre-stored.
			static float faZStartPositions[40000][2];//;[ iMapResolution ][2];


				// --------------------- guard flag line is here. --------
				//this guard flag is attempt to kill any jitter in the probe code by sticking everything in static vars that dont update every call.
				if( sbCalcFixes ){

							//map zoom level control - map range
							fStepSize = gizmo_TerrainMap_StepSize;


							XTrig::computeNewPositionOffset( fAcfH + 180, fStepSize, &fZOffX, &fZOffZ );
							XTrig::computeNewPositionOffset( fAcfH + 90, fStepSize, &fXOffX, &fXOffZ );


							iMapResolution = gizmo_TerrainMap_Size;
							if( iMapResolution > INSIGHT_MAX_MAP_RESOLUTION ){
								iMapResolution = INSIGHT_MAX_MAP_RESOLUTION;
							}


							fAcfX = XPLMGetDataf( gizmo_xpdrAcfX );
							fAcfZ = XPLMGetDataf( gizmo_xpdrAcfZ );

						//using track has some interesting issues with ground alignment, it behaves like a real GPS.
							fAcfH = XPLMGetDataf( gizmo_xpdrAcfH_Track );
					
							if( XPLMGetDataf( gizmo_xpdrGS ) < 0.1f ){
								fAcfH = XPLMGetDataf( gizmo_xpdrAcfH );
							}


							if(gizmo_TerrainMap_NorthUp){
								fAcfH = 0.f;
							}


						//find hypotenuse..
							float fTmpA = (fStepSize * (float)(( iMapResolution )/2));
							float fTmpB = fTmpA * fTmpA;
							float fStartDistance = sqrt( fTmpB + fTmpB );


							{
								float tmpX, tmpZ;
								
								if(gizmo_TerrainMap_LookAhead){
									//attemp at look ahead terrain probing. supposed to be a 70 degree arc
									//this code works by looking ahead in our track direction and shifting the map probe start point N units in that direction.
									XTrig::computeNewPositionOffset( fAcfH, (fStartDistance * 0.5), &tmpX, &tmpZ );
									fAcfX += tmpX;
									fAcfZ += tmpZ;
								}

								//start of terrain probe grid
								XTrig::computeNewPositionOffset( fAcfH - 45.f, fStartDistance, &tmpX, &tmpZ );
								fStartX = (fAcfX + tmpX); //go west
								fStartZ = (fAcfZ + tmpZ); //go north
							}



					//figure out probe points spread over the map.
					for( int newZ=0; newZ < iMapResolution; newZ++ ){
						faZStartPositions[newZ][0] = fStartX + (newZ * fZOffX);
						faZStartPositions[newZ][1] = fStartZ + (newZ * fZOffZ);
					}
					sbCalcFixes = false;
				}
				//------------ end guard flagged ------








				//these variables track the sharded probing i think.
				static int iProbeHandleCounter = 0; //keep our place in the dataset.

				static float fProbeX = fStartX;
				static float fProbeY = 0.f;
				static float fProbeZ = fStartZ;

					//runs when the counter gets reset by loops below
					if( iProbeHandleCounter == 0 ){
						fProbeX = fStartX;
						fProbeZ = fStartZ;
					}




				//this is the loop variable.
				static int cz=0;
				if( iForceFullUpdate ){
					// XPLMSpeakString("Full update forced.");
					XPLMDebugString("Gizmo: TAWS Core: Full update forced.\n");
					cz=0;
				}

				//run our probe code
				while( cz < iMapResolution ){
					fProbeZ = faZStartPositions[cz][1];
					fProbeX = faZStartPositions[cz][0];


					for(int x=0 ; x< iMapResolution; x++ ){

							//we've done a row of probes move the beam scan
							fProbeX += fXOffX;
							fProbeZ += fXOffZ;

							if( iProbeHandleCounter < (int)gizmo_XPSceneryProbeResult.size() ){
							//if( iProbeHandleCounter < INSIGHT_PROBE_STORAGE_SIZE ){

								//if( gizmo_ActiveTawsBuffer_Flip ){
									//write to primary buffer
									XPLMProbeTerrainXYZ(
													*(gizmo_XPSceneryProbeHandles[0]),
													fProbeX,
													fProbeY,
													fProbeZ,
													(gizmo_XPSceneryProbeResult[iProbeHandleCounter])

																		);
								/*
								}else{
									//write to secondary buffer
									XPLMProbeTerrainXYZ(
													*(gizmo_XPSceneryProbeHandles[0]),
													fProbeX,
													fProbeY,
													fProbeZ,
													(gizmo_XPSceneryProbeResult_B[iProbeHandleCounter])
													);
								}//end
								*/


							}//ensure we are using a valid probe slot
						iProbeHandleCounter++;

					}//end x loop

					iProbeHandleCounter++;

					cz++;
					//printf("cz: %i\n", cz);

					//int iMapRefreshRate = gizmo_TerrainMap_RefreshRate;
					if( ! iForceFullUpdate ){
						if( cz % gizmo_TerrainMap_RefreshRate == 0 ){
								//printf("probe shard returning: cz: %i\n", cz);
								return;
						}
					}



				}//end z loop;
				//z = 0;
				iProbeHandleCounter = 0;

				cz = 0;


			sbCalcFixes = true; //enable another set of fixes to be generated, this should eliminate map shear
			gizmo_CopyDataIntoDrawBuffer();


}//terrain probe function








//2nd stage, copy it into a new buffer.

void gizmo_CopyDataIntoDrawBuffer(){

		double dWorldLat, dWorldLon, dWorldAlt;

		int iMapResolution_Squared = sqr(gizmo_TerrainMap_Size); //sqr( xcdr_MapResolution->getInt() );

			//for( unsigned int cx=0; cx < gizmo_XPSceneryProbeResult.size(); cx++ ){ // <--perf bomb.
			for( int cx=0; cx < iMapResolution_Squared; cx++ ){

				//if( gizmo_ActiveTawsBuffer_Flip ){
					XPLMLocalToWorld(
						(gizmo_XPSceneryProbeResult[cx])->locationX,
						(gizmo_XPSceneryProbeResult[cx])->locationY,
						(gizmo_XPSceneryProbeResult[cx])->locationZ,
						&dWorldLat, &dWorldLon, &dWorldAlt );

					gizmo_DrawBuffer[cx]->dLon = dWorldLon;
					gizmo_DrawBuffer[cx]->dLat = dWorldLat;
					gizmo_DrawBuffer[cx]->dAlt = dWorldAlt;

					gizmo_DrawBuffer[cx]->iIsWet = (gizmo_XPSceneryProbeResult[cx])->is_wet;

				/*
				}else{
					XPLMLocalToWorld(
						(gizmo_XPSceneryProbeResult_B[cx])->locationX,
						(gizmo_XPSceneryProbeResult_B[cx])->locationY,
						(gizmo_XPSceneryProbeResult_B[cx])->locationZ,
						&dWorldLat, &dWorldLon, &dWorldAlt );

					gizmo_DrawBuffer_B[cx]->dLon = dWorldLon;
					gizmo_DrawBuffer_B[cx]->dLat = dWorldLat;
					gizmo_DrawBuffer_B[cx]->dAlt = dWorldAlt;

					gizmo_DrawBuffer_B[cx]->iIsWet = (gizmo_XPSceneryProbeResult_B[cx])->is_wet;

				}
				*/

			}//end copy loop

}//enn copy into draw buffer function








int gizmo_taws_iColorMode;// = gizmo_TerrainMap_ColorMode;

//these are used for predictive/relative modes
float gizmo_taws_fAcfAltMslFt;// = XPLMGetDataf(gizmo_xpdr_TerrainMap_AcfAltMsl) * 3.2808399f;
float gizmo_taws_fAcfVerticalSpeed;// = XPLMGetDataf(gizmo_xpdr_TerrainMap_AcfVerticalSpeed); //meters/sec
float gizmo_taws_fAcfHeading;



//3rd stage, render it into colors.

void gizmo_Mapping_TerrainBake( GLuint textureId ){

	//this function runs in x-planes main thread and is responsible for texture uploads.

	//we do NOT need an update every frame so we use trylock in this instance.

	static int iLockCounter=0;
	if( pthread_mutex_trylock( &gizmo_Taws_Bake_Mutex ) == 0 ){
		//XPLMDebugString("Gizmo: Terrain Bake: Mutex lock aquired, uploading and re-baking.\n");
		//we have control, we should upload the texture and set another bake operation in motion.
				XPLMBindTexture2d( textureId, GL_TEXTURE_2D);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gizmo_TerrainMap_Size, gizmo_TerrainMap_Size, 0, GL_RGB, GL_UNSIGNED_BYTE, gizmo_TerrainMap_TextureBlob);

				//write to these here, read to them in the thread, see if we go boom boom!
				gizmo_taws_iColorMode = gizmo_TerrainMap_ColorMode;
				gizmo_taws_fAcfAltMslFt = XPLMGetDataf(gizmo_xpdr_TerrainMap_AcfAltMsl) * 3.2808399f;
				gizmo_taws_fAcfVerticalSpeed = XPLMGetDataf(gizmo_xpdr_TerrainMap_AcfVerticalSpeed); //meters/sec
				gizmo_taws_fAcfHeading = XPLMGetDataf( gizmo_xpdrAcfH );


				pthread_mutex_unlock( &gizmo_Taws_Bake_Mutex ); //the thread is waiting for us to unlock this.

	}else{
		iLockCounter++;
		if( iLockCounter > 100 ){
			XPLMDebugString("Gizmo: Terrain Bake: 100 thread lock events.\n");
			iLockCounter = 0;
		}
	}

}




/**
This function bakes our sampled data into a texture blob suitable to push into OpenGL.
Baking is done from system ram, into system ram, in a worker thread. When the process is complete 
the thread yeilds and the system ram is pushed into VRAM.
*/
void *gizmo_pthreads_Mapping_TerrainBake( void* void_blob ){

	while(gizmo_TerrainMap_TimeDoDie == 0){
		pthread_mutex_lock( &gizmo_Taws_Bake_Mutex );// > 0 ){ /* wait for mutex lock*/ };

		unsigned char* blob = (unsigned char*)void_blob;   //calloc( (gizmo_TerrainMap_Size*gizmo_TerrainMap_Size)*3, 1 );
		unsigned int bx=0;

		int iProbeHandleCounter = 0;
		int iMaxProbeCounter = INSIGHT_PROBE_STORAGE_SIZE-1;//gizmo_DrawBuffer.size()-1;


		int iIsWet=0;
		float fPAlt=0.f;


		for( int y=0; y< gizmo_TerrainMap_Size; y++ ){
			for( int x=0; x< gizmo_TerrainMap_Size; x++ ){
				if( iProbeHandleCounter < iMaxProbeCounter ){


							fPAlt = (float)(gizmo_DrawBuffer[iProbeHandleCounter])->dAlt;
							iIsWet = (gizmo_XPSceneryProbeResult[iProbeHandleCounter])->is_wet;

								// *
								switch( gizmo_taws_iColorMode ){
									default:
									case gizmo_mapColorMode_Synthetic:
										gizmo_GetTerrainColor_Synthetic( fPAlt, iIsWet, (gizmo_DrawBuffer[iProbeHandleCounter])->rgb, gizmo_taws_fAcfAltMslFt, gizmo_taws_fAcfVerticalSpeed, gizmo_taws_fAcfHeading );
										break;
										
									case gizmo_mapColorMode_Topographic: //sectional/topo
										gizmo_GetTerrainColor_Topographic( fPAlt, iIsWet, (gizmo_DrawBuffer[iProbeHandleCounter])->rgb );
										break;

									case gizmo_mapColorMode_Relative: //relative
										gizmo_GetTerrainColor_Relative( fPAlt, iIsWet, (gizmo_DrawBuffer[iProbeHandleCounter])->rgb, gizmo_taws_fAcfAltMslFt );
										break;

									case gizmo_mapColorMode_Predictive: //predictive
										gizmo_GetTerrainColor_Predictive( fPAlt, iIsWet, (gizmo_DrawBuffer[iProbeHandleCounter])->rgb, gizmo_taws_fAcfAltMslFt, gizmo_taws_fAcfVerticalSpeed, gizmo_taws_fAcfHeading );
										break;

								}//end switch color mode
								// */
					

							//gizmo_GetTerrainColor_Synthetic( fPAlt, iIsWet, (gizmo_DrawBuffer[iProbeHandleCounter])->rgb, gizmo_taws_fAcfAltMslFt, gizmo_taws_fAcfVerticalSpeed, gizmo_taws_fAcfHeading );


							blob[bx] = (unsigned char)(int)((gizmo_DrawBuffer[iProbeHandleCounter])->rgb[0] * 255.f); bx++;
							blob[bx] = (unsigned char)(int)((gizmo_DrawBuffer[iProbeHandleCounter])->rgb[1] * 255.f); bx++;
							blob[bx] = (unsigned char)(int)((gizmo_DrawBuffer[iProbeHandleCounter])->rgb[2] * 255.f); bx++;



					}//ensure we are using a valid probe slot

				iProbeHandleCounter++;

			}//end x loop

			iProbeHandleCounter++;

		}//end looping probes


		pthread_mutex_unlock( &gizmo_Taws_Bake_Mutex ); //we're done bakin.


		//1000ms / 60 = 16.666666667
		int ms = 64; //32 still yeilds a few contentions @ 30fps

		    #if IBM
			Sleep( ms );
			#endif

			#if APL
			usleep( ms * 1000 );
			#endif

			#if LIN
			usleep( ms * 1000 );
			#endif




	}


	//we should never see this.
	pthread_exit(NULL);
	return 0;

}//end of tex bake loop.















//---- utility functions that set colors for the heighmap data

void gizmo_GetTerrainColor_Synthetic( float fAlt, int iWet, float *rgb, float fAcfAltMslFt, float fAcfVerticalSpeed, float fAcfTrack ){

	fAcfAltMslFt = fAcfAltMslFt - 1000.f; //500ft below = "safe"
	
	float fAltFeet = fAlt * 3.2808399f;
	float fAltDelta = fAltFeet - fAcfAltMslFt; //figure out the terrain alt delta.
	
	//aircraft agl controls step size.
	float fStepMult = 10.f;
	
	
	if( iWet ){//&& fAltFeet <= 3.28f ){ //gitbug:54 - determine ocean probing properly.
		rgb[0] = 0.5f;
		rgb[1] = 0.5f;
		rgb[2] = 1.f;
	
		
	}else if( fAltDelta < 0.f ){
		//this terrain is at or below us.
		//dark dark green.
		
		rgb[0] = 0.f;
		rgb[1] = 0.05f;
		rgb[2] = 0.f;
		

	}else if( fAltDelta < 10.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.1f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 20.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.2f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 30.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.3f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 40.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.4f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 50.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.5f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 60.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.6f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 70.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.7f;
		rgb[2] = 0.f;
		
	}else if( fAltDelta < 80.f * fStepMult ){
		rgb[0] = 0.f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 90.f * fStepMult ){
		rgb[0] = 0.1f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 100.f * fStepMult ){
		rgb[0] = 0.2f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;
		
	}else if( fAltDelta < 110.f * fStepMult ){
		rgb[0] = 0.3f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 120.f * fStepMult ){
		rgb[0] = 0.4f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 130.f * fStepMult ){
		rgb[0] = 0.5f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;
		
	}else if( fAltDelta < 140.f * fStepMult ){
		rgb[0] = 0.6f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;
		
	}else if( fAltDelta < 150.f * fStepMult ){
		rgb[0] = 0.7f;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;

		
	}else if( fAltDelta < 160.f * fStepMult ){
		rgb[0] = 0.8;
		rgb[1] = 0.8f;
		rgb[2] = 0.f;
		
	}else if( fAltDelta < 170.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.7f;
		rgb[2] = 0.f;

	}else if( fAltDelta < 180.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.6f;
		rgb[2] = 0.0f;		
		
	}else if( fAltDelta < 190.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.5f;
		rgb[2] = 0.0f;
		
	}else if( fAltDelta < 200.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.4f;
		rgb[2] = 0.0f;		

	}else if( fAltDelta < 210.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.3f;
		rgb[2] = 0.0f;

	}else if( fAltDelta < 220.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.2f;
		rgb[2] = 0.0f;

	}else if( fAltDelta < 230.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.1f;
		rgb[2] = 0.0f;
		
	}else if( fAltDelta < 240.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.0f;
		rgb[2] = 0.0f;

		
		
	}else if( fAltDelta < 250.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.1f;
		rgb[2] = 0.1f;

	}else if( fAltDelta < 260.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.2f;
		rgb[2] = 0.2f;
	
	}else if( fAltDelta < 270.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.3f;
		rgb[2] = 0.3f;

	}else if( fAltDelta < 280.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.4f;
		rgb[2] = 0.4f;

	}else if( fAltDelta < 290.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.5f;
		rgb[2] = 0.5f;
		
	}else if( fAltDelta < 300.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.6f;
		rgb[2] = 0.6f;
	
	}else if( fAltDelta < 310.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.7f;
		rgb[2] = 0.7f;
		
	}else if( fAltDelta < 320.f * fStepMult ){
		rgb[0] = 0.8f;
		rgb[1] = 0.8f;
		rgb[2] = 0.8f;
	
	}else{
		//this should render anything ridiculously high in white.
		//hopefully resolves the alpha channel bug in the texture baker.
		rgb[0] = 0.9f;
		rgb[1] = 0.9f;
		rgb[2] = 0.9f;
	}
	
	
}//end: getTerrainColor_Topographic(..)













void gizmo_GetTerrainColor_Topographic( float fAlt, int iWet, float *rgb ){
	/*
	FAA Color Chart Specs:
		http://virtualskies.arc.nasa.gov/navigation/tutorial/tutorial8.html

		Basic altitude graduation system taken from NASA.

		Color pallete below lifted from Sandel ST3400 XP screenshot.
		
		2011-01-15 - br@x-plugins.com
		The word lifted above is not very pretty.
		The color pallete below was "lifted" by using a screenshot of the Sandel XP product, the clearest resource for this pallete I could find at the time.
		The OS X "Digital Color Meter" tool was then used to mouse over each color well step by step, transcribing the colors into the floating point indexing below.
		If anyone objects to this as fair use or legitimate "re-wording" and is not happy  with the above description please let me know and I will rewrite or simply delete the code.
		

	*/

	float fAltFeet = fAlt * 3.2808399f;

			if( iWet ){//&& fAltFeet <= 3.28f ){ //gitbug:54 - determine ocean probing properly.
				rgb[0] = 0.5f;
				rgb[1] = 0.5f;
				rgb[2] = 1.f;

			}else if( fAltFeet <= 0.f ){
				rgb[0] = 0.843f;
				rgb[1] = 0.886f;
				rgb[2] = 0.792f;

			}else if( fAltFeet <= 1000.f ){
				rgb[0] = 0.729f;
				rgb[1] = 0.839f;
				rgb[2] = 0.729f;

			}else if( fAltFeet <= 2000.f ){
				rgb[0] = 0.620f;
				rgb[1] = 0.784f;
				rgb[2] = 0.671f;

			}else if( fAltFeet <= 3000.f ){
				rgb[0] = 0.882f;
				rgb[1] = 0.843f;
				rgb[2] = 0.800f;

			}else if( fAltFeet <= 5000.f ){
				rgb[0] = 0.839f;
				rgb[1] = 0.796f;
				rgb[2] = 0.635f;

			}else if( fAltFeet <= 7000.f ){
				rgb[0] = 0.851f;
				rgb[1] = 0.753f;
				rgb[2] = 0.576f;

			}else if( fAltFeet <= 9000.f ){
				rgb[0] = 0.769f;
				rgb[1] = 0.631f;
				rgb[2] = 0.435f;

			}else if( fAltFeet <= 11000.f ){
				rgb[0] = 0.773f;
				rgb[1] = 0.584f;
				rgb[2] = 0.369f;

			}else if( fAltFeet <= 13000.f ){
				rgb[0] = 0.740f;
				rgb[1] = 0.535f;
				rgb[2] = 0.295f;

			}else if( fAltFeet <= 15000.f ){
				rgb[0] = 0.697f;
				rgb[1] = 0.489f;
				rgb[2] = 0.237f;

			}else if( fAltFeet <= 17000.f ){
				rgb[0] = 0.621f;
				rgb[1] = 0.405f;
				rgb[2] = 0.129f;

			}else if( fAltFeet > 17000.f ){ //greater than!
				rgb[0] = 0.434f;
				rgb[1] = 0.325f;
				rgb[2] = 0.0f;

			}


}//end: getTerrainColor_Topographic(..)









void gizmo_GetTerrainColor_Relative( float fAlt, int iWet, float *rgb, float fAcfAltMslFt ){
	
	float fAltFeet = fAlt * 3.2808399f;
	float fAltDelta = fAltFeet - fAcfAltMslFt;
	
	if( fAltDelta >= 1500.f ){
		//dark red:
		rgb[0] = 0.485f;
		rgb[1] = 0.000f;
		rgb[2] = 0.084f;
		
	}else if( fAltDelta >= -100.f ){ //100 feet below and above
		//light red:
		rgb[0] = 0.957f;
		rgb[1] = 0.000f;
		rgb[2] = 0.169f;
		
	}else if( fAltDelta >= -1000.f ){ //1000 feet below and above
		//yellow:
		rgb[0] = 0.988f;
		rgb[1] = 1.000f;
		rgb[2] = 0.659f;
		
	}else if( fAltDelta >= -2000.f ){ //1000 feet below and above
		//green:
		rgb[0] = 0.282f;
		rgb[1] = 0.631f;
		rgb[2] = 0.443f;
		
	}else if( fAltDelta < -2000.f ){ //1000 feet below and above
		//black: duh.
		rgb[0] = 0.0f;
		rgb[1] = 0.0f;
		rgb[2] = 0.0f;
	}
	
	
	if( iWet ){
		rgb[0]=0.5f;
		rgb[1]=0.5f;
		rgb[2]=1.0f;
	}	
	
}//end: getTerrainColor_Relative(..)





void gizmo_GetTerrainColor_Predictive( float fAlt, int iWet, float *rgb, float fAcfAltMslFt, float fAcfVerticalSpeed, float fAcfTrack ){
	
	float fAltFeet = fAlt * 3.2808399f; //this is the altitude of the terrain point
	
	fAcfAltMslFt += fAcfVerticalSpeed; //basic 1 minute out check, add vertical speed in fpm to the altitude of the aircraft
	float fAltDelta = fAltFeet - fAcfAltMslFt;
	
	if( fAltDelta >= 1500.f ){
		//dark red:
		rgb[0] = 0.485f;
		rgb[1] = 0.000f;
		rgb[2] = 0.084f;
		
	}else if( fAltDelta >= -100.f ){ //100 feet below and above
		//light red:
		rgb[0] = 0.957f;
		rgb[1] = 0.000f;
		rgb[2] = 0.169f;
		
	}else if( fAltDelta >= -1000.f ){ //1000 feet below and above
		//yellow:
		rgb[0] = 0.988f;
		rgb[1] = 1.000f;
		rgb[2] = 0.659f;
		
	}else if( fAltDelta >= -2000.f ){ //1000 feet below and above
		//green:
		rgb[0] = 0.282f;
		rgb[1] = 0.631f;
		rgb[2] = 0.443f;
		
	}else if( fAltDelta < -2000.f ){ //1000 feet below and above
		//black: duh.
		rgb[0] = 0.0f;
		rgb[1] = 0.0f;
		rgb[2] = 0.0f;
	}
	
	
	if( iWet ){
		rgb[0]=0.5f;
		rgb[1]=0.5f;
		rgb[2]=1.0f;
	}
} //gizmo_GetTerrainColor_Predictive( float fAlt, int iWet, float *rgb, float fAcfAltMslFt, float fAcfVerticalSpeed, float fAcfTrack ){
