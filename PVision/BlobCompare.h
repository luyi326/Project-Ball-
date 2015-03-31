#ifndef BLOB_CONTROL_H
#define BLOB_CONTROL_H

#include <stdint.h>
#include "PVision.h"

//This function assume that both data for both cameras are already collected
//Only up to 2 blobs are handled
BlobCluster* normalize(uint8_t result_left, uint8_t result_right, PVision* pv_left, PVision* pv_right);

#endif
