/**
 *  File name: CEigen.h
 *
 *  Programmer: Yongqun He 
 *  Contact email: yohe@vt.edu
 *  Purpose: This is the .h file for the class CEigen. 
 *           It is to calculate eigenvalues and eigenvectors of a matrix.
 *           It mainly uses the dgees_() subroutine of CLAPACK
 *
 */

#ifndef COPASI_CEigen
#define COPASI_CEigen

#include "tnt/tnt.h"
#include "tnt/cmat.h"
#include "copasi.h"

//include clapack.h for eigenvalue calculations0
extern "C" {
  //#include "CLAPACK/clapack.h"       //use CLAPACK

  //use intel-lapack for faster reactions
  #include "/usr/local/intel-lapack/include/lapack-wrap.h"
  #include "/usr/local/intel-lapack/include/mkl.h"
}


/*
extern "C" int dgees_(char *jobvs, char *sort, L_fp select, long int *n,
        double *a, long int *lda, long int *sdim, double *wr,
        double *wi, double *vs, long int *ldvs, double *work,
        long int *lwork, long int *bwork, long int *info);
*/


class CEigen {

 private:
  /**
   * A Matrix, 
   * If input is a TNT matrix, a transformation of it to arrays will 
   * be performed
   */
  TNT::Matrix <C_FLOAT64> mMatrix;


  //there are 15 parameters in the dgees() subroutine

  /**
   * #1: (input) characer*1
   * = 'N': Schur vectors are not computed
   * = 'V': Schur vectors are computed
   */
  char mJobvs;

 /**
   * #2: (input) characer*1
   * = 'N': Eigenvalues are not ordered  
   * = 'S': Eigenvalues are ordered 
   */
  char mSort;

  /**
   * #3: (input) Logical function of two double precision arguments
   * It must be declared external
   * = 'S': Select is used to select eigenvalues to sort to the top left
   * of the Schur form 
   * = 'N': Eigenvalues are ordered Select is not refereced.
   */
  char mSelect;

  /**
   * #4: (input) The order of the matrix A 
   */
  C_INT32 mN;

 
  /**
   * #5: (input/output) The double precision array, dimension (LDA,N)
   * On entry, the N-by-N matrix A
   * On exit, A has been overwritten by its real Schur form T
   * Use a pointer variable here instead of array since we don't know
   * the dimension yet.
   */
  C_FLOAT64 * mA;


  /**
   * #6: (input) The leading dimension of the array A. LDA >= max(1,N)
   */
  C_INT32 mLDA;

  /**
   * #7: (output) an integer
   * if Sort = 'N', its value is 0
   * if Sort = 'S', its value = number of eigenvalues (after sorting)
   *                for which mSelect is true.
   */
  C_INT32 mSdim;

  /**
   * #8: array with dimension (mN)
   */
  C_FLOAT64 * mWR;

  /**
   * #9: array with dimension (mN)
   */
  C_FLOAT64 * mWI;

  /**
   * #10: (output) array with dimension (mLdvs, mN)
   * If mJobvs='V', mVS contains the orthogoanl matrix Z of Schur vectors
   * If mJobvs='N', mVS is not referenced
   */
  C_FLOAT64 * mVS;

  /**
   * #11: an integer, the leading dimension of the array VS. mLdvs >= 1;
   * if mJobvs='V', mLdvs >= N.
   */
  C_INT32 mLdvs;

  /**
   * #12: (workspace/output) double precision array, dimension (mLWork)
   * On exit, if mInfo=0; mWork(1) contains the optimal mLWork
   */
  C_FLOAT64 * mWork;

  /**
   * #13: (input) Dimension of array Work, its value >= max(1,3*mN).
   * For good performance, it must generally be larger
   */
  C_INT32 mLWork;

  /**
   * #14: (workspace) Logical array, dimension (N)
   * Not referenced if mSort = 'N'
   * ???????????????????????????????????????????
   */
  C_INT32 * mBWork;


  /**
   * #15: (output) an integer
   * =0: successful exit
   * <0: if mInfo=-i, the ith argument had an illegal value
   * >0: if mInfo=i, and i is
   *     <=N: the QR algorithm failed to compute all the eigenvalues;
   *     =N+1: the eigenvalues could not be reordered because some 
   *           eigenvalues were too close to separate (ill-conditioned)
   *     =N+2: after reordering, roundoff changed values of some 
   *           complex eigenvalues so that leading eigenvalues in the
   *           Schur form no longer satisfy mSelect=.True. This could
   *           caused by underflow due to scaling
   */
  C_INT32 mInfo;


 public:
  /**
   * Defaulut constructor
   */ 
  CEigen();

  /**
   * User defined constructor
   * @param rows is the max row number of the Matrix
   * @param cols is the max column number of the Matrix
   */
  CEigen(int rows, int cols);


  /**
   * Destructor
   */

  ~CEigen();

 
  /**
   * return the matrix
   */
  TNT::Matrix < C_FLOAT64 > getMatrix();

  /**
   * Set the Matrix
   */
  void setMatrix(int rows, int cols);


  /**
   * #12: Set the Work
   */
  void setWork(C_FLOAT64 * aWork);

  /**
   * #12: Get the Work
   */
  C_FLOAT64 * getWork() const;


  /**
   * Eigenvalue calculations
   */
  void CalcEigenvalues();


  //there are 15 parameters in the dgees() subroutine

  /**
   * #1: (input) characer*1
   */
  //char mJobvs;

 /**
   * #2: (input) characer*1
   */
  //char mSort;

  /**
   * #3: (input) Logical function of two double precision arguments
   */
  //char mSelect;

  /**
   * #4: (input) The order of the matrix A 
   */
  //C_INT32 mN;

 
  /**
   * #5: (input/output) The double precision array, dimension (LDA,N)
   */
  //C_FLOAT64 * mA;


  /**
   * #6: (input) The leading dimension of the array A. LDA >= max(1,N)
   */
  //C_INT32 mLDA;

  /**
   * #7: (output) an integer
   */
  //C_INT32 mSdim;

  /**
   * #8: array with dimension (mN)
   */
  //C_FLOAT64 * mWR;

  /**
   * #9: array with dimension (mN)
   */
  //C_FLOAT64 * mWI;

  /**
   * #10: (output) array with dimension (mLdvs, mN)
   */
  //C_FLOAT64 * mVS;

  /**
   * #11: an integer, the leading dimension of the array VS. mLdvs >= 1;
   */
  //C_INT32 mLdvs;

  /**
   * #12: (workspace/output) double precision array, dimension (mLWork)
   */
  //C_FLOAT64 * mWork;

  /**
   * #13: (input) Dimension of array Work, its value >= max(1,3*mN).
   */
  //C_INT32 mLWork;

  /**
   * #14: (workspace) Logical array, dimension (N)
   */
  //C_INT32 * mBWork;


  /**
   * #15: (output) an integer
   */
  //C_INT32 mInfo;


  /*

// variables for stability analysis
 double                 *eigen_r;
 double                 *eigen_i;
 double                 *eigen_jacob;
 double                 eigen_maxrealpart;
 double                 eigen_maximagpart;
 double                 eigen_nposreal;
 double                 eigen_nnegreal;
 double                 eigen_nreal;
 double                 eigen_nimag;
 double                 eigen_ncplxconj;
 double                 eigen_nzero;
 double                 eigen_stiffness;
 double                 eigen_hierarchy;

  */


};

#endif









