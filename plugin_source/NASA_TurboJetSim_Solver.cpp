/*
				 EngineSim - Design and Wind Tunnel Mode
	
	 Program to perform turbomachinery design and analysis
				a)	dry turbojet
				b)	afterburning turbojet
				c)	turbofan with separate nozzle
				d)	ramjet

					 Version 1.7a	- 26 Oct 05

						 Written by Tom Benson
						NASA Glenn Research Center

>							NOTICE
>This software is in the Public Domain.It may be freely copied and used in
>non-commercial products, assuming proper credit to the author is given.IT
>MAY NOT BE RESOLD.If you want to use the software for commercial
>products, contact the author.
>No copyright is claimed in the United States under Title 17, U. S. Code.
>This software is provided "as is" without any warranty of any kind, either
>express, implied, or statutory, including, but not limited to, any warranty
>that the software will conform to specifications, any implied warranties of
>merchantability, fitness for a particular purpose, and freedom from
>infringement, and any warranty that the documentation will conform to the
>program, or any warranty that the software will be error free.
>In no event shall NASA be liable for any damages, including, but not
>limited to direct, indirect, special or consequential damages, arising out
>of, resulting from, or in any way connected with this software, whether or
>not based on warranty, contract, tort or otherwise, whether or not injury
>was sustained by persons or property or otherwise, and whether or not loss
>was sustained from, or arose out of the results of, or use of, the software
>or services provided hereunder.
 
	New Test- 
				 * size for the portal
				 * re-size graphics
				 * re-size the .gifs
				 * change outputs
				 *	add pexit, pfexit and M2 to output
				 * update EngineSimU
				 * update application
				 * fix translation for viewer
				 * correct gross thrust calculation
				 * change view box for title


	Old Test-
					clean up 
 
													 TJB 26 Oct 05
*/


// 2008-08-21 - Ported Solver class and support items to C++ for use in XLua for the X-Plane flight simulator.









#include "NASA_TurboJetSim_Solver.h"



	NASA_TurboJetSim_Solver::NASA_TurboJetSim_Solver() {
	
		//converter: degrees to radians
		convdr = 3.14515926/180.;
	 
		//lunits = 1; //metric units flag
	
	}

	 void NASA_TurboJetSim_Solver::compute() { 
 
		numeng = 1 ;
		fireflag = 0 ;

		getFreeStream ();
 
		getThermo() ;
 
		if (inflag == 0) getGeo (); /*determine engine size and geometry*/
		if (inflag == 1) {
			if (iEngineType < 3)a8 = a8d * sqrt(trat[7]) / prat[7] ;
		}

		 //br: view.getDrawGeo() ;
	
		getPerform() ;

		//br: out.box.loadOut() ;
		//br: out.vars.loadOut() ;
		//br: in.fillBox() ;
 
		/* -plotter disabled
		if (plttyp >= 3 && plttyp <= 7){
			//br: out.plot.loadPlot () ;
			//br: out.plot.repaint() ;
		}
 		*/
		//br: view.repaint() ;
	
		//if (inflag == 0) myDesign() ;
	 }

	 void NASA_TurboJetSim_Solver::setDefaults() {
		int i ;
	
		move = 0;
		inptype = 0 ;
		siztype = 0 ;
		lunits = 0 ;//0 English, 1 Metric, 2 Experimental % Change
		lconv1 = 1.0 ; lconv2 = 1.0 ;fconv= 1.0 ;mconv1 = 1.0 ;
		pconv= 1.0 ; econv= 1.0 ;aconv= 1.0 ;bconv= 1.0 ;
		mconv2 = 1.0 ; dconv= 1.0 ;flconv = 1.0 ;econv2 = 1.0 ;
		tconv= 1.0 ;tref = 459.6;
		g0 = g0d = 32.2 ;

		 //counter = 0 ; //br: plot var
		showcom = 0 ;
		plttyp= 2 ;
		pltkeep = 0 ;
		iEngineType= 0 ;
		inflag= 0 ;
		varflag = 0 ;
		pt2flag = 0 ;
		wtflag= 0 ;
		fireflag = 0 ;
		gama = 1.4 ;
		gamopt = 1 ;
		u0d = 0.0 ;
		dFlightAltitude = 0.0 ;
		dFlightThrottle = 100. ;

		for (i=0; i<=19; ++i) {
			 trat[i] = 1.0 ;
			 tt[i]	= 518.6 ;
			 prat[i] = 1.0 ;
			 pt[i]	= 14.7 ;
			 eta[i]= 1.0 ;
		}
		tt[4] = tt4 = tt4d = 2500. ;
		tt[7] = tt7 = tt7d = 2500. ;
		prat[3] = p3p2d = 8.0 ;
		prat[13] = p3fp2d = 2.0 ;
		dBypassRatio = 1.0 ;
		iAfterBurnerOn = 0 ;
	
		fueltype = 0 ;
		fhvd = fhv = 18600. ;
		a2d = a2 = acore = 2.0 ;
		diameng =sqrt(4.0 * a2d / 3.14159) ;
		acap = .9*a2 ;
		a8rat = .35 ;
		a8 = .7 ;
		a8d = .40 ;
		arsched = 0 ;
		afan = 2.0 ;
		a4 = .418 ;
 
		athsched = 1 ;
		aexsched = 1 ;
		arthmn = 0.1;	 arthmx = 1.5 ;
		arexmn = 1.0;	 arexmx = 10.0 ;
		arthd = arth = .4 ;
		arexit = arexitd = 3.0 ;
 
		u0min= 0.0 ;	u0max = 1500.;
		altmin = 0.0 ;	altmax = 60000. ;
		thrmin = 30;	 thrmax = 100 ;
		etmin= .5;	 etmax= 1.0 ;
		cprmin = 1.0;	cprmax = 50.0 ;
		bypmin = 0.0;	bypmax = 10.0 ;
		fprmin = 1.0;	fprmax = 2.0 ;
		t4min = 1000.0;	t4max = 3200.0 ;
		t7min = 1000.0;	t7max = 4000.0 ;
		a8min = 0.1;	a8max = 0.4 ;
		a2min = .001;		a2max = 50.;
		diamin =sqrt(4.0*a2min/3.14159);
		diamax =sqrt(4.0*a2max/3.14159);
		pmax = 20.0;tmin = -100.0 + tref;tmax = 100.0 + tref ;
		vmn1 = u0min ;	 vmx1 = u0max ;
		vmn2 = altmin ;	vmx2 = altmax ;
		vmn3 = thrmin ;	vmx3 = thrmax ;
		vmn4 = arexmn ;	vmx4 = arexmx ;

		xtrans = 125.0 ;
		ytrans = 115.0 ;
		factor = 35. ;
		sldloc = 75 ;

		xtranp = 80.0 ;
		ytranp = 180.0 ;
		factp = 27. ;
		sldplt = 138 ;

		weight = 1000. ;
		minlt = 1; dinlt = 170.2 ;tinlt = 900. ;
		mfan = 2;dfan = 293.02 ; tfan = 1500. ;
		mcomp = 2; dcomp = 293.02 ; tcomp = 1500. ;
		mburner = 4 ; dburner= 515.2 ; tburner = 2500. ;
		mturbin = 4 ; dturbin = 515.2 ; tturbin = 2500. ;
		mnozl = 3; dnozl = 515.2 ; tnozl = 2500. ;
		mnozr = 5; dnozr = 515.2 ; tnozr = 4500. ;
		ncflag = 0 ; ntflag = 0 ;
		return ;
	 }

