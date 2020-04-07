from sys import argv, exit
from cs50 import SQL

# check number of command line arguments
if len(argv) != 2:
    exit(1)

# access db file for SQLite
db = SQL("sqlite:///students.db")

# get students data from db
data = db.execute('SELECT * FROM students WHERE house = ? ORDER BY last, first', argv[1])

# prepare data for every student
for student in data:

    # fromat full name
    name = [student['first'], student['last']]
    if student['middle']:
        name.insert(1, student['middle'])

    # format complete inforamtion
    full_info = f'{" ".join(name)}, born {student["birth"]}'

    # print information
    print(full_info)
