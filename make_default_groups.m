function [ group1, group2 ] = make_default_groups( g1_count, g2_count )
%make_groups: make group arguments
E12 = ones(g1_count,g2_count);
[L12(:,1), L12(:,2)] = find(E12);
[group1, group2] = make_group12(L12);
end

