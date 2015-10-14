# remove the bonus flag argument and
# reset all bonus attributes to zero

import os
import sys
import csv

if __name__ == '__main__':
    bonus_old = open('bonus_db.txt', 'r')
    bonus_new = open('bonus_db_.txt', 'w')

    # tweak the arguments until it parses the quoted fields entire =.=
    reader = csv.reader(bonus_old, delimiter = ',', quoting = csv.QUOTE_ALL, quotechar='"', dialect = csv.Dialect.quoting)
    for row in reader:
        # write the non-bonus entries verbatim
        # to preserve the comments and spacing
        length = len(row)
        if length <= 0:
            bonus_new.write('\n')
            continue
        elif length < 10:
            bonus_new.write("{0}\n".format(row[0]))
            continue

        bonus_new.write('{0},{1},{2},{3},"{4}",{5}'.format(row[0], 0, row[3], row[4], row[5], row[6]))

        for i in range(int(row[6])):
            bonus_new.write(',{0}'.format(row[i + 7]))
        bonus_new.write('\n')

    bonus_old.close()
    bonus_new.close()
