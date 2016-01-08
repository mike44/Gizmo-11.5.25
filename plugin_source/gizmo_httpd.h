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


#if APL
	#include <Carbon/Carbon.h>
#endif



#pragma mark Include Headers

//winsock
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <string.h>

#include <stdlib.h>


#include "XPLMDisplay.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"

#include "XPLMNavigation.h"

#include "XPLMMenus.h"

#include "XPStandardWidgets.h"
#include "XPWidgets.h"
#include "XPWidgetDefs.h"
#include "XPWidgetUtils.h"

#pragma mark --



//usefull STL classes
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>



#include "b64.h"


int sock; //the server socket


int bAllowRemoteConnections = 0; //allow http connections from hosts other than localhost
int bRequirePassword = 0; //require http authentication.

int bLogDebugToConsole = 1;



//x-plane data refs
XPLMDataRef xpdr_Lat, xpdr_Lon;
XPLMDataRef xpdr_AltMSL, xpdr_AltAGL;
XPLMDataRef xpdr_X, xpdr_Y, xpdr_Z;

XPLMDataRef xpdr_Heading, xpdr_HeadingMag, xpdr_HeadingTrack;

XPLMDataRef xpdr_Kias, xpdr_Ktas, xpdr_Kgs;

XPLMDataRef xpdr_WindDirection, xpdr_WindSpeed; //wind at aircraft.

XPLMDataRef xpdr_Nav1, xpdr_Nav1s;
XPLMDataRef xpdr_Com1, xpdr_Com1s;



XPLMDataRef xpdr_windAltHigh;
XPLMDataRef xpdr_windSpeedHigh;
XPLMDataRef xpdr_windDirHigh;
XPLMDataRef xpdr_windSheerSpeedHigh;
XPLMDataRef xpdr_windSheerDirectionHigh;
XPLMDataRef xpdr_windTurbulenceHigh;

XPLMDataRef xpdr_windAltMed;
XPLMDataRef xpdr_windSpeedMed;
XPLMDataRef xpdr_windDirMed;
XPLMDataRef xpdr_windSheerSpeedMed;
XPLMDataRef xpdr_windSheerDirectionMed;
XPLMDataRef xpdr_windTurbulenceMed;

XPLMDataRef xpdr_windAltLow;
XPLMDataRef xpdr_windSpeedLow;
XPLMDataRef xpdr_windDirLow;
XPLMDataRef xpdr_windSheerSpeedLow;
XPLMDataRef xpdr_windSheerDirectionLow;
XPLMDataRef xpdr_windTurbulenceLow;

/*
XPLMDataRef xpdr_;
XPLMDataRef xpdr_;
XPLMDataRef xpdr_;
XPLMDataRef xpdr_;
XPLMDataRef xpdr_;
*/



void do_httpd_core();


//handler functions for the web server.
void processConnection( int client );

char *queryStringV[256]; //lookup table for query string values after 1st stage parsing.
int queryStringVCount = 0;

void parseQuerystring( char *queryString );

int parseQuerystringForInt( char *key );
float parseQuerystringForFloat( char *key );
void parseQuerystringForString( char *key, char *ret, int retSize );


void header401Deny( char *header );

void header200OK_HTM( char *header );
void header200OK_TXT( char *header );
void header200OK_CSS( char *header );
void header200OK_XML( char *header );
void header200OK_PNG( char *header );
void header200OK_ICO( char *header );
void header200OK_SWF( char *header );
void header200OK_OctetStream( char *header );

void header404NF( char *header );

int getFileSize( char *filename );


//#define BUFSIZE 65536
#define BUFSIZE 4194304 //4 megs max

//too many defines 'cause once we had a read buffer and write buffer of differing sizes.
#define BUFFER_PAGE_SIZE BUFSIZE
#define INBUF_SIZE BUFSIZE


//write buffer pointers.
	char *inbuf;
	char *html;


unsigned char *auth_token_raw; //storage for our base64 encoded auth token.
unsigned char *auth_token_b64; //storage for our base64 encoded auth token.


unsigned char *generic_cache;

