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


#ifndef __XGL_SHADER__
#define __XGL_SHADER__


/*
#if IBM
    #include "GL/glu.h"
    #include "GL/glext.h"
#else
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
#endif
*/
//usefull STL classes
//#include "X_STL.h"


#define printOpenGLError() this->printOglError(__FILE__, __LINE__)

class XGLShader{
	public:
		int compileShaders( const char *shdrVertex, const char *shdrFragment );

		void enable();
		void disable();

		void setUniFloat( const char *name, float fV );

		GLhandleARB sp; // <-- shaders handle for openGL.

	//private:
		int validShader; //will only set to 1 if the compile succeeds. controls enable disable logic too.
		
		void printOglError( char *file, int line );
		void printInfoLog(GLhandleARB obj);

		GLint getUniLoc(GLhandleARB program, const GLcharARB *name);
	protected:

};





#endif



