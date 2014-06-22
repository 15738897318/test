file_input = '/Users/misfit/Desktop/Codes - Local/Working bench/bioSignalProcessing/eulerianMagnifcation/codeMatlab/Results/hr_results.csv'

import csv
from matplotlib import pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib import cm
import pandas as pd
import numpy as np

def metric_func(ratio_array):
	# Root-mean-square-error (RMSE)
	metric = np.sqrt(np.mean((ratio_array - 1) ** 2))
	
	# Mean-absolute-error (MAE)
	metric = np.mean(np.abs(ratio_array - 1))
	
	return (metric)



data_lines = csv.reader(open(file_input, 'rU'))
headers = data_lines.next()

HR_REF_COL = 0
HR_CHAN_COL = 1
HR_AUTOCORR_COL = 2
HR_PDA_COL = 3
HR_TYPE_COL = 4
HR_COLOURSPACE_COL = 5
HR_VIDFILE_COL = 6
HR_MIN_RATE_COL = 7
HR_MAX_RATE_COL = 8
HR_ALPHA_COL = 9
HR_LEVEL_COL = 10
HR_CHROMATN_COL = 11

data_table = []
for each_line in data_lines:
	data_line = [float(each_line[HR_REF_COL]), \
				 float(each_line[HR_CHAN_COL]), \
				 float(each_line[HR_AUTOCORR_COL]), \
				 float(each_line[HR_PDA_COL]), \
				 each_line[HR_TYPE_COL], \
				 each_line[HR_COLOURSPACE_COL], \
				 each_line[HR_VIDFILE_COL]]
				 
	data_table.append(data_line)
	
# Turn the data into R-style data-frame
data_table = pd.DataFrame(data_table)
data_table.columns = headers[0 : ]

output_table = [['cmap', 'cchan', 'vid_type', 'error_metric_autocorr', 'error_metric_pda', 'worst_vid_autocorr', 'worst_error_autocorr', 'worst_vid_pda', 'worst_error_pda']]
for key, group in data_table.groupby([headers[HR_COLOURSPACE_COL], headers[HR_CHAN_COL], headers[HR_TYPE_COL]]):
	output_line = [key[0], int(key[1]), key[2], \
				   metric_func(group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]]), \
				   metric_func(group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]]), \
				   group[headers[HR_VIDFILE_COL]][np.abs(group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]] - 1).argmax()].split('_')[0], \
				   (group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]] - 1)[np.abs(group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]] - 1).argmax()], \
				   group[headers[HR_VIDFILE_COL]][np.abs(group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]] - 1).argmax()].split('_')[0], \
				   (group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]] - 1)[np.abs(group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]] - 1).argmax()] ]
	output_table.append(output_line)
	
	fig = plt.figure()
	fig.suptitle('Colourspace = ' + str(key[0]) + ' & Channel = ' + str(int(key[1])) + ' & Video type = ' + str(key[2]))
	
	ax1 = fig.add_subplot(2, 1, 1)
	plt.title('Absolute values')
	ax1.hold(True)
	ax1.plot(group[headers[HR_REF_COL]], label = headers[HR_REF_COL])
	ax1.plot(group[headers[HR_AUTOCORR_COL]], label = headers[HR_AUTOCORR_COL])
	ax1.plot(group[headers[HR_PDA_COL]], label = headers[HR_PDA_COL])
	plt.ylabel('Heart rate (BPM)')
	
	ax1.set_xticks(range(group[headers[HR_REF_COL]].size))
	ax1.set_xticklabels(np.core.defchararray.partition(np.array(group[headers[HR_VIDFILE_COL]]).tolist(), '_')[:, 0], rotation = 90, fontsize = 8)
	plt.legend(loc = 'best', prop = {'size' : 10})
	
	ax2 = fig.add_subplot(2, 2, 3)
	plt.title('Error metric - Autocorr: %.1fpc and PDA: %.1fpc' \
			  % (100 * metric_func(group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]]), \
			     100 * metric_func(group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]])))
	ax2.hold(True)
	ax2.plot(100 * group[headers[HR_REF_COL]] / group[headers[HR_REF_COL]], label = headers[HR_REF_COL])
	ax2.plot(100 * group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]], label = headers[HR_AUTOCORR_COL])
	ax2.plot(100 * group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]], label = headers[HR_PDA_COL])
	
	#ax2.annotate(group[headers[HR_VIDFILE_COL]][np.abs(group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]] - 1).argmax()].split('_')[0], \
	#			 xy = (np.array(np.abs(group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]] - 1).tolist()).argmax() - 0.5, \
	#			 	   ((100 * group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]]).tolist())[np.array(np.abs(group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]] - 1).tolist()).argmax()]) )
	
	#ax2.annotate(group[headers[HR_VIDFILE_COL]][np.abs(group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]] - 1).argmax()].split('_')[0], \
	#			 xy = (np.array(np.abs(group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]] - 1).tolist()).argmax() - 0.5, \
	#			 	   ((100 * group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]]).tolist())[np.array(np.abs(group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]] - 1).tolist()).argmax()]) )
	
	plt.ylabel('Ratio wrt Basis reading (%)')
	
	ax2.set_xticks(range(group[headers[HR_REF_COL]].size))
	ax2.set_xticklabels(np.core.defchararray.partition(np.array(group[headers[HR_VIDFILE_COL]]).tolist(), '_')[:, 0], rotation = 90, fontsize = 8)
	plt.legend(loc = 'best', prop = {'size' : 10})
	
	
	ax3 = fig.add_subplot(2, 2, 4)
	ax3.boxplot([100 * group[headers[HR_AUTOCORR_COL]] / group[headers[HR_REF_COL]], \
				 100 * group[headers[HR_PDA_COL]] / group[headers[HR_REF_COL]]], \
				notch = 0, sym = 'gd', vert = 1, whis = 1.5)
	xtickNames = plt.setp(ax3, xticklabels = [headers[HR_AUTOCORR_COL], headers[HR_PDA_COL]])
	plt.setp(xtickNames, rotation = 0)
	plt.ylabel('Ratio wrt Basis reading (%)')

# Write the csv output file
file_output = '.'.join(file_input.split('.')[0 : -1]) + '-summary.csv'
file_handle = open(file_output, 'w')
write_handle = csv.writer(file_handle, delimiter = ',', quotechar='"')
for ind in range(len(output_table)):
	write_handle.writerow(output_table[ind])
file_handle.close()

plt.show()