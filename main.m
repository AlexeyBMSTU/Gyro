clear all
clc
clear all vareables
close all

%% 1. Dоwnload on memory
A = load("output_inert.txt");

%% 2. Parsing data
inert = A(:, 1);
time = A(:, 5);
gyromagnSOK = A(:, 3); 
gyroEquation = A(:,4);
magn = A(:, 2);

%% 3. Build graphycs
%% 3.1 Graphycs 
f1 = figure;

plot(time,inert)
grid on
grid minor
title("Значение курсов ЛА от времени")
xlabel('time, с')
ylabel('\Psi , degree')
hold on
p = polyfit(time,gyroEquation,5);
f = polyval(p,time);
skolzh = smooth(gyroEquation); %скользящее среднее

%plot(time, f, LineWidth=1.25 );
plot(time, gyromagnSOK, LineWidth=1.25 );
plot(time, gyroEquation, LineWidth=1.25 );
plot(time, magn, LineWidth=1.25 );
%ylim([255 275]);
plot(time, skolzh, LineWidth=1.25 );

legend('Инерциальный', 'ГиромагнитныйСОК', 'Гиромагнитный', 'Магнитный', 'Фильтр скользящего среднего')
hold off

%% 3.2 Graphyc error
f2 = figure;

error1 = gyromagnSOK - gyroEquation;
error2 = magn - gyroEquation;

plot(time,error1);
grid on
grid minor
title('Величина отклонения гиромагнитного курса при Т = 900')
xlabel('time, с')
ylabel('\Psi , degree')
hold on
p11 = polyfit(time,error1,7);
f11 = polyval(p11,time);

%plot(time,error2)
plot(time, f11, LineWidth=1.25 );
legend('Рассчитанная ошибка', 'Полиномиальное решение 7-ого порядка');
hold off

%% 3.3 Graphycs gyro for filter and gyro equation
f3 = figure;

%exp = expfit(gyroEquation')
plot(time, gyroEquation, 'r',LineWidth=0.1 );
grid on
grid minor
title('Гиромагнитный курс при Т = 900')
xlabel('time, с')
ylabel('\Psi , degree')
hold on

plot(time,skolzh, 'b',LineWidth=1.5);
legend('Фильтр скользящего среднего', 'Без фильтра');
hold off

%% 4. Import data
data=importdata('output_inert.txt');
save('magn', 'magn');

data2=importdata('output_inert.txt');
save('inert', 'inert');

data3=importdata('output_inert.txt');
save('gyro', 'gyroEquation');