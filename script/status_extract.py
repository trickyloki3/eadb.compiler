# generates a blank status_db.txt
# from a enum sc_type in status.h

# step 1: locate the status.h source file
#   rathena/src/map/status.h
#   eathena/src/map/status.h
# step 2: copy the enum sc_type to a file
#   python status_extract.py <file>
#
# status_extract.py will output a  status_db.txt
# file containing a line for each status
#
# the status_db.txt must be regenerate
# everytime there are major changes to
# the enumeration
import os
import sys

if __name__ == "__main__":
    # check that path specified
    if len(sys.argv) != 2:
        print "Usage: python status_extract.py <file>"
        sys.exit(1)

    path = sys.argv[1]

    # check that file exists
    if not os.access(path, os.F_OK | os.R_OK):
        print "{0} does not exist".format(path)
        sys.exit(1)

    status_file = open(path, "r")
    status_file_db = open("status_db.txt", "w")
    status_type = 0

    for status in status_file:
        # strip whitespace
        status = status.strip()

        # filter out statuses
        if status.startswith("SC_"):

            # remove the comments
            status = status.split(',')[0]

            # parse the assignment
            status = status.split('=')

            # sc_common_min  status id conflicts with sc_stone
            # sc_none or sc_max is not useful for translations
            status_name = status[0].strip().lower()
            if  status_name == "sc_common_min" or \
                status_name == "sc_common_max" or \
                status_name == "sc_none" or \
                status_name == "sc_max":
                continue

            if len(status) > 1:
                status_type = int(status[1])

            status_file_db.write("{0:<5},{1:>35},?,?,?,?,\"\",0\n".format(status_type, status_name))

            status_type = status_type + 1


    status_file.close()
    status_file_db.close()
