# get string input from a user
text = input("Text: ")

# count number of letters, words and sentences
letters = sum(c.isalpha() for c in text)
words = len(text.split())
sentences = 0
for c in text:
    if c in [".", "!", "?"]:
        sentences += 1

# letters and sentences per 100 words
letters_100 = letters * 100 / words
sentences_100 = sentences * 100 / words

# calculate Coleman-Liau index
index = round(0.0588 * letters_100 - 0.296 * sentences_100 - 15.8)

# print results
if index < 1:
    print("Before Grade 1")
elif index >= 16:
    print("Grade 16+")
else:
    print(f"Grade {index}")
