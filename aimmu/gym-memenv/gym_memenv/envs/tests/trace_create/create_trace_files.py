from fsplit.filesplit import Filesplit

import argparse


def split_cb(f, s):
      print("file: {0}, size: {1}".format(f, s))


def configure():

  parser = argparse.ArgumentParser()
  
  parser.add_argument('--source-file-path', default='na', 
      help='Please provide the source file, default = na')
  
  parser.add_argument('--out-dir-path', default='na', 
      help='Please provide the output directory path, default = na')

  args = parser.parse_args()
  print("source file path: ", args.source_file_path)
  print("output folder path: ", args.out_dir_path)
  return args

def main():
  args = configure()
  fs = Filesplit()
  fs.split(file=args.source_file_path, split_size=900000, output_dir=args.out_dir_path, callback=split_cb)


if __name__ == '__main__':
  main()

