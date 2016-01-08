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


#ifndef GIZMO_UTILS_FBOWRAPPER_H_INCLUDED
#define GIZMO_UTILS_FBOWRAPPER_H_INCLUDED



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





//handy stuff
#include "X_STL.h"

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
















class XLuaFboWrapper{
	public:
		GLuint fbo; //used to create the FBO
		GLuint depthbuffer;

		GLuint textureId; //refers back to the destination texture

		int width; //used to setup the viewport
		int height;

		XLuaFboWrapper( GLuint tex, int w, int h ){

			this->width = w;
			this->height = h;

			this->textureId = tex;


			//char caTmpDbg[2048];

			//sprintf( caTmpDbg, "Gizmo: XLuaFboWrapper( %i, %i, %i );\n", this->width, this->height, this->textureId );
			//XPLMDebugString( caTmpDbg );


			//upload a blob of blank data to the texture slot to clear it and hopefully allow us to start drawing properly.
					//glBindTexture( GL_TEXTURE_2D, texture );
					XPLMBindTexture2d( this->textureId, GL_TEXTURE_2D);

			

					//you can set other texture parameters if you want
					//these lines MUST be used to activate FBO's correctly.
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			//attempt to create an FBO
			glGenFramebuffersEXT(1, &(this->fbo));
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo);
			//XPLMDebugString( "Gizmo: XLuaFboWrapper: Created FBO handle..\n" );


			glGenRenderbuffersEXT(1, &(this->depthbuffer));
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depthbuffer);
			//XPLMDebugString( "Gizmo: XLuaFboWrapper: Created depth buff handle..\n" );


			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->width, this->height);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->depthbuffer);
			//XPLMDebugString( "Gizmo: XLuaFboWrapper: Created storage..\n" );


				this->bind();


			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			//XPLMDebugString( "Gizmo: XLuaFboWrapper: Finished, status check.\n" );


			if( status == GL_FRAMEBUFFER_COMPLETE_EXT ){
				//XPLMDebugString("Gizmo: gfx.newFBO: OK: GL_FRAMEBUFFER_COMPLETE_EXT\n");

			}else{
				XPLMDebugString("Gizmo: gfx.newFBO: FAIL\n");

			}
			
			
			//release FBO or lock GL stack.
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


		}
		
		
		void bind(){
			
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->textureId, 0);
			//XPLMDebugString( "Gizmo: XLuaFboWrapper: Setup size and byte format as RGBA..\n" );
		};
		
		
		//gitbug:12 - gfx.rebindFBO()
		void rebind( int tex_id, int width, int height ){
			
			// ---> Verify that OpenGL does not require any form of FBO binding/association release
		
			this->textureId = tex_id;
			this->width = width;
			this->height = height;
			
			this->bind();
		};


		void use(){

			//char caDbg[512];
			//sprintf(caDbg, "Gizmo: XLuaFboWrapper->use(); ptr->fbo: %i\n", this->fbo );
			//XPLMDebugString(caDbg);


			glPushAttrib(GL_ALL_ATTRIB_BITS);
				glPushMatrix();

					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo);

					glPushAttrib(GL_VIEWPORT_BIT);
						glViewport(0,0, this->width, this->height);


							#warning Horrible GL Hack here.
								int iScreenX, iScreenY;
								XPLMGetScreenSize( &iScreenX, &iScreenY );

								float fScaleX, fScaleY; //screen ratio scale factors

								fScaleX = (float)iScreenX / 1024.f;
								fScaleY = (float)iScreenY / 1024.f;

								glScalef( fScaleX, fScaleY, 1.f ); //recover from stupid scaling.

		}

		void release(){
			//XPLMDebugString("Gizmo: XLuaFboWrapper->release();\n");

					glPopAttrib();

					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

				glPopMatrix();
			glPopAttrib();

		}

};






#endif // GIZMO_UTILS_FBOWRAPPER_H_INCLUDED
