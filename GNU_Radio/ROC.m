%distribution mean for channel not detected
Pnotarget = makedist('Normal','mu', -74.532,'sigma',5.037);

%distribution mean for channel detected
Ptarget = makedist('Normal', 'mu', -53.046,'sigma',5.163);
threshold = -60 ; %dB

Pfa = 1 - cdf(Pnotarget,threshold) % prob of false alarm
Pd = 1 - cdf(Ptarget,threshold) % probability of detection

Level=[-80:-40];
figure(1);
plot(Level,Pnotarget.pdf(Level));
hold on
plot(Level,Ptarget.pdf(Level),'m');
title('RTL-SDR detection probability')
hold on
Y = 0:0.1:0.3;
X = threshold * ones(size(Y));
plot(X, Y, 'r--')
legend('no target present', 'target present','threshold')
xlabel ('Level (dB)')
Pfa_ROC = 1 - cdf(Pnotarget,Level); % prob of false alarm
Pd_ROC = 1 - cdf(Ptarget,Level); % prob of detection
figure(2);
plot(Pfa_ROC,Pd_ROC);
title('ROC curves')
ylabel ('Probability of Detection')
xlabel ('Probability of False Alarm')