/*
	 void NASA_TurboJetSim_Solver::myDesign() {

		ensav = iEngineType ;
		absav = iAfterBurnerOn ;
		flsav = fueltype ;
		fhsav = fhvd/flconv ;
		t4sav = tt4d/tconv ;
		t7sav = tt7d/tconv ;
		p3sav = p3p2d ;
		p3fsav = p3fp2d ;
		bysav = dBypassRatio;
		acsav = acore ;
		a2sav = a2d/aconv;
		a4sav = a4 ;
		a4psav = a4p;
		gamsav = gama ;
		gamosav = gamopt;
		ptfsav = pt2flag ;
		et2sav = eta[2] ;
		pr2sav = prat[2];
		pr4sav = prat[4];
		et3sav = eta[3];
		et4sav = eta[4];
		et5sav = eta[5];
		et7sav = eta[7];
		et13sav = eta[13];
		a8sav = a8d/aconv ; 
		a8mxsav = a8max/aconv ;
		a8rtsav = a8rat ;
	
		u0mxsav = u0max/lconv2 ;
		u0sav = u0d/lconv2 ;
		altsav = dFlightAltitude/lconv1 ;
		arssav = arsched ;
 
		wtfsav = wtflag; wtsav = weight;
		minsav = minlt; dinsav = dinlt;tinsav = tinlt;
		mfnsav = mfan;dfnsav = dfan;	tfnsav = tfan;
		mcmsav = mcomp; dcmsav = dcomp;tcmsav = tcomp;
		mbrsav = mburner; dbrsav = dburner; tbrsav = tburner;
		mtrsav = mturbin; dtrsav = dturbin; ttrsav = tturbin;
		mnlsav = mnozl; dnlsav = dnozl; tnlsav = tnozl;
		mnrsav = mnozr; dnrsav = dnozr; tnrsav = tnozr;
		ncsav = ncflag; ntsav = ntflag;

		if (iEngineType == 3) {
			arthsav = athsched ;
			arxsav = aexsched ;
			artsav = arthd ;
			arexsav = arexitd ;
		}
	
		return ;
	 }
*/

