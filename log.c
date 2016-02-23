#include <stdlib.h>
#include <string.h>
// #include <endian.h>
#define POSTGRES_EPOCH_DATE 946684800

#include "log.h"

log_t * log_init()
{
    log_t * handle;
    handle = calloc(1,sizeof(log_t));
    handle->conn = PQconnectdb("postgresql://zaid@localhost/zaid");

    if (PQstatus(handle->conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(handle->conn));
        PQfinish(handle->conn);
        free(handle);
        return NULL;
    }

    return handle;
}

bool log_free(log_t * handle)
{
    PQfinish(handle->conn);
    free(handle);
    return true;
}

bool log_write(log_t * handle, const char * message)
{
    PGresult   *res;
    const char *paramValues[1];

    paramValues[0] = message;
    res = PQexecParams(handle->conn,
                       "INSERT INTO logs (message) VALUES ($1)",
                       1,       /* one param */
                       NULL,    /* let the backend deduce param type */
                       paramValues,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       1);      /* ask for binary results */

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Insert Failed : %s", PQerrorMessage(handle->conn));
        PQclear(res);
        return false;
    }
    return true;
}

bool log_last_messages(log_t * handle, output_t * messages, uint32_t * size)
{
    PGresult   *res;
    char * command;
    uint16_t i, nTuples;
    uint16_t mfnumber, tfnumber;
    void * p;
    int64_t f;

    command = "SELECT time, message FROM logs ORDER BY time DESC LIMIT 10";
    res = PQexecParams(handle->conn, command, 0, NULL, NULL, NULL, NULL, 1);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SELECT FAILD: %s", PQerrorMessage(handle->conn));
        PQclear(res);
        return false;
    }

    mfnumber = PQfnumber(res,"message");
    tfnumber = PQfnumber(res,"time");

    nTuples = PQntuples(res);
    for (i =  0; i < nTuples && i < *size; i++)
    {
        p = PQgetvalue(res, i, tfnumber);
        f = ntohll(* ((int64_t *) p));
        messages[i].time.tv_sec = POSTGRES_EPOCH_DATE + f / 1000000;
        messages[i].time.tv_usec = f % 1000000;
        strncpy(messages[i].message, PQgetvalue(res, i, mfnumber), 1024);
    }
    *size = i;
    return true;
}
