function compareTopBotWrist()
close all;

%calc top of wrist stats
load topSet;
nFiles = length(topSet);
allTop = [];
aveTop = [];
for(k=1:nFiles)
    data = loadCSVData(topSet{k});
    data.ref(1:end) = nanmean(data.ref);
    statsNow = signalStats(data);
    allTop = [allTop;statsNow];
    fields = fieldnames(statsNow);
    for(k=1:length(fields))
        aveNow.(fields{k}) = nanmedian(vertcat([statsNow.(fields{k})]));
    end
    aveTop = [aveTop;aveNow];
end

%calc bot of wrist stats
load bottomSet;
nFiles = length(bottomSet);
allBot = [];
aveBot = [];
for(k=1:nFiles)
    data = loadCSVData(bottomSet{k});
    data.ref(1:end) = nanmean(data.ref);
    statsNow = signalStats(data);
    allBot = [allBot;statsNow];
    fields = fieldnames(statsNow);
    for(k=1:length(fields))
        aveNow.(fields{k}) = nanmedian(vertcat([statsNow.(fields{k})]));
    end
    aveBot = [aveBot;aveNow];
end


    
fields = fieldnames(aveBot(1));
for(k=1:length(fields))
    metric = fields{k};
    
    top = vertcat(aveTop.(fields{k}));
    bot = vertcat(aveBot.(fields{k}));
    
    gTop = repmat({'top'},length(top),1);
    gBot = repmat({'bottom'},length(bot),1);
    boxplot([top(:,end);bot(:,end)],[gTop;gBot]);
    title(metric);
    drawnow;
    pause;
    clf;
end




