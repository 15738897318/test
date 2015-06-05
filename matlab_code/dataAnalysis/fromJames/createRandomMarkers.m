%creates a cell array of markers for plot()
function markers = createRandomMarkers(numMarkers)

colors = {'b','g','r','c','m','k'};
styles = {'.','o','x','+','*','s','d','v','^','<','>','p','h'};

colors = colors(randperm(length(colors)));
styles = styles(randperm(length(styles)));

markers = cell(length(styles)*length(colors),1);

mIdx = 1;
for(sIdx = 1:length(styles))
    for(cIdx = 1:length(colors))
        markers{mIdx} = [colors{cIdx} styles{sIdx}];
        mIdx=mIdx+1;
    end
end


while(length(markers) < numMarkers)
    markers = [markers;markers];
end

%markers = markers(randperm(length(markers)));
markers = markers(1:numMarkers);