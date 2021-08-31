clc;
close all;
clear all;

% Input Data
Amp = 600;
Vref = 400;
Vdc = 700;   % DC-Link Voltage
f = 50; 
fs = 4e3;
a = 0.6;     %modulation index
Ts = (1/fs); %Time period is equivalen to total voltage
w = 2*pi*f;
