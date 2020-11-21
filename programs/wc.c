#include <ctype.h>
#include <stdio.h>
#include "binary_tree.h"
#include "check.h"
#include "log.c"
#include "map.h"
#include "str.h"
#include "type_interface.h"

str *scan_word(FILE *stream)
{
    char buf[128];
    char c;
    int i = 0;

    c = fgetc(stream);
    while (!feof(stream) && (ispunct(c) || isspace(c))) c = fgetc(stream);
    if (feof(stream)) return NULL;

    buf[i++] = c;
    while (!feof(stream)) {
        c = fgetc(stream);
        if (ispunct(c) || isspace(c)) break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    str *s = str_from_cstr(buf);

    return s;
}

int print_wc(btn *n, void *m)
{
    map *mp = m;
    str *word = btn_key(mp, n);
    int *count = btn_value(mp, n);
    printf("%s: %d\n", str_data(word), *count);
    return 0;
}

int main(int argc, char *argv[])
{
    log_files[0].stream=stderr;
    log_files[0].use_ansi_styles=1;
    log_files[0].suppress_debug_messages=0;
    log_files[0].suppress_call_logs=1;

    if (argc < 2) printf("%s\n", "USAGE: wc [FILE]");
    const char *fp = argv[1];
    FILE *f = fopen(fp, "r");
    check(f != NULL, "failed to open file");

    str *word;
    int *count;
    map *m = map_new(&str_type, &int_type);
    int one = 1;

    while (!feof(f)) {
        word = scan_word(f);
        if (word == NULL) break;
        if (map_has(m, word)) {
            count = map_get(m, word);
            ++(*count);
        } else {
            map_set(m, word, &one);
        }
        str_delete(word);
    }

    bt_traverse_nodes(m, print_wc, m);
    printf("There are %u unique words.\n", map_count(m));
    map_delete(m);
    fclose(f);

    return 0;
error:
    return -1;
}
