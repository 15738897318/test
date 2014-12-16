function analyzeMetrics

load largeSet;
dataSet = largeSet;
resultDir = '1';
settings = getLocalSettings();



allBias = [];
allSk = [];

for k = 1:length(largeSet)
    results(k) = load([settings.outputBaseDir resultDir '/' dataSet{k} '_HRresult.mat']);
end
idx = results(k).beatMetricsIdx;
   
testCol = results(k).beatMetricsIdx.rate1;
beatMetrics = vertcat(results.beatMetrics);
err = beatMetrics(:,idx.ref) - beatMetrics(:,testCol);
err=abs(err);


thresh = 10;

figure(34);
fields = fieldnames(idx);
for(k=1:length(fields))
    col = idx.(fields{k});
    m = beatMetrics(:,col);
    

    ind = find(isfinite(m+err));
    m=m(ind);
    err2=err(ind);
    
    T = err2<=thresh;
    
    
    
    
    iGood = find(T==1);
    iBad = find(T==0);
    if(nanmedian(m(iGood)) < nanmedian(m(iBad)))
        T=T==0;
    end
    
    
    thresholds = prctile(beatMetrics(:,col),[0:.5:100]);
    
    rocdata=[T m];
   
    bins = linspace(prctile(m,.1),prctile(m,99.9),100);
    
    
    [bestthresh,sensitivity,specificity,auc,sens,onemspec] = roc(rocdata,thresholds,1,0);

    figure(33);
    subplot(2,1,1);
    plotroc2(bestthresh,sensitivity,specificity,auc,sens,onemspec);
    subplot(2,1,2);
    %plot(m,err2,'x');
    c = corrcoef(m,err2);
    title([fields{k} ' ' num2str(c(1,2))]);
     [y,x] = hist(m(iBad),bins);
     y=y./length(m(iBad));
     plot(x,y);
     hold on;
     [y,x] = hist(m(iGood),bins);
     y=y./length(m(iGood));
     plot(x,(y),'g');
     title(fields{k});
     drawnow;
     pause;
     clf;


end













