import os
import os.path
filenames = os.listdir("./")
for file in filenames:
    print file
    if(os.path.is_dir(file)):
        print "is a file"    
