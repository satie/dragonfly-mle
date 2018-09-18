/*
$$LIC$$
 */
/*
**     ipfix_print.c - IPFIX message JSON funcs
**
**     Copyright Fraunhofer FOKUS
**
**     $Date: 2009-03-19 19:14:44 +0100 (Thu, 19 Mar 2009) $
**
**     $Revision: 1.3 $
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

/*----- revision id ------------------------------------------------------*/

static const char cvsid[]="$Id: ipfix_json.c 996 2009-03-19 18:14:44Z csc $";

/*----- globals ----------------------------------------------------------*/

static ipfix_col_info_t *g_colinfo =NULL;
//static char             tmpbuf[1024];



static int ipfix_json_newsource( ipfixs_node_t *s, void *arg ) 
{
    gzFile zfp = (gzFile)arg;

    gzprintf(zfp, "{ \"obeservation-domain\": %lu }\n", (u_long)s->odid );
    return 0;
}

static int ipfix_json_newmsg( ipfixs_node_t *s, ipfix_hdr_t *hdr, void *arg )
{
    gzFile zfp = (gzFile)arg;

    /* print header
     */
    gzprintf(zfp, "{ \"ipfix-header\":{\"version\":%u,", hdr->version );
    if ( hdr->version == IPFIX_VERSION_NF9 ) {
        gzprintf(zfp, " \"records\":%u, ", hdr->u.nf9.count );;
        gzprintf(zfp, " \"sysuptime\":%.3fs, \"unixtime\":%lu, ", 
              (double)(hdr->u.nf9.sysuptime)/1000.0, 
              (u_long)hdr->u.nf9.unixtime);
        gzprintf(zfp, "\"seqno\":%lu, ", (u_long)hdr->seqno );
        gzprintf(zfp, "\"sourceid\":%lu", (u_long)hdr->sourceid );
    }
    else {
        gzprintf(zfp, "\"length\":%u, ", hdr->u.ipfix.length );
        gzprintf(zfp, "\"unixtime\":%lu, ", (u_long)hdr->u.ipfix.exporttime);
        gzprintf(zfp, "\"seqno\":%lu, ", (u_long)hdr->seqno );
        gzprintf(zfp, "\"odid\":%lu" , (u_long)hdr->sourceid );
    }
    gzprintf(zfp, "}}\n" );
    return 0;
}

static int ipfix_json_trecord( ipfixs_node_t *s, ipfixt_node_t *t, void *arg )
{
    int   i;
    gzFile zfp = (gzFile)arg;

    gzprintf( zfp, "{ \"template\":{ " );
    gzprintf( zfp, "\"id\":%u, ", t->ipfixt->tid);
    gzprintf( zfp, "\"nfields\":%u, \"fields\":[", t->ipfixt->nfields );

    for ( i=0; i<t->ipfixt->nfields; i++ ) {
        if (i) gzprintf(zfp, ", ");
        gzprintf(zfp, "{\"ie\":%u, \"type\":%u, \"name\":\"%s\" }",
              t->ipfixt->fields[i].elem->ft->eno, 
              t->ipfixt->fields[i].elem->ft->ftype, 
              t->ipfixt->fields[i].elem->ft->name );
    }
    gzprintf(zfp, "]}}\n" );
    return 0;
}

static int ipfix_json_drecord( ipfixs_node_t      *s,
                                ipfixt_node_t      *t,
                                ipfix_datarecord_t *data,
                                void               *arg )
{
    char  tmpbuf[2000];
    int   i;
    gzFile zfp = (gzFile)arg;
 
    if ( !t || !s || !data )
        return -1;

    gzprintf( zfp, "{ \"data\":{ " );
    gzprintf( zfp, "\"template\":%u, ", t->ipfixt->tid);
    gzprintf( zfp, "\"nfields\":%u, \"fields\": [", t->ipfixt->nfields );
    for ( i=0; i<t->ipfixt->nfields; i++ ) {
        if (i) gzprintf(zfp, ", ");
        gzprintf( zfp, "{\"%s\":", t->ipfixt->fields[i].elem->ft->name );

        t->ipfixt->fields[i].elem->snprint( tmpbuf, sizeof(tmpbuf), 
                                            data->addrs[i], data->lens[i] );
        gzprintf(zfp, "\"%s\" }", tmpbuf );
    }
    gzprintf(zfp, "]}}\n" );
    gzflush (zfp, Z_FULL_FLUSH);
    return 0;
}

static void print_cleanup( void *arg )
{
    return;
}

/*----- export funcs -----------------------------------------------------*/

int ipfix_start_zout(char *filepath) 
{
    if ( g_colinfo ) {
        errno = EAGAIN;
        return -1;
    }

    if ( (g_colinfo=calloc( 1, sizeof(ipfix_col_info_t))) ==NULL) {
        return -1;
    }
    gzFile zfp = gzopen (filepath, "w");
    if (!zfp)
    {
        perror ("gzopen");
        exit (EXIT_FAILURE);
    }
    //FILE *fp = fopen (filepath, "w");
    //if (!fp)
    //{
    //    perror ("fopen");
    //    exit (EXIT_FAILURE);
    //}

    g_colinfo->export_newsource = ipfix_json_newsource;
    g_colinfo->export_newmsg    = ipfix_json_newmsg;
    g_colinfo->export_trecord   = ipfix_json_trecord;
    g_colinfo->export_drecord   = ipfix_json_drecord;
    g_colinfo->export_cleanup   = print_cleanup;
    g_colinfo->data = (void*)zfp;

    return ipfix_col_register_export( g_colinfo );
}

void ipfix_stop_zout()
{
    if ( g_colinfo ) {
        (void) ipfix_col_cancel_export( g_colinfo );
        free( g_colinfo );
        g_colinfo = NULL;
    }
}
#include <sys/socket.h>
