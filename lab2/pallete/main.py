pallete = {}
with open ("pallete5x5.txt", "r") as file:
    for line in file:
        print(line.split())
        pallete[line.split()[0]] = line.split()[1].strip()

for key, value in pallete.items():
    print('FF' + value, end='')

print()