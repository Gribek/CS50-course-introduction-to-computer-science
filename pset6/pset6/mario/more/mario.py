while True:
    try:
        height = int(input("Height: "))
    except ValueError:
        continue
    if height > 0 and height <= 8:
        break

for i in range(1, height + 1):
    print(" " * (height - i), "#" * i, "  ", "#" * i, sep="")
