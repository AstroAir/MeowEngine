import os
import sys

current_script_path = os.path.abspath(__file__)  
current_dir = os.path.dirname(current_script_path)
sys.path.append(current_dir)

from replace_util.replace import replace_headers_and_sources

def main():
    cmakelist_root = "./src/code_generator"
    header_root = "./src/runtime"
    src_root = ""
    if os.path.isdir(cmakelist_root):
        replace_headers_and_sources(cmakelist_root, header_root, src_root, header_prefix="../runtime/")
        print("Replace success.")
    else:
        print("Invalid folder path.")

if __name__ == "__main__":
    main()
