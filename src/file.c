#include "file.h"
#include <stdio.h>
#include <stdlib.h>

char *file_read(const char *path){
    //Open file
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "COULD NOT OPEN FILE %s", path);
        return NULL;
    }

    //Seek to end of file
    fseek(fp, 0, SEEK_END);
    if(fseek(fp, 0, SEEK_END) != 0){
        fprintf(stderr, "COULD NOT SEEK TO END OF FILE %s\n", path);
        fclose(fp);
        return NULL;
    }

    //Determine file size
    long size = ftell(fp);
    if(size < 0){
        fprintf(stderr, "COULD NOT TELL FILE SIZE FOR %s\n", path);
        fclose(fp);
        return NULL;
    }
    rewind(fp); //return to beginning

    //Allocate buffer
    char *buffer = (char *)malloc(size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "COULD NOT ALLOCATE %ld BYTES FOR %s", size, path);
        fclose(fp);
        return NULL;
    }

    size_t read_bytes = fread(buffer, sizeof(char), size, fp);
    if (read_bytes != (size_t)size) {
        fprintf(stderr, "COULD NOT FINISH READING FILE %s; GOT THROUGH %lld BYTES", path, read_bytes);
        fclose(fp);
        free(buffer);
        return NULL;
    }
    buffer[size] = '\0';

    fclose(fp);
    return buffer;
}

void file_print(const char *file_contents){
    if(file_contents == NULL){
        fprintf(stderr, "FILE CONTENTS POINTER IS NULL!");
        return;
    }
    else {
        printf("--------FILE--------\n");
        printf(" %s\n", file_contents);
        printf("--------------------\n");
    }
    return;
}

void file_free(char *file_contents){
    if(file_contents == NULL){
        fprintf(stderr, "FILE CONTENTS POINTER IS NULL!");
        return;
    }
    else {
        free(file_contents);
    }
    return;
}