/*
	 void NASA_TurboJetSim_Solver::loadMine() {

		iEngineType = ensav ;
		iAfterBurnerOn = absav ;
		fueltype = flsav ;
		fhvd = fhv = fhsav ;
		tt[4] = tt4 = tt4d = t4sav ;
		tt[7] = tt7 = tt7d = t7sav ;
		prat[3] = p3p2d = p3sav ;
		prat[13] = p3fp2d = p3fsav ;
		dBypassRatio = bysav;
		acore = acsav ;
		afan = acore * (1.0 + dBypassRatio) ;
		a2d = a2 = a2sav ;
		diameng =sqrt(4.0 * a2d / 3.14159) ;
		a4 = a4sav ;
		a4p = a4psav ;
		acap = .9*a2 ;
		gama = gamsav ;
		gamopt = gamosav ;
		pt2flag = ptfsav ;
		eta[2] = et2sav ;
		prat[2] = pr2sav ;
		prat[4] = pr4sav ;
		eta[3] = et3sav ;
		eta[4] = et4sav ;
		eta[5] = et5sav ;
		eta[7] = et7sav ;
		eta[13] = et13sav ;
		a8d = a8sav ; 
		a8max = a8mxsav ;
		a8rat = a8rtsav ;
	
		u0max = u0mxsav ;
		u0d = u0sav ;
		dFlightAltitude = altsav ;
		arsched = arssav ;
	
		wtflag = wtfsav; weight = wtsav;
		minlt = minsav; dinlt = dinsav;tinlt = tinsav;
		mfan = mfnsav;dfan = dfnsav;	tfan = tfnsav;
		mcomp = mcmsav; dcomp = dcmsav; tcomp = tcmsav;
		mburner = mbrsav; dburner = dbrsav; tburner = tbrsav;
		mturbin = mtrsav; dturbin = dtrsav; tturbin = ttrsav;
		mnozl = mnlsav; dnozl = dnlsav; tnozl = tnlsav;
		mnozr = mnrsav; dnozr = dnrsav; tnozr = tnrsav;
		ncflag = ncsav; ntflag = ntsav;

		if (iEngineType == 3) {
			athsched = arthsav;
			aexsched = arxsav ;
			arthd = artsav ;
			arexitd = arexsav;
		}
	
		//br: con.setPanl() ;
		return ;
	 }
*/

	 void NASA_TurboJetSim_Solver::loadCF6() {

		iEngineType = 2 ; //turbo fan
		iAfterBurnerOn = 0 ; //afterburner no
		fueltype = 0; //Jet A (burner tab)
		fhvd = fhv = 18600. ;//FHV BTU/lb (burner tab)
		tt[4] = tt4 = tt4d = 2500. ; //temp limits
		tt[7] = tt7 = tt7d = 2500. ; //temp limits
		prat[3] = p3p2d = 21.86 ; //pressure ratio of compressor stage
		prat[13] = p3fp2d = 1.745 ; //pressure ratio of fan stage
		dBypassRatio = 3.3 ; //bypass ratio of fan stage

		acore = 6.965 ; //core size?
		afan = acore * (1.0 + dBypassRatio) ;
		a2d = a2 = afan ;
		diameng =sqrt(4.0 * a2d / 3.14159) ;
		
		a4 = .290 ;
		a4p = 1.131 ;
		acap = .9*a2 ;
		
		gama = 1.4 ;//gam(T)
		gamopt = 1 ;//activate (T) version of gamma??
		pt2flag = 0 ;
		eta[2] = 1.0 ;//efficieny of ??
		prat[2] = 1.0 ;//pressure ratios?
		prat[4] = 1.0 ; 
		eta[3] = .959 ; //efficieny of compressor stage
		eta[4] = .984 ; //efficieny of burner stage
		eta[5] = .982 ; //efficieny of turbine stage
		eta[7] = 1.0 ;
		eta[13] = 1.0 ;
		a8d = 2.436 ; 
		a8max = .35 ; //nozzle?
		a8rat = .35 ; //nozzle?
	
		u0max = 1500. ;
		u0d = 0.0 ;
		dFlightAltitude = 0.0 ;
		arsched = 0 ;
	
		wtflag = 0; 
		weight = 8229.; //computed weight - size tab
		
		 
		 //materials
		 minlt = 1; //Aluminium
		 dinlt = 170.;tinlt = 900.; //inlet
		
		 mfan = 2;//Titanium
		 dfan = 293.;	tfan = 1500.; //fan
		
		 mcomp = 0; //custom material
		 dcomp = 293.; tcomp = 1600.; //compressor
		
		 mburner = 4;// nickel alloy
		dburner = 515.;//density
		tburner = 2500.; //temp limit
		
		 mturbin = 4; //turbine - nickel alloy
		dturbin = 515.; 
		tturbin = 2500.;
		
		mnozl = 3; //nozzle - nickel alloy
		dnozl = 515.; 
		tnozl = 2500.;
		
		 
		 
		 
		ncflag = 0; 
		ntflag = 0;

		//br: con.setPanl() ;
		return ;
	 }

	 void NASA_TurboJetSim_Solver::loadJ85() {
	
		iEngineType = 0 ; //turbojet
		iAfterBurnerOn = 0 ; 
		fueltype = 0; 
		fhvd = fhv = 18600. ;
		tt[4] = tt4 = tt4d = 2260. ;
		tt[7] = tt7 = tt7d = 4000. ;
		prat[3] = p3p2d = 8.3 ;
		prat[13] = p3fp2d = 1.0 ;
		dBypassRatio = 0.0 ;
		a2d = a2 = acore = 1.753 ;
		diameng =sqrt(4.0 * a2d / 3.14159) ;
		afan = acore * (1.0 + dBypassRatio) ;
		a4 = .323 ;
		a4p = .818 ;
		acap = .9*a2 ;
		gama = 1.4 ;
		gamopt = 1 ;
		pt2flag = 0 ;
		eta[2] = 1.0 ;
		prat[2] = 1.0 ;
		prat[4] = .85 ;
		eta[3] = .822 ;
		eta[4] = .982 ;
		eta[5] = .882;
		eta[7] = .97 ;
		eta[13] = 1.0 ;
		a8d = .818 ; 
		a8max = .467 ;
		a8rat = .467 ;

		u0max = 1500. ;
		u0d = 0.0 ;
		dFlightAltitude = 0.0 ;
		arsched = 1 ;

		wtflag = 0; weight = 561.;
		minlt = 1; dinlt = 170.;tinlt = 900.;
		mfan = 2;dfan = 293.;	tfan = 1500.;
		mcomp = 2; dcomp = 293.; tcomp = 1500.;
		mburner = 4; dburner = 515.; tburner = 2500.;
		mturbin = 4; dturbin = 515.; tturbin = 2500.;
		mnozl = 5; dnozl = 600.; tnozl = 4100.;
		ncflag = 0; ntflag = 0;

		//br: con.setPanl() ;
		return ;
	 }

	 void NASA_TurboJetSim_Solver::loadF100() {
	
		iEngineType = 1 ; //afterburner
		iAfterBurnerOn = 1 ; //burner on
		fueltype = 0;
		fhvd = fhv = 18600. ;
		tt[4] = tt4 = tt4d = 2499. ;
		tt[7] = tt7 = tt7d = 3905. ;
		prat[3] = p3p2d = 20.04 ;
		prat[13] = p3fp2d = 1.745 ;
		dBypassRatio = 0.0 ;
		a2d = a2 = acore = 6.00 ;
		diameng =sqrt(4.0 * a2d / 3.14159) ;
		afan = acore * (1.0 + dBypassRatio) ;
		a4 = .472 ;
		a4p = 1.524 ;
		acap = .9*a2 ;
		gama = 1.4 ;
		gamopt = 1 ;
		pt2flag = 0 ;
		eta[2] = 1.0 ;
		prat[2] = 1.0 ;
		prat[4] = 1.0 ;
		eta[3] = .959 ;
		eta[4] = .984 ;
		eta[5] = .982 ;
		eta[7] = .92 ;
		eta[13] = 1.0 ;
		a8d = 1.524 ; 
		a8max = .335 ;
		a8rat = .335 ;
	
		u0max = 1500. ;
		u0d = 0.0 ;
		dFlightAltitude = 0.0 ;
		arsched = 0 ;

		wtflag = 0; weight = 3875.;
		minlt = 1; dinlt = 170.;tinlt = 900.;
		mfan = 2;dfan = 293.;	tfan = 1500.;
		mcomp = 2; dcomp = 293.; tcomp = 1500.;
		mburner = 4; dburner = 515.; tburner = 2500.;
		mturbin = 4; dturbin = 515.; tturbin = 2500.;
		mnozl = 5; dnozl = 400.2 ; tnozl = 4100. ;
		ncflag = 0; ntflag = 0;

		//br: con.setPanl() ;
		return ;
	 }

	 void NASA_TurboJetSim_Solver::loadRamj() {
	
		iEngineType = 3 ; //ramjet
		athsched = 1;
		aexsched = 1 ;
		 arthd = .4 ;//burner: A7/A2
		 arexitd = 3.0 ;//burner: A8/A7
		iAfterBurnerOn = 0 ;
		fueltype = 0;
		fhvd = fhv = 18600. ;
		tt[4] = tt4 = tt4d = 4000. ;
		t4max = 4500. ;
		tt[7] = tt7 = tt7d = 4000. ;
		prat[3] = p3p2d = 1.0 ;
		prat[13] = p3fp2d = 1.0 ;
		dBypassRatio = 0.0 ;
		a2d = a2 = acore = 1.753 ;
		diameng =sqrt(4.0 * a2d / 3.14159) ;
		afan = acore * (1.0 + dBypassRatio) ;
		a4 = .323 ;
		a4p = .818 ;
		acap = .9*a2 ;
		gama = 1.4 ;
		gamopt = 1 ;
		pt2flag = 0 ;
		eta[2] = 1.0 ;
		prat[2] = 1.0 ;
		prat[4] = 1.0 ;
		eta[3] = 1.0 ;
		eta[4] = .982 ;
		eta[5] = 1.0 ;
		eta[7] = 1.0 ;
		eta[13] = 1.0 ;
		a8 = a8d = 2.00 ; 
		a8max = 15. ;
		a8rat = 4.0 ;
		a7 = .50 ;

		u0max = 4500. ;
		u0d = 2200.0 ;
		dFlightAltitude = 10000.0 ;
		arsched = 0 ;

		wtflag = 0; weight = 976.;
		minlt = 2; dinlt = 293.;tinlt = 1500.;
		mfan = 2;dfan = 293.;	tfan = 1500.;
		mcomp = 2; dcomp = 293.; tcomp = 1500.;
		mburner = 7; dburner = 515.; tburner = 4500.;
		mturbin = 4; dturbin = 515.; tturbin = 2500.;
		mnozr = 5; dnozr = 515.2 ; tnozr = 4500. ;
		ncflag = 0; ntflag = 0;

		//br: con.setPanl() ;
		return ;
	 }

	 void NASA_TurboJetSim_Solver::getFreeStream() {
		rgas = 1718. ;				/* ft2/sec2 R */
		if (inptype >= 2) {
		 ps0 = ps0 * 144. ;
		}
		if (inptype <= 1) {			/* input altitude */
		 alt = dFlightAltitude / lconv1;
		 if (alt < 36152. ) {
			ts0 = 518.6 - 3.56 * alt / 1000. ;
			ps0 = 2116. *pow(ts0/518.6, 5.256) ;
		 }
		 if (alt >= 36152. && alt <= 82345.) {	// Stratosphere
			ts0 = 389.98 ;
			ps0 = 2116. * .2236 *
				 exp((36000.-alt)/(53.35*389.98)) ;
		 }
		 if (alt >= 82345.) {
			ts0 = 389.98 + 1.645 * (alt-82345)/1000. ;
			ps0 = 2116. *.02456 *pow(ts0/389.98,-11.388) ;
		 }
		}
		a0 =sqrt(gama*rgas*ts0) ;			 /* speed of sound ft/sec */
		if (inptype == 0 || inptype == 2) {			/* input speed*/
		u0 = u0d /lconv2 *5280./3600. ;			/* airspeed ft/sec */
		fsmach = u0/a0 ;
		q0 = gama / 2.0*fsmach*fsmach*ps0 ;
		}
		if (inptype == 1 || inptype == 3) {			/* input mach */
		u0 = fsmach * a0 ;
		u0d = u0 * lconv2 / 5280.* 3600. ;	/* airspeed ft/sec */
		q0 = gama / 2.0*fsmach*fsmach*ps0 ;
		}
		if (u0 > .0001) rho0 = q0 /(u0*u0) ;
		else rho0 = 1.0 ;

		tt[0] = ts0*(1.0 + .5 * (gama -1.0) * fsmach * fsmach) ;
		pt[0] = ps0 *pow(tt[0]/ts0,gama/(gama-1.0)) ;
		ps0 = ps0 / 144. ;
		pt[0] = pt[0] / 144. ;
		cpair = getCp(tt[0],gamopt);			/*BTU/lbm R */
		tsout = ts0 ;
		psout = ps0 ;
	
		return ;
	 }
 
