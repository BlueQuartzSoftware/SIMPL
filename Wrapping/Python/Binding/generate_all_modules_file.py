import argparse

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('module_list_file')
  parser.add_argument('output_dir')
  
  args = parser.parse_args()

  with open(args.module_list_file, 'r') as module_list_file, open(f'{args.output_dir}/dream3d.py', 'w') as d3d_file:
    lines = module_list_file.readlines()
    for line in lines:
      line = line.strip()
      d3d_file.write(f'import {line}py\n')
