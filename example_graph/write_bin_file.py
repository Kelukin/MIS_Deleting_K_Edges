import sys

argv = sys.argv

outPath = argv[1]
x = [4, 1, 2, 3, 4]
if __name__ == "__main__":
    file_path = outPath + '_given_set.bin'
    with open(file_path, "wb") as f:
        for ele in x:
            content = ele.to_bytes(4, sys.byteorder)
            f.write(content)