void NASA_TurboJetSim_Solver::getThermo() {
	double dela,t5t4n,deriv,delan,m5;
	double delhc,delhht,delhf,delhlt;
	double deltc,deltht,deltf,deltlt;
	int itcount,index;
	float fl1;
	int i1;
		
	/*	inlet recovery*/
	if (pt2flag == 0) {					/*	 mil spec	*/
		if (fsmach > 1.0 ) {		/* supersonic */
			prat[2] = 1.0 - 0.075 * pow(fsmach - 1.0, 1.35) ;
		}else{
			prat[2] = 1.0 ;
		}
		
		eta[2] = prat[2] ;
		fl1 = filter3(prat[2]) ;
		//br: in.inlet.left.f1.setText(String.valueOf(fl1)) ;
		i1 = (int) (((prat[2] - etmin)/(etmax-etmin))*1000.0) ;
		//br: in.inlet.right.s1.setValue(i1) ;
	}else {						
		/* enter value */
		prat[2] = eta[2];		
	}
	
	/* protection for overwriting input */
	if (eta[3] < 0.5) eta[3] = 0.5;
	if (eta[5] < 0.5) eta[5] = 0.5;
		
	trat[7] = 1.0;
	prat[7] = 1.0;
	tt[2] = tt[1] = tt[0] ;
	pt[1] = pt[0] ;
	gam[2] = getGama(tt[2],gamopt) ;
	cp[2]= getCp(tt[2],gamopt);
	pt[2] = pt[1] * prat[2];
		
	/* design - p3p2 specified - tt4 specified */
	if( inflag == 0 ){
	
		if( iEngineType <= EngineType_Afterburner ){			/* turbojet */
			prat[3] = p3p2d;	/* core compressor */
			
			if (prat[3] < .5) prat[3] = .5 ;
			
			delhc = (cp[2]*tt[2]/eta[3])*
					( pow(prat[3],(gam[2]-1.0)/gam[2])-1.0) ;
			
			deltc = delhc / cp[2] ;
			pt[3] = pt[2] * prat[3] ;
			tt[3] = tt[2] + deltc ;
			trat[3] = tt[3]/tt[2] ;
			gam[3] = getGama(tt[3],gamopt) ;
			cp[3]= getCp(tt[3],gamopt);
			tt[4] = tt4 * dFlightThrottle/100.0 ;
			gam[4] = getGama(tt[4],gamopt) ;
			cp[4]= getCp(tt[4],gamopt);
			trat[4] = tt[4] / tt[3] ;
			pt[4] = pt[3] * prat[4] ;
			delhht = delhc ;
			deltht = delhht / cp[4] ;
			tt[5] = tt[4] - deltht ;
			gam[5] = getGama(tt[5],gamopt) ;
			cp[5]= getCp(tt[5],gamopt);
			trat[5] = tt[5]/tt[4] ;
			
			prat[5] =pow((1.0-delhht/cp[4]/tt[4]/eta[5]),
						(gam[4]/(gam[4]-1.0))) ;
			
			pt[5] = pt[4] * prat[5];
			
			/* fan conditions */
			prat[13] = 1.0 ;
			trat[13] = 1.0 ;
			tt[13]	= tt[2] ;
			pt[13]	= pt[2] ;
			gam[13]= gam[2] ;
			cp[13]	= cp[2] ;
			prat[15] = 1.0 ;
			pt[15]	= pt[5] ;
			trat[15] = 1.0 ;
			tt[15]	= tt[5] ;
			gam[15]= gam[5] ;
			cp[15]	= cp[5] ;
		}
	
		if(iEngineType == EngineType_TurboFan) {						 /* turbofan */
			prat[13] = p3fp2d ;
			
			if (prat[13] < .5) prat[13] = .5 ;
			
			delhf = (cp[2]*tt[2]/eta[13])*
					( pow(prat[13],(gam[2]-1.0)/gam[2])-1.0) ;
			deltf = delhf / cp[2] ;
			tt[13] = tt[2] + deltf ;
			pt[13] = pt[2] * prat[13] ;
			trat[13] = tt[13]/tt[2] ;
			gam[13] = getGama(tt[13],gamopt) ;
			cp[13]= getCp(tt[13],gamopt);
			prat[3] = p3p2d ;					/* core compressor */
			
			if (prat[3] < .5) prat[3] = .5 ;
			
			delhc = (cp[13]*tt[13]/eta[3])*
					( pow(prat[3],(gam[13]-1.0)/gam[13])-1.0) ;
			deltc = delhc / cp[13] ;
			tt[3] = tt[13] + deltc ;
			pt[3] = pt[13] * prat[3] ;
			trat[3] = tt[3]/tt[13] ;
			gam[3] = getGama(tt[3],gamopt) ;
			cp[3]= getCp(tt[3],gamopt);
			tt[4] = tt4 * dFlightThrottle/100.0 ;
			pt[4] = pt[3] * prat[4] ;
			gam[4] = getGama(tt[4],gamopt) ;
			cp[4]= getCp(tt[4],gamopt);
			trat[4] = tt[4]/tt[3] ;
			delhht = delhc ;
			deltht = delhht / cp[4] ;
			tt[5] = tt[4] - deltht ;
			gam[5] = getGama(tt[5],gamopt) ;
			cp[5]= getCp(tt[5],gamopt);
			trat[5] = tt[5]/tt[4] ;
			prat[5] =pow((1.0-delhht/cp[4]/tt[4]/eta[5]),
						(gam[4]/(gam[4]-1.0))) ;
			pt[5] = pt[4] * prat[5] ;
			delhlt = (1.0 + dBypassRatio) * delhf ;
			deltlt = delhlt / cp[5] ;
			tt[15] = tt[5] - deltlt ;
			gam[15] = getGama(tt[15],gamopt) ;
			cp[15]= getCp(tt[15],gamopt);
			trat[15] = tt[15]/tt[5] ;
			prat[15] =pow((1.0-delhlt/cp[5]/tt[5]/eta[5]),
						(gam[5]/(gam[5]-1.0))) ;
			pt[15] = pt[5] * prat[15] ;
		}

		if (iEngineType == EngineType_Ramjet) {			/* ramjet */
			prat[3] = 1.0 ;					
			pt[3] = pt[2] * prat[3] ;
			tt[3] = tt[2] ;
			trat[3] = 1.0 ;
			gam[3] = getGama(tt[3],gamopt) ;
			cp[3]= getCp(tt[3],gamopt);
			tt[4] = tt4 * dFlightThrottle/100.0 ;
			gam[4] = getGama(tt[4],gamopt) ;
			cp[4]= getCp(tt[4],gamopt);
			trat[4] = tt[4] / tt[3] ;
			pt[4] = pt[3] * prat[4] ;
			tt[5] = tt[4] ;
			gam[5] = getGama(tt[5],gamopt) ;
			cp[5]= getCp(tt[5],gamopt);
			trat[5] = 1.0 ;
			prat[5] = 1.0 ;
			pt[5] = pt[4] ;
											/* fan conditions */
			prat[13] = 1.0 ;
			trat[13] = 1.0 ;
			tt[13]	= tt[2] ;
			pt[13]	= pt[2] ;
			gam[13]= gam[2] ;
			cp[13]	= cp[2] ;
			prat[15] = 1.0 ;
			pt[15]	= pt[5] ;
			trat[15] = 1.0 ;
			tt[15]	= tt[5] ;
			gam[15]= gam[5] ;
			cp[15]	= cp[5] ;
		}

		tt[7] = tt7 ;
	
	}else{ //br: else for: if( inflag == 0 ){
	
		
		/* analysis -assume flow choked at both turbine entrances and nozzle throat ... then*/
		
		tt[4] = tt4 * dFlightThrottle/100.0 ;
		gam[4] = getGama(tt[4],gamopt) ;
		cp[4]= getCp(tt[4],gamopt);
		
		if (a4 < 0.02) a4 = 0.02 ;
	
		if (iEngineType <= EngineType_Afterburner) {			/* turbojet */
			dela = 0.2 ;							/* iterate to get t5t4 */
			trat[5] = 1.0 ;
			t5t4n = .5 ;
			itcount = 0 ;
			
			while( abs(dela) > .001 && itcount < 20) {
				++ itcount ;
				delan = a8d/a4 -sqrt(t5t4n)*
						pow((1.0-(1.0/eta[5])*(1.0-t5t4n)),
						-gam[4]/(gam[4]-1.0)) ;
				
				deriv = (delan-dela)/(t5t4n-trat[5]) ;
				dela = delan ;
				trat[5] = t5t4n ;
				t5t4n = trat[5] - dela/deriv ;
			}
			
			tt[5] = tt[4] * trat[5] ;
			gam[5] = getGama(tt[5],gamopt) ;
			cp[5]= getCp(tt[5],gamopt);
			deltht = tt[5]- tt[4] ;
			delhht= cp[4] * deltht ;
			
			prat[5] =pow((1.0-(1.0/eta[5])*(1.0-trat[5])),
						gam[4]/(gam[4]-1.0)) ;
			
			delhc = delhht;			/* compressor work */
			deltc = -delhc / cp[2] ;
			tt[3] = tt[2] + deltc ;
			gam[3] = getGama(tt[3],gamopt) ;
			cp[3]= getCp(tt[3],gamopt);
			trat[3] = tt[3]/tt[2] ;
			
			prat[3] =pow((1.0+eta[3]*(trat[3]-1.0)),
						gam[2]/(gam[2]-1.0)) ;
			
			trat[4] = tt[4]/tt[3] ;
			pt[3]	= pt[2] * prat[3] ;
			pt[4]	= pt[3] * prat[4] ;
			pt[5]	= pt[4] * prat[5] ;
										/* fan conditions */
			prat[13] = 1.0 ;
			trat[13] = 1.0 ;
			tt[13]	= tt[2] ;
			pt[13]	= pt[2] ;
			gam[13]= gam[2] ;
			cp[13]	= cp[2] ;
			prat[15] = 1.0 ;
			pt[15]	= pt[5] ;
			trat[15] = 1.0 ;
			tt[15]	= tt[5] ;
			gam[15]= gam[5] ;
			cp[15]	= cp[5] ;
		}

		if(iEngineType == EngineType_TurboFan) {						/*turbofan */
			dela = .2 ;							/* iterate to get t5t4 */
			trat[5] = 1.0 ;
			t5t4n = .5 ;
			itcount = 0 ;
			
			while( abs(dela) > .001 && itcount < 20) {
				++ itcount ;
				delan = a4p/a4 -sqrt(t5t4n)*
						pow((1.0-(1.0/eta[5])*(1.0-t5t4n)),
						-gam[4]/(gam[4]-1.0)) ;
				
				deriv = (delan-dela)/(t5t4n-trat[5]) ;
				dela = delan ;
				trat[5] = t5t4n ;
				t5t4n = trat[5] - dela/deriv ;
			}
			
			tt[5] = tt[4] * trat[5] ;
			gam[5] = getGama(tt[5],gamopt) ;
			cp[5]= getCp(tt[5],gamopt);
			deltht = tt[5] - tt[4] ;
			delhht= cp[4] * deltht ;
			
			prat[5] =pow((1.0-(1.0/eta[5])*(1.0-trat[5])),
					 gam[4]/(gam[4]-1.0)) ;
			
			/* iterate to get t15t14 */
			dela = .2 ;
			trat[15] = 1.0 ;
			t5t4n = .5 ;
			itcount = 0 ;
			
			while( abs(dela) > .001 && itcount < 20) {
				++ itcount ;
				delan = a8d/a4p -sqrt(t5t4n)*
						 pow((1.0-(1.0/eta[5])*(1.0-t5t4n)),
						-gam[5]/(gam[5]-1.0)) ;
				deriv = (delan-dela)/(t5t4n-trat[15]) ;
				dela = delan ;
				trat[15] = t5t4n ;
				t5t4n = trat[15] - dela/deriv ;
			}
			
			tt[15] = tt[5] * trat[15] ;
			gam[15] = getGama(tt[15],gamopt) ;
			cp[15]= getCp(tt[15],gamopt);
			deltlt = tt[15] - tt[5] ;
			delhlt = cp[5] * deltlt ;
			
			prat[15] =pow((1.0-(1.0/eta[5])*(1.0-trat[15])),
						gam[5]/(gam[5]-1.0)) ;
			
			dBypassRatio =afan / acore - 1.0;
			delhf = delhlt / (1.0 + dBypassRatio) ;			/* fan work */
			deltf =- delhf / cp[2] ;
			tt[13] = tt[2] + deltf ;
			gam[13] = getGama(tt[13],gamopt) ;
			cp[13]= getCp(tt[13],gamopt);
			trat[13] = tt[13]/tt[2] ;
			
			prat[13] =pow((1.0+eta[13]*(trat[13]-1.0)),
					 gam[2]/(gam[2]-1.0)) ;
			
			delhc = delhht;						 /* compressor work */
			deltc = -delhc / cp[13] ;
			tt[3] = tt[13] + deltc ;
			gam[3] = getGama(tt[3],gamopt) ;
			cp[3]= getCp(tt[3],gamopt);
			trat[3] = tt[3]/tt[13] ;
			
			prat[3] =pow((1.0+eta[3]*(trat[3]-1.0)),
						gam[13]/(gam[13]-1.0)) ;
			
			trat[4] = tt[4]/tt[3] ;
			pt[13]= pt[2]* prat[13] ;
			pt[3]	= pt[13] * prat[3] ;
			pt[4]	= pt[3]* prat[4] ;
			pt[5]	= pt[4]* prat[5] ;
			pt[15]= pt[5]* prat[15] ;
		}

		if (iEngineType == EngineType_Ramjet) {			/* ramjet */
			prat[3] = 1.0 ;					
			pt[3] = pt[2] * prat[3] ;
			tt[3] = tt[2] ;
			trat[3] = 1.0 ;
			gam[3] = getGama(tt[3],gamopt) ;
			cp[3]= getCp(tt[3],gamopt);
			tt[4] = tt4 * dFlightThrottle/100.0;
			trat[4] = tt[4] / tt[3] ;
			pt[4] = pt[3] * prat[4] ;
			tt[5] = tt[4] ;
			gam[5] = getGama(tt[5],gamopt) ;
			cp[5]= getCp(tt[5],gamopt);
			trat[5] = 1.0 ;
			prat[5] = 1.0 ;
			pt[5] = pt[4] ;
			
			/* fan conditions */
			prat[13] = 1.0 ;
			trat[13] = 1.0 ;
			tt[13]	= tt[2] ;
			pt[13]	= pt[2] ;
			gam[13]= gam[2] ;
			cp[13]	= cp[2] ;
			prat[15] = 1.0 ;
			pt[15]	= pt[5] ;
			trat[15] = 1.0 ;
			tt[15]	= tt[5] ;
			gam[15]= gam[5] ;
			cp[15]	= cp[5] ;
		}

		if (iAfterBurnerOn == 1) tt[7] = tt7 ;
	}//br: end: if( inflag == 0 ){
	
	
	prat[6] = 1.0;
	pt[6] = pt[15];
	trat[6] = 1.0 ;
	tt[6] = tt[15] ;
	gam[6] = getGama(tt[6],gamopt) ;
	cp[6]= getCp(tt[6],gamopt);
	
	if (iAfterBurnerOn > 0) {					/* afterburner */
		 trat[7] = tt[7] / tt[6] ;
		 m5 = getMach(0,getAir(1.0,gam[5])*a4/acore,gam[5]) ;
		 prat[7] = getRayleighLoss(m5,trat[7],tt[6]) ;
	}

	tt[7] = tt[6] * trat[7] ;
	pt[7] = pt[6] * prat[7] ;
	gam[7] = getGama(tt[7],gamopt) ;
	cp[7]= getCp(tt[7],gamopt);
	
	/* engine press ratio EPR*/
	epr = prat[7]*prat[15]*prat[5]*prat[4]*prat[3]*prat[13];
	
	/* engine temp ratio ETR */
	etr = trat[7]*trat[15]*trat[5]*trat[4]*trat[3]*trat[13];
	
	
	return;

}//end: getThermo()




