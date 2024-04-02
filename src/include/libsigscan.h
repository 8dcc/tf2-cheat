/**
 * @file   libsigscan.h
 * @brief  Single-header signature scanning library
 * @author 8dcc
 *
 * https://github.com/8dcc/libsigscan
 */

#ifndef LIBSIGSCAN_H_
#define LIBSIGSCAN_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>  /* fopen(), FILE* */
#include <stdlib.h> /* strtoull() */

/*----------------------------------------------------------------------------*/
/* Private structures */

typedef struct LibsigscanModuleBounds {
    void* start;
    void* end;
    struct LibsigscanModuleBounds* next;
} LibsigscanModuleBounds;

/*----------------------------------------------------------------------------*/
/* Private functions */

/*
 * Parse /proc/self/maps to get the start and end addresses of the specified
 * module.
 *
 * The function assumes the format of maps is always:
 *   0000DEADBEEF-0000ABADCAFE rwxp 000123AB 100:00 12345678   /path/module
 *
 * The format has to match this regex:
 *   [^\s]+-[^\s]+ [^\s]{4} [^\s]+ [^\s]+ [^\s]+\s+[^\s]*\n
 */
static LibsigscanModuleBounds* libsigscan_get_module_bounds(
  const char* module_name) {
    FILE* fd = fopen("/proc/self/maps", "r");
    if (!fd)
        return NULL;

    /* For converting to uint64_t using strtoull() */
    static char addr_buf[] = "FFFFFFFFFFFFFFFF";
    int addr_buf_pos;

    /* For the first module. Start `ret' as NULL in case no module is valid. */
    LibsigscanModuleBounds* ret = NULL;
    LibsigscanModuleBounds* cur = ret;

    int c;
    while ((c = fgetc(fd)) != EOF) {
        /* Read first address of the line */
        addr_buf_pos = 0;
        do {
            addr_buf[addr_buf_pos++] = c;
        } while ((c = fgetc(fd)) != '-');
        addr_buf[addr_buf_pos] = '\0';

#ifdef __i386__
        void* start_addr = (void*)strtoul(addr_buf, NULL, 16);
#else
        void* start_addr = (void*)strtoull(addr_buf, NULL, 16);
#endif

        /* Read second address of the line */
        addr_buf_pos = 0;
        while ((c = fgetc(fd)) != ' ')
            addr_buf[addr_buf_pos++] = c;
        addr_buf[addr_buf_pos] = '\0';

#ifdef __i386__
        void* end_addr = (void*)strtoul(addr_buf, NULL, 16);
#else
        void* end_addr   = (void*)strtoull(addr_buf, NULL, 16);
#endif

        /* Parse "rwxp". For now we only care about read permissions. */
        bool is_readable = ((c = fgetc(fd)) == 'r');

        /* Skip permissions and single space */
        while ((c = fgetc(fd)) != ' ')
            ;

        /* Skip 3rd column and single space */
        while ((c = fgetc(fd)) != ' ')
            ;

        /* Skip 4th column and single space */
        while ((c = fgetc(fd)) != ' ')
            ;

        /* Skip 5th column */
        while ((c = fgetc(fd)) != ' ')
            ;

        /* Skip spacing until the module name. First char of module name
         * will be saved in `c' after this loop. */
        while ((c = fgetc(fd)) == ' ')
            ;

        bool name_matches = true;
        if (module_name == NULL) {
            /* We don't want to filter the module name, just make sure it
             * doesn't start with '[' and skip to the end of the line. */
            if (c == '[')
                name_matches = false;

            while (c != '\n' && c != EOF)
                c = fgetc(fd);
        } else {
            /* Compare module name. Note that the output of maps has absolute
             * paths. */
            int i = 0;
            do {
                /* A character did not match the module name, ignore this line.
                 * We can't break out of the `for' because we have to get to the
                 * newline anyway. */
                if (name_matches && module_name[i] != '\0') {
                    if (c != module_name[i])
                        name_matches = false;

                    i++;
                }

                /* This check is needed so we don't skip over the '\n' on lines
                 * with no module name. */
                if (c != '\n')
                    c = fgetc(fd);
            } while (c != '\n' && c != EOF);
        }

        /* We can read it, and it's the module we are looking for. */
        if (is_readable && name_matches) {
            if (cur == NULL) {
                /* Allocate the first bounds struct */
                cur = malloc(sizeof(LibsigscanModuleBounds));

                /* This one will be returned */
                ret = cur;

                /* Save the addresses from this line of maps */
                cur->start = start_addr;
                cur->end   = end_addr;
            } else if (cur->end == start_addr && cur->end < end_addr) {
                /* If the end address of the last struct is the start of this
                 * one, just merge them. */
                cur->end = end_addr;
            } else {
                /* There was a gap between the end of the last block and the
                 * start of this one, allocate new struct. */
                cur->next = malloc(sizeof(LibsigscanModuleBounds));

                /* Set as current */
                cur = cur->next;

                /* Save the addresses from this line of maps */
                cur->start = start_addr;
                cur->end   = end_addr;
            }

            /* Indicate the end of the linked list */
            cur->next = NULL;
        }
    }

    fclose(fd);
    return ret;
}

