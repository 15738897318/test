function createHRSummaryPlots(dataSet,ver,pop,resultDir,label)

settings = getLocalSettings();
outputFileDir = [settings.outputBaseDir '/' ver];

resultFiles = dataSet;
for(k=1:length(resultFiles))
    resultFiles{k} = [outputFileDir '/' resultFiles{k} '_HRresult.mat'];
end

%create cell arrays of test andr reference for each subject
testVectors = cell(1,length(resultFiles));
refVectors = testVectors;
timeVectors = testVectors;
refType = testVectors;
for(k=1:length(resultFiles))
    procData = load(resultFiles{k});
    refType{k} = procData.refType;
    results = procData.results;
    idx = procData.resultsIdx; 
    testVectors{k} = results(:,idx.hr);
    refVectors{k} = results(:,idx.gold);
    timeVectors{k} = results(:,idx.tResult);
end

%bland altman
figure(1);
clf;
blandAltman(refVectors,testVectors);
xlabel '(HR MISFIT + HR REF)/2';
ylabel '(HR MISFIT - HR REF)';
title('Bland Altman HR MISFIT');
saveas(gcf,[resultDir '/' pop '_' ver '_' label '_BA'],'png');
%saveas(gcf,[resultDir '/' ver '_BA'],fig);


%regression
figure(2);
close all;
allRef = vertcat(refVectors{:});
allTest = vertcat(testVectors{:});
regressionPlot(allTest,allRef);
xlabel 'Reference';
ylabel 'PPG HR';
title  'Heart Rate Performance'
saveas(gcf,[resultDir '/' pop '_' ver '_' label '_REG'],'png');
%saveas(gcf,[resultDir '/' ver '_REG'],fig);