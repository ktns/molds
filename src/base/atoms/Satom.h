#ifndef INCLUDED_SATOM
#define INCLUDED_SATOM

using namespace std;
using namespace MolDS_base;

namespace MolDS_base_atoms{

class Satom : public Atom {
public:
   Satom(double x, double y, double z);
private:
};

Satom::Satom(double x, double y, double z) : Atom(x, y, z){
   this->atomType = S;
   this->atomicMass = 32.066*Parameters::GetInstance()->GetGMolin2AU();
   this->valence.push_back(s);
   this->valence.push_back(py);
   this->valence.push_back(pz);
   this->valence.push_back(px);
   if(Parameters::GetInstance()->GetCurrentTheory() == CNDO2 || 
      Parameters::GetInstance()->GetCurrentTheory() == INDO){
      this->valence.push_back(dxy);
      this->valence.push_back(dyz);
      this->valence.push_back(dzz);
      this->valence.push_back(dzx);
      this->valence.push_back(dxxyy);
   }
   this->bondingParameter = -18.150*Parameters::GetInstance()->GetEV2AU();
   this->bondingParameterSZindo = -14.0*Parameters::GetInstance()->GetEV2AU();
   this->bondingParameterDZindo =   4.0*Parameters::GetInstance()->GetEV2AU();
   this->coreCharge = 6.0;
   this->imuAmuS = 17.650*Parameters::GetInstance()->GetEV2AU();
   this->imuAmuP = 6.989*Parameters::GetInstance()->GetEV2AU();
   this->imuAmuD = 0.713*Parameters::GetInstance()->GetEV2AU();
   this->valenceShellType = m;
   this->effectiveNuclearChargeK = 15.70;
   this->effectiveNuclearChargeL = 11.85;
   if(Parameters::GetInstance()->GetCurrentTheory() == ZINDOS){
      this->effectiveNuclearChargeMsp = 1.925*3.0;
      this->effectiveNuclearChargeMd = 1.731*3.0;
   }
   else{
      this->effectiveNuclearChargeMsp = 5.45;
      this->effectiveNuclearChargeMd = 5.45;
   }
   this->numberValenceElectrons = 6;
   this->indoG1 = 0.267708;
   this->indoF2 = 0.17372;
   this->indoF0CoefficientS = 0.0;
   this->indoF0CoefficientP = 0.0;
   this->indoG1CoefficientS = 0.0;
   this->indoG1CoefficientP = 0.0;
   this->indoF2CoefficientS = 0.0;
   this->indoF2CoefficientP = 0.0;
   // the zindoF0ss for sulfer atoms are set to be equal 
   // to the one (10.09eV) in "ORCA 2.8"( http://www.thch.uni-bonn.de/tc/orca/ ).
   this->zindoF0ss = 10.09 * Parameters::GetInstance()->GetEV2AU(); 
   //this->zindoF0ss = 8.96 * Parameters::GetInstance()->GetEV2AU();                  
   this->zindoF0sd = 0.0;                   
   this->zindoF0dd = 0.0;                 
   this->zindoG1sp = 3.10 * Parameters::GetInstance()->GetEV2AU();                 
   this->zindoF2pp = 4.57 * Parameters::GetInstance()->GetEV2AU();
   this->zindoG2sd = 3.25 * Parameters::GetInstance()->GetEV2AU();
   this->zindoG1pd = 4.31 * Parameters::GetInstance()->GetEV2AU();        
   this->zindoF2pd = 3.45 * Parameters::GetInstance()->GetEV2AU();
   this->zindoG3pd = 2.57 * Parameters::GetInstance()->GetEV2AU();
   this->zindoF2dd = 3.55 * Parameters::GetInstance()->GetEV2AU();
   this->zindoF4dd = 2.31 * Parameters::GetInstance()->GetEV2AU();
   //this->zindoG1sp = 24807*Parameters::GetInstance()->GetKayser2AU();                 
   //this->zindoF2pp = 36600*Parameters::GetInstance()->GetKayser2AU();                 
   //this->zindoG2sd = 25972*Parameters::GetInstance()->GetKayser2AU();     
   //this->zindoG1pd = 34486*Parameters::GetInstance()->GetKayser2AU();        
   //this->zindoF2pd = 29173*Parameters::GetInstance()->GetKayser2AU();           
   //this->zindoG3pd = 20587*Parameters::GetInstance()->GetKayser2AU();           
   //this->zindoF2dd = 28411*Parameters::GetInstance()->GetKayser2AU();           
   //this->zindoF4dd = 18529*Parameters::GetInstance()->GetKayser2AU();           
   this->zindoL = 2;
   this->zindoM = 4;
   this->zindoN = 0;
   this->ionPotS = 21.11 * Parameters::GetInstance()->GetEV2AU();
   this->ionPotP = 12.39 * Parameters::GetInstance()->GetEV2AU();
   this->ionPotD = 4.11 * Parameters::GetInstance()->GetEV2AU();
   this->mndoCoreintegralS = -72.242281 * Parameters::GetInstance()->GetEV2AU();         
   this->mndoCoreintegralP = -56.973207 * Parameters::GetInstance()->GetEV2AU();         
   this->mndoOrbitalExponentS = 2.312962;      
   this->mndoOrbitalExponentP = 2.009146;      
   this->mndoBondingParameterS = -10.761670 * Parameters::GetInstance()->GetEV2AU();     
   this->mndoBondingParameterP = -10.108433 * Parameters::GetInstance()->GetEV2AU();     
   this->mndoAlpha = 2.478026 / Parameters::GetInstance()->GetAngstrom2AU();        
   this->mndoElecEnergyAtom = -226.01239 * Parameters::GetInstance()->GetEV2AU();        
   this->mndoHeatsFormAtom = 66.40 * Parameters::GetInstance()->GetKcalMolin2AU();
   this->mndoGss =  12.88 * Parameters::GetInstance()->GetEV2AU();   
   this->mndoGpp =   9.90 * Parameters::GetInstance()->GetEV2AU();   
   this->mndoGsp =  11.26 * Parameters::GetInstance()->GetEV2AU();   
   this->mndoGpp2 =  8.83 * Parameters::GetInstance()->GetEV2AU();  
   this->mndoHsp =   2.26 * Parameters::GetInstance()->GetEV2AU();   
   this->mndoDerivedParameterD[0] =   0.0;
   this->mndoDerivedParameterD[1] =   0.9189935137;
   this->mndoDerivedParameterD[2] =   0.8328513971;
   this->mndoDerivedParameterRho[0] = 0.5/0.4733275064;
   this->mndoDerivedParameterRho[1] = 0.5/0.5544352823;
   this->mndoDerivedParameterRho[2] = 0.5/0.5585137839;
   this->am1CoreintegralS = -56.694056 * Parameters::GetInstance()->GetEV2AU();         
   this->am1CoreintegralP = -48.717049 * Parameters::GetInstance()->GetEV2AU();         
   this->am1OrbitalExponentS = 2.366515;      
   this->am1OrbitalExponentP = 1.667263;      
   this->am1BondingParameterS = -3.920566 * Parameters::GetInstance()->GetEV2AU();     
   this->am1BondingParameterP = -7.905278 * Parameters::GetInstance()->GetEV2AU();     
   this->am1Alpha = 2.461648 / Parameters::GetInstance()->GetAngstrom2AU();        
   this->am1Gss =  11.786329 * Parameters::GetInstance()->GetEV2AU();   
   this->am1Gpp =  10.039308 * Parameters::GetInstance()->GetEV2AU();   
   this->am1Gsp =   8.663127 * Parameters::GetInstance()->GetEV2AU();   
   this->am1Gpp2 =  7.781688 * Parameters::GetInstance()->GetEV2AU();  
   this->am1Hsp =   2.532137 * Parameters::GetInstance()->GetEV2AU();   
   this->am1DerivedParameterD[0] = 0.0;    
   this->am1DerivedParameterD[1] = 0.9004264562;    
   this->am1DerivedParameterD[2] = 1.0036329320;    
   this->am1DerivedParameterRho[0] = 0.5/0.4331361580;
   this->am1DerivedParameterRho[1] = 0.5/0.5906953135;  
   this->am1DerivedParameterRho[2] = 0.5/0.6454793983;  
   this->am1ParameterK[0] =-0.509195 * Parameters::GetInstance()->GetEV2AU();
   this->am1ParameterK[1] =-0.011863 * Parameters::GetInstance()->GetEV2AU();
   this->am1ParameterK[2] = 0.012334 * Parameters::GetInstance()->GetEV2AU();
   this->am1ParameterK[3] = 0.00 * Parameters::GetInstance()->GetEV2AU();
   this->am1ParameterL[0] = 4.593691 / pow(Parameters::GetInstance()->GetAngstrom2AU(),2.0);
   this->am1ParameterL[1] = 5.865731 / pow(Parameters::GetInstance()->GetAngstrom2AU(),2.0);
   this->am1ParameterL[2] = 13.557336 / pow(Parameters::GetInstance()->GetAngstrom2AU(),2.0);
   this->am1ParameterL[3] = 0.00 / pow(Parameters::GetInstance()->GetAngstrom2AU(),2.0);
   this->am1ParameterM[0] = 0.770665 * Parameters::GetInstance()->GetAngstrom2AU();
   this->am1ParameterM[1] = 1.503313 * Parameters::GetInstance()->GetAngstrom2AU();
   this->am1ParameterM[2] = 2.009173 * Parameters::GetInstance()->GetAngstrom2AU();
   this->am1ParameterM[3] = 0.00 * Parameters::GetInstance()->GetAngstrom2AU();
   this->pm3CoreintegralS = -49.895371 * Parameters::GetInstance()->GetEV2AU();         
   this->pm3CoreintegralP = -44.392583 * Parameters::GetInstance()->GetEV2AU();         
   this->pm3OrbitalExponentS = 1.891185;      
   this->pm3OrbitalExponentP = 1.658972;      
   this->pm3BondingParameterS = -8.827465 * Parameters::GetInstance()->GetEV2AU();     
   this->pm3BondingParameterP = -8.091415 * Parameters::GetInstance()->GetEV2AU();     
   this->pm3Alpha = 2.269706 / Parameters::GetInstance()->GetAngstrom2AU();        
   this->pm3Gss = 8.964667 * Parameters::GetInstance()->GetEV2AU();   
   this->pm3Gpp = 9.968164 * Parameters::GetInstance()->GetEV2AU();   
   this->pm3Gsp = 6.785936 * Parameters::GetInstance()->GetEV2AU();   
   this->pm3Gpp2 = 7.970247 * Parameters::GetInstance()->GetEV2AU();   
   this->pm3Hsp = 4.041836 * Parameters::GetInstance()->GetEV2AU();   
   this->pm3DerivedParameterD[0] = 0.0;    
   this->pm3DerivedParameterD[1] = 1.1214312500;    
   this->pm3DerivedParameterD[2] = 1.0086487614;    
   this->pm3DerivedParameterRho[0] = 0.5/0.3294428165;
   this->pm3DerivedParameterRho[1] = 0.5/0.6678906502;  
   this->pm3DerivedParameterRho[2] = 0.5/0.6137333700;  
   this->pm3ParameterK[0] = -0.399191 * Parameters::GetInstance()->GetEV2AU();
   this->pm3ParameterK[1] = -0.054899 * Parameters::GetInstance()->GetEV2AU();
   this->pm3ParameterK[2] = 0.0;
   this->pm3ParameterK[3] = 0.0;
   this->pm3ParameterL[0] = 6.000669 / pow(Parameters::GetInstance()->GetAngstrom2AU(),2.0);
   this->pm3ParameterL[1] = 6.001845 / pow(Parameters::GetInstance()->GetAngstrom2AU(),2.0);
   this->pm3ParameterL[2] = 0.00;
   this->pm3ParameterL[3] = 0.00;
   this->pm3ParameterM[0] = 0.962123 * Parameters::GetInstance()->GetAngstrom2AU();
   this->pm3ParameterM[1] = 1.579944 * Parameters::GetInstance()->GetAngstrom2AU();
   this->pm3ParameterM[2] = 0.00;
   this->pm3ParameterM[3] = 0.00;
}
}
#endif