void NASA_TurboJetSim_Solver::getPerform (){		/* determine engine performance */
	double fac1,game,cpe,cp3,rg,p8p5,rg1 ;
	int index ;

	rg1 = 53.3 ;
	rg = cpair * (gama-1.0)/gama ;
	cp3 = getCp(tt[3],gamopt);				/*BTU/lbm R */
	g0 = 32.2 ;
	ues = 0.0 ;
	game = getGama(tt[5],gamopt) ;
	fac1 = (game - 1.0)/game ;
	cpe = getCp(tt[5],gamopt) ;
	if (eta[7] < .8) eta[7] = .8 ;	/* protection during overwriting */
	if (eta[4] < .8) eta[4] = .8 ;

	/*specific net thrust- thrust / (g0*airflow) -	lbf/lbm/sec*/
	// turbine engine core
	if (iEngineType <= EngineType_TurboFan ) {	
						/* airflow determined at choked nozzle exit */
		pt[8] = epr*prat[2]*pt[0] ;
		
		eair = getAir(1.0,game) * 144.*a8 * pt[8]/14.7 /
				 sqrt(etr*tt[0]/518.)	;
		
		m2 = getMach(0,eair* sqrt(tt[0]/518.)/
				(prat[2]*pt[0]/14.7*acore*144.),gama) ;
		
		npr = pt[8]/ps0;
		
		uexit =sqrt(2.0*rgas/fac1*etr*tt[0]*eta[7]*
				(1.0- pow(1.0/npr,fac1)));
		
		if( npr <= 1.893 ){
			pexit = ps0;
		}else{
			pexit = 0.52828 * pt[8];
		}
		
		fgros = (uexit + (pexit - ps0) * 144. *a8 /eair) / g0 ;
	}//end: if iEngineType = EngineType_TurboFan

	// turbo fan -- added terms for fan flow
	if (iEngineType == EngineType_TurboFan ) {
		fac1 = (gama - 1.0)/gama ;
		snpr = pt[13]/ps0;
		
		ues =sqrt(2.0*rgas/fac1*tt[13]*eta[7]*
				 (1.0- pow(1.0/snpr,fac1)));
		
		m2 = getMach(0,eair*(1.0+dBypassRatio)* sqrt(tt[0]/518.)/
				 (prat[2]*pt[0]/14.7*afan*144.),gama) ;
		
		if( snpr <= 1.893 ){
			pfexit = ps0;
		}else{ 
			pfexit = 0.52828 * pt[13];
		}
		
		fgros = fgros + (dBypassRatio * ues + (pfexit - ps0) * 144.0 * dBypassRatio * acore / eair) / g0 ;
	}

// ramjets
	if (iEngineType == EngineType_Ramjet) {
		/* airflow determined at nozzle throat */
		eair = getAir(1.0,game)*144.0*a2*arthd * epr*prat[2]*pt[0]/14.7 /
				 sqrt(etr*tt[0]/518.)	;
		m2 = getMach(0,eair* sqrt(tt[0]/518.)/
				(prat[2]*pt[0]/14.7*acore*144.),gama) ;
		mexit = getMach(2,(getAir(1.0,game) / arexitd),game) ;
		uexit = mexit *sqrt(game * rgas * etr * tt[0] *eta[7] /
				 (1.0 + .5 *(game-1.0) * mexit *mexit)) ;
		pexit =pow((1.0 + .5 *(game-1.0) * mexit *mexit),(-game/(game-1.0)))
				 * epr * prat[2] * pt[0] ;
		fgros = (uexit + (pexit - ps0)*arexitd*arthd*a2/eair/144.) / g0 ;
	}

// ram drag
		dram = u0 / g0 ;
		if (iEngineType == EngineType_TurboFan) dram = dram + u0 * dBypassRatio / g0 ;

// mass flow ratio 
		if (fsmach > .01) mfr = getAir(m2,gama)*prat[2]/getAir(fsmach,gama) ;
		else mfr = 5.;

// net thrust
		fnet = fgros - dram;
		if (iEngineType == 3 && fsmach < .3) {
		 fnet = 0.0 ;
		 fgros = 0.0 ;
		}

// thrust in pounds
		dNetThrust = fnet * eair ;
		fglb = fgros * eair ;
		drlb = dram * eair ;
	
//fuel-air ratio and sfc
		fa = (trat[4]-1.0)/(eta[4]*fhv/(cp3*tt[3])-trat[4]) +
		 (trat[7]-1.0)/(fhv/(cpe*tt[15])-trat[7]) ;
		if ( fnet > 0.0){
			sfc = 3600. * fa /fnet ;
			flflo = sfc*dNetThrust ;
			isp = (dNetThrust/flflo) * 3600. ;
		}
		else {
			dNetThrust = 0.0 ;
			flflo = 0.0 ;
			sfc = 0.0 ;
			isp = 0.0 ;
		}

 // plot output
		tt[8] = tt[7] ;
		t8 = etr * tt[0] - uexit * uexit /(2.0*rgas*game/(game-1.0)) ;
		trat[8] = tt[8]/tt[7] ;
		p8p5 = ps0 / (epr * prat[2] *pt[0]) ;
		cp[8] = getCp(tt[8],gamopt) ;
		pt[8] = pt[7] ;
		prat[8] = pt[8]/pt[7] ;
		/* thermal efficiency */
		if (iEngineType == EngineType_TurboFan) {
			eteng = (a0*a0*((1.0+fa)*(uexit*uexit/(a0*a0))
			+ dBypassRatio*(ues*ues/(a0*a0))
			- (1.0+dBypassRatio)*fsmach*fsmach))/(2.0*g0*fa*fhv*778.16)	;
		}
		else {
			eteng = (a0*a0*((1.0+fa)*(uexit*uexit/(a0*a0))
			- fsmach*fsmach))/(2.0*g0*fa*fhv*778.16)	;
		}

		s[0] = s[1] = .2 ;
		v[0] = v[1] = rg1*ts0/(ps0*144.) ;
		for (index=2; index <=7 ; ++index ) {	 /* compute entropy */
		 s[index] = s[index-1] + cpair* log(trat[index])
								- rg* log(prat[index]);
		 v[index] = rg1*tt[index]/(pt[index]*144.) ;
		}
		s[13] = s[2] + cpair* log(trat[13])-rg* log(prat[13]);
		v[13] = rg1*tt[13]/(pt[13]*144.) ;
		s[15] = s[5] + cpair* log(trat[15])-rg* log(prat[15]);
		v[15] = rg1*tt[15]/(pt[15]*144.) ;
		s[8]= s[7] + cpair* log(t8/(etr*tt[0]))- rg* log(p8p5);
		v[8]= rg1*t8/(ps0*144.) ;
		cp[0] = getCp(tt[0],gamopt) ;

		fntot	= numeng * dNetThrust ;
		fuelrat = numeng * flflo ;
	// weightcalculation
		if (wtflag == 0) {
		if (iEngineType == EngineType_TurboJet) {
			weight = .132 *sqrt(acore*acore*acore) * 
			 (dcomp * lcomp + dburner * lburn + dturbin * lturb + dnozl * lnoz);
		}
		if (iEngineType == EngineType_Afterburner) {
			weight = .100 *sqrt(acore*acore*acore) * 
			 (dcomp * lcomp + dburner * lburn + dturbin * lturb + dnozl * lnoz);
		}
		if (iEngineType == EngineType_TurboFan) {
			weight = .0932 * acore * ((1.0 + dBypassRatio) * dfan * 4.0 + dcomp * (ncomp -3) +
					dburner + dturbin * nturb + dburner * 2.0) *sqrt(acore / 6.965) ;
		}
		if (iEngineType == EngineType_Ramjet) {
			weight = .1242 * acore * (dburner + dnozr *6. + dinlt * 3.) *sqrt(acore / 1.753) ;
		}
		}
	 // check for temp limits
		 /* br
		out.vars.to1.setForeground(Color.yellow) ;
		out.vars.to2.setForeground(Color.yellow) ;
		out.vars.to3.setForeground(Color.yellow) ;
		out.vars.to4.setForeground(Color.yellow) ;
		out.vars.to5.setForeground(Color.yellow) ;
		out.vars.to6.setForeground(Color.yellow) ;
		out.vars.to7.setForeground(Color.yellow) ;
		*/
		
		if (iEngineType < EngineType_Ramjet) {
		if (tt[2] > tinlt) {
			 fireflag =1 ;	
			 //br: out.vars.to1.setForeground(Color.red) ;
			 //br: out.vars.to2.setForeground(Color.red) ;
		}
		if (tt[13] > tfan) {
			 fireflag =1 ;	
			 //br: out.vars.to2.setForeground(Color.red) ;
		}
		if (tt[3] > tcomp) {
			 fireflag =1 ;	
			 //br: out.vars.to3.setForeground(Color.red) ;
		}
		if (tt[4] > tburner) {
			 fireflag =1 ;	
			 //br: out.vars.to4.setForeground(Color.red) ;
		}
		if (tt[5] > tturbin) {
			 fireflag =1 ;	
			 //br: out.vars.to5.setForeground(Color.red) ;
		}
		if (tt[7] > tnozl) {
			 fireflag =1 ;	
			 //br: out.vars.to6.setForeground(Color.red) ;
			 //br: out.vars.to7.setForeground(Color.red) ;
		}
		}
		if (iEngineType == EngineType_Ramjet) {
		if (tt[3] > tinlt) {
			 fireflag =1 ;	
			 //br: out.vars.to1.setForeground(Color.red) ;
			 //br: out.vars.to2.setForeground(Color.red) ;
			 //br: out.vars.to3.setForeground(Color.red) ;
		}
		if (tt[4] > tburner) {
			 fireflag =1 ;	
			 //br: out.vars.to4.setForeground(Color.red) ;
		}
		if (tt[7] > tnozr) {
			 fireflag =1 ;	
			 //br: out.vars.to5.setForeground(Color.red) ;
			 //br: out.vars.to6.setForeground(Color.red) ;
			 //br: out.vars.to7.setForeground(Color.red) ;
		}
		}
		//br: if (fireflag == 1) view.start() ;
}



