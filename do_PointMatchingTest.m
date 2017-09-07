% MATLAB demo code of Reweighted Random Walks Graph Matching of ECCV 2010
% 
% Minsu Cho, Jungmin Lee, and Kyoung Mu Lee, 
% Reweighted Random Walks for Graph Matching, 
% Proc. European Conference on Computer Vision (ECCV), 2010
% http://cv.snu.ac.kr/research/~RRWM/
% Updated at Nov 1st, 2011

clear all; close all; clc;
disp('************************ Point Matching Test ************************');disp(' ');

%% Settings Evaluations
setPath; setPointMatching; setMethods;

%%
plotSet.lineWidth = 3; % Line width
plotSet.markerSize = 10; % Marker Size
plotSet.fontSize = 20; % Font Size
plotSet.font = '\fontname{times new roman}'; % Font default

%% Test Methods
Accuracy = zeros(length(settings{Con}{4}), length(methods), Set.nTest);
MatchScore = zeros(length(settings{Con}{4}), length(methods), Set.nTest);
Time = zeros(length(settings{Con}{4}), length(methods), Set.nTest);

t_start = clock;
fprintf(['Experiment starts: ' num2str(t_start(4)) ':' num2str(t_start(5)) ':' num2str(round(t_start(6))) '\n']);
for kk = 1:Set.nTest, fprintf('Test: %d of %d ', kk, Set.nTest);
    for i = 1:length(settings{Con}{4})
        eval(['Set.' settings{Con}{3} '=' num2str(settings{Con}{4}(i)) ';']);
        problem = makePointMatchingProblem(Set);
        eval(['Set.' settings{Con}{3} '= settings{' num2str(Con) '}{4};']);
        for j = 1:length(methods)
            [Accuracy(i,j,kk) MatchScore(i,j,kk) Time(i,j,kk)] = wrapper_GM(methods(j), problem);
        end
        fprintf('.');
    end
    clf; handleCount = 0; yData = mean(Accuracy(:,:,1:kk),3); yLabelText = 'Accuracy'; plotResults;
    str = ['Average accuracy from test 1 to test ' num2str(kk)]; title(str, 'FontSize', 16); drawnow;
    t_now = clock; elap = etime(t_now, t_start); t_end = add_time(t_start, elap/(kk)*(Set.nTest));
    fprintf(['  expected time to end' num2str(t_end(4)) ':' num2str(t_end(5)) ':' num2str(round(t_end(6))) '\n']);
end
clear i j k temp X Xbin ind p val str
close all

%% Plot Results
meanAccuracy = mean(Accuracy,3);
meanMatchScore = mean(MatchScore,3);
meanTime = mean(Time,3);

%%
handleCount = 0;
yData = meanAccuracy; yLabelText = 'Accuracy'; plotResults;
yData = meanMatchScore; yLabelText = 'Objective score'; plotResults;
yData = meanTime; yLabelText = 'Time'; plotResults;