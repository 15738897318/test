function analyzeHRV()
close all;

ALG_VERSIONS = {'1'};
ALG_LABELS = {'PPG'};
ALG_COLORS = {'b'};
outDirPrefix = 'hrv_';

if(length(ALG_VERSIONS) > 1)
    outDir = outDirPrefix;
    for(k=1:length(ALG_VERSIONS)-1)
        outDir = [outDir ALG_VERSIONS{k} '_vs_' ];
    end
    outDir = [outDir ALG_VERSIONS{end}];
else
    outDir = [outDirPrefix ALG_VERSIONS{1}];
end

settings = getLocalSettings();
%outputFileDir = [settings.outputBaseDir '/' algVer];
resultDir = [settings.resultsBaseDir '/' outDir];

if(~exist(resultDir,'dir'))
    mkdir(resultDir);
end

trendPPT = [resultDir '/trendplots'];

%sets to analyze
load hrvSet;
populations.hrvSet = hrvSet;

popNames = fieldnames(populations);
allData = {};
for(k=1:length(popNames))
    allData = [allData;populations.(popNames{k})];
end
allData = unique(allData);

%create trend plots
exportToPPTX('new');
for(k=1:length(allData))
    figure(1);
    clf;
    plotHRVTrend(allData{k},ALG_VERSIONS,ALG_LABELS,ALG_COLORS);
    exportToPPTX('addslide');
    exportToPPTX('addpicture',gcf,'scale','max');
end
exportToPPTX('saveandclose',trendPPT);


for(k=1:length(popNames))
    for(v=1:length(ALG_VERSIONS))
        createHRVSummaryPlots(populations.(popNames{k}),ALG_VERSIONS{v},popNames{k},resultDir,ALG_LABELS{v}); 
    end
end






