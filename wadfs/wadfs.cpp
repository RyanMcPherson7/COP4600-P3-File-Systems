#include <fuse.h>
#include "../libWad/Wad.h"
#include <cstring>
#include <vector>
#include <iostream>

Wad* wad;

static int getattr_callback(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));

    // directory
    if (wad->isDirectory(path)) {
        stbuf->st_mode = S_IFDIR | 0555;
        stbuf->st_nlink = 2;
        return 0;
    }

    // file
    if (wad->isContent(path)) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = wad->getSize(path);
        return 0;
    }

    return -ENOENT;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
    return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    return wad->getContents(path, buf, size, offset);
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    // fill children names
    vector<string> childrenNames;
    wad->getDirectory(path, &childrenNames);

    for (string name : childrenNames) {
	    filler(buf, name.c_str(), NULL, 0);
    }

    return 0;
}

static struct fuse_operations fuse_example_operations = {
    .getattr = getattr_callback,
    .open = open_callback,
    .read = read_callback,
    .readdir = readdir_callback,
};

int main(int argc, char *argv[]) {
    wad = Wad::loadWad(argv[1]);
    argv[argc - 2] = argv[argc - 1];
    argv[argc - 1] = NULL;
    argc--;

    return fuse_main(argc, argv, &fuse_example_operations, NULL);
}