void NASA_TurboJetSim_Solver::getGeo () {
							/* determine geometric variables */
	double game;
	float fl1;
	int i1;

	if (iEngineType <= EngineType_TurboFan) {		// turbine engines
		if (afan < acore) afan = acore ;
		
		a8max = .75 * sqrt(etr) / epr ; /* limits compressor face mach numberto < .5	*/
		
		if (a8max > 1.0){ a8max = 1.0 ; }
		
		if (a8rat > a8max) {
			a8rat = a8max ;
			if (lunits <= 1) {
				fl1 = filter3(a8rat) ;
				//br: in.nozl.left.f3.setText(String.valueOf(fl1)) ;
				i1 = (int) (((a8rat - a8min)/(a8max-a8min))*1000.) ;
				//br: in.nozl.right.s3.setValue(i1) ;
			}
			/* br: % delta change output
			if (lunits == 2) {
				fl1 = filter3(100.*(a8rat - a8ref)/a8ref) ;
				//br: in.nozl.left.f3.setText(String.valueOf(fl1)) ;
				i1 = (int) ((((100.*(a8rat - a8ref)/a8ref) +10.0)/20.0)*1000.) ;
				//br: in.nozl.right.s3.setValue(i1) ;
			}
			*/
		}
			/*	dumb down limit - a8 schedule */
		if (arsched == 0) {
			a8rat = a8max ;
			fl1 = filter3(a8rat) ;
			//br: in.nozl.left.f3.setText(String.valueOf(fl1)) ;
			i1 = (int) (((a8rat - a8min)/(a8max-a8min))*1000.) ;
			//br: in.nozl.right.s3.setValue(i1) ;
		}
		
		a8 = a8rat * acore;
		a8d = a8 * prat[7] / sqrt(trat[7]) ;
			 /* assumes choked a8 and a4 */
		a4 = a8*prat[5]*prat[15]*prat[7]/
			sqrt(trat[7]*trat[5]*trat[15]);
		
		a4p = a8*prat[15]*prat[7]/sqrt(trat[7]*trat[15]);
		acap = .9 * a2 ;
	}

	if (iEngineType == EngineType_Ramjet) {	// ramjets
		game = getGama(tt[4],gamopt) ;
		
		if (athsched == 0) {	// scheduled throat area
			arthd = getAir(fsmach,gama) * sqrt(etr) /
				(getAir(1.0,game) * epr * prat[2]) ;
			
			if (arthd < arthmn) arthd = arthmn ;
			if (arthd > arthmx) arthd = arthmx ;
			fl1 = filter3(arthd) ;
			//br: in.nozr.left.f3.setText(String.valueOf(fl1)) ;
			i1 = (int) (((arthd - arthmn)/(arthmx-arthmn))*1000.) ;
			//br: in.nozr.right.s3.setValue(i1) ;
		}
		if (aexsched == 0) {	// scheduled exit area
			mexit = sqrt((2.0/(game-1.0))*((1.0+.5*(gama-1.0)*fsmach*fsmach)
				* pow((epr*prat[2]),(game-1.0)/game) - 1.0) ) ;
			
			arexitd = getAir(1.0,game) / getAir(mexit,game) ;
			if (arexitd < arexmn) arexitd = arexmn ;
			if (arexitd > arexmx) arexitd = arexmx ;
			fl1 = filter3(arexitd) ;
			//br: in.nozr.left.f4.setText(String.valueOf(fl1)) ;
			i1 = (int) (((arexitd - arexmn)/(arexmx-arexmn))*1000.) ;
			//br: in.nozr.right.s4.setValue(i1) ;
		}
	}
}//end: function getGeo()
 




