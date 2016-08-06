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
* @file sai_stp_defs.h
*
* @brief This file contains the stp datastructures definitions and utility
*        functions
*
*************************************************************************/

/* OPENSOURCELICENSE */

#ifndef __SAI_STP_DEFS_H__
#define __SAI_STP_DEFS_H__

#include "std_rbtree.h"

#include "saitypes.h"

/**
 * @brief Datastructure for STG instance
 */
typedef struct _dn_sai_stp_info_t {

    /** STP Instance Id */
    sai_object_id_t stp_inst_id;

    /** Vlans attached to this STG tree */
    rbtree_handle vlan_tree;
} dn_sai_stp_info_t;

#endif /* __SAI_STP_DEFS_H__ */
