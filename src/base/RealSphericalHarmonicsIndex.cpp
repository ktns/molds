#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<sstream>
#include<string>
#include"Enums.h"
#include"RealSphericalHarmonicsIndex.h"
using namespace std;
namespace MolDS_base{

RealSphericalHarmonicsIndex::RealSphericalHarmonicsIndex(){}

RealSphericalHarmonicsIndex::RealSphericalHarmonicsIndex(OrbitalType orbitalType){
   string errorMessageInvalidOrbital = "Error in base::RealSphericalHarmonicIndex::RealSphericalHarmonicIndex: invalid orbitalType. Indicated orbitalType is not prepared\n";

   if(orbitalType == s){
      this->l = 0;
      this->m = 0;
   }
   else if(orbitalType == py){
      this->l = 1;
      this->m = -1;
   }
   else if(orbitalType == pz){
      this->l = 1;
      this->m = 0;
   }
   else if(orbitalType == px){
      this->l = 1;
      this->m = 1;
   }
   else if(orbitalType == dxy){
      this->l = 2;
      this->m = -2;
   }
   else if(orbitalType == dyz){
      this->l = 2;
      this->m = -1;
   }
   else if(orbitalType == dzz){
      this->l = 2;
      this->m = 0;
   }
   else if(orbitalType == dzx){
      this->l = 2;
      this->m = 1;
   }
   else if(orbitalType == dxxyy){
      this->l = 2;
      this->m = 2;
   }
   else{
      // ToDo: error handling
      cout << errorMessageInvalidOrbital;
   }

}

RealSphericalHarmonicsIndex::RealSphericalHarmonicsIndex(int l, int m){
   this->l = l;
   this->m = m;
}

int RealSphericalHarmonicsIndex::GetL(){
   return this->l;
}

int RealSphericalHarmonicsIndex::GetM(){
   return this->m;
}

}
