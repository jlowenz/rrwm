%% Makes current problem into Graph Matching form
function [accuracy score time X Xraw] = wrapper_GM(method, cdata)
% Make function evaluation script
str = ['feval(@' func2str(method.fhandle)];
for j = 1:length(method.variable), str = [str ',cdata.' method.variable{j} ]; end
if ~isempty(method.param), for i = 1:length(method.param), str = [str, ',method.param{' num2str(i) '}']; end; end
str = [str, ')']; 
% Function evaluation & Excution time Check
tic; Xraw = eval(str); time = toc;
% Discretization by one-to-one mapping constraints
if 1
    % Hungarian assignment
    X = zeros(size(cdata.E12)); X(find(cdata.E12)) = Xraw;
    X = discretisationMatching_hungarian(X,cdata.E12); X = X(find(cdata.E12));
else % Greedy assignment
    X = greedyMapping(Xraw, cdata.group1, cdata.group2);
end
% Matching Score
score = X'*cdata.affinityMatrix*X; % objective score function
if length(cdata.GTbool) ~= length(cdata.affinityMatrix)
    accuracy = NaN; % Exception for no GT information
else
    accuracy = (X(:)'*cdata.GTbool(:))/sum(cdata.GTbool);
end