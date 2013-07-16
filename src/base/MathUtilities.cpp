//************************************************************************//
// Copyright (C) 2011-2012 Mikiya Fujii                                   // 
//                                                                        // 
// This file is part of MolDS.                                            // 
//                                                                        // 
// MolDS is free software: you can redistribute it and/or modify          // 
// it under the terms of the GNU General Public License as published by   // 
// the Free Software Foundation, either version 3 of the License, or      // 
// (at your option) any later version.                                    // 
//                                                                        // 
// MolDS is distributed in the hope that it will be useful,               // 
// but WITHOUT ANY WARRANTY; without even the implied warranty of         // 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          // 
// GNU General Public License for more details.                           // 
//                                                                        // 
// You should have received a copy of the GNU General Public License      // 
// along with MolDS.  If not, see <http://www.gnu.org/licenses/>.         // 
//************************************************************************//
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<sstream>
#include<math.h>
#include<stdexcept>
#include<boost/format.hpp>
#include"config.h"
#ifdef HAVE_BOOST_MATH_SPECIAL_FUNCTIONS_FACTORIALS_HPP
#include<boost/math/special_functions/factorials.hpp>
#endif
#include"Uncopyable.h"
#include"../mpi/MpiProcess.h"
#include"PrintController.h"
#include"MolDSException.h"
#include"../wrappers/Lapack.h"
#include"Enums.h"
#include"MathUtilities.h"
#include"MallocerFreer.h"
using namespace std;

namespace MolDS_base{

// n!
int Factorial(int n){
   if(n<0){
      stringstream ss;
      ss << "Error in base::MathUtility::Factorial: n<0 \n";
      throw MolDSException(ss.str());
   }
#ifdef HAVE_BOOST_MATH_SPECIAL_FUNCTIONS_FACTORIALS_HPP
   return static_cast<int>(boost::math::factorial<double>(n));
#else
   else if (n>1){
      return n*Factorial(n-1);
   }
   else{
      return 1;
   }
#endif
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

// rotating matrix
void CalcRotatingMatrix(double matrix[][3], double theta, CartesianType cartesianType){
   if(cartesianType == XAxis){
      matrix[0][0] = 1.0;
      matrix[0][1] = 0.0;
      matrix[0][2] = 0.0;

      matrix[1][0] = 0.0;
      matrix[1][1] = cos(theta);
      matrix[1][2] = sin(theta);

      matrix[2][0] = 0.0;
      matrix[2][1] = -sin(theta);
      matrix[2][2] = cos(theta);
   }
   else if(cartesianType == YAxis){
      matrix[0][0] = cos(theta);
      matrix[0][1] = 0.0;
      matrix[0][2] = -sin(theta);

      matrix[1][0] = 0.0;
      matrix[1][1] = 1.0;
      matrix[1][2] = 0.0;

      matrix[2][0] = sin(theta);
      matrix[2][1] = 0.0;
      matrix[2][2] = cos(theta);
   }
   else if(cartesianType == ZAxis){
      matrix[0][0] = cos(theta);
      matrix[0][1] = sin(theta);
      matrix[0][2] = 0.0;

      matrix[1][0] = -sin(theta);
      matrix[1][1] = cos(theta);
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

// calculate determinant of the matrix
double GetDeterminant(double** matrix, int dim){
   double determinant=1.0;
   MolDS_wrappers::molds_lapack_int* ipiv=NULL;
   MallocerFreer::GetInstance()->Malloc<MolDS_wrappers::molds_lapack_int>(&ipiv, dim);
   MolDS_wrappers::Lapack::GetInstance()->Dgetrf(matrix, ipiv, dim, dim);
   for(int i=0; i<dim; i++){
      determinant*=matrix[i][i];
      if(ipiv[i] != i-1){
         determinant *= -1.0;
      }
   }
   MallocerFreer::GetInstance()->Free<MolDS_wrappers::molds_lapack_int>(&ipiv, dim);
   return determinant;
}
}
 
