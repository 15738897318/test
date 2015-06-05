%create an HR trend plot
%fileName - name of the file/subject (e.g., matt_hrv)
%versions - a cell array of version to plot (settings.outputBaseDir '/' %versions{k}
%a label for each version (to be put on the legend)
%colors - a plot color for each version ('b','r','k',etc)

function plotHRTrend(fileName,versions,labels,colors)
settings = getLocalSettings;
if(~iscell(versions) && numel(versions)==1)
    versions = {versions};
    labels = {labels};
    colors = {colors};
end
a(1) = subplot(2,1,1);
procData(1) = load([settings.outputBaseDir versions{1} '/' fileName '_HRresult.mat']);
idx = procData(1).resultsIdx;
plot(procData(1).results(:,idx.tResult),procData(1).results(:,idx.gold),'y','linewidth',2);
hold on;
plot(procData(1).results(:,idx.tResult),procData(1).results(:,idx.hr),colors{1});
for(k=2:length(versions))
    procData(k) = load([settings.outputBaseDir versions{k} '/' fileName '_HRresult.mat']);
    plot(procData(k).results(:,idx.tResult),procData(k).results(:,idx.hr),colors{k});
end
grid on;
allResults = vertcat(procData(1).results);
allHRVectors = allResults(:,[idx.hr idx.gold]);
allHRVectors = allHRVectors(:);
axis([-inf inf min(allHRVectors)-10 max(allHRVectors)+10]);
title(fileName,'interpreter','none');
legend([{procData(1).refType},labels],'orientation','horizontal','location','southwest');


inputData = loadCSVData(fileName);
allTimes = allResults(:,[idx.tResult]);
ind = find(inputData.tPleth < min(allTimes) | inputData.tPleth > max(allTimes));
inputData.tPleth(ind) = [];
inputData.accel(ind,:)= [];
a(2) = subplot(2,1,2);
plot(inputData.tPleth,inputData.accel);
grid on;
axis tight;
title('accel');
linkaxes(a,'x');
set(gcf,'color','w')
drawnow;


