/*
 *  api_matrix.cpp
 *  Gizmo
 *
 *  Created by Ben Russell on 20/03/11.
 *  Copyright 2011 X-Plugins.com. All rights reserved.
 *
 */




/**
matrix.transform3D( x,y,z,  tx,ty,tz,  tp,tr,th )
 Returns: transformed_x, transformed_y, transformed_z
 
 This function can be used to apply a 3D transformation to a position vector.
 
 API: 11.4+
 */
int gizmo_builtin_matrix_transform3D(lua_State *L){

	//inputs:
	//x,y,z
	//tx,ty,tz, tp,tr,th   ----- **NOTE: gizmo_Transform3D( x,y,z, HEADING, pitch, roll );
	
	//returns: x,y,z
	
	double x = lua_tonumber( L, -9 );
	double y = lua_tonumber( L, -8 );
	double z = lua_tonumber( L, -7 );
	
	double tx = lua_tonumber( L, -6 );
	double ty = lua_tonumber( L, -5 );
	double tz = lua_tonumber( L, -4 );

	double tp = lua_tonumber( L, -3 );
	double tr = lua_tonumber( L, -2 );
	double th = lua_tonumber( L, -1 );
	
	
	const double inVect[3] = { x,y,z };
	const double inTran[6] = { tx,ty,tz, th,tp,tr };
	
	double outVect[3] = {0,0,0};
	
	gizmo_Transform3D( inVect, inTran, outVect ); //**NOTE: gizmo_Transform3D( x,y,z, HEADING, pitch, roll );
	
	lua_pushnumber( L, outVect[0] );
	lua_pushnumber( L, outVect[1] );
	lua_pushnumber( L, outVect[2] );
	
	return 3;

}//matrix.transform3D(...)






// -- This is the core code for the Transform3D call --
void gizmo_mmult(const double inX[3], const double R[3][3], double outX[3]){
	
	for(int n=0; n<3; n++){
		outX[n]=0;
	}
	
	for(int i=0; i<3; i++){
		for(int j=0; j<3; j++){
			outX[i] += R[i][j] * inX[j];
		}
	}
	
}

 
 
#define DEG2RAD 0.0174532925
 
void gizmo_Transform3D(
					const double inLocalCoords[3],	// acf coordinates
					const double inMap[6],			// x, y, z, heading, pitch, roll (X-Plane has angles in degrees)
					double       outCoords[3]
				){
	
	
	//http://en.wikipedia.org/wiki/Rotation_matrix
	//http://inside.mines.edu/~gmurray/ArbitraryAxisRotation/ArbitraryAxisRotation.html
	
	//Make constants easier to handle. 
	//Put angles into radians.
	const double H_rad = -inMap[3]*DEG2RAD; //heading
	const double P_rad =  inMap[4]*DEG2RAD; //pitch
	const double R_rad = -inMap[5]*DEG2RAD; //roll
	
	const double cos_H = cos(H_rad);
	const double sin_H = sin(H_rad);
	const double cos_P = cos(P_rad);
	const double sin_P = sin(P_rad);
	const double cos_R = cos(R_rad);
	const double sin_R = sin(R_rad);
	
	//Pitch rotation matrix
	const double Mx[3][3] = {
		{      1,       0,       0  },
		{      0,   cos_P,  -sin_P  },
		{      0,   sin_P,   cos_P  }
	};
	
	//Heading rotation matrix
	const double My[3][3] = {
		{  cos_H,       0,   sin_H  },
		{      0,       1,       0  },
		{ -sin_H,       0,   cos_H  }
	};
	
	//Roll rotation matrix
	const double Mz[3][3] = {
		{  cos_R,  -sin_R,       0  },
		{  sin_R,   cos_R,       0  },
		{      0,       0,       1  }
	};
	
	//What we want is out = [Mx*My*Mz] * in
	//We will use three consequent multiplications
	//The _only_ correcy sequence is 
	//Mz 
	//Mx 
	//My
	//otherwise you get garbage (see multiplication rules for matrices...)
	
	double step1[3];
	double step2[3];
	
	// step1 = Mz * in
	gizmo_mmult(inLocalCoords, Mz, step1);
	
	// step2 = Mx * step1
	gizmo_mmult(step1,         Mx, step2);
    
	// out = My * step2
	gizmo_mmult(step2,         My, outCoords);
	
	// Finally, apply translation
	for(int i=0; i<3; i++){
		outCoords[i]+=inMap[i]; // X, Y, Z
	}
		
}



