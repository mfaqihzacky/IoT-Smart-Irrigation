% Define the range for x
x = 0:0.1:50;

% Initialize membership functions
mu_Dingin = zeros(size(x));
mu_Sejuk = zeros(size(x));
mu_Normal = zeros(size(x));
mu_Panas = zeros(size(x));
mu_SangatPanas = zeros(size(x));

% Calculate membership values
for i = 1:length(x)
    % μDingin(x)
    if x(i) <= 5
        mu_Dingin(i) = 1;
    elseif x(i) > 5 && x(i) <= 25
        mu_Dingin(i) = (25 - x(i)) / 20;
    else
        mu_Dingin(i) = 0;
    end

    % μSejuk(x)
    if x(i) >= 20 && x(i) <= 25
        mu_Sejuk(i) = (x(i) - 20) / 5;
    elseif x(i) > 25 && x(i) <= 30
        mu_Sejuk(i) = (30 - x(i)) / 5;
    else
        mu_Sejuk(i) = 0;
    end

    % μNormal(x)
    if x(i) >= 25 && x(i) <= 30
        mu_Normal(i) = (x(i) - 25) / 5;
    elseif x(i) > 30 && x(i) <= 35
        mu_Normal(i) = (35 - x(i)) / 5;
    else
        mu_Normal(i) = 0;
    end

    % μPanas(x)
    if x(i) >= 30 && x(i) <= 35
        mu_Panas(i) = (x(i) - 30) / 5;
    elseif x(i) > 35 && x(i) <= 40
        mu_Panas(i) = (40 - x(i)) / 5;
    else
        mu_Panas(i) = 0;
    end

    % μSangatPanas(x)
    if x(i) >= 35 && x(i) <= 40
        mu_SangatPanas(i) = (x(i) - 35) / 5;
    elseif x(i) >= 40
        mu_SangatPanas(i) = 1;
    else
        mu_SangatPanas(i) = 0;
    end
end

% Plot the membership functions
figure;
plot(x, mu_Dingin, 'b', 'LineWidth', 2); hold on;
plot(x, mu_Sejuk, 'g', 'LineWidth', 2);
plot(x, mu_Normal, 'y', 'LineWidth', 2);
plot(x, mu_Panas, 'm', 'LineWidth', 2);
plot(x, mu_SangatPanas, 'r', 'LineWidth', 2);
axis([0 45 0 1.2]);
xlabel('x');
ylabel('\mu(x)');
legend('Dingin', 'Sejuk', 'Normal', 'Panas', 'Sangat Panas', 'Location', 'NorthWest');
title('Membership Functions for Temperature Categories');
grid on;