double NASA_TurboJetSim_Solver::getCp(double temp, int opt){
			// Utility to get cp as a function of temp 
	double number,a,b,c,d ;
							/* BTU/R */
	a =-4.4702130e-13;
	b =-5.1286514e-10;
	c =	2.8323331e-05;
	d =0.2245283;
	if(opt == 0) {
		number = .2399 ;
	}
	else {
		number = a*temp*temp*temp + b*temp*temp + c*temp +d ;
	}
	return(number) ;
}






float NASA_TurboJetSim_Solver::filter3(double inumbr) {
	//output only to .001
	float number ;
	int intermed ;
	
	intermed = (int) (inumbr * 1000.) ;
	number = (float) (intermed / 1000. );
	return number ;
}





double NASA_TurboJetSim_Solver::getGama(double temp, int opt) {
			 // Utility to get gamma as a function of temp 
	double number,a,b,c,d ;
	a =-7.6942651e-13;
	b =1.3764661e-08;
	c =-7.8185709e-05;
	d =1.436914;
	if(opt == 0) {
		number = 1.4 ;
	}
	else {
		number = a*temp*temp*temp + b*temp*temp + c*temp +d ;
	}
	return(number) ;
}



double NASA_TurboJetSim_Solver::getAir(double mach, double gamma) {
/* Utility to get the corrected airflow per area given the Mach number */
	double number,fac1,fac2;
	fac2 = (gamma+1.0)/(2.0*(gamma-1.0)) ;
	fac1 = pow((1.0+.5*(gamma-1.0)*mach*mach),fac2);
	number =.50161* sqrt(gamma) * mach/ fac1 ;
 
	return(number) ;
}




