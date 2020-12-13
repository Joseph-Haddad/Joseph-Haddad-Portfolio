% ------------------------------------------------- %
% ---------------- Input Variables ---------------- %
% ------------------------------------------------- %

% Assumptions
% - Mass of motor is 0.5kgs
% - Load Torque is arounf 0.2N



disp('This program assists with the designing of your DC motor.')
disp('Please Read the prompt below.')

coils = 8;
z = 640;     % Number of Conductors
K1 = 10.66666667;    %ZP/60a            kprime
K2 = 101.8591636;    %ZP/2*pi*a
phi = 0.11127/1000;
mass_rotor = 0.5; %kg
radius_rotor = 0.02;    %m
Ra = 0.54400512;
input_Vt = input('Enter a value for Voltage in (V): '); %V
Vt = str2double(sprintf('%.2f',input_Vt)); % sprintf is used to round numbers
Ia = 5; %A - armature current limit
simulation_time = 5; % seconds
time_step = 0.1;
t = 0:time_step:simulation_time;

% if this is zero it is no load conditions
% else a loaded state is approximately
% 0.2.
torque_load = 0.02; %Nm

J = (mass_rotor*(radius_rotor^2))/2;
torque_setep = 0.01;
%torque_increase = 0:torque_step:torque_load;

% Magnet Specifications
B2522 = 1400/10000;
A2522 = 10/10000;
Phi2522 = 0.000080;

% Code = 3495
B3495 = 3709/10000;
A3495 = 6/10000;
Phi3495 = 0.000213;

% Code = 3460
B3460 = 2408/10000;
A3460 = 3.125/10000;
Phi3460 = 0.000138;

% Code = 3470
B3470 = 3625/10000;
A3470 = 3.123/10000;
Phi3470 = 0.000208;

% Code = LM1614
BLM1614 = 580/1000;
ALM1614 = 3/1000;
PhiLM1614 = 0.0174/1000;

% ------------------------------------------------- %
% ----------- Things this section finds ----------- %
% ------------------------------------------------- %

% Speed - nm(t)
Component_a = Ra / (K1 * K2 * (phi^2) );
Component_b = ((K2 * phi * Vt) / Ra) - torque_load;
Component_c = 1 - exp(((-60 * K1 * K2 * (phi^2) ) / (2 * pi * Ra * J)) * t);

nmRPM = Component_a * Component_b * Component_c;
subplot(2,3,1), plot(t,nmRPM,'LineWidth',2.5);
ylabel("Speed -(RPM)");
xlabel("Time - (s)");
title("Speed vs Time");

% Back EMF

Ea = K1 * phi * nmRPM;
subplot(2,3,2), plot(t,Ea,'LineWidth',2.5);
ylabel("Back EMF - (V)");
xlabel("Time - (s)");
title("Back EMF vs Time");

% Current

Iat = (Vt - K1 * phi * nmRPM);
subplot(2,3,3), plot(t,Iat,'LineWidth',2.5);
ylabel("Current - (A)");
xlabel("Time - (s)");
title("Current vs Time");

% Torque (ADD COLOUR CODING and LEGEND)

Tind = K2 * phi * Iat;
subplot(2,3,4);

TIat2522 = K2 * (Phi2522/2) * Iat;
mag_a = plot(t,TIat2522, 'r','LineWidth',2.5);
hold on;
TIat3495 = K2 * (Phi3495/2) * Iat;
mag_b = plot(t,TIat3495, 'g','LineWidth',2.5);
hold on;
TIat3460 = K2 * (Phi3460/2) * Iat;
mag_c = plot(t,TIat3460,'y','LineWidth',2.5);
hold on;
TIat3470 = K2 * (Phi3470/2) * Iat;
mag_d = plot(t,TIat3470,'b','LineWidth',2.5);
hold on;
TIatLM1614 = K2 * (PhiLM1614/2) * Iat;
mag_e = plot(t,TIatLM1614,'m','LineWidth',2.5);

ylabel("Torque - (N*m)");
xlabel("Time - (s)");
title("Torque vs Time");
legend([mag_a,mag_b,mag_c,mag_d,mag_e],'#2522','#3495','#3460','#3470','#LM1614')

% Acceleration

wt = ((K2 * phi * Iat ) - torque_load)/J;
subplot(2,3,5), plot(t,wt,'LineWidth',2.5);
ylabel("Angular Acceleration - (m/s^2)");
xlabel("Time - (s)");
title("Angular Acceleration vs Time");

% Speed with increasing Torque

subplot(2,3,6), plot(Tind,nmRPM,'LineWidth',2.5);
ylabel("Speed - (RPM)");
xlabel("Torque - (N*m)");
title("Speed vs Increasing Torque");
