/*-
 * Copyright (c) 2017 Farhan Khan <khanzf@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

char *choices = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
size_t choices_len = 62; 

void
chomp(char *s)
{
	int i;
	i = 0;

	for(i = 0; i < strlen(s); i++) {
		if (s[i] == '\n') {
			s[i] = '\0';
			return;
		}
	}
}

long
resolvtime(char *hostname)
{
	struct timeval before;
	struct timeval after;
	struct addrinfo hints, *servinfo;
	int ret;
	uint64_t diff;

	memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	gettimeofday(&before, NULL);
	ret = getaddrinfo(hostname, "http", &hints, &servinfo);
	gettimeofday(&after, NULL);

	if (ret) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		fprintf(stderr, "One of the resolution attempts failed.\n");
		fprintf(stderr, "This will corrupt the average generation, so exiting here\n");
		return 0;
		//exit(1);
	}

	diff = ((after.tv_sec * 1000000) + after.tv_usec) -
	       ((before.tv_usec * 1000000) + before.tv_usec);

//	printf("%s: Resolv Time: %lu\n", hostname, diff);
	return diff;
}

void
justresolv(char *hostname)
{
	struct addrinfo hints, *servinfo;
	int ret;

	memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(hostname, "http", &hints, &servinfo);

	if (ret) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		fprintf(stderr, "This will corrupt the send stream!\n");
		exit(1);
	}
}

void
randomPrefix(char *prefix, int len)
{
	int n;
	int rand;
	int rfd;
	int key;

	rfd = open("/dev/random", O_RDONLY);

	for(n = 0; n < len; n++) {
		read(rfd, &rand, 1);
		key = rand % choices_len;
		prefix[n] = choices[key];
	}

	close(rfd);
	prefix[len] = '\0';
}

char *
randomDomain(char *domain, char *hostname)
{
	char prefix[10 + 1];

	randomPrefix(prefix, 10);
	snprintf(hostname, MAXHOSTNAME, "%s.%s", prefix, domain); 

	return hostname;
}
