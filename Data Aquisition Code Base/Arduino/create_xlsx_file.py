import sys
import os
import re
import string
import xlsxwriter
import math 

def main(argv):
    lines = list(open(argv[1]))
    mylist = list()
    listnopunc = list()    
    listoflists = list()
    # remove the new lines
    for line in lines:
        line = line.rstrip()
        mylist.append(line)
    i = 0
    for item in mylist:
        item = re.sub('[^0-9ena\-\.]+', " ", item)
        listnopunc.append(item)

    for item in listnopunc:
        item = item.split()
        listoflists.append(item)

    # create the workbook
    workbook = xlsxwriter.Workbook('Data00.xlsx')
    worksheet = workbook.add_worksheet()
    row = 0
    col = 0
    for sublist in listoflists:
        for entry in sublist:
            if math.isnan(float(entry)):
                entry = '0'
            worksheet.write(row, col, float(entry))
            col += 1
        row += 1
        col = 0    



if __name__ == '__main__':
    main(sys.argv[0:])
