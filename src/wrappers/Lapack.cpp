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
#include<algorithm>
#include<math.h>
#include<string>
#include<stdexcept>
#include<boost/format.hpp>
#include"../base/Enums.h"
#include"../base/Uncopyable.h"
#include"../base/PrintController.h"
#include"../base/MolDSException.h"
#include"../base/MallocerFreer.h"
#include"../mpi/MpiProcess.h"
#include"Lapack.h"

#ifdef __INTEL_COMPILER
#include"mkl.h"
#include"mkl_scalapack.h"
#include"mkl_blacs.h"
#include"mkl_pblas.h"
#else
#if ( __WORDSIZE == 32 )
#else
#define HAVE_LAPACK_CONFIG_H
#define LAPACK_ILP64
#endif
#include"lapacke.h"
#endif

#ifdef __INTEL_COMPILER
#define MOLDS_LAPACK_malloc(a,b) mkl_malloc(a,b)
#define MOLDS_LAPACK_free(a) mkl_free(a)
#else
#define MOLDS_LAPACK_malloc(a,b) malloc(a)
#define MOLDS_LAPACK_free(a) free(a)
#endif


using namespace std;
using namespace MolDS_base;

namespace MolDS_wrappers{
Lapack* Lapack::lapack = NULL;

Lapack::Lapack(){
   this->errorMessageDsyevdInfo = "Error in wrappers::Lapack::Dsyevd: info != 0: info = ";
   this->errorMessageDsyevdSize = "Error in wrappers::Lapack::Dsyevd: size of matirx < 1\n";
   this->errorMessageDsysvInfo = "Error in wrappers::Lapack::Dsysv: info != 0: info = ";
   this->errorMessageDsysvSize = "Error in wrappers::Lapack::Dsysv: size of matirx < 1\n";
   this->errorMessageDgetrsInfo = "Error in wrappers::Lapack::Dgetrs: info != 0: info = ";
   this->errorMessageDgetrsSize = "Error in wrappers::Lapack::Dgetrs: size of matirx < 1\n";
   this->errorMessageDgetrfInfo = "Error in wrappers::Lapack::Dgetrf: info != 0: info = ";
}

Lapack::~Lapack(){
}

Lapack* Lapack::GetInstance(){
   if(lapack == NULL){
      lapack = new Lapack();
      //this->OutputLog("Lapack created.\n\n");
   }
   return lapack;
}

void Lapack::DeleteInstance(){
   if(lapack != NULL){
      delete lapack;
      //this->OutputLog("Lapack deleted\n\n");
   }
   lapack = NULL;
}


/***
 *
 * Eigenvalue and eigenvector of a real symmetirc matrix are calculated:
 *   - i-th eigenvalue will be stored in eigenValues[i].
 *   - i-th eigenvector will be stored as (matirx[i][0], matirx[i][1], matirx[i][2], ....).
 *
 * ***/
molds_lapack_int Lapack::Dsyevd_sca(double** matrix, double* eigenValues, molds_lapack_int size, bool calcEigenVectors){
   if(size < 1 ){
      stringstream ss;
      ss << errorMessageDsyevdSize;
      MolDSException ex(ss.str());
      throw ex;
   }

   molds_lapack_int info = 0;
   char job;
   char uplo = 'U';
   molds_lapack_int lda = size;
   double* localMatrix;
   double* localEigenVector;
   double* tempEigenValues;

   // set job type
   if(calcEigenVectors){
      job = 'V';
   }
   else{
      job = 'N';
   }

   char order='R';
   molds_lapack_int npRow=2;     // ToDo: change to dynamical setting
   molds_lapack_int npCol=2;     // ToDo: change to dynamical setting
   molds_lapack_int blockSize=3; // ToDo: change to dynamical setting

   //tmporal values
   molds_lapack_int intOne=1;
   molds_lapack_int intZero=0;
   double dblOne =1e0;
   double dblZero=0e0;
   molds_lapack_int myRow   =intZero;
   molds_lapack_int myCol   =intZero;
   molds_lapack_int iContext=intZero;
   molds_lapack_int what    =intZero;
   molds_lapack_int val     =intZero;
   molds_lapack_int rsrc    =intZero;
   molds_lapack_int csrc    =intZero;
   molds_lapack_int mpiRank =intZero;
   molds_lapack_int mpiSize =intZero;

   // initialize blacs for calling scalapack
   blacs_pinfo_(&mpiRank, &mpiSize);
   blacs_get_(&iContext, &what, &val);
   blacs_gridinit_(&iContext, &order, &npRow, &npCol);
   blacs_gridinfo_(&iContext, &npRow, &npCol, &myRow, &myCol);

   // calculate size of local matrix on each node
   molds_lapack_int mp = numroc_(&size, &blockSize, &myRow, &rsrc, &npRow);
   molds_lapack_int nq = numroc_(&size, &blockSize, &myCol, &rsrc, &npCol);
   localMatrix = (double*)MOLDS_LAPACK_malloc( sizeof(double)*mp*nq, 16 );
   localEigenVector   = (double*)MOLDS_LAPACK_malloc( sizeof(double)*mp*nq, 16 );
   tempEigenValues = (double*)MOLDS_LAPACK_malloc( sizeof(double)*size, 16 );
   molds_lapack_int lldA=max(intOne,mp);
   molds_lapack_int lldZ=max(intOne,mp);
   molds_lapack_int descA[9];
   molds_lapack_int descZ[9];
   descinit_(descA, &size, &size, &blockSize, &blockSize, &intZero, &intZero, &iContext, &lldA,       &info);
   descinit_(descZ, &size, &size, &blockSize, &blockSize, &intZero, &intZero, &iContext, &lldZ,       &info);

   // distribute data to local matrix on each node accoding to the block syclic decomposition
   for(molds_lapack_int i=0; i<mp; i++){
      for(molds_lapack_int j=0; j<nq; j++){
         molds_lapack_int globalI = (myRow + (i/blockSize)*npRow)*blockSize + i%blockSize;
         molds_lapack_int globalJ = (myCol + (j/blockSize)*npCol)*blockSize + j%blockSize;
         localMatrix[j*mp+i] = matrix[globalI][globalJ];
      }
   }

   // calculate working array space for pdsyevd
   molds_lapack_int  lwork = -1;
   molds_lapack_int liwork = -1;
   double           tmpWork[3]  = {0.0, 0.0, 0.0};
   molds_lapack_int tmpIwork[3] = {0, 0, 0};
   molds_lapack_int ia=intOne;
   molds_lapack_int ja=intOne;
   molds_lapack_int iz=intOne;
   molds_lapack_int jz=intOne;
   pdsyevd(&job, &uplo, &size, localMatrix, &ia, &ja, descA, tempEigenValues, localEigenVector, &iz, &jz ,descZ, tmpWork, &lwork, tmpIwork, &liwork, &info);

   // cll scalapack (pdsyevd)
   info = 0;
   double*            work=NULL;
   molds_lapack_int* iwork=NULL;
   lwork  = tmpWork[0];
   liwork = tmpIwork[0];
   work = (double*)MOLDS_LAPACK_malloc( sizeof(double)*lwork, 16 );
   iwork = (molds_lapack_int*)MOLDS_LAPACK_malloc( sizeof(molds_lapack_int)*liwork, 16 );
   pdsyevd(&job, &uplo, &size, localMatrix, &ia, &ja, descA, eigenValues, localEigenVector, &iz, &jz ,descZ, work, &lwork, iwork, &liwork, &info);

   // gathter block cyclic decomposed eigenvector to the global data
   MallocerFreer::GetInstance()->Initialize<double>(matrix, size, size);
   for(molds_lapack_int j=0; j<nq; j++){
      molds_lapack_int globalJ = (myCol + (j/blockSize)*npCol)*blockSize + j%blockSize;
      for(molds_lapack_int i=0; i<mp; i++){
         molds_lapack_int globalI = (myRow + (i/blockSize)*npRow)*blockSize + i%blockSize;
         matrix[globalJ][globalI] = localEigenVector[j*mp+i]; // globalj-th row    is globalj-th eigen vector
         //matrix[globalI][globalJ] = localEigenVector[j*mp+i]; // globalj-th column is globalj-th eigen vector
      }
   }
   MolDS_mpi::MpiProcess::GetInstance()->AllReduce(&matrix[0][0], size*size, std::plus<double>());

   for(molds_lapack_int i=0;i<size;i++){
      double temp = 0.0;
      for(molds_lapack_int j=0;j<size;j++){
         temp += matrix[i][j];
      }
      if(temp<0){
         for(molds_lapack_int j=0;j<size;j++){
            matrix[i][j]*=-1.0;
         }
      }
   }   

   // free
   MOLDS_LAPACK_free(work);
   MOLDS_LAPACK_free(iwork);
   MOLDS_LAPACK_free(localMatrix);
   MOLDS_LAPACK_free(localEigenVector);
   MOLDS_LAPACK_free(tempEigenValues);

   blacs_gridexit_(&iContext);
   return info;
}

/***
 *
 * Eigenvalue and eigenvector of a real symmetirc matrix are calculated:
 *   - i-th eigenvalue will be stored in eigenValues[i].
 *   - i-th eigenvector will be stored as (matirx[i][0], matirx[i][1], matirx[i][2], ....).
 *
 * ***/
molds_lapack_int Lapack::Dsyevd(double** matrix, double* eigenValues, molds_lapack_int size, bool calcEigenVectors){
   molds_lapack_int info = 0;
   molds_lapack_int k = 0;
   molds_lapack_int lwork;
   molds_lapack_int liwork;
   char job;
   char uplo = 'U';
   molds_lapack_int lda = size;
   double* convertedMatrix;
   double* tempEigenValues;
   double* work;
   molds_lapack_int* iwork;

   // set job type
   if(calcEigenVectors){
      job = 'V';
   }
   else{
      job = 'N';
   }

   // calc. lwork and liwork
   if(size < 1 ){
      stringstream ss;
      ss << errorMessageDsyevdSize;
      MolDSException ex(ss.str());
      ex.SetKeyValue<int>(LapackInfo, info);
      throw ex;
   }
   else if(size == 1){
      lwork = 1;
      liwork = 1;
   }
   else if(1 < size && job == 'N'){
      lwork = 2*size + 1;
      liwork = 2;
   }
   else{
      // calc. k
      double temp = log((double)size)/log(2.0);
      if( (double)((molds_lapack_int)temp) < temp ){
         k = (molds_lapack_int)temp + 1;
      }
      else{
         k = (molds_lapack_int)temp;
      }
      lwork = 3*size*size + (5+2*k)*size + 1;
      liwork = 5*size + 3;
   }

   // malloc
   work = (double*)MOLDS_LAPACK_malloc( sizeof(double)*lwork, 16 );
   iwork = (molds_lapack_int*)MOLDS_LAPACK_malloc( sizeof(molds_lapack_int)*liwork, 16 );
   convertedMatrix = (double*)MOLDS_LAPACK_malloc( sizeof(double)*size*size, 16 );
   tempEigenValues = (double*)MOLDS_LAPACK_malloc( sizeof(double)*size, 16 );

   for(molds_lapack_int i = 0; i < size; i++){
      for(molds_lapack_int j = i; j < size; j++){
         convertedMatrix[i+j*size] = matrix[i][j];
      }
   }

   // call Lapack
#ifdef __INTEL_COMPILER
   dsyevd(&job, &uplo, &size, convertedMatrix, &lda, tempEigenValues, work, &lwork, iwork, &liwork, &info);
#else
   info = LAPACKE_dsyevd_work(LAPACK_COL_MAJOR, job, uplo, size, convertedMatrix, lda, tempEigenValues, work, lwork, iwork, liwork);
#endif

   for(molds_lapack_int i = 0; i < size; i++){
      for(molds_lapack_int j = 0; j < size; j++){
         matrix[i][j] = convertedMatrix[j+i*size];  //i-th row is i-th eigen vector
         //matrix[j][i] = convertedMatrix[j+i*size];  //i-th column is i-th eigen vector
      }
   }

   for(molds_lapack_int i=0;i<size;i++){
      double temp = 0.0;
      for(molds_lapack_int j=0;j<size;j++){
         temp += matrix[i][j];
      }
      if(temp<0){
         for(molds_lapack_int j=0;j<size;j++){
            matrix[i][j]*=-1.0;
         }
      }
   }   

   for(molds_lapack_int i = 0; i < size; i++){
      eigenValues[i] = tempEigenValues[i];
   }

   molds_lapack_int mpiRank;
   molds_lapack_int mpiSize;
   blacs_pinfo_(&mpiRank, &mpiSize);
   if(mpiRank==0){
      for(molds_lapack_int i = 0; i < size; i++){
         printf("eig:%ld %e\n",i,eigenValues[i]);
      }
   }

   //this->OutputLog(boost::format("size=%d lwork=%d liwork=%d k=%d info=%d\n") % size % lwork % liwork % k % info);

   // free
   MOLDS_LAPACK_free(work);
   MOLDS_LAPACK_free(iwork);
   MOLDS_LAPACK_free(convertedMatrix);
   MOLDS_LAPACK_free(tempEigenValues);
  
   if(info != 0){
      stringstream ss;
      ss << errorMessageDsyevdInfo;
      ss << info << endl;
      MolDSException ex(ss.str());
      ex.SetKeyValue<int>(LapackInfo, info);
      throw ex;
   }
   return info;
}

/***
 *
 * "matrix*X=b" is solved, then we get X by this method.
 * The X will be stored in b.
 *
 */
molds_lapack_int Lapack::Dsysv(double const* const* matrix, double* b, molds_lapack_int size){
   molds_lapack_int info = 0;
   molds_lapack_int lwork;
   char uplo = 'U';
   molds_lapack_int lda = size;
   molds_lapack_int ldb = size;
   molds_lapack_int nrhs = 1;
   double* convertedMatrix;
   double* work;
   double* tempB;
   molds_lapack_int* ipiv;

   if(size < 1 ){
      stringstream ss;
      ss << errorMessageDsysvSize;
      throw MolDSException(ss.str());
   }

   // malloc
   ipiv = (molds_lapack_int*)MOLDS_LAPACK_malloc( sizeof(molds_lapack_int)*2*size, 16 );
   convertedMatrix = (double*)MOLDS_LAPACK_malloc( sizeof(double)*size*size, 16 );
   tempB = (double*)MOLDS_LAPACK_malloc( sizeof(double)*size, 16 );

   for(molds_lapack_int i = 0; i < size; i++){
      for(molds_lapack_int j = i; j < size; j++){
         convertedMatrix[i+j*size] = matrix[i][j];
      }
   }
   for(molds_lapack_int i = 0; i < size; i++){
      tempB[i] = b[i];
   }

   // calc. lwork
   double blockSize=0.0;
#pragma omp critical
   {
      lwork = -1;
      double tempWork[3]={0.0, 0.0, 0.0};
#ifdef __INTEL_COMPILER
         dsysv(&uplo, &size, &nrhs, convertedMatrix, &lda, ipiv, tempB, &ldb, tempWork, &lwork, &info);
#else
         info = LAPACKE_dsysv_work(LAPACK_COL_MAJOR, uplo, size, nrhs, convertedMatrix, lda, ipiv, tempB, ldb, tempWork, lwork);
#endif
      blockSize = tempWork[0]/size;
   }
   info = 0;
   lwork = blockSize*size;
   work = (double*)MOLDS_LAPACK_malloc( sizeof(double)*lwork, 16 );

   // call Lapack
#ifdef __INTEL_COMPILER
   dsysv(&uplo, &size, &nrhs, convertedMatrix, &lda, ipiv, tempB, &ldb, work, &lwork, &info);
#else
   info = LAPACKE_dsysv_work(LAPACK_COL_MAJOR, uplo, size, nrhs, convertedMatrix, lda, ipiv, tempB, ldb, work, lwork);
#endif
   for(molds_lapack_int i = 0; i < size; i++){
      b[i] = tempB[i];
   }

   // free
   MOLDS_LAPACK_free(convertedMatrix);
   MOLDS_LAPACK_free(ipiv);
   MOLDS_LAPACK_free(work);
   MOLDS_LAPACK_free(tempB);
  
   if(info != 0){
      stringstream ss;
      ss << errorMessageDsysvInfo;
      ss << info << endl;
      MolDSException ex(ss.str());
      ex.SetKeyValue<int>(LapackInfo, info);
      throw ex;
   }
   return info;
}

/***
 *
 * "matrix*X[i]=b[i] (i=0, 1, ... , nrhs-1) is solved, then we get X[i] by this method.
 * The X[i] will be stored in b[i].
 * b[i][j] is j-th element of i-th solution, b[i].
 *
 */
molds_lapack_int Lapack::Dgetrs(double const* const* matrix, double** b, molds_lapack_int size, molds_lapack_int nrhs) const{
   molds_lapack_int info = 0;
   char trans = 'N';
   molds_lapack_int lda = size;
   molds_lapack_int ldb = size;
   double* convertedMatrix;
   double* convertedB;
   molds_lapack_int* ipiv;

   if(size < 1 ){
      stringstream ss;
      ss << errorMessageDgetrsSize;
      throw MolDSException(ss.str());
   }


   try{
      // malloc
      ipiv = (molds_lapack_int*)MOLDS_LAPACK_malloc( sizeof(molds_lapack_int)*2*size, 16 );
      convertedMatrix = (double*)MOLDS_LAPACK_malloc( sizeof(double)*size*size, 16 );
      convertedB = (double*)MOLDS_LAPACK_malloc( sizeof(double)*nrhs*size, 16 );
      for(molds_lapack_int i = 0; i < size; i++){
         for(molds_lapack_int j = 0; j < size; j++){
            convertedMatrix[i+j*size] = matrix[i][j];
         }
      }
      for(molds_lapack_int i = 0; i < nrhs; i++){
         for(molds_lapack_int j = 0; j < size; j++){
            convertedB[j+i*size] = b[i][j];
         }
      }
      this->Dgetrf(convertedMatrix, ipiv, size, size);
#ifdef __INTEL_COMPILER
      dgetrs(&trans, &size, &nrhs, convertedMatrix, &lda, ipiv, convertedB, &ldb, &info);
#else
      info = LAPACKE_dgetrs_work(LAPACK_COL_MAJOR, trans, size, nrhs, convertedMatrix, lda, ipiv, convertedB, ldb);
#endif
      for(molds_lapack_int i = 0; i < nrhs; i++){
         for(molds_lapack_int j = 0; j < size; j++){
            b[i][j] = convertedB[j+i*size];
         }
      }
   }
   catch(MolDSException ex){
      // free
      MOLDS_LAPACK_free(convertedMatrix);
      MOLDS_LAPACK_free(convertedB);
      MOLDS_LAPACK_free(ipiv);
      throw ex;
   }
   // free
   MOLDS_LAPACK_free(convertedMatrix);
   MOLDS_LAPACK_free(convertedB);
   MOLDS_LAPACK_free(ipiv);
  
   if(info != 0){
      stringstream ss;
      ss << errorMessageDgetrsInfo;
      ss << info << endl;
      throw MolDSException(ss.str());
   }
   return info;
}

// Argument "matrix" is sizeM*sizeN matrix.
// Argument "matrix" will be LU-decomposed.
molds_lapack_int Lapack::Dgetrf(double** matrix, molds_lapack_int sizeM, molds_lapack_int sizeN) const{
   molds_lapack_int* ipiv = (molds_lapack_int*)MOLDS_LAPACK_malloc( sizeof(molds_lapack_int)*2*sizeM,        16 );
   this->Dgetrf(matrix, ipiv, sizeM, sizeN);
   MOLDS_LAPACK_free(ipiv);
   molds_lapack_int info = 0;
   return info;
}

// Argument "matrix" is sizeM*sizeN matrix.
// Argument "matrix" will be LU-decomposed.
molds_lapack_int Lapack::Dgetrf(double** matrix, molds_lapack_int* ipiv, molds_lapack_int sizeM, molds_lapack_int sizeN) const{
   double* convertedMatrix = (double*)MOLDS_LAPACK_malloc( sizeof(double)*sizeM*sizeN, 16 );
   for(molds_lapack_int i=0; i<sizeM; i++){
      for(molds_lapack_int j=0; j<sizeN; j++){
         convertedMatrix[i+j*sizeM] = matrix[i][j];
      }
   }
   this->Dgetrf(convertedMatrix, ipiv, sizeM, sizeN);
   for(molds_lapack_int i=0; i<sizeM; i++){
      for(molds_lapack_int j=0; j<sizeN; j++){
         matrix[i][j] = convertedMatrix[i+j*sizeM];
      }
   }
   MOLDS_LAPACK_free(convertedMatrix);
   molds_lapack_int info = 0;
   return info;
}

// Argument "matrix" is sizeM*sizeN matrix.
// The each element of "matrix" should be stored in 1-dimensional vecotre with column major (Fortran type).
molds_lapack_int Lapack::Dgetrf(double* matrix, molds_lapack_int* ipiv, molds_lapack_int sizeM, molds_lapack_int sizeN) const{
   molds_lapack_int info = 0;
   molds_lapack_int lda = sizeM;
#ifdef __INTEL_COMPILER
   dgetrf(&sizeM, &sizeN, matrix, &lda, ipiv, &info);
#else
   info = LAPACKE_dgetrf_work(LAPACK_COL_MAJOR, sizeM, sizeN, matrix, lda, ipiv);
#endif
   if(info != 0){
      stringstream ss;
      ss << errorMessageDgetrfInfo;
      ss << info << endl;
      MolDSException ex(ss.str());
      ex.SetKeyValue<int>(LapackInfo, info);
      throw ex;
   }
   return info;
}
}
