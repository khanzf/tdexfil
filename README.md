# Time-Based DNS Query Exfiltrator
This is an attempt to implement a time-based DNS proxy. This would be used to exfiltrate small volumes of data over a network.
## Purpose
This was written to demonstrate a security risk in exposing one's DNS server to the internet. 
## How It Works
DNS servers typically cache requests to prevent double-lookup. The second request time tends to be noticeably shorter than the first. If you can estimate how long a fresh DNS query takes, you can discern if someone has previously resolved a given hostname or not.

First, both sender and receiver must generate an agreed upon list of random, junk domain names. If the message is 2 bytes, they should generate 16 shared domain names.

**Sender:** The sender will convert the clear-text message to a binary stream, and iterate through the pre-shared list of hostnames. If a bit is 1, the sender will resolve that hostname, thus cacheing it in the DNS server. If the the bit is 0, the sender will not query that hostname, and it will not be cached.

**Receiver:** At an agreed upon time, the receiver will first query a large number of junk domain names to estimate an average time-to-query of fresh queries, then identify the time-to-query of cached hostname. Using these two input fields, at an agreed upon time, he will query the pre-shared list of hostnames. If the time-to-query is closer to the cached time, the bit will be set to 1. Conversely, if the time-to-query is closer to the new query time, the bit will be set to 0. The stream of 0's and 1's is then converted to ASCII and printed out to the terminal.

## Usage
```make all
./generate example-hostname.com [number of bytes] > hostnames.txt
```
Share this file between the sender and receiver.
At an agreed upon time, the sender performs:
```
./sender
```
Maybe 1 second later, the receiver should run:
```
./receiver
```
If all goes according to plan, the message should populate on the terminal.

## Notes
* Works on FreeBSD and Linux.
* You likely cannot send large volumes of data at once. And, it fails a LOT. But when it works...its glorious!! 
* It is highly unreliable and should not be used other than to demonstrate a proof-of-concept.
* Took me a few hours to write. Had to scratch an itch :)
* 2-Clause BSD licensed. What else? GPL?
