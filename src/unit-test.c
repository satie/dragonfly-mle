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

#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>

#include <limits.h>
#ifdef __linux__
#include <linux/limits.h>
#endif
#ifdef __FreeBSD__
#include <pthread_np.h>
#include <sys/limits.h>
#endif

#include "dragonfly-lib.h"
#include "test.h"

int g_chroot = 0;
int g_verbose = 0;
int g_drop_priv = 0;

/*
 * ---------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------
 */

int main(int argc, char **argv)
{
	
#ifdef _GNU_SOURCE
	pthread_setname_np(pthread_self(), "dragonfly");
#endif

	openlog("dragonfly", LOG_PERROR, LOG_USER);
	fprintf (stderr, "Running unit tests for dragonfly-mle\n\n\n");
	dragonfly_mle_test(TMP_DIR);
	closelog();
    
	exit(EXIT_SUCCESS);
}

/*
 * ---------------------------------------------------------------------------------------
 */
