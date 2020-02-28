from sys import exit

# imput credit card number from user
card_number = input("Number: ")

# Luhn’s Algorithm
# total sum
sum = 0

# loop through every other digit, starting with second-to-last digit
for digit in card_number[-2::-2]:
    digit = int(digit) * 2
    if digit > 9:
        digit = str(digit)
        sum += int(digit[0]) + int(digit[1])
    else:
        sum += digit

# add remaining digits
for digit in card_number[::-2]:
    sum += int(digit)

# check if card number is valid
if sum % 10 != 0:
    print("INVALID")
    exit(0)

# credit card number length
length = len(card_number)

# check type of credit card
if length == 15 and card_number[:2] in ["37", "34"]:
    print("AMEX")
elif length == 16 and card_number[:2] in ["51", "52", "53", "54", "55"]:
    print("MASTERCARD")
elif (length == 13 or length == 16) and card_number[0] == "4":
    print("VISA")
else:
    print("INVALID")
