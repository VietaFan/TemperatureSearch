# Processes raw station data file from ftp://ftp.ncdc.noaa.gov/pub/data/normals/1981-2010/station-inventories/allstations.txt
# Connects station names with underscores for processing later on.

stationFile = open('allstations.txt','r')
outputFile = open('stationdata.txt','w')

for line in stationFile.readlines():
    pieces = line.split()
    statName = '_'.join(pieces[5:])
    outputPieces = pieces[:5]+[statName]
    outputFile.write(' '.join(outputPieces)+'\n')
    
stationFile.close()
outputFile.close()
