#include <rrwm/crrwm.hpp>
#include <armadillo>
#include <limits>
#include <iostream>

using namespace arma;

// Handle missing armadillo functions in v6.5 (vs latest 8.1)
// slow Ubuntu - update this stuff!
template<class T>
T regspace(int start, int end)
{
  return linspace<T>(start, end-1, end - start);
}


mat trimatl(const mat& A, int k)
{
  mat t = trimatl(A);
  if (k < 0) {
    while (++k <= 0) {
      t.diag(k).eval() = 0.0;
    }
  } else {
    while (--k >= 0) {
      t.diag(k).eval() = 0.0;
    }
  }
  return t;
}

int
main(int argc, char** argv)
{
  int n_in = 40; // number of inliers
  int n_out_p = 0; // number of outliers in model
  int n_out_q = 0; // numebr of outliers in query
  double ratio_fill = 1.0;
  double deformation = 0.0;
  double scale_2D = 0.15;

  rrwm::init();

  srand(time(NULL));  

  // create a model graph
  // create a derived, noisy, query graph

  int nG1 = n_in + n_out_p;
  int nG2 = n_in + n_out_q;
  uvec seq = shuffle(regspace<uvec>(0,nG2));
  std::cout << "Seq: " << seq << std::endl;
  
  mat G1 = trimatl(randu(nG1,nG1), -1);
  G1 = G1 + G1.t();
  mat P = trimatl(randu(nG1,nG1), -1);
  P = P + P.t();
  uvec selected = find(P > ratio_fill);
  G1.elem(selected).eval() = std::numeric_limits<double>::quiet_NaN();

  mat N = deformation * trimatl(randn(nG2,nG2),-1);
  N = N + N.t();

  mat G2 = trimatl(randu(nG2,nG2), -1);
  G2 = G2 + G2.t();
  P = trimatl(randu(nG2,nG2),-1);
  P = P + P.t();
  selected = find(P > ratio_fill);
  G2.elem(selected).eval() = std::numeric_limits<double>::quiet_NaN();
  auto rc = seq(span(0,n_in-1));
  std::cout << rc << std::endl;
  G2.submat(rc, rc) = G1(span(0,n_in-1),span(0,n_in-1));
  G2 += N;

  // M = (repmat(G1, nP2, nP2)-kron(G2,ones(nP1))).^2;
  // M = exp(-M./scale_2D);
  // M(isnan(M)) = 0;
  // M(1:(length(M)+1):end)=0;
  // M = M+M';

  mat M = pow(repmat(G1,nG2,nG2) - kron(G2,ones(nG1,nG1)), 2);
  M = exp(-M / scale_2D);
  M.elem(find_nonfinite(M)).zeros();
  M.diag().eval() = 0.0;
  M = M + M.t();
  
  // %% Ground Truth
  // GT.seq = seq(1:nInlier);
  // GT.matrix = zeros(nP1, nP2);
  // for i = 1:nInlier, GT.matrix(i,seq(i)) = 1; end
  // GT.bool = GT.matrix(:);
  mat GT = zeros(nG1, nG2);
  for (int i = 0; i < n_in; ++i) {
    GT(i,seq(i)) = 1;
  }
  
  sp_umat group1, group2; // logicals
  rrwm::make_groups(group1, group2, nG1, nG2);
  rrwm::results_t results;
  rrwm::rrwm(results, M, group1, group2);

  //std::cout << "Results: " << results.X << std::endl;
  double acc;
  acc = dot(results.X, vectorise(GT)) / accu(GT);
  std::cout << "Accuracy: " << acc << std::endl;

  rrwm::shutdown();
  
  return 0;
}
