import webbrowser
import sys

lat1 = sys.argv[1]
long1 = sys.argv[2]
lat2 = sys.argv[3]
long2 = sys.argv[4]

webbrowser.open('https://www.google.com/maps/dir/%s,%s/%s,%s/' % (lat1, long1, lat2, long2))


#f = open('cityout.txt', 'r')
#a = f.readline()
#state_a = a.split()[0]
#place_a = a.split()[1]
##place_a = ''
##u = 0
##for char in place_a0[:-1]:
##    if char == '_':
##        place_a += '+'
##        u += 1
##        if u >= 3:
##            break
##    else:
##        place_a += char
#b = f.readline()
#state_b = b.split()[0]
#place_b = b.split()[1]
##place_b = ''
##y = 0
##for char in place_b0[:-1]:
##    if char == '_':
##        place_b += '+'
##        y+=1
##        if y >= 3:
##            break
##    else:
##        place_b += char
#webbrowser.open('https://www.google.com/maps/dir/%s+%s/%s+%s/' % (place_a,state_a,place_b,state_b))
#webbrowser.open('https://www.google.com/maps/place/%s+%s/' % (place_a,state_a),new=2)
#webbrowser.open('https://www.google.com/maps/place/%s+%s/' % (place_b,state_b),new=2)
#webbrowser.open('https://www.google.com/maps/dir/%s,%s/%s,%s/' % (state_a,place_a,state_b,place_b))
#f.close()
