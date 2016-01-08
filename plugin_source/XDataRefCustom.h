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



#ifndef __XDATAREFCUSTOM__
#define __XDATAREFCUSTOM__


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




//usefull STL classes
#include "X_STL.h"



#ifdef DMALLOC
#include "dmalloc.h"
#endif





class XDataRefCustom{
	public:
		XDataRefCustom(){ 
			this->dataRefType = xplmType_Float;
			this->iCustomMode = 0;
			this->dataRefHandle = NULL;
			
			this->dValue = 0.0;
			this->fValue = 0.0f;
			this->iValue = 0;
			
			this->iElementCount = 0;
		}
	
		void setName( const char *name, int CustomMode=0 ){ 
			this->m_sName = name;
			if( CustomMode ){
				this->iCustomMode = 1;
			}else{
				this->iCustomMode = 0;
				this->dataRefHandle = XPLMFindDataRef( name );
			}
		}
		
		std::string getName(){ return this->m_sName; }




		void setHookName( const char *name ){ 
			this->sHookName = name;
		}
		
		std::string getHookName(){ return this->sHookName; }


	
		
		void setInt( int i ){ 
			if( this->iCustomMode ){
				this->iValue = i;
			}else{
				XPLMSetDatai( this->dataRefHandle, i );
			}
		}
		
		void setFloat( float f ){
			if( this->iCustomMode ){
				//printf("Set custom dataref float value: %0.3f\n", f);
				this->fValue = f; 
			}else{
				//printf("Set x-plane dataref float value: %0.3f\n", f);
				XPLMSetDataf( this->dataRefHandle, f );
			}
		}
		
		void setDouble( double d ){ 
			if( this->iCustomMode ){
				this->dValue = d; 
			}else{
				XPLMSetDatad( this->dataRefHandle, d );
			}
		}
		
		
		int getInt(){
			//printf("XDataRefCustom_getInt()..\n");
			if( this->iCustomMode ){
				//printf("..custom mode, returning class member value..\n");
				return this->iValue;
			}else{
				//printf("..normal mode, returning x-plane value..\n");
				return XPLMGetDatai( this->dataRefHandle );
			}
			//printf("Finished XDataRefCustom_getInt()..\n");
		}
		std::vector<int>* getIntV( int offset, int count ){
		
					
			if( this->iCustomMode ){
				//gitbug:39 - full support for array datarefs
				//this code snippet should behave the same as X-Plane with regards to offset and count inputs.
				//x-plane sdk can return the size of the array when the args are right
				//sometimes people will ask for a sub-slice of the vars
				
				std::vector<int>* retValues = new std::vector<int>;
				
				for( unsigned int x=0; x < this->ivValues.size(); x++ ){
					retValues->push_back( this->ivValues[x] );
				}
				
				return retValues;
			}else{
				//this function retrieves an int array from x-plane and converts it to a vector before returning it

				int *tmpValues = new int[count];

				XPLMGetDatavi(this->dataRefHandle, tmpValues, offset, count);
				
				std::vector<int>* retValues = new std::vector<int>;
					for( int x=0; x<count; x++ ){
						retValues->push_back( tmpValues[x] );
					}
					
					
				delete[] tmpValues;

				return retValues;

			}
		

		
		
		}
		
		float getFloat(){ 
			if( this->iCustomMode ){
				return this->fValue;
			}else{
				return XPLMGetDataf( this->dataRefHandle );
			}
		}
		std::vector<float>* getFloatV(int offset, int count){ 
			
			if( this->iCustomMode ){
				//gitbug:39 - full support for array datarefs
				//this code snippet should behave the same as X-Plane with regards to offset and count inputs.
				//x-plane sdk can return the size of the array when the args are right
				//sometimes people will ask for a sub-slice of the vars
				
				std::vector<float>* retValues = new std::vector<float>;
				
				for( unsigned int x=0; x < this->fvValues.size(); x++ ){
					retValues->push_back( this->fvValues[x] );
				}

				return retValues;
				
				//return this->fvValues;
			}else{
				//this function retrieves an float array from x-plane and converts it to a vector before returning it

				float *tmpValues = new float[count];
								
				XPLMGetDatavf(this->dataRefHandle, tmpValues, offset, count);

				std::vector<float>* retValues = new std::vector<float>;
					for( int x=0; x < count; x++ ){
						retValues->push_back( tmpValues[x] );
					}
					
				delete[] tmpValues;
				
				return retValues;

			}
		
		}
		
		double getDouble(){ 
			if( this->iCustomMode ){
				return this->dValue;
			}else{
				return XPLMGetDatad( this->dataRefHandle );
			}
		}
		std::vector<double> getDoubleV(){ return this->dvValues; }


		void resetIntV(){ this->ivValues.resize(0); };
		void resetFloatV(){ this->fvValues.resize(0); };
		void resetDoubleV(){ this->dvValues.resize(0); };

		
		//these are probably not used yet, vector support for custom stuff is bad.
		void setElementCount( int i ){ this->iElementCount=i; }
		int getElementCount(){ return this->iElementCount; }
	
	
		XPLMDataRef dataRefHandle;
	
		void setType( XPLMDataTypeID t ){ this->dataRefType = t; }
		XPLMDataTypeID getType(){ return this->dataRefType; }
	
	
		//set to 1 if this was a custom dataref not an x-plane or other plugin provided dataref
		int iCustomMode;
		
		
		//sHookName defines the name of a Lua scripted function to use when synthesizing events.
		//Using the hookname we append _OnWrite( value ) and _OnRead() and then call into the Lua engine
		std::string sHookName; 
	
	
		std::string m_sName; //dataref name: eg, sim/foo/bar


		//storage for the actual value of the dataref.
		//we should probably convert these to some kind of union structure?
		double dValue;
		float fValue;
		int iValue;
		
		
		//how many items does our array of values have?
		int iElementCount;
		
		std::vector<double> dvValues;
		std::vector<float> fvValues;
		std::vector<int> ivValues;



		//flag to tell x-plane what type of data we store inside us by default.
		XPLMDataTypeID dataRefType;

};



#endif
