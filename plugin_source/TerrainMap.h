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


#ifndef __GIZMO_TERRAIN_MAP_H__
#define __GIZMO_TERRAIN_MAP_H__




#if IBM
    //#include <windows.h>
    #include "GLee.h"
	#include <GL/glu.h>
	#include <GL/gl.h>
	#include <GL/glext.h>

#endif

#if APL
    #include "GLee.h"
    #include <GLUT/glut.h>
#endif


#if LIN
    #include "GLee.h"
    #include <GL/glut.h>
    #include <stdio.h>
#endif



#include "X_STL.h"
#include "XTrig.h"




#include "XPLMMenus.h"
#include "XPLMDisplay.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMPlugin.h"
#include "XPLMPlanes.h"
#include "XPLMGraphics.h"
#include "XPLMScenery.h"

#include "XPLMNavigation.h"

#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPUIGraphics.h"
#include "XPWidgetDefs.h"
#include "XPWidgetUtils.h"


#include <pthread.h>


/**
Quick port of code required to setup datarefs.
*/
void gizmo_Mapping_Init();

/**
Called to let the thread know when to die.
*/
void gizmo_Mapping_TimeToDie();


/**
Bakes our neatly sorted terrain data into a nice texture.
*/
void gizmo_Mapping_TerrainBake( GLuint textureId );
void *gizmo_pthreads_Mapping_TerrainBake( void* void_blob );								   

/**
Core function that does all SDK terrain probe work.
*/
void gizmo_ProbeTerrain( int iForceFullUpdate );


/**
Copies data into a 2nd buffer so we can draw from it without corruption or conflict.
*/
void gizmo_CopyDataIntoDrawBuffer();


/**
Functions to create a color-mapped bitmap.
*/

void gizmo_GetTerrainColor_Predictive( float fAlt, int iWet, float *rgb, float fAcfAltMslFt, float fAcfVerticalSpeed, float fAcfTrack );
void gizmo_GetTerrainColor_Relative( float fAlt, int iWet, float *rgb, float fAcfAltMslFt );
void gizmo_GetTerrainColor_Synthetic( float fAlt, int iWet, float *rgb, float fAcfAltMslFt, float fAcfVerticalSpeed, float fAcfTrack );
void gizmo_GetTerrainColor_Topographic( float fAlt, int iWet, float *rgb );


	   
void gizmo_SetTerrainMode_Defaults();

void gizmo_SetTerrainMode_Relative();
void gizmo_SetTerrainMode_Synthetic();
void gizmo_SetTerrainMode_Topographic();
void gizmo_SetTerrainMode_Predictive();


void gizmo_SetTerrainOption_Brightness( float b ); //<--active?
void gizmo_SetTerrainOption_Range( float r );
void gizmo_SetTerrainOption_RefreshRate( int r );
void gizmo_SetTerrainOption_Size( int s );

void gizmo_SetTerrainOption_LookAhead( int b );
void gizmo_SetTerrainOption_NorthUp( int b );


enum e_GaugeMapColorMode{
	gizmo_mapColorMode_Synthetic,
	gizmo_mapColorMode_Topographic,
	gizmo_mapColorMode_Relative,
	gizmo_mapColorMode_Predictive
};





struct gizmo_DrawBufferEntry_t{
	//int objectId;
	double dLon;
	double dLat;
	double dAlt;

	int iIsWet;

	int iIsCaution; //mild TAWS event
	int iIsWarning; //severe TAWS event

	float rgb[3];

	//XPLMDrawInfo_t xpDrawLocation;
};






#endif
