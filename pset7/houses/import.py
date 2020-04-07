import csv
from cs50 import SQL
from sys import exit, argv

# check number of command line arguments
if len(argv) != 2:
    exit(1)

# access db file for SQLite
db = SQL("sqlite:///students.db")

# open csv file with character's data
with open(argv[1], 'r') as file:
    reader = csv.DictReader(file)

    for row in reader:
        # get character data
        full_name = row['name'].split()
        birth = row['birth']
        house = row['house']

        # split name into three parts
        first_name = full_name[0]
        last_name = full_name[-1]
        if len(full_name) == 3:
            middle_name = full_name[1]
        else:
            middle_name = None

        # insert character data into db
        db.execute('INSERT INTO students (first, middle, last, house, birth) VALUES (?, ?, ?, ?, ?)',
                   first_name, middle_name, last_name, house, birth)
