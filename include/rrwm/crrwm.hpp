#pragma once

#include <rrwm/librrwm.h>

#include <armadillo>

// what matrix library?
// maybe armadillo?
using namespace arma;

namespace rrwm {

  struct results_t
  {
    mat X; // results of stable random walk, after discretizing with Hunagarian algorithm
    mat score; // scores for each match
    double time; // time for the algorithm run
  };

  // %   prob_c: 0.2
  // %   amp_max: 30
  // %   iter_max: 300
  // %   conv_threshold: 1e-25
  // %   tolC: 1e-3
  
  struct params_t
  {
    double prob_c;
    double amp_max;
    int iter_max;
    double conv_threshold;
    double tolC;

    // construct with default values
    params_t() : prob_c(0.2), amp_max(30), iter_max(300), conv_threshold(1e-25), tolC(1e-3) {}
  };

  void init_rrwm();
  void shutdown_rrwm();
  
  /**
   * Run the reweighted random walk graph matching algorithm.
   *
   * Requires the following params:
   * - results : the results structure for a successful run
   * - A : NxN matrix where N = (n*m), represents the affinities between the two graph edges
   * - group1/group2 : the groups returned from the make_groups function, otherwise representing the constraints for the random walk
   *
   * returns: true if the algorithm completed successfully, false otherwise
   */
  bool
  rrwm(results_t& results, const mat& A, const mat& group1, const mat& group2, const params_t& params = params_t());

  /**
   * Compute a default pair of groups using the edge size of the 
   */
  bool
  make_groups(mat& group1, mat& group2, int count1, int count2);

}
