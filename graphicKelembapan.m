% Define the range of x
x = 0:0.1:100;

% Define the membership functions
mu_SangatKering = zeros(size(x));
mu_Kering = zeros(size(x));
mu_Sedang = zeros(size(x));
mu_CukupBasah = zeros(size(x));
mu_Basah = zeros(size(x));

% Calculate the membership values for Sangat Kering
mu_SangatKering(x = 0) = 1;
mu_SangatKering(x > 0 & x <= 25) = (25 - x(x > 0 & x <= 25)) / 25;
mu_SangatKering(x > 25) = 0;

% Calculate the membership values for Kering
mu_Kering(x >= 20 & x <= 25) = (x(x >= 20 & x <= 25) - 20) / 5;
mu_Kering(x > 25 & x <= 40) = 1;
mu_Kering(x > 40 & x <= 45) = (45 - x(x > 40 & x <= 45)) / 5;

% Calculate the membership values for Sedang
mu_Sedang(x >= 40 & x <= 45) = (x(x >= 40 & x <= 45) - 40) / 5;
mu_Sedang(x > 45 & x <= 60) = 1;
mu_Sedang(x > 60 & x <= 65) = (65 - x(x > 60 & x <= 65)) / 5;

% Calculate the membership values for Cukup Basah
mu_CukupBasah(x >= 60 & x <= 65) = (x(x >= 60 & x <= 65) - 60) / 5;
mu_CukupBasah(x > 65 & x <= 80) = 1;
mu_CukupBasah(x > 80 & x <= 85) = (85 - x(x > 80 & x <= 85)) / 5;

% Calculate the membership values for Basah
mu_Basah(x >= 80 & x <= 85) = (x(x >= 80 & x <= 85) - 80) / 5;
mu_Basah(x > 85) = 1;

% Plot the membership functions
figure;
hold on;
plot(x, mu_SangatKering, 'r', 'DisplayName', 'Sangat Kering');
plot(x, mu_Kering, 'g', 'DisplayName', 'Kering');
plot(x, mu_Sedang, 'b', 'DisplayName', 'Sedang');
plot(x, mu_CukupBasah, 'c', 'DisplayName', 'Cukup Basah');
plot(x, mu_Basah, 'm', 'DisplayName', 'Basah');
hold off;

xlabel('Soil Moisture (%)');
ylabel('Membership Degree');
title('Fuzzy Membership Functions for Soil Moisture');
legend('show');
grid on;

