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



#include "X_STL.h"

#include "stdio.h"

#include "StringTools.h"


class XAC3D_Vert{
	public:
		float X;
		float Y;
		float Z;
};

class XAC3D_SurfVert{
	public:
		int vertIndex;
		float U;
		float V;
};


class XAC3D_Surf{
	public:
		int surfaceType; //not used yet ( 0 = polygon, 1 = closedline, 2 = line )
		std::vector<XAC3D_SurfVert*> surfVerts;
};


class XAC3D_Mesh{
	public:
		std::string name;
		std::vector<XAC3D_Vert*> verts;
		std::vector<XAC3D_Surf*> surfaces;
};


class XAC3D_ButtonRegion_Quad{
	public:
		float MinX, MinY;
		float MaxX, MaxY;
		std::string name;

		int hitTest( float x, float y ){
			int r = 0;
				if( (x < this->MaxX) && (x > this->MinX) ){
					if( (y < this->MaxY) && (y > this->MinY) ){
						r = 1; //hit!
					}
				}
			return r;
		};
};



enum loadMode{
	ac_look, //look for a keyword we know
	ac_mesh, //found a mesh tag, start storing verts
	ac_name, //object name...
	ac_uv //found a uv tag, start storing uvs
};


class XAC3DLoader {
	public:

		std::vector<XAC3D_Mesh*> meshes;
		std::vector<XAC3D_ButtonRegion_Quad*> buttons;

		int loadFile( const char* filename ){
			FILE* fh = fopen( filename, "r" );
			if( fh != NULL ){

				std::string sFileContents;

				char tmp[4096];
				//while( ! feof(fh) ){
				while( fgets( tmp, 4096, fh ) != NULL ){
					sFileContents += tmp;
				}

					this->parseString( sFileContents );

				fclose( fh );
				return 1;
			}

			return 0;
		};


		int parseString( std::string sObjBuffer ){

				std::vector<std::string> svLines = StringTools::explodeString( sObjBuffer, "\n" );
				std::vector<std::string> svTokens;

				int iLoaderMode = ac_look;
				XAC3D_Mesh* tmpMesh;
				XAC3D_Vert* tmpVert;
				XAC3D_Surf* tmpSurf;
				XAC3D_SurfVert* tmpSurfVert;


				for( unsigned int lx=0; lx < svLines.size(); lx++ ){
					//printf("line %i: %s\n", lx, svLines[lx].c_str() );

					svTokens = StringTools::explodeString( svLines[lx], " " );

					switch( iLoaderMode ){
						case ac_look:
							break;
						case ac_name:
							tmpMesh->name = svTokens[0];
							//printf("Found mesh name token: %s\n", svTokens[0].c_str());
							iLoaderMode = ac_look;
							break;
						case ac_mesh:
							//verts! nom.
							if( svTokens[0] == "numsurf" ){
								iLoaderMode = ac_look;
							}else if( svTokens.size() == 3 ){
								//printf("found vert: %s %s %s\n", svTokens[0].c_str(), svTokens[1].c_str(), svTokens[2].c_str() );

								tmpVert = new XAC3D_Vert();
								tmpVert->X = atof( svTokens[0].c_str() );
								tmpVert->Y = atof( svTokens[1].c_str() );
								tmpVert->Z = atof( svTokens[2].c_str() );

								tmpMesh->verts.push_back( tmpVert );

							}

							break;
						case ac_uv:
							//uvs! nom.

							if( svTokens[0] == "kids" ){
								iLoaderMode = ac_look;
							}else if( svTokens.size() == 3 ){
								//vert index
								//u
								//v
								//printf("found uv: %s %s %s\n", svTokens[0].c_str(), svTokens[1].c_str(), svTokens[2].c_str() );

								int iVertIndex = atoi(svTokens[0].c_str());

								tmpSurfVert = new XAC3D_SurfVert();
								tmpSurfVert->vertIndex = iVertIndex;

								tmpSurfVert->U = atof( svTokens[1].c_str() );
								tmpSurfVert->V = atof( svTokens[2].c_str() );

								tmpSurf->surfVerts.push_back( tmpSurfVert );

								//this->meshes[ this->meshes.size()-1 ]->surfaces[ this->meshes[ this->meshes.size()-1 ]->surfaces.size()-1 ]->verts[ iVertIndex ]->U = atof( svTokens[1].c_str() );
								//this->meshes[ this->meshes.size()-1 ]->surfaces[ this->meshes[ this->meshes.size()-1 ]->surfaces.size()-1 ]->verts[ iVertIndex ]->V = atof( svTokens[2].c_str() );

							}

							break;
					}



					if( svTokens[0] == "OBJECT" ){
						//new sub object
						if( svTokens[1] != "world" ){
							tmpMesh = new XAC3D_Mesh();
							this->meshes.push_back( tmpMesh );
							//printf("Created a new mesh instance...\n");
						}

					}else if( svTokens[0] == "data" ){
						iLoaderMode = ac_name; //next line in the file will be the mesh name.
					/*
					}else if( svTokens[0] == "name" ){
						//store mesh name
						tmpMesh->name = svTokens[1];
						printf("Found mesh name token: %s\n", svTokens[1].c_str());
					*/

					}else if( svTokens[0] == "SURF" ){
						//new surface in the mesh.
						tmpSurf = new XAC3D_Surf();
						tmpMesh->surfaces.push_back( tmpSurf );
						//this->meshes[ this->meshes.size()-1 ]->surfaces.push_back( tmpSurf );

					}else if( svTokens[0] == "numvert" ){
						//leave verts..
						iLoaderMode = ac_mesh;

					}else if( svTokens[0] == "refs" ){
						//leave UVs
						iLoaderMode = ac_uv;

					}

				}//end parse lines.


				parseButtons();

			return 1;
		};





