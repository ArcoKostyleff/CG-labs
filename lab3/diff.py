def count_different_symbols(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        content1 = f1.read()
        content2 = f2.read()

    count = 0
    for symbol1, symbol2 in zip(content1, content2):
        if symbol1 != symbol2:
            count += 1

    return count

for case_no in range(1, 8): 
    print(f'\n\nCase {case_no}')
    for i in range(1, 19):
        filename_pattern = f'build/output{case_no}_%s_{i}.txt'
        print(count_different_symbols(filename_pattern % 'DDA', filename_pattern % 'Bresenham'))