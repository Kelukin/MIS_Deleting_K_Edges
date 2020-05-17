import os
import struct
if __name__ == "__main__":
    filepath = "./b_given_set.bin"
    with open(filepath, "rb") as binfile:
        size = os.path.getsize(filepath)
        i = 0
        while i < size:
            data = binfile.read(4)
            num = struct.unpack('I', data)
            print(num[0])
            i += 4