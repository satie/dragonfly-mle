/* Copyright (C) 2017-2018 CounterFlow AI, Inc.
 *
 * You can copy, redistribute or modify this Program under the terms of
 * the GNU General Public License version 2 as published by the Free
 * Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/*
 *
 * author Randy Caldejon <rc@counterflowai.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>

#include "dragonfly-io.h"
#include "config.h"

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */
DF_HANDLE *ipfix_open(const char *ipfix_uri, int spec)
{
      // parse ipfix_uri

        

        DF_HANDLE *dh = (DF_HANDLE *)malloc(sizeof(DF_HANDLE));
        if (!dh)
        {
                syslog(LOG_ERR, "%s: malloc failed", __FUNCTION__);
                return NULL;
        }
        memset(dh, 0, sizeof(DF_HANDLE));

        //dh->fd = socket_handle;
        dh->io_type = io_type;
        dh->path = strndup(ipfx_uri, PATH_MAX);
#ifdef __DEBUG3__
        syslog(LOG_INFO, "%s: %s", __FUNCTION__, path);
#endif
        return dh;
}

/*
 * ---------------------------------------------------------------------------------------
 *
 * 
 * ---------------------------------------------------------------------------------------
 */
int ipfix_read_line(DF_HANDLE *dh, char *buffer, int len)
{
        int n = read(dh->fd, buffer, len);
        if (n < 0)
        {
                syslog(LOG_ERR, "read error: %s", strerror(errno));
                perror("read");
                exit(EXIT_FAILURE);
        }
        return n;
}

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */
void ipfix_close(DF_HANDLE *dh)
{
        if (dh)
        {
                close(dh->fd);
                dh->fd = -1;
        }
}

/*
 * ---------------------------------------------------------------------------------------
 */
