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

bool *
encode_byte(char *msg, int len)
{
	int p;
	int b;

	bool tmp;

	bool *result;

	result = malloc(len * 8);

	for (p=0;p<len;p++) {
		// Send the least significant bit first
		for (b = 0; b < 7 ; b++) {
			tmp  = msg[p] >> b;
			tmp  &= 0x1;
			result[(p*8) + b] = tmp;
			if (tmp == 0x1)
				printf("1");
			else
				printf("0");
			fflush(stdout);
		}
	}

	return result;
}

void
sender(bool *bitstream, int msglen)
{
	FILE *fp;
	char *hostnameline = NULL;
	size_t len = 0;
	int c;

	fp = fopen("hostnames.txt", "r");
	if (fp == NULL) {
		printf("Unable to open hostnames.txt\n");
		exit(1);
	}

	for (c = 0; c < msglen; c++) {
		if (getline(&hostnameline, &len, fp) == -1) {
			//perror("End of hostnames file");
			fprintf(stderr, "\nReached end of hostname file.\n");
			break;
		}
		chomp(hostnameline);

		if (bitstream[c] == 0x1) {
			justresolv(hostnameline);
			usleep(100000);
		}
		printf("."); fflush(stdout);
	}

	putchar('\n');

	return;
}

int
realmain()
{
	char msg[] = "FARHAN IS 31337";
	int msglen = strlen(msg);
	bool *bitstream;

	printf("Encoding data...");
	bitstream = encode_byte(msg, msglen);
	printf("Done!\n");

	sender(bitstream, (msglen * 8));
	
	return 0;
}

int
main()
{
	int ret = realmain();

	return ret;
}
