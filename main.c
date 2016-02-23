#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "log.h"

void exit_nicely(PGconn *conn) {

    PQfinish(conn);
    exit(1);
}

int main() {
    log_t * l;
    output_t messages[10];
    uint32_t nmessages;
    uint32_t i;
    struct tm * info;
    char buf[1024];

    l = log_init();
    log_write(l, "Hello Wolrd");
    // Option 3
    nmessages = 10;
    log_last_messages(l, messages, &nmessages);
    for (i = 0; i < nmessages; i++) {
        info = localtime( &messages[i].time.tv_sec );
        strftime(buf,1024,"%x - %I:%M%p", info);
        printf("Message %d : %s %s\n", i,buf, messages[i].message);
    }

    log_free(l);
    return 0;
}
