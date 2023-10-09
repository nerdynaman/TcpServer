#include "wrapping_integers.hh"

#include <iostream>

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { 
	uint64_t modVal = (pow(2.0,32.0));
	uint64_t res = ( (uint64_t)isn.raw_value() +n) % modVal;
	return WrappingInt32(res);
	}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
	// Calculate the difference between the received sequence number and the expected sequence number
	uint64_t seqDiff = n.raw_value() - wrap(checkpoint, isn).raw_value();

	// Calculate the absolute sequence number that corresponds to the received sequence number
	uint64_t absSeqNum = seqDiff + checkpoint;

	uint64_t p32 = pow(2,32);
	// Handle wraparound if the difference is greater than or equal to 2^31 and the absolute sequence number is greater than or equal to 2^32
	if (absSeqNum >= p32) {
		if (seqDiff >= uint64_t(pow(2,31))) {
			absSeqNum -= p32;
		}
	}
	return absSeqNum;
}
