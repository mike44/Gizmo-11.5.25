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




void XGLShader::printOglError( char *file, int line ){
        GLenum  glErr;
        int     retCode = 0;
        char    *err;
  
  // returns 1 if an OpenGL error occurred, 0 otherwise.
  
	glErr = glGetError();
  while (glErr != GL_NO_ERROR) {
    err = (char*)gluErrorString(glErr);
    fprintf(stderr,"file: %s \n line: %d \n glError: %s\n",file,line,err);
    retCode = 1;
    glErr = glGetError();
  }
  //return retCode
};


void XGLShader::printInfoLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLcharARB *infoLog;

    printOpenGLError();  // Check for OpenGL errors

    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                         (GLint*)&infologLength);
    printOpenGLError();  // Check for OpenGL errors

    if (infologLength > 0)
    {
        infoLog = (GLcharARB*)malloc(infologLength);
        if (infoLog == NULL)
        {
            printf("ERROR: Could not allocate InfoLog buffer\n");
            exit(1);
        }
        glGetInfoLogARB(obj, infologLength, (GLint*)&charsWritten, infoLog);
        printf("InfoLog:\n%s\n\n", infoLog);
        free(infoLog);
    }
    printOpenGLError();  // Check for OpenGL errors
}



GLint XGLShader::getUniLoc(GLhandleARB program, const GLcharARB *name)
{
    GLint loc;

    loc = glGetUniformLocationARB(program, name);

    if (loc == -1)
        printf("No such uniform named \"%s\"\n", name);

    printOpenGLError();  // Check for OpenGL errors
    return loc;
}




int XGLShader::compileShaders( const char *shdrVertex, const char *shdrFragment )
{

    GLhandleARB shdrVS, shdrFS;//, shdrProg;   // handles to objects
    GLint       vertCompiled, fragCompiled;    // status values
    GLint       linked;

    // Create a vertex shader object and a fragment shader object

    shdrVS = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    shdrFS = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
    // Load source code strings into shaders



	glShaderSourceARB(shdrVS, 1, &shdrVertex, NULL);
    glShaderSourceARB(shdrFS, 1, &shdrFragment, NULL);
	
	
	
    // Compile the shdr vertex shader and print out
    // the compiler log file.

    glCompileShaderARB(shdrVS);
    printOpenGLError();  // Check for OpenGL errors
    glGetObjectParameterivARB(shdrVS,
                GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    this->printInfoLog(shdrVS);

    // Compile the shdr vertex shader and print out
    // the compiler log file.

    glCompileShaderARB(shdrFS);
    printOpenGLError();  // Check for OpenGL errors
    glGetObjectParameterivARB(shdrFS,
                GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
    this->printInfoLog(shdrFS);

    if (!vertCompiled || !fragCompiled)
        return 0;
    // Create a program object and attach the two compiled shaders

    this->sp = glCreateProgramObjectARB();
    glAttachObjectARB(this->sp, shdrVS);
    glAttachObjectARB(this->sp, shdrFS);

    // Link the program object and print out the info log

    glLinkProgramARB(this->sp);
    printOpenGLError();  // Check for OpenGL errors
    glGetObjectParameterivARB(this->sp,
                GL_OBJECT_LINK_STATUS_ARB, &linked);
    this->printInfoLog(this->sp);

    if (!linked)
        return 0;

    // Install program object as part of current state

    //glUseProgramObjectARB(shdrProg);

    // Set up initial uniform values

    //glUniform3fARB( getUniLoc(shdrProg, "BrickColor"), 1.0, 0.3, 0.2);
    //glUniform3fARB( getUniLoc(shdrProg, "MortarColor"), 0.85, 0.86, 0.84);
    //glUniform2fARB( getUniLoc(shdrProg, "BrickSize"), 0.30, 0.15);
    //glUniform2fARB( getUniLoc(shdrProg, "BrickPct"), 0.90, 0.85);
    //glUniform3fARB( getUniLoc(shdrProg, "LightPosition"), 0.0, 0.0, 4.0);

	this->validShader = 1;

    return 1;
}

void XGLShader::setUniFloat( const char *name, float fV ){
	glUniform1fARB( getUniLoc(this->sp, name), fV );
	//printf("set uni float %s to: %f\n", name, fV);
}


void XGLShader::enable(){
	if( this->validShader ){
		//we need this in versions of x-plane prior to about 8.4 or 8.5 that did not have native shader support
			//glEnable(GL_VERTEX_PROGRAM_ARB);
			//glEnable(GL_FRAGMENT_PROGRAM_ARB);

		glUseProgramObjectARB( this->sp );
		
	}else{
		printf("XGLShader - tried to enable an invalid shader instance. Ignored.\n");
	}
}

void XGLShader::disable(){
	if( this->validShader ){
		//we need this in versions of x-plane prior to about 8.4 or 8.5 that did not have native shader support
			//glDisable(GL_VERTEX_PROGRAM_ARB);
			//glDisable(GL_FRAGMENT_PROGRAM_ARB);

		glUseProgramObjectARB( 0 );
	}
}

