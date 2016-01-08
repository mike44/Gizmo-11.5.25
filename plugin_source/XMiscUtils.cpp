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


#include "XMiscUtils.h"
#include <string.h>



//xor hash function. crap but annoying.
std::string zaw( std::string value, std::string salt ){

//void checksumEnvelope( unsigned char *buff, int size, unsigned char *checkReturn ){

	if( salt.length() < 8 ){
		return "";
	}


	std::string ret = "";

	unsigned char ca_salt[8];
		ca_salt[0] = salt[0];
		ca_salt[1] = salt[1];
		ca_salt[2] = salt[2];
		ca_salt[3] = salt[3];
		ca_salt[4] = salt[4];
		ca_salt[5] = salt[5];
		ca_salt[6] = salt[6];
		ca_salt[7] = salt[7];
		

    char tmp[ value.size() ];

    memset(tmp, 0, value.size() ); //clean first
    memcpy(tmp, value.c_str(), value.size() );
	
	unsigned char cChecksum = 0;
	unsigned char cTmp = 0;

	for( int sx=0; sx<8; sx++ ){
		cChecksum = salt[sx];
			
			for(int x=0; x<(int)value.size(); x++){
				cTmp = cChecksum ^ tmp[x];
				cChecksum = cTmp;
			}
			//printf("%x.", cChecksum);
			ret += cChecksum;
			
	}


	return ret;


}
