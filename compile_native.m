% Compile some functions into a native C/C++ library for use in other
% programs
mkdir('outlib')
mcc -W lib:librrwm -T link:lib -d outlib cRRWM.m make_default_groups.m