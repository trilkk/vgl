#!/usr/bin/env python3

import filecmp
import os
import pathlib
import shutil
import sys

g_ignore_names = ('dnload.h', '.gitignore')
g_source_directory_names = ('source', 'sources', 'src')
g_source_file_suffixes = ('.c', '.cmake', '.cpp', '.h', '.hpp', '.md', '.py', '.rst', '.sh')
g_other_source_files = ('cmakelists.txt', 'licence', 'readme')

def require_regular_file(pth: pathlib.Path):
    """Requires given file to be a regular file."""
    if not pth.exists():
        raise RuntimeError("'%s' does not exist" % (str(pth)))
    if not pth.is_file():
        raise RuntimeError("'%s' is not a regular file" % (str(pth)))

def copy_if_diff(src_path: pathlib.Path, dst_path: pathlib.Path, require_dst: bool = False):
    require_regular_file(src_path)
    if require_dst:
        require_regular_file(dst_path)
    elif dst_path.exists():
        if not dst_path.is_file():
            raise RuntimeError("'%s' is not a regular file" % (str(dst_path)))
    if (not dst_path.exists()) or (not filecmp.cmp(src_path, dst_path)):
        print("'%s' does not match '%s', replacing." % (str(dst_path), str(src_path)))
        shutil.copyfile(src_path, dst_path)

def find_parent_dir(curr_path: pathlib.Path, lookup: str):
    """Looks for a given directory, walking up."""
    while True:
        test_path = curr_path / lookup
        if test_path.is_dir():
            return test_path
        next_path = curr_path.parent
        if next_path == curr_path:
            return None
        curr_path = next_path

def find_child_dir(curr_path: pathlib.Path, lookup: str):
    """Looks for a given directory, walking down."""
    test_path = curr_path / lookup
    if test_path.is_dir():
        return test_path
    for ii in curr_path.iterdir():
        if ii.is_dir():
            found_path = find_child_dir(ii, lookup)
            if found_path:
                return found_path
    return None

def find_src_dir(curr_path: pathlib.Path, lookup: str):
    """Descends the tree from given location, looking for a named source directory."""
    test_path = curr_path / lookup
    if test_path.is_dir():
        return test_path
    for ii in curr_path.iterdir():
        if ii.is_dir() and (ii.name.lower() in g_source_directory_names):
            found_path = find_child_dir(ii, lookup)
            if found_path:
                return found_path
    return None

def is_source_file(pth: pathlib.Path):
    """Determines if a file is classified as a source file."""
    if pth.suffix in g_source_file_suffixes:
        return True
    return pth.name.lower() in g_other_source_files

def mirror_src_dir_shallow(src_path: pathlib.Path, dst_path: pathlib.Path, ignore_names = []):
    """Performs a shallow mirror of a source directory."""
    src_lst = list(src_path.iterdir())
    dst_lst = list(dst_path.iterdir())
    unknown_lst = []
    for ii in src_lst:
        if (ii.name.lower() not in ignore_names) and is_source_file(ii):
            copy_if_diff(ii, dst_path / ii.name)
    for ii in dst_lst:
        if (ii.name.lower() not in ignore_names):
            if is_source_file(ii):
                if (src_path / ii.name) not in src_lst:
                    print("'%s' not in source path, removing." % (ii.name))
                    os.unlink(ii)
            else:
                unknown_lst += [ii]
    if unknown_lst:
        print("WARNING: unknown files in destination directory: %s" % (str(list(map(lambda x: x.name, unknown_lst)))))

if __name__ == '__main__':
    lookup_dir = 'vgl'
    script_path = pathlib.Path(sys.argv[0]).resolve()

    src_path = find_parent_dir(script_path.parent.parent, lookup_dir)
    if not src_path:
        raise RuntimeError("could not find '%s' looking from '%s'" % (lookup_dir, str(script_path.parent.parent)))

    dst_path = find_src_dir(script_path.parent, lookup_dir)
    if not dst_path:
        raise RuntimeError("could not find source directory '%s' looking from '%s'" % (lookup_dir, str(script_path.parent)))

    src_script_path = src_path / script_path.name
    copy_if_diff(src_script_path, script_path, True)

    print("Mirroring source directory '%s' into '%s':" % (str(src_path), str(dst_path)))
    mirror_src_dir_shallow(src_path, dst_path, g_ignore_names + (script_path.name,))

    sys.exit(0)
