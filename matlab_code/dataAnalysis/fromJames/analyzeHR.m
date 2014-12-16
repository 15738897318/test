function analyzeHR()
close all;

ALG_VERSIONS = {'1','2'};
ALG_LABELS = {'WithANC','WoANC'};
ALG_COLORS = {'b','r'};
outDirPrefix = 'all_';

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



%set to analyze
load largeSet;
populations.all = largeSet;
load noMoSet;
populations.noMo = noMoSet;
load motionSet;
populations.mo = motionSet;
load ecgRefSet;
populations.ecgRef = ecgRefSet;

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
    plotHRTrend(allData{k},ALG_VERSIONS,ALG_LABELS,ALG_COLORS);
    exportToPPTX('addslide');
    exportToPPTX('addpicture',gcf,'scale','max');
end
exportToPPTX('saveandclose',trendPPT);


for(k=1:length(popNames))
    for(v=1:length(ALG_VERSIONS))
        createHRSummaryPlots(populations.(popNames{k}),ALG_VERSIONS{v},popNames{k},resultDir,ALG_LABELS{v}); 
    end
end






