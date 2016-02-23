#include <stdio.h>
#include <stdbool.h>
#include <libpq-fe.h>
#include <sys/time.h>

// #include <datatype/timestamp.h>

struct log_s {
    PGconn *conn;
};
typedef struct log_s log_t;

struct output_s {
    struct timeval time;
    char message[1024];
};
typedef struct output_s output_t;

log_t * log_init();
bool log_write(log_t *, const char *);
bool log_last_messages(log_t *, output_t *, uint32_t *);
bool log_free(log_t *);
