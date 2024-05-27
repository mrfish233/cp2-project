#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE 4096

/**
 * Open a file.
 * @param file the file stream pointer.
 * @param fileName the file name.
 * @param mode the open mode.
 * @return 0 if the file is opened successfully, 1 otherwise.
 */
int32_t openFile(FILE **file, char *fileName, char *mode);

/**
 * Read a line from a file stream.
 * @param file the file stream pointer
 * @param buffer the buffer to store the line
 * @param size the buffer size
 * @return 0 if the line is read successfully, 1 if end of file is reached, 2 otherwise.
 */
int32_t readLine(FILE *file, char *buffer, size_t size);

/**
 * Close a file.
 * @param file the file stream pointer.
 */
void closeFile(FILE *file);
