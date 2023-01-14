import os
import sys


def create_dir_if_not_exists(path):

    folders = path.split("/")
    next_folder = folders[0]

    try:
        os.mkdir(next_folder)
    except FileExistsError:
        pass

    for folder in folders[1:]:
        next_folder = f"{next_folder}/{folder}"
        try:
            os.mkdir(next_folder)
        except FileExistsError:
            pass


create_dir_if_not_exists(sys.argv[0])