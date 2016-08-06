/************************************************************************
* LEGALESE:   "Copyright (c) 2015, Dell Inc. All rights reserved."
*
* This source code is confidential, proprietary, and contains trade
* secrets that are the sole property of Dell Inc.
* Copy and/or distribution of this source code or disassembly or reverse
* engineering of the resultant object code are strictly forbidden without
* the written consent of Dell Inc.
*
************************************************************************/
/**
* @file sai_debug_utils.h
*
* @brief This file contains utility APIs for SAI debug functions
*
*************************************************************************/
#ifndef __SAI_DEBUG_UTILS_H__
#define __SAI_DEBUG_UTILS_H__

#include <stdio.h>

/** \defgroup SAIDEBUGUTILAPIS SAI - Common Utility API
 *   Contains Debug Utility APIs to be used by other SAI components
 *  \{
 */


/** Utility to be used for debug/dump routines */
#define SAI_DEBUG(msg, ...) \
    do { \
        printf (msg"\n", ##__VA_ARGS__); \
    } while (0)


/**
 * \}
 */

#endif