		void parseButtons(){

			XAC3D_Vert* tmpVert;
			XAC3D_SurfVert* tmpSurfVert;

			XAC3D_ButtonRegion_Quad* tmpButton;

			//printf("rendering..\n");
			for( unsigned int mx=0; mx < this->meshes.size(); mx++ ){

				//printf("\tmesh: %i: %s\n", mx, acLoader.meshes[mx]->name.c_str() );
				for( unsigned int sx=0; sx < this->meshes[mx]->surfaces.size(); sx++ ){

					std::vector<std::string> svMeshNameTokens = StringTools::explodeString( this->meshes[mx]->name, "_" );

					//button click chekc
					//if( acLoader.meshes[mx]->name == "btn_terr" ){
					if( svMeshNameTokens.size() == 2 && svMeshNameTokens[0] == "btn" ){

						float fBtnMinX=99999, fBtnMinY=99999, fBtnMaxX=-99999, fBtnMaxY=-99999; //calculate bounds... fun.

						//determine button bounds.
						for( unsigned int vx=0; vx < this->meshes[mx]->surfaces[sx]->surfVerts.size(); vx++ ){
							tmpSurfVert = this->meshes[mx]->surfaces[sx]->surfVerts[vx];
							tmpVert = this->meshes[mx]->verts[tmpSurfVert->vertIndex];

							//X bounds
							if( tmpVert->X < fBtnMinX ){
								fBtnMinX = tmpVert->X;
							}
							if( tmpVert->X > fBtnMaxX ){
								fBtnMaxX = tmpVert->X;
							}

							//Y bounds
							if( tmpVert->Y < fBtnMinY ){
								fBtnMinY = tmpVert->Y;
							}
							if( tmpVert->Y > fBtnMaxY ){
								fBtnMaxY = tmpVert->Y;
							}
						}//end iterate button verts


						//button has been parsed, store.
						tmpButton = new XAC3D_ButtonRegion_Quad();
							tmpButton->MinX = fBtnMinX;
							tmpButton->MinY = fBtnMinY;

							tmpButton->MaxX = fBtnMaxX;
							tmpButton->MaxY = fBtnMaxY;

							tmpButton->name = svMeshNameTokens[1];

						this->buttons.push_back( tmpButton );

					}//end if button check

				}//end loop surfaces.

			}//end loop meshes

			//printf("Parsed %i buttons.\n", this->buttons.size()-1 );

		};



		void draw( int texture, float centerX, float centerY ){

			glPushAttrib(GL_ALL_ATTRIB_BITS);

			glDisable(GL_CULL_FACE); //essential for drawing AC3D shit
			glPushMatrix();

			//glTranslatef( 512, 384, 0 );
			glTranslatef( centerX, centerY, 0 );

			//glScalef( 2.f, 2.f, 2.f );

			//glBindTexture(GL_TEXTURE_2D,texture);
			XPLMBindTexture2d( texture, GL_TEXTURE_2D );


				XAC3D_Vert* tmpVert;
				XAC3D_SurfVert* tmpSurfVert;

				//printf("rendering..\n");
				for( unsigned int mx=0; mx < this->meshes.size(); mx++ ){


						glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );


						//printf("\tmesh: %i: %s\n", mx, this->meshes[mx]->name.c_str() );
						for( unsigned int sx=0; sx < this->meshes[mx]->surfaces.size(); sx++ ){

							//printf("\tsurf: %i\n", sx);

							glBegin( GL_POLYGON );
								//glColor4f( 1.0, 1.0, 1.0, 1.0 );

								for( unsigned int vx=0; vx < this->meshes[mx]->surfaces[sx]->surfVerts.size(); vx++ ){
									tmpSurfVert = this->meshes[mx]->surfaces[sx]->surfVerts[vx];
									tmpVert = this->meshes[mx]->verts[tmpSurfVert->vertIndex];

									//printf("\t vert: %i\n", vx);
									//printf("\t %.3f %.3f %.3f - %.3f %.3f\n", tmpVert->X*200.f, (tmpVert->Y*200.f), tmpVert->Z*200.f, tmpSurfVert->U, tmpSurfVert->V );

									glTexCoord2f(tmpSurfVert->U, tmpSurfVert->V);
									glVertex2f( tmpVert->X, (tmpVert->Y) );
								}


							glEnd();

						}


				}

			glPopMatrix();
			glPopAttrib();

		};


};



