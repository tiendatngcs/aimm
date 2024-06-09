from os import listdir
from os.path import isfile, join

full_path = []
training_list = []
testing_list = []

if __name__=='__main__':
  from sys import argv
  #list_file_name = argv[1]
  print(argv)
  for a in argv[1:]:
    mypath = a
    print("the file path: ", mypath)
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    #print(onlyfiles)
    full_path = []
    for f in onlyfiles:
      f_path = mypath + "/" + f
      full_path.append(f_path)
    
    training_set_size = int((len(full_path) * 2)/3)
    training_list.extend(full_path[0:training_set_size])
    testing_list.extend(full_path[training_set_size:])

  training_file_ptr = open("training_file_list.txt", "w")

  for L in training_list:
    training_file_ptr.writelines(L) 
    training_file_ptr.writelines('\n') 
  
  testing_file_ptr = open("testing_file_list.txt", "w")

  for L in testing_list:
    testing_file_ptr.writelines(L) 
    testing_file_ptr.writelines('\n') 
