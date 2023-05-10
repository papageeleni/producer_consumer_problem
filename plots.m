close all;
clear;
load('data_for_plotting.mat')

figure
plot(data.VarName2(1:7),data.VarName3(1:7),"-o")
grid on
hold on
plot(data.VarName2(8:14),data.VarName3(8:14),"-o")
plot(data.VarName2(15:21),data.VarName3(15:21),"-o")
plot(data.VarName2(22:28),data.VarName3(22:28),"-o")
plot(data.VarName2(29:35),data.VarName3(29:35),"-o")
plot(data.VarName2(36:42),data.VarName3(36:42),"-o")
plot(data.VarName2(43:49),data.VarName3(43:49),"-o")

legend("Prucers:1","Producers:2","Producers:4","Producers:8","Producers:12","Producers:16","Producers:20")
xlabel("consumers #");
ylabel("time(usec)")
title("Time between adding an item to the buffer and deleting it");