double NASA_TurboJetSim_Solver::getMach (int sub, double corair, double gamma) {
/* Utility to get the Mach number given the corrected airflow per area */
	double number,chokair;	/* iterate for mach number */
	double deriv,machn,macho,airo,airn;
	int iter ;

	chokair = getAir(1.0, gamma) ;
	if (corair > chokair) {
		number = 1.0 ;
		return (number) ;
	}
	else {
		airo = .25618 ;				 /* initial guess */
		if (sub == 1) macho = 1.0 ;	/* sonic */
		else {
			if (sub == 2) macho = 1.703 ; /* supersonic */
			else macho = .5;				/* subsonic */
			iter = 1 ;
			machn = macho - .2;
			while ( abs(corair - airo) > .0001 && iter < 20) {
			airn =getAir(machn,gamma) ;
			deriv = (airn-airo)/(machn-macho) ;
			airo = airn ;
			macho = machn ;
			machn = macho + (corair - airo)/deriv ;
			++ iter ;
			}
		}
		number = macho ;
	}
	return(number) ;
}

double NASA_TurboJetSim_Solver::getRayleighLoss(double mach1, double ttrat, double tlow) {
										 /* analysis for rayleigh flow */
	double number ;
	double wc1,wc2,mgueso,mach2,g1,gm1,g2,gm2 ;
	double fac1,fac2,fac3,fac4;
	
	g1 = getGama(tlow,gamopt);
	gm1 = g1 - 1.0 ;
	wc1 = getAir(mach1,g1);
	g2 = getGama(tlow*ttrat,gamopt);
	gm2 = g2 - 1.0 ;
	number = .95 ;
							/* iterate for mach downstream */
	mgueso = .4 ;				 /* initial guess */
	mach2 = .5 ;
	while ( abs(mach2 - mgueso) > .0001) {
		mgueso = mach2 ;
		fac1 = 1.0 + g1 * mach1 * mach1 ;
		fac2 = 1.0 + g2 * mach2 * mach2 ;
		fac3 = pow((1.0 + .5 * gm1 * mach1 * mach1),(g1/gm1)) ;
		fac4 = pow((1.0 + .5 * gm2 * mach2 * mach2),(g2/gm2)) ;
		number = fac1 * fac4 / fac2 / fac3 ;
		wc2 = wc1 * sqrt(ttrat) / number ;
		mach2 = getMach(0,wc2,g2) ;
	}
	return(number) ;
}
