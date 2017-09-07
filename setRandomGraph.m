%% Configuration Settings
nSet=0; %% Display (bool) / Display Name / Variable Name / Variable Value, Vector, String
nSet=nSet+1; settings{nSet} = {0, '# of tests', 'nTest', 30};
switch 1
    case 1 % Deformation Test
        nSet=nSet+1; settings{nSet} = {1, '# of inliers \itn_{\rmin}\rm', 'nInlier', 20};
        nSet=nSet+1; settings{nSet} = {1, '# of outliers \itn_{\rmout}\rm', 'nOutlier', 0};
        nSet=nSet+1; settings{nSet} = {1, 'Deformation noise \it\sigma\rm', 'deformation', 0:0.05:0.4};
        nSet=nSet+1; settings{nSet} = {1, 'Edge density \it\rho\rm', 'ratioFill', 1};
    case 2 % Outlier Test
        nSet=nSet+1; settings{nSet} = {1, '# of inliers \itn_{\rmin}\rm', 'nInlier', 20};
        nSet=nSet+1; settings{nSet} = {1, '# of outliers \itn_{\rmout}\rm', 'nOutlier', 0:2:20};
        nSet=nSet+1; settings{nSet} = {1, 'Deformation noise \it\sigma\rm', 'deformation', 0};
        nSet=nSet+1; settings{nSet} = {1, 'Edge density \it\rho\rm', 'ratioFill', 1};
    case 3 % Edge Density
        nSet=nSet+1; settings{nSet} = {1, '# of inliers \itn_{\rmin}\rm', 'nInlier', 20};
        nSet=nSet+1; settings{nSet} = {1, '# of outliers \itn_{\rmout}\rm', 'nOutlier', 0};
        nSet=nSet+1; settings{nSet} = {1, 'Deformation noise \it\sigma\rm', 'deformation', 0.15};
        nSet=nSet+1; settings{nSet} = {1, 'Edge density \it\rho\rm', 'ratioFill', 0.1:0.1:1};
    otherwise
end

nSet=nSet+1; settings{nSet} = {0, 'Scale for 2nd order', 'scale_2D', 0.15}; % Used in 2D
nSet=nSet+1; settings{nSet} = {0, 'Permute Nodes', 'bPermute', 1}; % boolean
nSet=nSet+1; settings{nSet} = {0, 'Outliers Both Side', 'bOutBoth', 1}; % boolean

%% Evaluate Settings
nFix = 0; nCon = 0;
for n = 1:nSet
    if isscalar(settings{n}{4})
        eval(['Set.' settings{n}{3} '=' num2str(settings{n}{4}) ';']);    
        if settings{n}{1}, nFix=nFix+1; Fix(nFix) = n; end
    else
        eval(['Set.' settings{n}{3} '= settings{n}{4};']);    
        if settings{n}{1}, nCon=nCon+1; Con(nCon) = n; end
    end
end
clear n nFix nCon nSet

disp('* Check experiment settings *'); disp(Set);