#ifndef INCLUDED_MATHUTILITIES
#define INCLUDED_MATHUTILITIES

#include <stdio.h> 
#include <stdlib.h> 

using namespace std;

namespace MolDS_base{

// n!
int Factorial(int n){

   if(n<0){
      stringstream ss;
      ss << "Error in base::MathUtility::Factorial: n<0 \n";
      throw MolDSException(ss.str());
   }
   else if (n>1){
      return n*Factorial(n-1);
   }
   else{
      return 1;
   }
}

// nCk
int Conbination(int n, int k){
   
   if(n < 0){ 
      stringstream ss;
      ss << "Error in base::MathUtility::Conbination: n<0 \n";
      throw MolDSException(ss.str());
   }
   else if(k < 0){ 
      stringstream ss;
      ss << "Error in base::MathUtility::Conbination: k<0 \n";
      throw MolDSException(ss.str());
   }
   else if(n < k){ 
      stringstream ss;
      ss << "Error in base::MathUtility::Conbination: n<k \n";
      throw MolDSException(ss.str());
   }
   else{
      return Factorial(n)/(Factorial(k)*Factorial(n-k));
   }

}

// max
template <typename T> T Max(T a, T b){
   if(a<b){
      return b;
   }
   else{
      return a;
   }
}

// min
template <typename T> T min(T a, T b){
   if(a<b){
      return a;
   }
   else{
      return b;
   }
}


}
#endif
 
