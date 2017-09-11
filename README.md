# TemperatureSearch
Finds a pair of cities with temperatures as close as possible to a specified optimal temperature, if it is required to stay in the cities for consecutive blocks of the year. (For example, one possible answer would be to choose Grand Marias, MN from May through September and Chisos Basin, TX from October through April.)

# Usage Description
The program reads the sequence of geographical constraints and the optimal temperature from inputs.txt, where they are specified by the user. It then finds a pair of cities subject to the constraints that minimizes the average monthly deviation from the optimal temperature. Depending on the options chosen, it opens a Google maps page showing the route between the two cities. It also generates a graph showing the monthly temperatures in the two cities, which month to move between the cities in order to minimize the average deviation. An example plot is shown below.

# Data Sources

This project uses the monthly average temperature data provided by the US National Climatic Data Center for around 8,000 weather stations. The two main files used for the data are ftp://ftp.ncdc.noaa.gov/pub/data/normals/1981-2010/products/temperature/mly-tmax-normal.txt, which is tempdata.txt, and ftp://ftp.ncdc.noaa.gov/pub/data/normals/1981-2010/station-inventories/allstations.txt, which is allstations.txt. 

Running stationdatafixer.py reformats the allstations.txt data file, saving the result in stationdata.txt.
