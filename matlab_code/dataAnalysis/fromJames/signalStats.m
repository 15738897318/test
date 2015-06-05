%a function to pull out some simple signal statistics without 
%running an HR algorithm
function stats = signalStats(signals)
Fs = signals.Fs;
step = round(1*Fs);
winLen = round(30*Fs);
winLen = min(winLen,length(signals.pleth));
stats = [];
for(e = winLen:step:length(signals.pleth))
    s = e-winLen+1;
    win = getSubWin(signals,s,e);
    winStats = subWinStats(win);
    stats = [stats;winStats];
end

function stats = subWinStats(signals)
    Fs = signals.Fs;

    stats.dc = mean(signals.pleth);

    signals.pleth = signals.pleth - repmat(stats.dc,length(signals.pleth),1);

    %remove respiration frequencies
    highPassFreq = 35/60;
    [b,a] = butter(3,highPassFreq*2*(1/Fs),'high');
    signals.pleth = filtfilt(b,a,signals.pleth);

    stats.ac = std(signals.pleth);
    stats.mod = 100*(prctile(signals.pleth,90)-prctile(signals.pleth,10))./stats.dc;
    stats.accelVar = var(signals.accel);
    stats.skew = skewness(filter(ones(1,10)/10,1,diff(signals.pleth)));

    HRbar = nanmean(signals.ref);
    if(isnan(HRbar))
        stats.pRatio = nan;
        stats.SNR = nan(1,size(signals.pleth,2));
        return;
    end

    %find beats using known HR
    lpFreq = HRbar*1.25;
    hpFreq = HRbar*.75;
    [b,a] = butter(3,[hpFreq lpFreq]*2*(1/Fs)/60);
    bpPleth = filtfilt(b,a,signals.pleth);
    thresh = 0;
    idxCross = find(bpPleth(2:end,end) > thresh & bpPleth(1:end-1,end) <= thresh);
    %get measure of high frequency noise
    pRatio = zeros(1,length(idxCross)-1);
    for(k=2:length(idxCross))
        pulse = signals.pleth(idxCross(k-1):idxCross(k),end);
        pRatio(k-1) = sum(abs(diff(pulse)))/range(pulse); 
    end
    stats.pRatio = mean(pRatio(pRatio>0));




    %get SNR
    [f,Y] = doFFT(signals.pleth,1/Fs,8);

    idxHR = [];
    for(k=1:6)
        hrMax = (k*HRbar + 15)/60;
        hrMin = (k*HRbar - 15)/60;
        idxHR = [idxHR find(f > hrMin & f < hrMax)];
    end
    YHR = zeros(size(Y));
    YNoise = Y;
    YHR(idxHR,:) = Y(idxHR,:);
    YNoise(idxHR,:) = 0;
    pNoise = sqrt(sum(YNoise).^2);
    pHR = sqrt(sum(YHR).^2);
    stats.SNR = 20 * log10(pHR./pNoise);
















