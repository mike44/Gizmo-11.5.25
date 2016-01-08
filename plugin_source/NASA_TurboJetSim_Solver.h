#ifndef __NASA__TURBOJETSIM_SOLVER__
#define __NASA__TURBOJETSIM_SOLVER__


#include "X_STL.h"
#include <math.h>


#ifdef DMALLOC
#include "dmalloc.h"
#endif




//br: order and values of these enums matter. do not change.
enum e_EngineTypes{
	EngineType_TurboJet 	= 0,
	EngineType_Afterburner 	= 1,
	EngineType_TurboFan 	= 2,
	EngineType_Ramjet		= 3
};



class NASA_TurboJetSim_Solver{
	public:
		NASA_TurboJetSim_Solver();

		void compute();
		void setDefaults();
		
		//void myDesign();
		//void loadMine();
		
		void loadCF6();
		void loadJ85();
		void loadF100();
		void loadRamj();

		void setAltitude( double d ){ this->dFlightAltitude = d; }
		void setThrottle( double d ){ this->dFlightThrottle = d; }
		void setSpeed( double d ){ this->u0d = d; }
		void setAfterBurner( int flag ){ this->iAfterBurnerOn = flag; }
		
		double getNetThrust(){ return dNetThrust; }
		  
	private:
		double getCp(double temp, int opt);
		float filter3(double inumbr);
		double getGama(double temp, int opt);
		double getAir(double mach, double gamma);		
		double getRayleighLoss(double mach1, double ttrat, double tlow);
		double getMach (int sub, double corair, double gamma);
		void getFreeStream();
		void getThermo();
		void getPerform ();
		void getGeo ();
	  
	   double convdr; //degrees to radds
	 
		int lunits;// = 1; // 1 = metric units flag
		
	   int iAfterBurnerOn,iEngineType,inflag,varflag,pt2flag,wtflag ;
	   int abkeep,pltkeep,move ;
	   int numeng,gamopt,arsched,plttyp,showcom ;
	   int athsched,aexsched,fueltype,inptype,siztype ;
				   // Flow variables
	   double g0d,g0,rgas,gama,cpair ;
		double dFlightThrottle; //used for input, throttle
	   double tt4,tt4d,tt7,tt7d,t8,p3p2d,p3fp2d,dBypassRatio;
		double u0d; //used for input params, possibly speed in mph
		double dFlightAltitude; //used for inptu params, altitude
	   double fsmach,alt,ts0,ps0,q0,u0,a0,rho0,tsout,psout;
	   double epr,etr,npr,snpr,fnet,fgros,dram,sfc,fa,eair,uexit,ues;
	   double fnd,dNetThrust,fglb,drlb,flflo,fuelrat,fntot,eteng;
	   double arth,arthd,arexit,arexitd ;
	   double mexit,pexit,pfexit ;
	   double arthmn,arthmx,arexmn,arexmx ;
	   double a8,a8rat,a8d,afan,a7,m2,isp;
	   double acap,a2,a2d,acore,a4,a4p,fhv,fhvd,mfr,diameng ;
	   double altmin,altmax,u0min,u0max,thrmin,thrmax,pmax,tmin,tmax;
	   double etmin,etmax,cprmin,cprmax,t4min,t4max;
	   double a2min,a2max,a8min,a8max,t7min,t7max,diamin,diamax;
	   double bypmin,bypmax,fprmin,fprmax;
	   double vmn1,vmn2,vmn3,vmn4,vmx1,vmx2,vmx3,vmx4 ;
	   double lconv1,lconv2,fconv,pconv,tconv,tref,mconv1,mconv2,econv,econv2 ;
	   double aconv,bconv,dconv,flconv ;
				   // weight and materials
	   double weight,wtref,wfref ;
	   int mcomp,mfan,mturbin,mburner,minlt,mnozl,mnozr ;
	   int ncflag,ncomp,ntflag,nturb,fireflag;
	   double dcomp,dfan,dturbin,dburner ;
	   double tcomp,tfan,tturbin,tburner ;
	   double tinlt,dinlt,tnozl,dnozl,tnozr,dnozr ;
	   double lcomp,lburn,lturb,lnoz;   // component length
				   // Station Variables
	   double trat[20];// = new double[20] ;
	   double tt[20];//   = new double[20] ;
	   double prat[20];// = new double[20] ;
	   double pt[20];//   = new double[20] ;
	   double eta[20];// = new double[20] ;
	   double gam[20];// = new double[20] ;
	   double cp[20];//   = new double[20] ;
	   double s[20];//    = new double[20] ;
	   double v[20];//    = new double[20] ;
					 /* drawing geometry  */
	   double xtrans,ytrans,factor,gains,scale ;
	   double xtranp,ytranp,factp ;
	   double xg[13][45];//  = new double[13][45] ;
	   double yg[13][45];//  = new double[13][45] ;
	   int sldloc,sldplt,ncompd;
	   int antim,ancol;
	
	
	
	
	/*
					 //  Percentage  variables
	   double u0ref,altref,thrref,a2ref,et2ref,fpref,et13ref,bpref ;
	   double cpref,et3ref,et4ref,et5ref,t4ref,p4ref,t7ref,et7ref,a8ref;
	   double fnref,fuelref,sfcref,airref,epref,etref,faref ;
	*/
	
	/*
					 // save design
	   int ensav,absav,gamosav,ptfsav,arssav,arthsav,arxsav,flsav ;
	   double fhsav,t4sav,t7sav,p3sav,p3fsav,bysav,acsav ;
	   double a2sav,a4sav,a4psav,gamsav,et2sav,pr2sav,pr4sav ;
	   double et3sav,et4sav,et5sav,et7sav,et13sav,a8sav,a8mxsav ;
	   double a8rtsav,u0mxsav,u0sav,altsav ;
	   double trsav,artsav,arexsav ; 
					  // save materials info
	   int wtfsav,minsav,mfnsav,mcmsav,mbrsav,mtrsav,mnlsav,mnrsav,ncsav,ntsav;
	   double wtsav, dinsav, tinsav, dfnsav, tfnsav, dcmsav, tcmsav;
	   double dbrsav, tbrsav, dtrsav, ttrsav, dnlsav, tnlsav, dnrsav, tnrsav;
	*/
	/*
					 // plot variables
	   int lines,nord,nabs,param,npt,ntikx,ntiky ;
	   int counter ;
	   int ordkeep,abskeep ;
	   double begx,endx,begy,endy ;
	   double pltx[26];// = new double[26] ;
	   double plty[26];// = new double[26] ;
		std::string labx,laby,labyu,labxu ;
	*/

};    // end Solver




#endif
