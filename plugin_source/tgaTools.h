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


// This workd is heavily based on Sandy Barbours.


//OpenGL
#if APL
	//framework path differs
	#include <OpenGL/glu.h>
#else
	#include <GL/glu.h>
#endif



#include <sys/types.h>
#include <sys/stat.h>


#include <stdlib.h>
#include <stdio.h>

#include <string.h>



#ifdef XPLM200
	#include <XPLMUtilities.h>
#endif


/*
#ifndef FALSE
	#define FALSE 0
#endif
*/






bool gizmo_LoadTGA_File(int textureId, char* filename);
bool gizmo_LoadTGA_DecodeHeaders(int textureId, unsigned char* blob, unsigned int blob_size);
bool gizmo_LoadTGA_Blob_Core(int textureId, unsigned char* blob, GLubyte* header, unsigned int blob_size, unsigned int width, unsigned int height );

void gizmo_uploadBuffer32( GLuint texture, GLubyte* buffer, int x, int y );
void gizmo_uploadBuffer24( GLuint texture, GLubyte* buffer, int x, int y );



int gizmo_isValidTextureSize( int size );




void gizmo_debugLog( char* foo );



typedef struct gizmo_CustomGLTextureStruct_t
{
	GLubyte	*imageData;
	GLuint	bpp;
	GLuint	width;
	GLuint	height;
	//XPLMTextureID texID;
} gizmo_TextureImage;
