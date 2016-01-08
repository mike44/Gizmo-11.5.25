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





#include "tgaTools.h"
#include <XPLMGraphics.h>

int gizmo_isValidTextureSize( int size ){

    switch( size ){
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
        case 128:
        case 256:
        case 512:
        case 1024:
		case 2048:
		//case 4096:
		    return 1;
            break;

        default:
            return 0;
            break;
    }

	return 0;
}




void gizmo_debugLog( char* foo ){ };



void gizmo_uploadBuffer24( GLuint texture, GLubyte* buffer, int x, int y ){

	printf("uploadBuffer24..\n");

	//glBindTexture( GL_TEXTURE_2D, texture );
	XPLMBindTexture2d( texture, GL_TEXTURE_2D);

	glTexImage2D( GL_TEXTURE_2D, 0, 3,
					x,	y, 0,
					GL_RGB, GL_UNSIGNED_BYTE, buffer );

	//you can set other texture parameters if you want
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void gizmo_uploadBuffer32( GLuint texture, GLubyte* buffer, int x, int y ){

	printf("uploadBuffer32..\n");

	//glBindTexture( GL_TEXTURE_2D, texture );
	XPLMBindTexture2d( texture, GL_TEXTURE_2D);

	glTexImage2D( GL_TEXTURE_2D, 0, 4,
					x,	y, 0,
					GL_RGBA, GL_UNSIGNED_BYTE, buffer );

	//you can set other texture parameters if you want
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}








/// Loads TGA file
bool gizmo_LoadTGA_File(int textureId, char *filename)				// Loads A TGA File Into Memory
{

	//{ char cDbg[1024]; sprintf(cDbg, "Gizmo: LoadTGA_File(%i,'%s')\n", textureId, filename); XPLMDebugString( cDbg ); }


	struct stat fsize;
	unsigned int iFileSize = 0;

	if( !stat(filename, &fsize) ){
		iFileSize = fsize.st_size;
	}
	if( iFileSize <= 0 ){
		{ char cDbg[1024]; sprintf(cDbg, "Gizmo: gizmo_LoadTGA_File: Filesize: %i bytes, aborting.\n", iFileSize); XPLMDebugString( cDbg ); }
		return false;
	}




	//{ char cDbg[1024]; sprintf(cDbg, "Gizmo: LoadTGA_File: Allocating blob for read..\n"); XPLMDebugString( cDbg ); }

	unsigned char* blob = (unsigned char*)calloc( 1, iFileSize );

	FILE *fp = fopen( filename, "rb" );
	if( fp != NULL ){
		fread(blob, 1, iFileSize, fp);
		fclose( fp );
	}else{
		{ char cDbg[1024]; sprintf(cDbg, "Gizmo: gizmo_LoadTGA_File: Failed. Couldn't read file: (%s)\n", filename); XPLMDebugString( cDbg ); }
		free( blob );
		return false;
	}

	//{ char cDbg[1024]; sprintf(cDbg, "Gizmo: LoadTGA_File: Handing off to LoadTGA_Blob..\n"); XPLMDebugString( cDbg ); }

		bool ret = gizmo_LoadTGA_DecodeHeaders( textureId, blob, iFileSize );


	free( blob );
	return ret;

}//end: gizmo_LoadTGA_File(..)




bool gizmo_LoadTGA_DecodeHeaders(int textureId, unsigned char* blob, unsigned int blob_size){

	//{ char cDbg[1024]; sprintf(cDbg, "Gizmo: Loading TGA blob: GL tex target: %i, %i bytes.\n", textureId, blob_size); XPLMDebugString( cDbg ); }

	gizmo_TextureImage *texture = new gizmo_TextureImage();

	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	GLubyte		TGAcompare[12];									// Used To Compare TGA Header
	GLubyte		header[6];										// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;									// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;										// Used To Store The Image Size When Setting Aside Ram


	int blob_offset=0;

	//printf("\tdoing memcpy\n");
	memcpy( TGAcompare, blob, 12 );
	blob_offset += 12;

	if( memcmp( TGAheader, TGAcompare, 12 ) != 0 ){
		//file header data does not match target
		{ char cDbg[1024]; sprintf(cDbg, "Gizmo: gizmo_LoadTGA_DecodeHeaders: This blob does not look like TGA data, aborting.\n"); XPLMDebugString( cDbg ); }
		return false;
	}




	//looks like a TGA file, proceed with decoding.
	memcpy( header, blob+blob_offset, 6 );
	blob_offset += 6;


	texture->width  = header[1] * 256 + header[0];				// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];				// Determine The TGA Height	(highbyte*256+lowbyte)

 	if(	texture->width	<=0	||									// Is The Width Less Than Or Equal To Zero
		texture->height	<=0	||									// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))						// Is The TGA 24 or 32 Bit?
	{

			{ char cDbg[1024]; sprintf(cDbg, "Gizmo: LoadTGA_Blob: failed. Texture size or bit depth error.\n"); XPLMDebugString( cDbg ); }

		//something failed, textures not correct size or not expected byte packing format.
		return false;											// Return False
	}



	//printf("\tmaking space for pixel data\n");

	texture->bpp	= header[4];								// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;							// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture->width * texture->height * bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	//this holds ONLY the raw image data, no header info.

	//printf("image size: calculated: %i\n", (int)imageSize);
	//printf(" file size: calculated: %i\n", (int)imageSize+blob_offset);
	//printf("delta blob: %i\n", (int)(blob_size - (imageSize+blob_offset)));

	
	
	bool bRet = false;	//assume text building failse
		//{ char cDbg[1024]; sprintf(cDbg, "Gizmo: LoadTGA_Blob: w/h: %i %i\n", (int)texture->width, (int)texture->height); XPLMDebugString( cDbg ); }


		unsigned char* newBlob = (unsigned char*)calloc(1, imageSize);
		memcpy( newBlob, blob+blob_offset, imageSize ); //header strip.

			//{ char cDbg[1024]; sprintf(cDbg, "Gizmo: LoadTGA_Blob: handing off to LoadTGA_Blob_Core..\n"); XPLMDebugString( cDbg ); }
			
			bRet = gizmo_LoadTGA_Blob_Core( textureId, newBlob, header, imageSize, texture->width, texture->height );

		free( newBlob ); //FIXES: ?? #84



	return bRet;												// Texture Building Went Ok, Return True

}









/// Loads TGA blob from RAM
bool gizmo_LoadTGA_Blob_Core(int textureId, unsigned char* blob, GLubyte* header, unsigned int size, unsigned int width, unsigned int height )				// Loads A TGA File Into Memory
{

	//TextureImage *texture = new TextureImage();
	int bpp = header[4];								// Grab The TGA's Bits Per Pixel (24 or 32)


	
	//check texture dimensions to see if we need to bake it.
    if(
        !(gizmo_isValidTextureSize( width ))  ||  !(gizmo_isValidTextureSize( height ))
        ){
		
		//texture is non power of 2, bake... scroll down for else branch.

			{
				char cDbg[1024]; sprintf(cDbg, "* WARNING: Tex not ^2: %i/%i - will be baked.\n", (int)width, (int)height);
				XPLMDebugString( cDbg );
			}



            {
                int iHalfWidth = width / 2;
                int iHalfHeight = height / 2;

                //how far into the destination row do we start?
                int bakeX = 512 - iHalfWidth;
                int bakeY = 512 - iHalfHeight;
                unsigned int y = 0;//512 - iHalfHeight;
                //y = y * 2;

                int offx=(bakeX*4);
                offx += bakeY * 4096; //vertical centering. yay.

                char* rowBytes = (char*)malloc( width * 3 ); //used to copy bytes out of the image

                for(; y<height; y++){ //loop over the source data

                    memcpy( rowBytes, blob+(y*(width*3)), (width*3) );
                    int rowMarker=0;
                    //memcpy( rowBytes, blob, (texture->width*3) );

                    //write alpha
                    for(unsigned int blockx=0; blockx<1024 ; blockx++ ){
                        //red
                        if( blockx > width ){
                            memset(blob+(offx), 0xFF, 1);
                            offx++;
                        }else{
                            memcpy(blob+(offx), rowBytes+(rowMarker+2), 1 ); offx++; //blue byte is transposed at load
                            //rowMarker++;
                        }
                        //green
                        if( blockx > width ){
                            memset(blob+(offx), 0xFF, 1);
                            offx++;
                        }else{
                            memcpy(blob+(offx), rowBytes+(rowMarker+1), 1 ); offx++;
                            //rowMarker++;
                        }
                        //blue
                        if( blockx > width ){
                            memset(blob+(offx), 0xFF, 1);
                            offx++;
                        }else{
                            memcpy(blob+(offx), rowBytes+(rowMarker), 1 ); offx++; //red byte is transposed at load
                            rowMarker+=3;
                        }

                        //alpha?
                        //rowMarker++; //if the source image blob contains an alpha channel increment this counter
                        if( blockx > width-1 || y > height-1 ){
                            memset(blob+(offx), 0x00, 1); offx++;
                        }else{
                            memset(blob+(offx), 0xFF, 1); offx++;
                        }
                    }

                }
                free( rowBytes );

            }
            //force texture size
            width = 1024;
            height = 1024;
            //force depth
            bpp = 32;





    }else{
		//texture size is valid as loaded ---- not baking

		unsigned char temp;

		if(bpp == 32){

			int imageSize = width * height * 4;

			//{ char cDbg[1024]; sprintf(cDbg, "Gizmo: TGA Tools: NOT Doing 32bit Transpose.\n"); XPLMDebugString( cDbg ); }

			for(GLuint i=0; i<(unsigned int)(imageSize); i+=4)			// Loop Through The Image Data
			{															// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
				temp=blob[i];				// Temporarily Store The Value At Image Data 'i'
				blob[i] = blob[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
				blob[i + 2] = temp;			// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
			}


		}else{

			int imageSize = width * height * 3;

			//{ char cDbg[1024]; sprintf(cDbg, "TGA Tools: Doing 24bit Transpose.\n"); XPLMDebugString( cDbg ); }

			for(GLuint i=0; i<(unsigned int)(imageSize); i+=3)			// Loop Through The Image Data
			{															// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
				temp=blob[i];				// Temporarily Store The Value At Image Data 'i'
				blob[i] = blob[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
				blob[i + 2] = temp;			// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
			}
		}

    }//end check for image dimensions






	if(1){

		GLuint type=GL_RGBA;

		switch(bpp){
			case 24:
				XPLMDebugString("\t 24bit GL upload. RGB src.\n");
				type=GL_RGB;
				break;

			default:
			case 32:
				XPLMDebugString("\t 32bit GL upload. RGBA src.\n");
				type=GL_RGBA;
				break;
		}



		XPLMBindTexture2d( textureId, GL_TEXTURE_2D);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered

		glTexImage2D(
					 GL_TEXTURE_2D,		//target
					 0,					//level
					 GL_RGBA,			//internal format when it hits the video card, RGBA.
					 width,	//
					 height,
					 0,					//border
					 type,			//format of blob loaded from disk, TGA likeds BGR and BGRA
					 GL_UNSIGNED_BYTE,	//type
					 blob //blob
					 );
	}


	return true;												// Texture Building Went Ok, Return True
}
