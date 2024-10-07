% Define the range of x for pump speed
x_pump = 0:0.1:100;

% Define the membership functions for pump speed
mu_Rendah = zeros(size(x_pump));
mu_Sedang = zeros(size(x_pump));
mu_Tinggi = zeros(size(x_pump));

% Calculate the membership values for Rendah
mu_Rendah(x_pump <= 0) = 1;
mu_Rendah(x_pump > 0 & x_pump <= 35) = (35 - x_pump(x_pump > 0 & x_pump <= 35)) / 35;
mu_Rendah(x_pump > 35) = 0;

% Calculate the membership values for Sedang
mu_Sedang(x_pump >= 30 & x_pump <= 35) = (x_pump(x_pump >= 30 & x_pump <= 35) - 30) / 5;
mu_Sedang(x_pump > 35 & x_pump <= 60) = 1;
mu_Sedang(x_pump > 60 & x_pump <= 65) = (65 - x_pump(x_pump > 60 & x_pump <= 65)) / 5;

% Calculate the membership values for Tinggi
mu_Tinggi(x_pump >= 60 & x_pump <= 65) = (x_pump(x_pump >= 60 & x_pump <= 65) - 60) / 5;
mu_Tinggi(x_pump > 65 & x_pump <= 100) = 1;

% Plot the membership functions
figure;
hold on;
plot(x_pump, mu_Rendah, 'r', 'DisplayName', 'Rendah');
plot(x_pump, mu_Sedang, 'g', 'DisplayName', 'Sedang');
plot(x_pump, mu_Tinggi, 'b', 'DisplayName', 'Tinggi');
hold off;

xlabel('Pump Speed (%)');
ylabel('Membership Degree');
title('Fuzzy Membership Functions for Pump Speed');
legend('show');
grid on;

