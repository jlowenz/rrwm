#include <atomic>
#include <armadillo>

#include <rrwm/crrwm.hpp>
#include <rrwm/librrwm.h>


namespace rrwm {

  // globals for the MATLAB initialization
  static std::atomic_bool g_initialized(false);
  static std::atomic_uint g_refcount(0);


  /**
   * Initialize MATLAB if needed. Called for every function or class.
   */
  void init_rrwm()
  {
    if (!g_initialized.load()) {
      std::cout << "Initializing MATLAB/RRWM (should run once)" << std::endl;
      // F^&* you, MATLAB. I hate you. Just so you know.
      const char* mat_args[] = {"-nojvm","-nodisplay"};
      if (!mclInitializeApplication(mat_args, 2)) {
        fprintf(stderr, 
                "An error occurred while initializing: \n %s\n",
                mclGetLastErrorMessage());
        throw std::exception();
      }    
      librrwmInitialize();    
      g_initialized.store(true);
    }
    g_refcount.fetch_add(1);  
  }

  void shutdown_rrwm()
  {
    // do something
  }

  // Some useful classes for converting between MATLAB and C++
  struct scalar_t
  {
    mxArray* mv_;
    double* v_;
  
    scalar_t() : mv_(mxCreateDoubleScalar(0.0)), v_(mxGetPr(mv_)) {}
    scalar_t(double d) : mv_(mxCreateDoubleScalar(d)), v_(mxGetPr(mv_)) {}

    virtual ~scalar_t() { mxDestroyArray(mv_); v_ = NULL; }

    scalar_t& operator=(const scalar_t& s)
    {
      *v_ = *s.v_;
      return *this;
    }
    scalar_t& operator=(double d)
    {
      *v_ = d;
      return *this;
    }
    scalar_t& operator=(const mxArray* m)
    {
      double* mv = mxGetPr(m);
      *v_ = *mv;
    }

    operator double() {
      return *v_;
    }

    operator mxArray*() {
      return mv_;
    }
  };

  struct array2d_t
  {
    size_t rows, cols;
    mxArray* mA_;
    double* buf_;
    mat* A_;

    array2d_t() : rows(0), cols(0), mA_(NULL), buf_(NULL), A_(NULL) {}
  
    array2d_t(int rows_, int cols_)
    {
      construct(rows_, cols_);
    }
    array2d_t(mxArray* M)
    {
      mwSize ndim = mxGetNumberOfDimensions(M); assert(ndim == 2); // this is a 2d array    
      const mwSize* sz = mxGetDimensions(M);
      construct(sz[0], sz[1], M);
    }
    array2d_t(const mat& X)
    {
      construct(X.n_rows, X.n_cols);
      *A_ = X; // should make a copy of X into A_
    }

    virtual ~array2d_t()
    {
      destroy();
    }
  
    array2d_t& operator=(const mat& X)
    {
      // destroys existing array if needed, creates a new one, then
      // copies in the new data
      if (X.n_rows != rows || X.n_cols != cols) {
        destroy();
        construct(X.n_rows, X.n_cols);
      } 
      *A_ = X;
      return *this;
    }

    array2d_t& operator=(const mxArray* M)
    {
      const mwSize* sz = mxGetDimensions(M);
      if (sz[0] != rows || sz[1] != cols) {
        destroy();
        construct(sz[0], sz[1], M);
      } else {
        // copy the contents, without creating new arrays
        double* a = mxGetPr(mA_);
        double* m = mxGetPr(M);
        memcpy(a, m, sizeof(double)*mxGetNumberOfElements(M));
      }
      return *this;
    }

    operator mxArray*() { return mA_; }
    operator mat() { return *A_; }
  
  private:
    void construct(int r, int c, const mxArray* M = NULL) {
      rows = r;
      cols = c;
      mwSize sz[] = {rows, cols};
      if (M) {
        mA_ = mxDuplicateArray(M);
      } else {
        mA_ = mxCreateNumericArray(2, sz, mxDOUBLE_CLASS, mxREAL);
      }
      buf_ = mxGetPr(mA_);
      A_ = new arma::mat(buf_, rows, cols, false, true);
    }
    void destroy()
    {
      if (mA_) {
        mxDestroyArray(mA_);
        delete A_;
      }
    }
  };


  bool
  rrwm(results_t& results, const mat& A, const mat& group1, const mat& group2,
       const params_t& params)
  {
    array2d_t X;
    scalar_t time;
    scalar_t score;

    array2d_t m_group1(group1);
    array2d_t m_group2(group2);
    array2d_t m_A(A);

    scalar_t prob_c(params.prob_c);
    scalar_t amp_max(params.amp_max);
    scalar_t iter_max(params.iter_max);
    scalar_t conv_threshold(params.conv_threshold);
    scalar_t tolC(params.tolC);

    mxArray* mX, *mTime, *mScore;
    bool res = mlfCRRWM(3, &mX, &mScore, &mTime, m_A, m_group1, m_group2,
                        prob_c, amp_max, iter_max, conv_threshold, tolC);

    X = mX;
    time = mTime;
    score = mScore;
    results.X = X;
    results.score = score;
    results.time = time;
    return res;
  }

  bool
  make_groups(mat& group1, mat& group2, int count1, int count2)
  {
    mxArray* m_group1;
    mxArray* m_group2;
    scalar_t c1(count1), c2(count2);
    mlfMake_groups(2, &m_group1, &m_group2, c1, c2);
    array2d_t g1(m_group1), g2(m_group2);
    group1 = g1;
    group2 = g2;
  }

}
