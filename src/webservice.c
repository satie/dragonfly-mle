/*
     This file was derived from the libmicrohttpd file example.
     (C) 2007 Christian Grothoff (and other contributing authors)

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <stddef.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <microhttpd.h>
#include <unistd.h>

#define ERROR_PAGE ("<html><head><title>Analyzer not found</title></head><body>File not found</body></html>")

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */
static ssize_t
file_reader(void *cls, uint64_t pos, char *buf, size_t max)
{
  FILE *file = cls;

  (void)fseek(file, pos, SEEK_SET);
  return fread(buf, 1, max, file);
}

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */
static void free_callback(void *cls)
{
  FILE *file = cls;
  fclose(file);
}

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */
static int ServeJsonFile(void *cls,
                         struct MHD_Connection *connection,
                         const char *url,
                         const char *method,
                         const char *version, 
                         const char *upload_data,
                         size_t *upload_data_size, void **ptr)
{
  static int aptr;
  struct MHD_Response *response;
  int ret;
  FILE *file;
  const char *document_root = (const char *)cls;
  struct stat buf;


  if (0 != strcmp(method, MHD_HTTP_METHOD_GET))
  {
    return MHD_NO; /* unexpected method */
  }

fprintf (stderr, "%s1: %s<>%s\n", __FUNCTION__, document_root, url);
  /* validate the url */
  int len = strnlen(document_root, 32);
  if (0 != strncmp(document_root, url, len))
  {
    return MHD_NO; /* unexpected method */
  }

fprintf (stderr, "%s2: file: %s\n", __FUNCTION__, url);
  if (&aptr != *ptr)
  {
    /* do never respond on first call */
    *ptr = &aptr;
    return MHD_YES;
  }
  *ptr = NULL; /* reset when done */

  if (0 == stat(&url[1], &buf))
  {
    file = fopen(&url[1], "rb");
  }
  else
  {
    file = NULL;
  }
  if (file == NULL)
  {
    response = MHD_create_response_from_buffer(strlen(ERROR_PAGE),
                                               (void *)ERROR_PAGE,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
  }
  else
  {
    response = MHD_create_response_from_callback(buf.st_size, 32 * 1024, /* 32k page size */
                                                 &file_reader,
                                                 file,
                                                 &free_callback);
    if (response == NULL)
    {
      fclose(file);
      return MHD_NO;
    }
    (void)MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
  }
  return ret;
}

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */

void *start_web_server(char *document_root, int port)
{
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  
//  struct MHD_Daemon *d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_DEBUG,
  struct MHD_Daemon *d = MHD_start_daemon(MHD_USE_POLL_INTERNALLY | MHD_USE_DEBUG,
                                          port, NULL, NULL, &ServeJsonFile, document_root,
                                          MHD_OPTION_SOCK_ADDR, &address, MHD_OPTION_END);
  return (void *)d;
}

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */
void stop_web_server(void *ctx)
{
  struct MHD_Daemon *d = (struct MHD_Daemon *)ctx;
  MHD_stop_daemon (d);
}

#if 0
/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */

int main (int argc, char *const *argv)
{
  void *ctx = start_web_server ("/www/", 7070);

  (void) getc (stdin);

  stop_web_server (ctx);
  return 0;
}
#endif
/*
 * ---------------------------------------------------------------------------------------
 */
