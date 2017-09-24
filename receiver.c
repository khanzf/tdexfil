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

char *
decode_data(bool *stream, int len)
{
	int p, b;
	char *output;

	output = malloc((len / 8)+1);
	memset(output, '\0', (len/8)+1);

	for(p=0;p<(len/8);p++) {
		for(b=0;b<7;b++) {
			if (stream[(p*8)+b] == 0x1)
				output[p] |= stream[(p*8) + b] << b;
		}
	}

	output[(len / 8)+1] = '\0';

	return output;
}

bool *
receiver(int bitlen, long unknown, long known)
{
	FILE *fp;
	bool *stream;
	size_t len;
	int c;
	char *hostnameline = NULL;
	long longstream[bitlen]; 

	stream = malloc(bitlen);

	fp = fopen("hostnames.txt", "r");
	if (fp == NULL) {
		printf("Unable to open hostnames.txt\n");
		exit(1);
	}

	for (c = 0; c < bitlen; c++) {
		if (getline(&hostnameline, &len, fp) == -1) {
			perror("End of hostnames file");
			return stream;
		}
		chomp(hostnameline);

		longstream[c] = resolvtime(hostnameline);
		usleep(200000);

		if (labs(unknown - longstream[c]) > labs(known - longstream[c])) {
			stream[c] = 0x1; 
			printf("1");
		}
		else {
			stream[c] = 0x0;
			printf("0");
		}
		fflush(stdout);

	}

	putchar('\n');

	return stream;
}

int
realmain()
{
	bool *stream;
	char hostname[MAXHOSTNAME+1];
	long unknown; // New hostname
	long known;
	char *original;

	int c;

	printf("Calculating hostname resolution average...");
	unknown = 0;
	for(c = 0; c < AVGCOUNT; c++) {
		randomDomain("farhan.codes", hostname);
		unknown = unknown + resolvtime(hostname);
	}
	printf("Done!\n");
	unknown = unknown / AVGCOUNT;

	justresolv("known.farhan.codes"); // Resolve it once to cache it
	known = resolvtime("known.farhan.codes");

	printf("Average for unknown domains:\t\t%lu\n", unknown);
	printf("Average for known domain:\t\t%lu\n", known);

	if (unknown <= known) {
		printf("Somehow time of unknown domain is less. Try again...\n");
		exit(1);
	}

	stream = receiver(( 15 * 8), unknown, known);
	original = decode_data(stream, 15*8);

	printf("%s\n", original);
	
	return 0;
}

int
main()
{
	int ret = realmain();

	return ret;
}
