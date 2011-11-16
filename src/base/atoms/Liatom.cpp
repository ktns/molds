#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<sstream>
#include<math.h>
#include<vector>
#include"../Enums.h"
#include"../EularAngle.h"
#include"../Parameters.h"
#include"Atom.h"
#include"Liatom.h"
using namespace std;
using namespace MolDS_base;

namespace MolDS_base_atoms{
Liatom::Liatom(double x, double y, double z) : Atom(x, y, z){
   this->atomType = Li;
   this->atomicMass = 6.941*Parameters::GetInstance()->GetGMolin2AU();
   this->coreCharge = 1.0;
   this->numberValenceElectrons = 1;
   this->valenceShellType = l;
   this->valence.push_back(s);
   this->valence.push_back(py);
   this->valence.push_back(pz);
   this->valence.push_back(px);
   this->bondingParameter = -9.0*Parameters::GetInstance()->GetEV2AU();
   this->imuAmuS = 3.106*Parameters::GetInstance()->GetEV2AU();
   this->imuAmuP = 1.258*Parameters::GetInstance()->GetEV2AU();
   this->imuAmuD = 0.0;
   this->effectiveNuclearChargeK = 2.7;
   this->effectiveNuclearChargeL = 1.3;
   this->effectiveNuclearChargeMsp = 0.0;
   this->effectiveNuclearChargeMd = 0.0;
   this->indoG1 = 0.092012;
   this->indoF2 = 0.049865;
   this->indoF0CoefficientS = 0.5;
   this->indoF0CoefficientP = 0.5;
   this->indoG1CoefficientS = 0.0;
   this->indoG1CoefficientP = -1.0/12.0;
   this->indoF2CoefficientS = 0.0;
   this->indoF2CoefficientP = 0.0;
   this->zindoBondingParameterS = 0.0;
   this->zindoBondingParameterD = 0.0;
   this->zindoF0ss = 0.0;
   this->zindoF0sd = 0.0;        
   this->zindoF0dd = 0.0;              
   this->zindoG1sp = 20194*Parameters::GetInstance()->GetKayser2AU();           
   this->zindoF2pp = 10944*Parameters::GetInstance()->GetKayser2AU();        
   this->zindoG2sd = 0.0;                
   this->zindoG1pd = 0.0;                 
   this->zindoF2pd = 0.0;                
   this->zindoG3pd = 0.0;             
   this->zindoF2dd = 0.0;             
   this->zindoF4dd = 0.0;       
   this->zindoL = 1;
   this->zindoM = 0;
   this->zindoN = 0;
   this->zindoIonPotS = 5.39 * Parameters::GetInstance()->GetEV2AU();
   this->zindoIonPotP = 3.54 * Parameters::GetInstance()->GetEV2AU();
   this->zindoIonPotD = 0.0 * Parameters::GetInstance()->GetEV2AU();
}
}