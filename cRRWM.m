function [ X, score, time ] = cRRWM( A, g1_count, g2_count, ...
    prob_c, amp_max, iter_max, conv_threshold, tolC)
%CRRWM compute the reweighted random walk matching algorithm on the given
%affinity matrix A
%   prob_c: 0.2
%   amp_max: 30
%   iter_max: 300
%   conv_threshold: 1e-25
%   tolC: 1e-3

% this may not suffice... i.e. we may want to build the groups ourselves
[group1, group2] = make_groups(g1_count, g2_count);

% Function evaluation & Excution time Check
tic; Xraw = RRWM(A, group1, group2, 'c', prob_c, 'amp_max', amp_max, ...
    'iterMax', iter_max, 'thresConvergence', conv_threshold, ...
    'tolC', tolC); time = toc;
% Discretization by one-to-one mapping constraints
E12 = ones(g1_count, g2_count);
% Hungarian assignment
X = zeros(size(E12)); X(find(E12)) = Xraw;
X = discretisationMatching_hungarian(X,cdata.E12); 
X = X(find(E12));
% Matching Score
score = X'*cdata.affinityMatrix*X; % objective score function
end

