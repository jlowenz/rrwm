MATLAB demo code of Reweighted Random Walks Graph Matching of ECCV 2010

Minsu Cho, Jungmin Lee, and Kyoung Mu Lee, 
Reweighted Random Walks for Graph Matching, 
Proc. European Conference on Computer Vision (ECCV), 2010
http://cv.snu.ac.kr/research/~RRWM/

Please cite our work if you find this code useful in your research. 

written by Minsu Cho & Jungmin Lee, 2010, Seoul National University, Korea
http://cv.snu.ac.kr/~minsucho/
http://cv.snu.ac.kr/~jungminlee/


Date: 31/05/2013
Version: 1.22

* Update report
  This code is updated to be easily applied to any affinity matrix of candidate matches. 
  For image matching demo, combine this with some additional codes and data provided in our site.
  
  - 31/05/2013: A small bug in eliminating conflicting elements is fixed. Thanks to Liang XIAO.
  - 28/12/2011: A bug in the density experiment is fixed.
==================================================================================================


1. Overview

do_GraphMatchingTest.m   : main script for graph matching demo
do_PointMatchingTest.m   : main script for point matching demo

Current test parameters are set to deformation-varing experiments.
For other tests, modify 'setGraphMatching.m' or 'setPointMatching.m' in the folder 'utils'.
Refer to our paper for the settings in our experiments. 
Performance of each algorithm is represented in terms of accuracy, score,and time.
   Accuracy: the ratio between # of true positive matches and # of groundtruth matches.
   Score: the sum of all affinity values related to the matching result.               


compile.m             : script for c-mex compile

If c-mex functions are incompatible, re-comple c-mex functions by running 'compile.m'

setMethods.m        : script for settings of algorithms being tested
                      the list of methods and their options for visualization

RRWM.m              : Matlab function of Reweighted Random Walk Graph Matching 

If you want to add your own algorithm for comparison, three steps are required:
1. Create 'YOUR_ALGORITHM_NAME' folder in 'Methods' folder. Then put your code in it.
2. Add the folder in the script 'setPath.m' so that your method can be called.
3. Modify 'setMethods.m' for your method. Note that you should follow the 'methods' structure. 

If you find this code useful in your research, please cite our paper
Minsu Cho, Jungmin Lee, and Kyoung Mu Lee, Reweighted Random Walks for Graph Matching, 
Proc. European Conference on Computer Vision (ECCV), 2010


2. References

This code includes our implementation of spectral matching for comparison:
M. Leordeanu and M. Hebert. "A Spectral Technique for Correspondence Problems Using Pairwise Constraints", ICCV 2005. 

We utilized some functions of the following public implementations;

bistocastic normalization functions of Timothee Cour's: 
http://www.seas.upenn.edu/~timothee/software/graph_matching/graph_matching.html

Hungarian algorithm of Markus Buehren's for final discretization (optional):
http://www.markusbuehren.de/
