#ifndef COPY_H
#define COPY_H

// This file needs to take a file and copy it to the existing .backup folder
// in the current directory

typedef struct args {
    char *path;
    char *file_name;
} copy_args;

#endif
