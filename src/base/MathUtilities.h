#ifndef INCLUDED_MATHUTILITIES
#define INCLUDED_MATHUTILITIES

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

void CalcRotatingMatrix(double matrix[][3], double sita, CartesianType cartesianType){
   if(cartesianType == XAxis){
      matrix[0][0] = 1.0;
      matrix[0][1] = 0.0;
      matrix[0][2] = 0.0;

      matrix[1][0] = 0.0;
      matrix[1][1] = cos(sita);
      matrix[1][2] = sin(sita);

      matrix[2][0] = 0.0;
      matrix[2][1] = -sin(sita);
      matrix[2][2] = cos(sita);
   }
   else if(cartesianType == YAxis){
      matrix[0][0] = cos(sita);
      matrix[0][1] = 0.0;
      matrix[0][2] = -sin(sita);

      matrix[1][0] = 0.0;
      matrix[1][1] = 1.0;
      matrix[1][2] = 0.0;

      matrix[2][0] = sin(sita);
      matrix[2][1] = 0.0;
      matrix[2][2] = cos(sita);
   }
   else if(cartesianType == ZAxis){
      matrix[0][0] = cos(sita);
      matrix[0][1] = sin(sita);
      matrix[0][2] = 0.0;

      matrix[1][0] = -sin(sita);
      matrix[1][1] = cos(sita);
      matrix[1][2] = 0.0;

      matrix[2][0] = 0.0;
      matrix[2][1] = 0.0;
      matrix[2][2] = 1.0;
   }
   else{
      stringstream ss;
      ss << "Error in base::MathUtility::CalcRotatingMatrix: invalid cartesianType \n";
      throw MolDSException(ss.str());
   }
}






}
#endif
 
