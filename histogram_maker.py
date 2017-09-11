import pylab

tempFile = open('citytemps.txt')
optTemp = float(tempFile.readline().split()[0])

tempsA = []
tempsB = []

# read temperatures and times for each city from file
for i in range(12):
    cityNum, tempVal = tempFile.readline().split()
    if (cityNum == '1'):
        tempsA.append(float(tempVal))
        tempsB.append(0)
    else:
        tempsA.append(0)
        tempsB.append(float(tempVal))
        
loc1 = tempFile.readline().split('_')[0]
loc2 = tempFile.readline().split('_')[0]

tempFile.close()

# create plot of temperatures
pylab.bar(range(12), tempsA, color='b')
pylab.bar(range(12), tempsB, color='r')
pylab.plot([0,12], [optTemp, optTemp], color='g')
pylab.title("Monthly Average Highs for Optimal Locations")
pylab.ylabel("Average Temperature (Fahrenheit)")
pylab.xlabel("Month (red in %s, blue in %s)" % (loc1, loc2))
pylab.show()
