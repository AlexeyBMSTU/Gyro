clear all
clc
A = load("output_inert.txt");
y = A(:, 1);
x = A(:, 2);
plot(x,y)
grid on
grid minor
title("Инерциальный курс от времени")
xlabel('time, с')
ylabel('\Psiи , degree')
hold on
p = polyfit(x,y,15);
f1 = polyval(p,x);
plot(x, f1, LineWidth=2 );

legend('исходная зависимость', 'полиномиальная зависимость')