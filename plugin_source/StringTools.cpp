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




#include "StringTools.h"

std::vector<std::string> StringTools::explodeString( std::string s, char* delim ){
	
	//printf( "exploding string: %s\n", s.c_str() );
	
	std::vector<std::string> retVal;
	
	std::string tmp;
		//printf( "delim search: %s:%i\n", delim, s.find( delim ) );

		if( (int)s.find( delim, 0 ) > -1 ){
			//printf("found one....\n");
			//there is at least one delimiter to go off and find.
			
			int dX=0; //location of delimiter.
			do{
				
				dX = s.find( delim );
				tmp = s.substr( 0, dX );

				//printf("found: %s\n", tmp.c_str() );;
				retVal.push_back( tmp );
				
				s = s.substr( dX+1 ); //chop the head off
				//printf("left to search: %s\n", s.c_str() );
			
				
			}while( dX > -1 );
		
		}else{
			//printf("couldnt find delimiter.\n");
			//no delimitered, return the string we were sent.
			retVal.push_back( s );
		}



	return retVal;

}










void StringTools::escapeStringForXml( std::string *pS ){

	//printf("escaping string for xml..(%s)\n", pS->c_str() );

	for( unsigned int x=0 ; x < pS->length() ; x++ ){
	
		const char *data = pS->data();
	
		
		if( data[x] == '&' ){ //must be first or we get recursive escaping.
			//printf("\tampersand..\n");
			
			pS->replace( x, 1, "&#38;" );
			
		}else
		if( data[x] == '\'' ){
			//printf("\tsingle quote..\n");
			
			pS->replace( x, 1, "&#39;" );
			
		}else
		if( data[x] == '"' ){
			//printf("\tdouble quote..\n");
			
			pS->replace( x, 1, "&#34;" );
			
		}else
		if( data[x] == '<' ){
			//printf("\tlt..\n");
			
			pS->replace( x, 1, "&#60;" );
			
		}else
		if( data[x] == '>' ){
			//printf("\tgt..\n");
			
			pS->replace( x, 1, "&#62;" );
			
		}

	
	}
	
	//printf("finished escaping: (%s)\n", pS->c_str() );

}










