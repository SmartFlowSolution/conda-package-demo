#include "libzhello.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

enum {
    ZHELLO_OK = 0,
    ZHELLO_ERR_INVALID_ARGUMENT = 1,
    ZHELLO_ERR_IO = 2,
    ZHELLO_ERR_ALLOC = 3,
    ZHELLO_ERR_COMPRESS = 4,
    ZHELLO_ERR_DECOMPRESS = 5
};

static int allocate_buffer(size_t size, zhello_buffer *buffer) {
    buffer->data = NULL;
    buffer->size = 0;

    if (!buffer) {
        return ZHELLO_ERR_INVALID_ARGUMENT;
    }

    if (size == 0) {
        size = 1;
    }

    buffer->data = (unsigned char *)malloc(size);
    if (!buffer->data) {
        return ZHELLO_ERR_ALLOC;
    }

    buffer->size = size;
    return ZHELLO_OK;
}

int zhello_compress_text(const unsigned char *input, size_t input_size, int level, zhello_buffer *output) {
    uLongf bound;
    uLongf compressed_size;
    int rc;

    if (!input || !output || level < Z_NO_COMPRESSION || level > Z_BEST_COMPRESSION) {
        return ZHELLO_ERR_INVALID_ARGUMENT;
    }

    bound = compressBound((uLong)input_size);
    rc = allocate_buffer((size_t)bound, output);
    if (rc != ZHELLO_OK) {
        return rc;
    }

    compressed_size = bound;
    rc = compress2(output->data, &compressed_size, input, (uLong)input_size, level);
    if (rc != Z_OK) {
        zhello_free_buffer(output);
        return ZHELLO_ERR_COMPRESS;
    }

    output->size = (size_t)compressed_size;
    return ZHELLO_OK;
}

int zhello_decompress_text(const unsigned char *input, size_t input_size, size_t expected_size, zhello_buffer *output) {
    uLongf restored_size = (uLongf)expected_size;
    int rc;

    if (!input || !output) {
        return ZHELLO_ERR_INVALID_ARGUMENT;
    }

    rc = allocate_buffer(expected_size + 1, output);
    if (rc != ZHELLO_OK) {
        return rc;
    }

    rc = uncompress(output->data, &restored_size, input, (uLong)input_size);
    if (rc != Z_OK) {
        zhello_free_buffer(output);
        return ZHELLO_ERR_DECOMPRESS;
    }

    output->data[restored_size] = '\0';
    output->size = (size_t)restored_size;
    return ZHELLO_OK;
}

int zhello_read_file(const char *path, zhello_buffer *output) {
    FILE *fp;
    long file_size;
    size_t bytes_read;
    int rc;

    if (!path || !output) {
        return ZHELLO_ERR_INVALID_ARGUMENT;
    }

    fp = fopen(path, "rb");
    if (!fp) {
        return ZHELLO_ERR_IO;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return ZHELLO_ERR_IO;
    }

    file_size = ftell(fp);
    if (file_size < 0) {
        fclose(fp);
        return ZHELLO_ERR_IO;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return ZHELLO_ERR_IO;
    }

    rc = allocate_buffer((size_t)file_size + 1, output);
    if (rc != ZHELLO_OK) {
        fclose(fp);
        return rc;
    }

    bytes_read = fread(output->data, 1, (size_t)file_size, fp);
    fclose(fp);

    if (bytes_read != (size_t)file_size) {
        zhello_free_buffer(output);
        return ZHELLO_ERR_IO;
    }

    output->data[bytes_read] = '\0';
    output->size = bytes_read;
    return ZHELLO_OK;
}

int zhello_write_file(const char *path, const unsigned char *data, size_t size) {
    FILE *fp;
    size_t bytes_written;

    if (!path || (!data && size > 0)) {
        return ZHELLO_ERR_INVALID_ARGUMENT;
    }

    fp = fopen(path, "wb");
    if (!fp) {
        return ZHELLO_ERR_IO;
    }

    bytes_written = fwrite(data, 1, size, fp);
    fclose(fp);

    return bytes_written == size ? ZHELLO_OK : ZHELLO_ERR_IO;
}

void zhello_fill_stats(size_t input_size, size_t compressed_size, zhello_stats *stats) {
    if (!stats) {
        return;
    }

    stats->input_size = input_size;
    stats->compressed_size = compressed_size;
    stats->ratio = input_size == 0 ? 0.0 : (double)compressed_size / (double)input_size;
}

void zhello_free_buffer(zhello_buffer *buffer) {
    if (!buffer) {
        return;
    }

    free(buffer->data);
    buffer->data = NULL;
    buffer->size = 0;
}

const char *zhello_error_string(int code) {
    switch (code) {
        case ZHELLO_OK:
            return "ok";
        case ZHELLO_ERR_INVALID_ARGUMENT:
            return "invalid argument";
        case ZHELLO_ERR_IO:
            return "i/o error";
        case ZHELLO_ERR_ALLOC:
            return "memory allocation failed";
        case ZHELLO_ERR_COMPRESS:
            return "compression failed";
        case ZHELLO_ERR_DECOMPRESS:
            return "decompression failed";
        default:
            return "unknown error";
    }
}
