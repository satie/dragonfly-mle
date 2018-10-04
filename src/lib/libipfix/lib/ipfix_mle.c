/*
**     ipfix_mle.c - IPFIX message stream to Dragonfly MLE
**
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <zlib.h>

#include "mlog.h"
#include "ipfix.h"
#include "ipfix_col.h"

/*----- globals ----------------------------------------------------------*/

static ipfix_col_info_t *g_colinfo = NULL;

static int ipfix_mle_newsource(ipfixs_node_t *s, void *arg)
{
    FILE *fp = (FILE *)arg;
    fprintf(fp, "{ \"observation-domain\": %lu }\n", (u_long)s->odid);
    return 0;
}

static int ipfix_mle_newmsg(ipfixs_node_t *s, ipfix_hdr_t *hdr, void *arg)
{
    FILE *fp = (FILE *)arg;
    /* print header
     */
    fprintf(fp, "{ \"ipfix-header\": { version=%u,", hdr->version);
    if (hdr->version == IPFIX_VERSION_NF9)
    {
        fprintf(fp, " \"records\":%u, ", hdr->u.nf9.count);
        fprintf(fp, " \"sysuptime\":%.3fs, \"unixtime\"=%lu, ",
             (double)(hdr->u.nf9.sysuptime) / 1000.0,
             (u_long)hdr->u.nf9.unixtime);
        fprintf(fp, "\"seqno\":%lu, ", (u_long)hdr->seqno);
        fprintf(fp, "\"sourceid\":%lu", (u_long)hdr->sourceid);
    }
    else
    {
        fprintf(fp, "\"length\":%u, ", hdr->u.ipfix.length);
        fprintf(fp, "\"unixtime\":%lu, ", (u_long)hdr->u.ipfix.exporttime);
        fprintf(fp, "\"seqno\":%lu,", (u_long)hdr->seqno);
        fprintf(fp, "\"odid\":%lu", (u_long)hdr->sourceid);
    }
    fprintf(fp, "} }\n");
    return 0;
}

static int ipfix_mle_trecord(ipfixs_node_t *s, ipfixt_node_t *t, void *arg)
{
    int i;
    FILE *fp = (FILE *)arg;

    fprintf(fp, "{ \"template\": { ");
    fprintf(fp, "\"id\":%u, ", t->ipfixt->tid);
    fprintf(fp, "\"nfields\":%u, \"fields\": [", t->ipfixt->nfields);

    for (i = 0; i < t->ipfixt->nfields; i++)
    {
        if (i)
            fprintf(fp, ", ");
        fprintf(fp, "{\"ie\"=%u, \"type\":%u, \"name\"=%s }",
             t->ipfixt->fields[i].elem->ft->eno,
             t->ipfixt->fields[i].elem->ft->ftype,
             t->ipfixt->fields[i].elem->ft->name);
    }
    fprintf(fp, "]}}\n");
    return 0;
}

static int ipfix_mle_drecord(ipfixs_node_t *s,
                             ipfixt_node_t *t,
                             ipfix_datarecord_t *data,
                             void *arg)
{
    int i;
    char tmpbuf[2048];
    FILE *fp = (FILE *)arg;

    if (!t || !s || !data)
        return -1;

    fprintf(fp, "{ \"data\":{ ");
    fprintf(fp, "\"template\":%u, ", t->ipfixt->tid);
    fprintf(fp, "\"nfields\":%u, \"fields\": [", t->ipfixt->nfields);
    for (i = 0; i < t->ipfixt->nfields; i++)
    {
        if (i)
            fprintf(fp, ", ");
        fprintf(fp, "{\"%s\":", t->ipfixt->fields[i].elem->ft->name);

        t->ipfixt->fields[i].elem->snprint(tmpbuf, (sizeof(tmpbuf)-1),
                                           data->addrs[i], data->lens[i]);
        fprintf(fp, "\"%s\" }", tmpbuf);
    }
    fprintf(fp, "]}}\n");
    return 0;
}

static void ipfix_mle_cleanup(void *arg)
{
    FILE *fp = (FILE *)arg;
    fflush (fp);
    return;
}

/*----- export funcs -----------------------------------------------------*/

int ipfix_start_mle(FILE *fp)
{
    if (g_colinfo)
    {
        errno = EAGAIN;
        return -1;
    }

    if ((g_colinfo = calloc(1, sizeof(ipfix_col_info_t))) == NULL)
    {
        return -1;
    }

    g_colinfo->export_newsource = ipfix_mle_newsource;
    g_colinfo->export_newmsg = ipfix_mle_newmsg;
    g_colinfo->export_trecord = ipfix_mle_trecord;
    g_colinfo->export_drecord = ipfix_mle_drecord;
    g_colinfo->export_cleanup = ipfix_mle_cleanup;
    g_colinfo->data = (void *)fp;

    return ipfix_col_register_export(g_colinfo);
}

void ipfix_stop_mle()
{
    if (g_colinfo)
    {
        (void)ipfix_col_cancel_export(g_colinfo);
        free(g_colinfo);
        g_colinfo = NULL;
    }
}
