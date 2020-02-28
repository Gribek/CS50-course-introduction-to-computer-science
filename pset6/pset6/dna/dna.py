from sys import argv, exit
import csv
import re

# check number of command-line arguments provided
if len(argv) != 3:
    print("Usage: python dna.py data.csv sequence.txt")
    exit(1)

# database
person_db = []

# STR sequences to check
str_sequences = []

# STR consecutive repeats
str_repeats = {}

# open csv file and copy provided database
with open(argv[1], "r") as csv_file:
    csv_reader = csv.DictReader(csv_file)
    for row in csv_reader:
        person = {}
        for key, value in row.items():
            person[key] = value
        person_db.append(person)
    else:
        for i in row.keys():
            str_sequences.append(i)
        str_sequences.remove("name")

# open txt file and copy dna sequence
with open(argv[2], "r") as file:
    dna_sequence = file.read().strip()

# find number of consecutive repeats for each STR sequence
for str in str_sequences:
    str_length = len(str)
    max_consecutive_repeats = 0
    for i in range(len(dna_sequence) - str_length + 1):
        if dna_sequence[i:i+str_length] == str:
            repeats = 1
            j = i + str_length
            while dna_sequence[j:j+str_length] == str:
                repeats += 1
                j += str_length
            if max_consecutive_repeats < repeats:
                max_consecutive_repeats = repeats
    str_repeats[str] = max_consecutive_repeats

# check person database for match
for person in person_db:
    check_person = [True if int(person[str]) == str_repeats[str] else False
    for str in str_sequences]
    if all(check_person):
        print(person["name"])
        break
else:
    print("No match")
