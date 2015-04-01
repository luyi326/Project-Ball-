#include "BlobCompare.h"
#include <iostream>
using namespace std;

inline BlobCluster sortedBlob(uint8_t result, PVision* pv);
inline void invalidateBlob(Blob*);

BlobCluster* normalize(uint8_t result_left, uint8_t result_right, PVision* pv_left, PVision* pv_right) {
	// Sanitize results
	result_left &= 0xF;
	result_right &= 0xF;

	BlobCluster sortet_left = sortedBlob(result_left, pv_left);
	BlobCluster sortet_right = sortedBlob(result_right, pv_right);


	// If both cameras see same amount of Blobs
	if (sortet_left.validBlobCount == sortet_right.validBlobCount) {
		// Number doesn't match, need alignment algorithm
	} else if (sortet_left.validBlobCount == 0) {
		// If left didn't see shit, no alignment is needed
	} else if (sortet_left.validBlobCount == 1) {
		// If right didn't see shit, no alignment is needed
		if (sortet_right.validBlobCount == 0) {
		} else {
			// Right must have seen 2 blobs, left might have seen the right blob
			sortet_left.second = sortet_left.first;
			invalidateBlob(&sortet_left.first);
		}
	} else if (sortet_left.validBlobCount == 2) {
		// If right didn't see shit, no alignment is needed
		if (sortet_right.validBlobCount == 0) {
		}
		// Right must have seen 1 blobs, right might have seen the left blob, no alignment needed
	}
	BlobCluster* resultCluster = new BlobCluster[2];
	resultCluster[0] = sortet_left;
	resultCluster[1] = sortet_right;
	// cout << resultCluster[0].first << endl;
	// cout << resultCluster[0].second << endl;
	// cout << resultCluster[1].first << endl;
	// cout << resultCluster[1].second << endl;
	return resultCluster;
}

inline BlobCluster sortedBlob(uint8_t result, PVision* pv) {
	BlobCluster sortedResult;
	sortedResult.validBlobCount = 0;
	if ((result & BLOB1) && (result & BLOB2)) {
		sortedResult.validBlobCount = 2;
		if (pv->Blob1.X < pv->Blob2.X) {
			sortedResult.first = pv->Blob1;
			sortedResult.second = pv->Blob2;
		} else {
			sortedResult.first = pv->Blob2;
			sortedResult.second = pv->Blob1;
		}
	} else if (result & BLOB1) {
		sortedResult.validBlobCount = 1;
		sortedResult.first = pv->Blob1;
		invalidateBlob(&sortedResult.second);
	}
	return sortedResult;
}

inline void invalidateBlob(Blob* b) {
	b->X = -1;
	b->Y = -1;
	b->Size = 1;
}
