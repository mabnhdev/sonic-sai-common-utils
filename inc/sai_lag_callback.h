/************************************************************************
* * LEGALESE:   "Copyright (c) 2015, Dell Inc. All rights reserved."
* *
* * This source code is confidential, proprietary, and contains trade
* * secrets that are the sole property of Dell Inc.
* * Copy and/or distribution of this source code or disassembly or reverse
* * engineering of the resultant object code are strictly forbidden without
* * the written consent of Dell Inc.
* *
************************************************************************/
/***
 * \file    sai_lag_callback.h
 *
 * \brief Declaration of SAI LAG related CALLBACKs
*/

#if !defined (__SAILAGCALLBACK_H_)
#define __SAILAGCALLBACK_H_

#include "saitypes.h"
#include "saistatus.h"

/*LAG Operation: List of operations possible on a LAG*/
typedef enum _sai_lag_operation_t {
    /*Create a LAG*/
    SAI_LAG_OPER_CREATE,
    /*Delete a LAG*/
    SAI_LAG_OPER_DELETE,
    /*Add ports to a LAG*/
    SAI_LAG_OPER_ADD_PORTS,
    /*Delete ports from a a LAG*/
    SAI_LAG_OPER_DEL_PORTS,
    /*Set the list of ports on a LAG*/
    SAI_LAG_OPER_SET_PORTS
} sai_lag_operation_t;

/** SAI LAG CALLBACK API - LAG RIF Callback
      \param[in] lag_id LAG Identifier
      \param[in] rif_id Router interface Identifier
      \param[in] port_list List of ports
      \param[in] lag_operation Operation performed on LAG
*/
typedef sai_status_t (*sai_lag_l3_rif_callback) (
    sai_object_id_t lag_id,
    sai_object_id_t rif_id,
    const sai_object_list_t *port_list,
    sai_lag_operation_t lag_operation
);

/*Lag Callback: List of callbacks registered with LAG Module*/
typedef struct _sai_lag_callback_t {
    /*rif_callback: Router interface callback*/
    sai_lag_l3_rif_callback rif_callback;
} sai_lag_callback_t;

/** SAI LAG CALLBACK API - Register Router interface callback
      \param[in]rif_callback Router interface callback
*/
void sai_lag_rif_callback_register(sai_lag_l3_rif_callback rif_callback);

#endif
