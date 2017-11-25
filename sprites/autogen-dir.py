import os
import sys

if (len(sys.argv) == 1):
    print("Pass a directory (. for current dir)")
    sys.exit(1)

print("[")
for dirname, dirnames, filenames in os.walk(sys.argv[1]):
    # print path to all filenames.
    for filename in filenames:
        print("\t\""+os.path.join(dirname, filename)+"\",")

print("]")
    

