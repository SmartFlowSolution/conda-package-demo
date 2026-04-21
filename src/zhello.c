#include "libzhello.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

static void print_help(const char *program_name) {
    printf("Usage: %s [options] [text]\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  --file PATH            Read input from a file.\n");
    printf("  --save-compressed PATH Save compressed bytes to a file.\n");
    printf("  --level N              Compression level from 0 to 9.\n");
    printf("  --repeat N             Run the compression N times for a tiny benchmark.\n");
    printf("  --json                 Print machine-readable JSON output.\n");
    printf("  --help                 Show this help.\n");
}

static long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((long long)ts.tv_sec * 1000000000LL) + ts.tv_nsec;
}

int main(int argc, char **argv) {
    const char *input_text = NULL;
    const char *input_file = NULL;
    const char *save_compressed_path = NULL;
    int level = 6;
    int repeat = 1;
    int json = 0;
    int argi;
    zhello_buffer input = {0};
    zhello_buffer compressed = {0};
    zhello_buffer restored = {0};
    zhello_stats stats = {0};
    long long started_ns = 0;
    long long ended_ns = 0;
    int rc;

    for (argi = 1; argi < argc; ++argi) {
        if (strcmp(argv[argi], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        }
        if (strcmp(argv[argi], "--json") == 0) {
            json = 1;
            continue;
        }
        if (strcmp(argv[argi], "--file") == 0 && argi + 1 < argc) {
            input_file = argv[++argi];
            continue;
        }
        if (strcmp(argv[argi], "--save-compressed") == 0 && argi + 1 < argc) {
            save_compressed_path = argv[++argi];
            continue;
        }
        if (strcmp(argv[argi], "--level") == 0 && argi + 1 < argc) {
            level = atoi(argv[++argi]);
            continue;
        }
        if (strcmp(argv[argi], "--repeat") == 0 && argi + 1 < argc) {
            repeat = atoi(argv[++argi]);
            continue;
        }
        input_text = argv[argi];
    }

    if (repeat < 1 || level < 0 || level > 9) {
        fprintf(stderr, "Invalid arguments: level must be 0..9 and repeat must be >= 1\n");
        return 2;
    }

    if (input_file) {
        rc = zhello_read_file(input_file, &input);
        if (rc != 0) {
            fprintf(stderr, "Could not read '%s': %s\n", input_file, zhello_error_string(rc));
            return 1;
        }
    } else {
        if (!input_text) {
            input_text = "Hello from zhello";
        }
        input.size = strlen(input_text);
        input.data = (unsigned char *)malloc(input.size + 1);
        if (!input.data) {
            fprintf(stderr, "memory allocation failed\n");
            return 1;
        }
        memcpy(input.data, input_text, input.size + 1);
    }

    started_ns = now_ns();
    for (argi = 0; argi < repeat; ++argi) {
        zhello_free_buffer(&compressed);
        rc = zhello_compress_text(input.data, input.size, level, &compressed);
        if (rc != 0) {
            fprintf(stderr, "Compression failed: %s\n", zhello_error_string(rc));
            zhello_free_buffer(&input);
            return 1;
        }
    }
    ended_ns = now_ns();

    rc = zhello_decompress_text(compressed.data, compressed.size, input.size, &restored);
    if (rc != 0) {
        fprintf(stderr, "Decompression failed: %s\n", zhello_error_string(rc));
        zhello_free_buffer(&input);
        zhello_free_buffer(&compressed);
        return 1;
    }

    zhello_fill_stats(input.size, compressed.size, &stats);

    if (save_compressed_path) {
        rc = zhello_write_file(save_compressed_path, compressed.data, compressed.size);
        if (rc != 0) {
            fprintf(stderr, "Could not write '%s': %s\n", save_compressed_path, zhello_error_string(rc));
            zhello_free_buffer(&input);
            zhello_free_buffer(&compressed);
            zhello_free_buffer(&restored);
            return 1;
        }
    }

    if (json) {
        printf("{\n");
        printf("  \"input_size\": %zu,\n", stats.input_size);
        printf("  \"compressed_size\": %zu,\n", stats.compressed_size);
        printf("  \"ratio\": %.6f,\n", stats.ratio);
        printf("  \"repeat\": %d,\n", repeat);
        printf("  \"elapsed_ms\": %.3f,\n", (double)(ended_ns - started_ns) / 1000000.0);
        printf("  \"restored\": \"%s\"\n", restored.data);
        printf("}\n");
    } else {
        printf("input_size: %zu\n", stats.input_size);
        printf("compressed_size: %zu\n", stats.compressed_size);
        printf("ratio: %.6f\n", stats.ratio);
        printf("repeat: %d\n", repeat);
        printf("elapsed_ms: %.3f\n", (double)(ended_ns - started_ns) / 1000000.0);
        printf("restored: %s\n", restored.data);
        if (save_compressed_path) {
            printf("saved_compressed: %s\n", save_compressed_path);
        }
    }

    zhello_free_buffer(&input);
    zhello_free_buffer(&compressed);
    zhello_free_buffer(&restored);
    return 0;
}
