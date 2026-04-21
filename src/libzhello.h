#ifndef ZHELLO_LIBZHELLO_H
#define ZHELLO_LIBZHELLO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char *data;
    size_t size;
} zhello_buffer;

typedef struct {
    size_t input_size;
    size_t compressed_size;
    double ratio;
} zhello_stats;

int zhello_compress_text(const unsigned char *input, size_t input_size, int level, zhello_buffer *output);
int zhello_decompress_text(const unsigned char *input, size_t input_size, size_t expected_size, zhello_buffer *output);
int zhello_read_file(const char *path, zhello_buffer *output);
int zhello_write_file(const char *path, const unsigned char *data, size_t size);
void zhello_fill_stats(size_t input_size, size_t compressed_size, zhello_stats *stats);
void zhello_free_buffer(zhello_buffer *buffer);
const char *zhello_error_string(int code);

#ifdef __cplusplus
}
#endif

#endif
