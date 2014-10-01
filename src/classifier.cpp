/*
 * =====================================================================================
 *
 *       Filename:  classifier.cpp
 *
 *    Description:  implementation of classifier.h methods
 *
 *        Version:  1.0
 *        Created:  23/06/2014 15:30
 *       Revision:  
 *       Compiler:  gcc
 *
 *         Author:  Davide Ori 
 *   Organization:  Dept.Physics University of Bologna
 *
 * =====================================================================================
 */

#include "classifier.h"
#include <radarlib/radar.hpp>
#include <sstream>
#include "algo/elabora_volume.h"
#include"image.h"

using namespace elaboradar;
using namespace volume;
using namespace std;
namespace odim = OdimH5v21;

PROB::PROB(double z,double zdr,double rhohv, double lkdp, double sdz, double sdphidp, double vrad)
{
	this->resize(10,6);
	this->row(0)=prob_class(GC_AP, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(1)=prob_class(   BS, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(2)=prob_class(   DS, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(3)=prob_class(   WS, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(4)=prob_class(   CR, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(5)=prob_class(   GR, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(6)=prob_class(   BD, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(7)=prob_class(   RA, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(8)=prob_class(   HR, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
	this->row(9)=prob_class(   RH, z, zdr, rhohv, lkdp, sdz, sdphidp, vrad);
}

Matrix2D<double> PROB::prob_class(EchoClass classe,double z, double zdr, double rhohv, double lkdp, double sdz, double sdphidp, double vrad)
{
	Matrix2D<double> probability(1,6);
	probability=Matrix2D::Constant(1,6,0.);
	double f1=f_1(z);
	double f2=f_2(z);
	double f3=f_3(z);
	double g1=g_1(z);
	double g2=g_2(z);
	switch(classe)
	{
		case GC_AP:
			if(abs(vrad)<1.)
			{
				probability<<trap(15.,20.,70.,80.,z),trap(-4.,-2.,1.,2.,zdr),trap(0.5,0.6,0.9,0.95,rhohv),
				trap(-30.,-25.,10.,20.,lkdp),trap(2.,4.,10.,15.,sdz),trap(30.,40.,50.,60.,sdphidp);
			}
			return probability;
		case BS:
			if(rhohv<0.97)
			{
				probability<<trap(5.,10.,20.,30.,z),trap(0.,2.,10.,12.,zdr),trap(0.3,0.5,0.8,0.83,rhohv),
				trap(-30.,-25.,10.,10.,lkdp),trap(1.,2.,4.,7.,sdz),trap(8.,10.,40.,60.,sdphidp);
			}
			return probability;
		case DS:
			if(zdr<2.)
			{
				probability<<trap(5.,10.,35.,40.,z),trap(-0.3,0.,0.3,0.6,zdr),trap(0.95,0.98,1.,1.01,rhohv),
				trap(-30.,-25.,10.,20.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		case WS:
			if(z>20.&&zdr>0.)
			{
				probability<<trap(25.,30.,40.,50.,z),trap(0.5,1.,2.,3.0,zdr),trap(0.88,0.92,0.95,0.985,rhohv),
				trap(-30.,-25.,10.,20.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		case CR:
			if(z<40.)
			{
				probability<<trap(0.,5.,20.,25.,z),trap(0.1,0.4,3.,3.3,zdr),trap(0.95,0.98,1.,1.01,rhohv),
				trap(-5.,0.,10.,20.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		case GR:
			if(z>10.&&z<60.)
			{
				probability<<trap(25.,35.,50.,55.,z),trap(-0.3,0,f1,f1+0.3,zdr),trap(0.9,0.97,1.,1.01,rhohv),
				trap(-30.,-25.,10.,20.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		case BD:
			if(zdr>(f2-0.3))
			{
				probability<<trap(20.,25.,45.,50.,z),trap(f2-0.3,f2,f3,f3+1.,zdr),trap(0.92,0.95,1.,1.01,rhohv),
				trap(g1-1.,g1,g2,g2+1.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		case RA:
			if(z<50.)
			{
				probability<<trap(5.,10.,45.,50.,z),trap(f1-0.3,f1,f2,f2+0.5,zdr),trap(0.95,0.97,1.,1.01,rhohv),
				trap(g1-1.,g1,g2,g2+1.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		case HR:
			if(z>30.)
			{
				probability<<trap(40.,45.,55.,60.,z),trap(f1-0.3,f1,f2,f2+0.5,zdr),trap(0.92,0.95,1.,1.01,rhohv),
				trap(g1-1.,g1,g2,g2+1.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		case RH:
			if(z>40.)
			{
				probability<<trap(45.,50.,75.,80.,z),trap(-0.3,0.,f1,f1+0.5,zdr),trap(0.85,0.9,1.,1.01,rhohv),
				trap(-10.,-4.,g1,g1+1.,lkdp),trap(0.,0.5,3.,6.,sdz),trap(0.,1.,15.,30.,sdphidp);
			}
			return probability;
		default:
			cout<<"ERROR!!!   PROBability calculator does not known the requested echo type "<<classe<<endl;
			return probability;	// without it produces compile warnings and runtime error if reached
	}
}

double PROB::trap(double x1, double x2, double x3, double x4, double val)
{
	if(val<=x3&&val>=x2) return 1.;
	else if(val<x2&&val>x1) return val/(x2-x1)-x1/(x2-x1);
	else if (val<x4&&val>x3) return val/(x3-x4)-x4/(x3-x4);
	else return 0.; // (val<=x1||val>=x4)
}

HCA_Park::HCA_Park(double Z, double ZDR, double RHOHV, double LKDP, double SDZ, double SDPHIDP, double VRAD,
		double PHIDP, double SNR, double GPHITH, double GPHIPHI, double GZTH, double GZPHI, double GZDRTH, double GZDRPHI)
	: z(Z),zdr(ZDR),rhohv(RHOHV),lkdp(LKDP),sdz(SDZ),sdphidp(SDPHIDP), vrad(VRAD),phidp(PHIDP),snr(SNR),gradphitheta(GPHITH),
	gradphiphi(GPHIPHI),gradZtheta(GZTH),gradZphi(GZPHI),gradZdrtheta(GZDRTH),gradZdrphi(GZDRPHI)
{
	PROB Pij(z,zdr,rhohv,lkdp,sdz,sdphidp,vrad);
	//CONF Qi;	// TODO: confidence vector calculation not implemented,
			// currently it uses a vector of ones.
	CONF Qi(phidp, rhohv, snr, gradphitheta, gradphiphi, gradZtheta, gradZphi, gradZdrtheta, gradZdrphi);	// gradients must be precomputed

	Matrix2D<double> Wij(10,6);
//		Z	Zdr	rhohv	lkdp	SDZ	SDphidp
	Wij <<	0.2,	0.4,	1.0,	0.0,	0.6,	0.8,	// GC_AP	3.0
		0.4,	0.6,	1.0,	0.0,	0.8,	0.8,	// BS		3.6
		1.0,	0.8,	0.6,	0.0,	0.2,	0.2,	// DS		2.8
		0.6,	0.8,	1.0,	0.0,	0.2,	0.2,	// WS		2.8
		1.0,	0.6,	0.4,	0.5,	0.2,	0.2,	// CR		2.9
		0.8,	1.0,	0.4,	0.0,	0.2,	0.2,	// GR		2.6
		0.8,	1.0,	0.6,	0.0,	0.2,	0.2,	// BD		2.8
		1.0,	0.8,	0.6,	0.0,	0.2,	0.2,	// RA		2.8
		1.0,	0.8,	0.6,	1.0,	0.2,	0.2,	// HR		3.8
		1.0,	0.8,	0.6,	1.0,	0.2,	0.2;	// RH		3.8
// TOT =	8.8	7.6	6.8	2.5	3.0	3.2
	Ai.resize(10);
	Ai=((Wij.array()*Pij.array()).matrix()*Qi).array()/(Wij*Qi).array();
}

classifier::classifier(const string& file, const Site& site):pathname(file)
{
	printf("il nome del mio file è %s\n", pathname.c_str());

	volume::ODIMLoader loader_all(site, false, 1024);

	volume::Scans<double> full_volume_z;
	volume::Scans<double> full_volume_zdr;
	volume::Scans<double> full_volume_rhohv;
	volume::Scans<double> full_volume_phidp;
	volume::Scans<double> full_volume_vrad;
	volume::Scans<double> full_volume_snr;

	loader_all.request_quantity(odim::PRODUCT_QUANTITY_DBZH,&full_volume_z);
	loader_all.request_quantity(odim::PRODUCT_QUANTITY_ZDR,&full_volume_zdr);
	loader_all.request_quantity(odim::PRODUCT_QUANTITY_RHOHV,&full_volume_rhohv);
	loader_all.request_quantity(odim::PRODUCT_QUANTITY_PHIDP,&full_volume_phidp);
	loader_all.request_quantity(odim::PRODUCT_QUANTITY_VRAD,&full_volume_vrad);
	loader_all.request_quantity(odim::PRODUCT_QUANTITY_SNR,&full_volume_snr);

	loader_all.load(pathname);
	printf("Non so se è andato tutto bene, ma almeno sono arrivato in fondo\n");

	volume::volume_resample<double>(full_volume_z, loader_all.azimuth_maps, vol_z, volume::merger_closest<double>);
	volume::volume_resample<double>(full_volume_zdr, loader_all.azimuth_maps, vol_zdr, volume::merger_closest<double>);
	volume::volume_resample<double>(full_volume_rhohv, loader_all.azimuth_maps, vol_rhohv, volume::merger_closest<double>);
	volume::volume_resample<double>(full_volume_phidp, loader_all.azimuth_maps, vol_phidp, volume::merger_closest<double>);
	volume::volume_resample<double>(full_volume_vrad, loader_all.azimuth_maps, vol_vrad, volume::merger_closest<double>);
	volume::volume_resample<double>(full_volume_snr, loader_all.azimuth_maps, vol_snr, volume::merger_closest<double>);
	vol_hca.quantity="CLASS";

	cout<<vol_z.load_info->acq_date<<endl;
}

void classifier::compute_lkdp()
{
	/// METODO DI VULPIANI 2012 ///
	unsigned half_win6km=12;
//	unsigned half_win2km=4;
	double kdp;
	unsigned tries;
	double phidp1, phidp2;
	double undet,nodat;

	for(unsigned el=0;el<vol_phidp.size();el++)
	{
		vol_phidp_6km.push_back(vol_phidp[el]);
		undet=vol_phidp[el].undetect;
		nodat=vol_phidp[el].nodata;

		for(unsigned az=0;az<vol_phidp[el].beam_count;az++)
		{
			vol_phidp_6km[el].set(az,0,0.);
			for(unsigned rg=0;rg<vol_phidp[el].beam_size;rg++)
			{
				if(rg<half_win6km||rg>(vol_phidp[el].beam_size-half_win6km-1)){kdp=0.;}
				else
				{
					phidp1=vol_phidp[el].get(az,rg-half_win6km);
					phidp2=vol_phidp[el].get(az,rg+half_win6km);
					if(phidp1==undet||phidp1==nodat||phidp2==undet||phidp2==nodat){kdp=0.;}
					else
					{
						kdp=0.5*(phidp2-phidp1)/6.;
						tries=0;
						while(tries<3)
						{
							if((kdp<-2.)||(kdp>20.))
							{
								if(kdp<-10.)	// vulpiani diceva -20, ma considerava ricetrasmettitori simultanei (360°) e L=7km
								{
									kdp=0.5*(phidp2-phidp1+180.)/6.;
								}
								else
								{
									kdp=0.;
								}
								tries++;
							}
							else {tries=4;}
						}
					}
				}
				if(rg){vol_phidp_6km[el].set(az,rg,vol_phidp_6km[el].get(az,rg-1)+2.*kdp*vol_phidp[el].cell_size*0.001);}
				//vol_lkdp_6km[el](az,rg)=kdp>0.001?10.*log10(kdp):-30;
				//cout<<"fil 6km rg "<<el<<" "<<rg<<"  "<<kdp<<"  "<<vol_phidp_6km[el].get(az,rg)<<endl;
			}	
		}
	}	// finita la ricostruzione di phidp secondo Vulpiani 2012
	moving_average_slope(vol_phidp_6km,vol_lkdp_6km,6000.,false);
	moving_average_slope(vol_phidp_6km,vol_lkdp_2km,2000.,false);

	vol_lkdp_2km.quantity="LKDP";
	vol_lkdp_2km.units="°/km";
	vol_lkdp_6km.quantity="LKDP";
	vol_lkdp_6km.units="°/km";
	vol_lkdp_2km*=1000.;
	vol_lkdp_6km*=1000.;
	for(unsigned el=0;el<vol_lkdp_6km.size();el++)
	{
		vol_lkdp_2km[el].nodata=-9999;
		vol_lkdp_2km[el].undetect=-30;
		vol_lkdp_6km[el].nodata=-9999;
		vol_lkdp_6km[el].undetect=-30;
		for(unsigned az=0;az<vol_lkdp_6km[el].beam_count;az++)
			for(unsigned rg=0;rg<vol_lkdp_6km[el].beam_size;rg++)
			{
				vol_lkdp_6km[el](az,rg)=vol_lkdp_6km[el](az,rg)>0.001?10*log10(vol_lkdp_6km[el](az,rg)):vol_lkdp_6km[el].undetect;
				vol_lkdp_2km[el](az,rg)=vol_lkdp_2km[el](az,rg)>0.001?10*log10(vol_lkdp_2km[el](az,rg)):vol_lkdp_2km[el].undetect;
			}
	}

}


void classifier::correct_phidp()
{
	// It is assumed that vol_phidp exist and has been initialized
	// moving window average over a range length of 2km and 6km as prescribed by Park et al. (2009)
	printf("filtro phidp 2 km\n");
	filter(vol_phidp,vol_phidp_2km,2000.,0.,true);
	printf("filtro phidp 6 km\n");
	filter(vol_phidp,vol_phidp_6km,6000.,0.,true);
}

void classifier::correct_for_attenuation()
{
//	for(unsigned rg=0;rg<50;rg++) cout<<fixed<<vol_z[0](85,rg)<<" ";
//	cout<<endl;
	for(unsigned el=0;el<vol_z.size();el++)
	{
		vol_z[el]+=0.06*vol_phidp_6km[el];
		vol_zdr[el]+=0.01*vol_phidp_6km[el];
	}
//	for(unsigned rg=0;rg<50;rg++) cout<<fixed<<vol_z[0](85,rg)<<" ";
//	cout<<endl;
}

void classifier::correct_for_snr()
{
	cout<<"inizio snr"<<endl;
	Volume<double> vol_snr_linear;
	dB2lin(vol_snr,vol_snr_linear);
	Volume<double> vol_zdr_linear;
	dB2lin(vol_zdr,vol_zdr_linear);
	double alpha=1.48; // horizontal to vertical noise ratio. 1.48 S-band (Schuur 2003)
	for(unsigned el=0;el<vol_rhohv.size();el++)
	{
		// vol_rhohv[el].array()*=(vol_snr_linear[el].array()+1)/vol_snr_linear[el].array(); // TODO: rhohv è già > 1 in molti casi
													// questa correzione la aumenta ancora di più
		vol_snr_linear[el].array()*=alpha;
		vol_zdr_linear[el].array()=vol_snr_linear[el].array()*vol_zdr_linear[el].array()/(vol_snr_linear[el].array()+alpha-vol_zdr_linear[el].array());
	}
	lin2dB(vol_zdr_linear,vol_zdr);
	cout<<"finito snr"<<endl;
}

void classifier::compute_derived_volumes()
{
	//correct_phidp();	//TODO: probabilmente inutile se adottiamo il metodo di Vulpiani che stima direttamente la kdp
	//correct_for_snr();	//TODO: fa danni indicibili questa correzione. Maledetto Schuur 2003 !!!  

	compute_lkdp();
	correct_for_attenuation();
	
/*	const unsigned elev=4;
	const unsigned azim=370;
	for(unsigned rg=0;rg<vol_phidp[elev].beam_size;rg++)
	{
		cout<<fixed<<vol_phidp[elev](azim,rg)<<"\t"<<vol_phidp_2km[elev](azim,rg)<<"\t"<<vol_phidp_6km[elev](azim,rg)<<"\t"
				<<vol_lkdp_2km[elev](azim,rg)<<"\t"<<vol_lkdp_6km[elev](azim,rg)<<endl;
	}
*/
	// filtro i volumi
	printf("filtro Z 1 km\n");
	filter(vol_z,vol_z_1km,1000.,0.,false);
	printf("filtro Zdr 2 km\n");
	filter(vol_zdr,vol_zdr_2km,2000.,0.,false);	//TODO: test new filter on azimuth to enhance melting characteristics
	printf("filtro rhohv 2 km\n");
	filter(vol_rhohv,vol_rhohv_2km,2000.,3.,false);

	// calcolo le texture
	textureSD(vol_z,vol_sdz,1000.,0.,false);
	textureSD(vol_phidp,vol_sdphidp,2000.,0.,true);

	printf("calcolo i gradienti\n");
	gradient_azimuth(vol_z_1km, vol_grad_z_phi, false);
	gradient_elevation(vol_z_1km, vol_grad_z_theta, false);
	gradient_azimuth(vol_zdr_2km, vol_grad_zdr_phi,false);
	gradient_elevation(vol_zdr_2km, vol_grad_zdr_theta,false);
	gradient_azimuth(vol_phidp, vol_grad_phi_phi,true);
	gradient_elevation(vol_phidp, vol_grad_phi_theta,true);
}

void classifier::melting_layer_classification(MeltingLayer& ML)
{
	double elev;
	double Hb,Ht;
	for(unsigned el=0;el<vol_z.size();el++)
	{
		elev=vol_z.scan(el).elevation;
		cout<<"El "<<el<<"\t"<<elev<<endl;
		for(unsigned az=0;az<vol_z.scan(el).beam_count;az++)
		{
			Ht=ML.top[az];
			Hb=ML.bot[az];
			bool flag=true;
			unsigned rg=0;
			while(flag&&rg<vol_z.scan(el).beam_size)
			{
				if(vol_z.scan(el).height(rg,+0.45)<Hb)
				{
					vol_Ai[el][az][rg].Ai[DS]=0.;
					vol_Ai[el][az][rg].Ai[WS]=0.;
					vol_Ai[el][az][rg].Ai[CR]=0.;
					vol_Ai[el][az][rg].Ai[GR]=0.;
					rg++;
				}
				else flag=false;				
			}
			flag=true;
			while(flag&&rg<vol_z.scan(el).beam_size)
			{
				if(vol_z.scan(el).height(rg)<Hb)
				{
					vol_Ai[el][az][rg].Ai[DS]=0.;
					vol_Ai[el][az][rg].Ai[CR]=0.;
					rg++;
					//vol_Ai[el][az][rg].Ai[GR]=0.; // TODO: aggiunta a posteriori per vedere l'effetto che fa
				}
				else flag=false;
			}
			flag=true;
			while(flag&&rg<vol_z.scan(el).beam_size)
			{
				if(vol_z.scan(el).height(rg)<Ht)
				{
					vol_Ai[el][az][rg].Ai[CR]=0.;
					vol_Ai[el][az][rg].Ai[RA]=0.;
					vol_Ai[el][az][rg].Ai[HR]=0.;
					rg++;
				}
				else flag=false;
			}
			flag=true;
			while(flag&&rg<vol_z.scan(el).beam_size)
			{
				if(vol_z.scan(el).height(rg,-0.45)<Ht)
				{
					vol_Ai[el][az][rg].Ai[RA]=0.;
					vol_Ai[el][az][rg].Ai[HR]=0.;
					rg++;
				}
				else flag=false;
			}
			flag=true;
			while(rg<vol_z.scan(el).beam_size)
			{
				vol_Ai[el][az][rg].Ai[GC_AP]=0.;
				vol_Ai[el][az][rg].Ai[BS]=0.;
				vol_Ai[el][az][rg].Ai[WS]=0.;
				vol_Ai[el][az][rg].Ai[BD]=0.;
				vol_Ai[el][az][rg].Ai[RA]=0.;
				vol_Ai[el][az][rg].Ai[HR]=0.;
				rg++;
			}

		}
	}
}

void classifier::class_designation()
{
	for(unsigned el=0;el<vol_z.size();el++)
	{
		vol_hca.push_back(PolarScan<EchoClass>(vol_z.scan(el).beam_count,vol_z.scan(el).beam_size, NC));
		vol_hca[el].elevation=vol_z[el].elevation;
		for(unsigned az=0;az<vol_z.scan(el).beam_count;az++)
		{
			for(unsigned rg=0;rg<vol_z.scan(el).beam_size;rg++)
			{
				vol_hca[el](az,rg)=vol_Ai[el][az][rg].echo(0.00001);
			}
		}
	}
}

void classifier::HCA_Park_2009()
{
	vector< vector< HCA_Park > > SCAN;
	vector< HCA_Park > BEAM;
	printf("inizio HCA\n");
	vol_Ai.resize(vol_z.size());
	double Z,Zdr,rhohv,lkdp,sdz,sdphidp,vrad;
	double phidp,snr,gradphitheta,gradphiphi,gradZtheta,gradZphi,gradZdrtheta,gradZdrphi;
	for(unsigned el=0;el<vol_z.size();el++)
	{
		cout<<"\tHCA el "<<el<<endl;
		SCAN.resize(vol_z.scan(el).beam_count);
		for(unsigned az=0;az<vol_z.scan(el).beam_count;az++)
		{
			BEAM.resize(vol_z.scan(el).beam_size);
			for(unsigned rg=0;rg<vol_z.scan(el).beam_size;rg++)
			{
				Z=vol_z_1km.scan(el).get(az,rg);
				Zdr=vol_zdr_2km.scan(el).get(az,rg);
				rhohv=vol_rhohv_2km.scan(el).get(az,rg);
				lkdp=Z>40?vol_lkdp_2km[el].get(az,rg):vol_lkdp_6km[el].get(az,rg);
				sdz=vol_sdz.scan(el).get(az,rg);
				sdphidp=vol_sdphidp.scan(el).get(az,rg);
				vrad=vol_vrad.scan(el).get(az,rg);
				phidp=vol_phidp[el](az,rg);
				snr=vol_snr[el](az,rg);
				gradphitheta=vol_grad_phi_theta[el](az,rg);
				gradphiphi=vol_grad_phi_phi[el](az,rg);
				gradZtheta=vol_grad_z_theta[el](az,rg);
				gradZphi=vol_grad_z_phi[el](az,rg);
				gradZdrtheta=vol_grad_zdr_theta[el](az,rg);
				gradZdrphi=vol_grad_zdr_phi[el](az,rg);
				HCA_Park hca(Z,Zdr,rhohv,lkdp,sdz,sdphidp,vrad,phidp,snr,gradphitheta,gradphiphi,gradZtheta,gradZphi,gradZdrtheta,gradZdrphi);
				BEAM[rg]=hca;
			}
			SCAN[az]=BEAM;
		}
		vol_Ai[el]=SCAN;
	}
	// Dopo aver calcolato i valori di aggregazione cerco il melting layer
	MeltingLayer ML(vol_z,vol_zdr,vol_rhohv,vol_Ai);
	cout<<"applico ML criteria ad HCA"<<endl;
	melting_layer_classification(ML);
	class_designation();
	unsigned elev=2;
	unsigned azim=350;
/*	cout<<"GC\tBS\tDS\tWS\tCR\tGR\tBD\tRA\tHR\tRH"<<endl;
	for(unsigned rg=0;rg<vol_Ai[elev][azim].size();rg++)
	{
		cout.precision(5);
		cout<<fixed<<vol_Ai[elev][azim][rg].Ai[GC_AP]<<"\t"<<vol_Ai[elev][azim][rg].Ai[BS]<<"\t"<<
		vol_Ai[elev][azim][rg].Ai[DS]<<"\t"<<vol_Ai[elev][azim][rg].Ai[WS]<<"\t"<<
		vol_Ai[elev][azim][rg].Ai[CR]<<"\t"<<vol_Ai[elev][azim][rg].Ai[GR]<<"\t"<<
		vol_Ai[elev][azim][rg].Ai[BD]<<"\t"<<vol_Ai[elev][azim][rg].Ai[RA]<<"\t"<<
		vol_Ai[elev][azim][rg].Ai[HR]<<"\t"<<vol_Ai[elev][azim][rg].Ai[RH]<<"\t"<<
		vol_hca[elev](azim,rg)<<endl;
	}
*/
}

void classifier::print_ppi_class(int elev=-1)
{
	gdal_init_once();
	if(elev>=0)
	{
		ostringstream filename;
		filename<<"class_"<<elev<<".png";
		Matrix2D<unsigned short> img;
		img=vol_hca[elev].cast<unsigned short>();
		img*=6553;
		write_image(img, filename.str(), "PNG");
	}
	else
	{
		for(unsigned el=0;el<vol_hca.size();el++)
		{
			ostringstream filename;
			filename<<"class_"<<el<<".png";
			Matrix2D<unsigned short> img;
			img=vol_hca[el].cast<unsigned short>();
			img*=6553;
			write_image(img, filename.str(), "PNG");
		}
	}
}
