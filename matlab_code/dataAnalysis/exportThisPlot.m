function exportThisPlot(varargin)

% OPTIONS

plotPath    = '/Users/kalmar/Documents/misfit/results/';
plotDir     = [];
fileIdx     = 0;
autoIdx     = 0;
expDate     = datestr(date, 29); 
name        = []; 
setFigName  = 1;
orderPlots  = 0;
plotType    = 'png';
paperPositionAuto = [0.25 2.5 8 6];
paperPositionFig  = [0.75 2.5 6 4.5];
paperPositionFull = [0.75 0.75 12 9];
screenPositionFull = [1 -21 1627 978];
plotFullSized  = 1;
plotForPrint   = 1;
plotForFig     = 0;
plotForSlide   = 0;
highRes        = 1;
lowRes         = 0;
superHighRes   = 0;
uniqueFilename = 1;

assignopts(who, varargin);

%% use this while transitioning code such that 'plotForFig' is replaced with 'plotForSlide'
if plotForFig 
  plotForSlide = 1;
  plotForPrint = 0;
end
if plotForSlide 
    plotFullSized = 0;
end
if plotForPrint
  plotForSlide = 0;
end

if isempty(name)
  name        = sprintf('%s-plot', expDate);
end  

if strcmp(plotType,'screenshot')
    plotTypeExt = 'png';
else
    plotTypeExt = plotType;
end

if orderPlots
  filename    = sprintf('%02d-%s.%s', fileIdx, name, plotTypeExt);
elseif autoIdx
  filename    = sprintf('%s-%02d.%s', name, fileIdx, plotTypeExt);
else
  filename    = sprintf('%s.%s', name, plotTypeExt);
end

baseDir     = pwd;
if ~exist(plotPath)
  mkdir(plotPath)
end
cd(plotPath)
if ~isempty(plotDir)
  path = fullfile(plotPath, plotDir);
  if exist(path,'dir') == 0
    mkdir([plotPath], plotDir);
    unix(sprintf('chmod 775  %s', plotDir));
  end
  plotPath = path;
end

cd(plotPath);

%% should make this a standalone, more general function
%% check to see if plot with that name exists, if so, give it a unique name
if uniqueFilename
    filenameExists = 1;

    while filenameExists
      if ~exist(filename)
        if orderPlots
          name = sprintf('%02d-%s', fileIdx, name);
        elseif autoIdx
          name = sprintf('%s-%02d', name, fileIdx');
        else
          name = sprintf('%s.%s', name, plotType);
        end
        filenameExists = 0;
      else
        fileIdx = fileIdx + 1;
        if orderPlots
          filename = sprintf('%02d-%s.%s', fileIdx, name, plotType);      
        else  
          filename = sprintf('%s-%02d.%s', name, fileIdx, plotType);
        end
      end
    end
end

if setFigName
  set(gcf,'Name',name);
end

if plotFullSized
    hFig = gcf;
    origPosition = get(hFig,'Position');
    set(hFig, 'Position', screenPositionFull);
end

disp(sprintf('Saving %s...', filename));
if plotForSlide
  set(gcf,'PaperPositionMode','manual','PaperPosition',paperPositionFig);
elseif plotFullSized
  set(gcf,'PaperPositionMode','auto');
  %set(gcf,'PaperPositionMode','auto','PaperPosition',paperPositionFull);  
else
  set(gcf,'PaperPositionMode','manual','PaperPosition',paperPositionAuto);
end

if strcmp(plotType, 'jpg')
    print(gcf, '-djpeg', '-r0', filename);
elseif strcmp(plotType, 'eps')
    print(gcf, '-depsc', '-loose', filename);
elseif strcmp(plotType, 'png')
    if superHighRes
        print(gcf, '-dpng', '-r600', filename);
    elseif highRes
        print(gcf, '-dpng', '-r300', filename);
    elseif lowRes
        print(gcf, '-dpng', '-r85', filename);        
    else
        print(gcf, '-dpng', filename);
    end
elseif strcmp(plotType, 'fig')
    saveas(gcf, filename, 'fig');
elseif strcmp(plotType,'screenshot')
    screencapture(gcf,[],filename)
end

if plotFullSized
    set(hFig,'Position',origPosition);
end
    
unix(sprintf('chmod 775 %s', filename));
cd(baseDir);
  
 
