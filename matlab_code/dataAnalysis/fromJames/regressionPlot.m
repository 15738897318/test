function stats = regressionPlot(x,y)

P = polyfit(x,y,1);

plot(x,y,'x');
hold on;
plot(x,x,'k--');
plot(x,polyval(P,x),'r--');

Rsq = corrcoef(x,y);
Rsq = Rsq(1,2);

axis tight;
grid on;


r = rmsd(x,y);
p = precision(x,y);
b = meanbias(x,y);

newline = 10;
plotText = [...
    'R^2  = ' num2str(Rsq,'%.02f') newline ...
    'y    = ' num2str(P(1),'%.02f') 'x' ' + ' num2str(P(2),'%.02f') newline ...
    'rmsd = ' num2str(r,'%.02f') newline ...
    'prec = ' num2str(p,'%.02f') newline ...
    'bias = ' num2str(b,'%.02f') ...
    ];

a = axis(gca);
text(a(1)+(a(2)-a(1))*.05,a(4) - (a(4)-a(3))*.05,plotText,'VerticalAlignment','top','HorizontalAlignment','left','EdgeColor',[0 0 0],'FontWeight','bold','FontSize',14);

stats.rmsd = r;
stats.precision = p;
stats.bias = b;

