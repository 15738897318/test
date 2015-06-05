function blandAltman(ref,test)

multiSubject = false;

if(iscell(ref) && iscell(test))
    allRef = vertcat(ref{:});
    allTest = vertcat(test{:});
    multiSubject = true;
else
    allRef = ref;
    allTest = test;
end


sd = std(allTest-allRef);
md = mean(allTest-allRef);
allMean = mean([allTest allRef],2);

if(multiSubject)
   Nsubjects = numel(ref);
   markers = createRandomMarkers(Nsubjects);
   for(k=1:Nsubjects)
       x = mean([ref{k} test{k}],2);
       y = test{k} - ref{k};
       plot(x,y,markers{k});
       hold on;
   end
else
   x = mean([ref test],2);
   y = test - ref;
   plot(x,y,'b*');
   hold on;
end

plot(allMean,md+zeros(size(allMean)),'k--','linewidth',1.5);
plot(allMean,-1.96*sd+zeros(size(allMean)),'r--');
plot(allMean,1.96*sd+zeros(size(allMean)),'r--');
axis tight;
a = axis(gca);
text(a(2),-1.96*sd,num2str(-1.96*sd),'HorizontalAlignment','left');
text(a(2),+1.96*sd,num2str(+1.96*sd),'HorizontalAlignment','left');
text(a(2),md,num2str(md),'HorizontalAlignment','left');
set(gcf,'color','w');
grid on;
