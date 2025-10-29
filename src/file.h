#ifndef FILE_H
#define FILE_H

char *file_read(const char *path);
void file_print(const char *file_contents);
void file_free(char *file_contents);

#endif // FILE_H