/* Free a linked list of ModuleBounds structures */
static void libsigscan_free_module_bounds(LibsigscanModuleBounds* bounds) {
    LibsigscanModuleBounds* cur = bounds;
    while (cur != NULL) {
        LibsigscanModuleBounds* next = cur->next;
        free(cur);
        cur = next;
    }
}

#ifdef LIBSIGSCAN_DEBUG
/* Print a linked list of ModuleBounds structures */
static void libsigscan_print_module_bounds(LibsigscanModuleBounds* bounds) {
    printf("[DEBUG] List of module bounds:\n");

    if (!bounds) {
        printf("(No module bounds)");
        return;
    }

    int i = 0;
    for (LibsigscanModuleBounds* cur = bounds; cur != NULL;
         cur                         = cur->next, i++)
        printf("[%02d] %p - %p\n", i, cur->start, cur->end);
    putchar('\n');
}
#endif

/* Used for getting the bytes from IDA patterns.
 * Converts: "E0" -> 224 */
static uint8_t libsigscan_hex_to_byte(const char* hex) {
    int ret = 0;

    /* Skip leading spaces, if any */
    while (*hex == ' ')
        hex++;

    /* Store a byte (two digits of string) */
    for (int i = 0; i < 2 && hex[i] != '\0'; i++) {
        char c = hex[i];

        /* For example "E ", although the format should always be "0E" */
        if (c == ' ')
            break;

        uint8_t n = 0;
        if (c >= '0' && c <= '9')
            n = c - '0';
        else if (c >= 'a' && c <= 'f')
            n = 10 + c - 'a';
        else if (c >= 'A' && c <= 'F')
            n = 10 + c - 'A';

        /* Shift size of 0xF and add the next half of byte */
        ret <<= 4;
        ret |= n & 0xF;
    }

    return ret & 0xFF;
}

/* Search for `pattern' from `start' to `end'. */
static void* libsigscan_do_scan(void* start, void* end, const char* pattern) {
    if (!start || !end)
        return NULL;

    /* Skip preceding spaces from pattern, if any */
    while (*pattern == ' ')
        pattern++;

    /* Current position in memory and current position in pattern */
    uint8_t* mem_ptr    = start;
    const char* pat_ptr = pattern;

    /* Iterate until we reach the end of the memory or the end of the pattern */
    while ((void*)mem_ptr < end && *pat_ptr != '\0') {
        /* Wildcard, always match */
        if (*pat_ptr == '?') {
            mem_ptr++;

            /* "A1 ?? ?? B2" -> "A1 ? ? B2" */
            while (*pat_ptr == '?')
                pat_ptr++;

            /* Remove trailing spaces after '?'
             * NOTE: I reused this code, but you could use `goto` */
            while (*pat_ptr == ' ')
                pat_ptr++;

            continue;
        }

        /* Convert "E0" into 224.
         * TODO: Would be better to only do this once at the start of the
         * function with some kind of ida2bytes function (We would need a mask
         * for the '?' vs. 0x3F). */
        uint8_t cur_byte = libsigscan_hex_to_byte(pat_ptr);

        if (*mem_ptr == cur_byte) {
            /* Found exact byte match in sequence, go to next byte in memory */
            mem_ptr++;

            /* Go to next byte separator in pattern (space) */
            while (*pat_ptr != ' ' && *pat_ptr != '\0')
                pat_ptr++;
        } else {
            /* Byte didn't match, check pattern from the begining on the next
             * position in memory */
            start++;
            mem_ptr = start;
            pat_ptr = pattern;
        }

        /* Skip trailing spaces */
        while (*pat_ptr == ' ')
            pat_ptr++;
    }

    /* If we reached end of pattern, return the match. Otherwise, NULL */
    return (*pat_ptr == '\0') ? start : NULL;
}

/*----------------------------------------------------------------------------*/
/* Public functions */

/* Search for `ida_pattern' in the specified `module'. */
static void* sigscan_module(const char* module, const char* ida_pattern) {
    /* Get a linked list of ModuleBounds, containing the start and end addresses
     * of all the regions that match `module'. */
    LibsigscanModuleBounds* bounds = libsigscan_get_module_bounds(module);

#ifdef LIBSIGSCAN_DEBUG
    libsigscan_print_module_bounds(bounds);
#endif

    /* Iterate them, and scan each one until we find a match. */
    void* ret = NULL;
    for (LibsigscanModuleBounds* cur = bounds; cur != NULL; cur = cur->next) {
        void* cur_result =
          libsigscan_do_scan(cur->start, cur->end, ida_pattern);

        if (cur_result != NULL) {
            ret = cur_result;
            break;
        }
    }

    /* Free the ModuleBounds linked list */
    libsigscan_free_module_bounds(bounds);

    return ret;
}

/* Search for `ida_pattern' in all the loaded modules. */
static inline void* sigscan(const char* ida_pattern) {
    return sigscan_module(NULL, ida_pattern);
}

#endif /* LIBSIGSCAN_H_ */
