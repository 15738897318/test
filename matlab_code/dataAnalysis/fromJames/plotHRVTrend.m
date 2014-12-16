%create an HR trend plot
%fileName - name of the file/subject (e.g., matt_hrv)
%versions - a cell array of version to plot (settings.outputBaseDir '/' %versions{k}
%a label for each version (to be put on the legend)
%colors - a plot color for each version ('b','r','k',etc)
function plotHRVTrend(fileName,versions,labels,colors)
settings = getLocalSettings;
if(~iscell(versions) && numel(versions)==1)
    versions = {versions};
    labels = {labels};
    colors = {colors};
end
procData(1) = load([settings.outputBaseDir versions{1} '/' fileName '_HRresult.mat']);
idx = procData(1).resultsIdx;
plot(procData(1).results(:,idx.tResult),procData(1).results(:,idx.hrvGold),'y','linewidth',2);
hold on;
plot(procData(1).results(:,idx.tResult),procData(1).results(:,idx.hrv),colors{1});
for(k=2:length(versions))
    procData(k) = load([settings.outputBaseDir versions{k} '/' fileName '_HRresult.mat']);
    plot(procData(k).results(:,idx.tResult),procData(k).results(:,idx.hrv),colors{k});
end
grid on;
allResults = vertcat(procData(1).results);
allHRVectors = allResults(:,[idx.hrv idx.hrvGold]);
allHRVectors = allHRVectors(:);
axis([-inf inf min(allHRVectors)-2 max(allHRVectors)+2]);
title(fileName,'interpreter','none');
legend([{'HRV-ECG'},labels],'orientation','horizontal','location','southwest','interpreter','none');




