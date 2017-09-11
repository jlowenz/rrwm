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
  void init()
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

  void shutdown()
  {
    // do something
    g_refcount.fetch_add(-1);
    if (g_refcount.load() <= 0) {
      librrwmTerminate();
      g_initialized.store(false);
    }
  }

  // Some useful classes for converting between MATLAB and C++
  struct scalar_t
  {
    mxArray* mv_;
    double* v_;
  
    scalar_t() : mv_(mxCreateDoubleScalar(0.0)), v_(mxGetPr(mv_))
    {
      assert(mv_);
      std::cout << "created scalar: " << mv_ << " ~ " << *v_ << std::endl;
    }
    scalar_t(double d) : mv_(mxCreateDoubleScalar(d)), v_(mxGetPr(mv_))
    {
      assert(mv_);
      std::cout << "created scalar: " << mv_ << " ~ " << *v_ << std::endl;
    }

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

  template<typename T>
  struct sparse_traits
  {
  };

  template<typename T>
  struct is_logical {
    typedef T type;
    static const bool value = false;
    template<typename A>
    static T convert(const A& t) { return t; }
  };

  template<>
  struct is_logical<mxLogical> {
    typedef mxLogical type;
    static const bool value = true;
    template<typename A>
    static type convert(const A& t) { return static_cast<bool>(t); }
  };
  
#define SPECIALIZE_SPARSE(T,A) template<>                               \
  struct sparse_traits<T> {                                             \
  typedef T value_type;                                                 \
  typedef A arma_value_type;                                            \
  typedef SpMat<A> arma_type;                                           \
  static arma_type to_arma(mxArray* M) {                                \
    assert(mxIsSparse(M));                                              \
    const mwSize* sz = mxGetDimensions(M);                              \
    mwSize nzmax = mxGetNzmax(M);                                       \
    mwIndex* jc = mxGetJc(M);                                           \
    mwIndex* ir = mxGetIr(M);                                           \
    value_type* data;                                                   \
    data = (value_type*)mxGetData(M);                                   \
    umat locs(2,nzmax);                                                 \
    Col<arma_value_type> vals(nzmax);                                   \
    int k = 0;                                                          \
    for (int j = 0; j < sz[1]; ++j) {                                   \
      int count = jc[j+1] - jc[j];                                      \
      for (int i = 0; i < count; ++i) {                                 \
        locs(0,k) = ir[jc[j]+i];                                        \
        locs(1,k) = j;                                                  \
        vals(k++) = data[jc[j]+i];                                      \
      }                                                                 \
    }                                                                   \
    locs.resize(2,k);                                                   \
    vals.resize(k);                                                     \
    return arma_type(locs, vals, sz[0], sz[1]);                         \
  }                                                                     \
  static mxArray* to_matlab(const arma_type& m)                         \
  {                                                                     \
  mxArray* out;                                                         \
  int nzmax = m.n_rows * m.n_cols * 0.3;                                \
  auto start = m.begin();                                               \
  auto end = m.end();                                                   \
  int n = std::distance(start, end);                                    \
  nzmax = std::max(nzmax, n);                                           \
  if (is_logical<value_type>::value) {                                  \
  out = mxCreateSparseLogicalMatrix(m.n_rows, m.n_cols, nzmax);         \
} else {                                                                \
  out = mxCreateSparse(m.n_rows, m.n_cols, nzmax, mxREAL);              \
 }                                                                      \
 mwIndex* jc = mxGetJc(out);                                            \
 mwIndex* ir = mxGetIr(out);                                            \
 value_type* data = (value_type*)mxGetData(out);                        \
 auto it = start;                                                       \
 int k = 0;                                                             \
 for (int j = 0; j < m.n_cols; ++j) {                                   \
   jc[j] = k;                                                           \
   while (it != end && it.col() == j) {                                 \
     value_type v = is_logical<value_type>::convert(*it);               \
     data[k] = v;                                                       \
     ir[k] = it.row();                                                  \
     k++;                                                               \
     it++;                                                              \
   }                                                                    \
 }                                                                      \
 jc[m.n_cols] = k;                                                      \
 return out;                                                            \
 }                                                                      \
};                                                                      \
  
  SPECIALIZE_SPARSE(mxLogical, sp_umat::elem_type)
  SPECIALIZE_SPARSE(double, sp_mat::elem_type)

  template<typename T>
  typename sparse_traits<T>::arma_type to_arma(mxArray* M)
  {
    return sparse_traits<T>::to_arma(M);
  }

  template<typename T>
  mxArray* to_matlab(const typename sparse_traits<T>::arma_type& m)
  {
    return sparse_traits<T>::to_matlab(m);
  }

  // hmmm, can we do something different?
  
  bool
  rrwm(results_t& results, const mat& A, const sp_umat& group1, const sp_umat& group2,
       const params_t& params)
  {
    array2d_t X;
    scalar_t time;
    scalar_t score;

    //array2d_t m_group1(group1);
    //array2d_t m_group2(group2);
    mxArray* m_group1 = sparse_traits<mxLogical>::to_matlab(group1);
    mxArray* m_group2 = sparse_traits<mxLogical>::to_matlab(group2);
    array2d_t m_A(A);

    scalar_t prob_c(params.prob_c);
    scalar_t amp_max(params.amp_max);
    scalar_t iter_max(params.iter_max);
    scalar_t conv_threshold(params.conv_threshold);
    scalar_t tolC(params.tolC);

    mxArray* mX=NULL, *mTime=NULL, *mScore=NULL;
    // mwSize sz[] = {2,2};
    // mX = mxCreateNumericArray(2, sz, mxDOUBLE_CLASS, mxREAL);
    // mTime = mxCreateDoubleScalar(0.0);
    // mScore = mxCreateDoubleScalar(0.0);
    // I keep forgetting, they need to be valid arrays of the correct
    // type. HOW DOES THAT MAKE ANY SENSE?
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
  make_groups(sp_umat& group1, sp_umat& group2, int count1, int count2)
  {
    std::cout << "make_groups " << count1 << " " << count2 << std::endl;
    mxArray* m_group1 = mxCreateSparseLogicalMatrix(1,1,1);
    mxArray* m_group2 = mxCreateSparseLogicalMatrix(1,1,1);
    scalar_t c1(count1), c2(count2);
    mlfMake_default_groups(2, &m_group1, &m_group2, c1, c2);

    size_t n = mxGetNumberOfDimensions(m_group1);
    const mwSize* sz = mxGetDimensions(m_group1);
    std::cout << "m_group1 " << n << ", ";
    for (size_t i = 0; i < n; ++i) {
      std::cout << sz[i] << " ";
    }
    std::cout << std::endl;

    group1 = to_arma<mxLogical>(m_group1);
    group2 = to_arma<mxLogical>(m_group2);
  }

}
