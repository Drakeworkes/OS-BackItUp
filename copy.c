#include "copy.h"

int copy(const char *src, const char *dest) {
    int in, out;
    // Open source file
    if ((in = open(src, O_RDONLY)) < 0){
        fprintf(stderr, "Unable to read source file %s", src);
        return -1;
    }

    // if destination file exists, open it. If not, then create it
    if (access(dest, F_OK) != -1) {
        if ((out = open(dest, 0660)) < 0){
                fprintf(stderr, "Unable to open dest file %s for writing", dest);
                return -1;
            }

    } else {
        if ((out = creat(dest, 0660)) < 0) {
            fprintf(stderr, "Unable to create destination file %s", dest);
            close(in);
            return -1;
        }
    }

    // get size of source file and send to destination
    off_t bytesCopied = 0;
    struct stat fileinfo = {0};
    if (fstat(in, &fileinfo) < 0) {
        fprintf(stderr, "Unable to read size of file %s", dest);
        close(in);
        close(out);
        return -1;
    }
    int result = sendfile(out, in, &bytesCopied, fileinfo.st_size);
    printf("Copied %ld bytes from %s to %s", fileinfo.st_size, src, dest);

    close(in);
    close(out);

    return result;
}



void *backup(void *args){

    Copy_args_t *file = (Copy_args_t *)args;

    char file_path[100];
    char backup_path[100];
    char backup_file_name[100];

    strcpy(file_path, file->path);
    strcat(file_path, "/");
    char *src = strcat(file_path, file->file_name);

    strcpy(backup_path, file->path);
    strcat(backup_path, "/.backup/");

    strcpy(backup_file_name, file->file_name);
    strcat(backup_file_name, ".bak");

    char *dest = strcat(backup_path, backup_file_name);

    struct stat src_stat, dest_stat;
    printf("Backing up %s", file->file_name);

    // if destination already exists, only copy if source is more recent
    if (access(dest, F_OK) != -1) {

        if (stat(src, &src_stat) != 0) {
            fprintf(stderr, "Unable to find stats for %p", src);
            exit(1);
        }
        if (stat(dest, &dest_stat) != 0) {
            fprintf(stderr, "Unable to find stats for %p", dest);
            exit(1);
        }
        
        if (src_stat.st_mtime > dest_stat.st_mtime) {
            printf("WARNING: Overwriting %s", backup_file_name);
            if (copy(src, dest) < 0) {
                fprintf(stderr, "unable to copy %p into backup %p", src, dest);
                exit(1);
            } 
        } else { 
            printf("%s does not need backing up", backup_file_name);
        }

    } else {
        
        if (copy(src, dest) < 0) {
            fprintf(stderr, "unable to copy %p into backup %p", src, dest);
            exit(1);
        } 
    }

    exit(0);
}
void *restore(void *args) {

    Copy_args_t *file = (Copy_args_t *)args;

    char file_path[100];
    char file_name[100];
    char backup_path[100];
    
    // Get destination path
    size_t path_len = strlen(file->path);
    strncpy(file_path, file->path, path_len - 9);
    
    size_t name_len = strlen(file->file_name);
    strncpy(file_name, file->file_name, name_len - 4);
    
    strcat(file_path, "/");
    char *dest = strcat(file_path, file_name);

    // Get source path
    strcpy(backup_path, file->path);
    strcat(backup_path, "/");
    char *src = strcat(backup_path, file->file_name);

    struct stat src_stat, dest_stat;
    printf("Restoring %s", file_name);
    
    // if destination already exists, only copy if source is more recent
    if (access(dest, F_OK) != -1) {

        if (stat(src, &src_stat) != 0) {
            fprintf(stderr, "Unable to find stats for %p", src);
            exit(1);
        }
        if (stat(dest, &dest_stat) != 0) {
            fprintf(stderr, "Unable to find stats for %p", dest);
            exit(1);
        }
        
        if (src_stat.st_mtime > dest_stat.st_mtime) {
            printf("WARNING: Overwriting %s", file_name);
            if (copy(src, dest) < 0) {
                fprintf(stderr, "unable to copy %p from backup %p", dest, src);
                exit(1);
            } 
        } else { 
            printf("%s is already the most current version", file_name);
        }

    } else {
        
        if (copy(src, dest) < 0) {
            fprintf(stderr, "unable to copy %p from backup %p", dest, src);
            exit(1);
        } 
    }

    exit(0);
}