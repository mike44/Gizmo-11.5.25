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


#ifndef GIZMO_UTILS_CURLINTERCHANGE_H_INCLUDED
#define GIZMO_UTILS_CURLINTERCHANGE_H_INCLUDED


//when starting a download we create an instance of this
//pass it to a new thread via the arg pointer
//the thread aquires the mutex
class XLuaCurlInterchange{
	public:
	
	
		// ********** WARNING: Class is used for thread data interchange, nothing is threadsafe. Do not use SDK functions.
	
	

		CURL* curl_ezhandle;		//used to suck data down

		int ready;
		int expired;

		std::string url;
		std::string cb_function_name;
		
		std::string user_agent;
		
		std::string post_data;
		
		
		
		int useProxy;
		std::string proxyString;
		
		
		


		unsigned int rx_buffer_offset;
		char* rx_buffer;
		#define GIZMO_CURL_RX_BUFFER_SIZE 8192000


		XLuaCurlInterchange(){
		
			this->useProxy = 0;
			this->proxyString = "";

			this->curl_ezhandle = curl_easy_init();


			this->rx_buffer_offset = 0;
			this->rx_buffer = (char*)malloc( GIZMO_CURL_RX_BUFFER_SIZE );
			memset( this->rx_buffer, 0, GIZMO_CURL_RX_BUFFER_SIZE );

			this->ready = 0;
			this->expired = 0;
			this->url = "";
			//this->payload = "";
			this->cb_function_name = "";
			
			this->post_data = "";

		};

		~XLuaCurlInterchange(){
			free( this->rx_buffer );
		};

		void resetBuffer(){
			this->rx_buffer_offset = 0;
            memset( this->rx_buffer, 0, GIZMO_CURL_RX_BUFFER_SIZE );
		};

		void eatCurlChunk( void* buffer, unsigned int byteCount ){
			memcpy( this->rx_buffer + this->rx_buffer_offset, buffer, byteCount );
			this->rx_buffer_offset += byteCount;
		};


};


#endif // GIZMO_UTILS_CURLINTERCHANGE_H_INCLUDED
