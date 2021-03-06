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
/**
* @file sai_lag_utils.c
*
* @brief This file contains utility APIs for SAI LAG module
*************************************************************************/

#include "saistatus.h"
#include "saitypes.h"
#include "std_llist.h"
#include "sai_lag_common.h"
#include "std_mutex_lock.h"
#include "std_assert.h"
#include "sai_switch_utils.h"
#include "sai_port_utils.h"
#include "sai_gen_utils.h"
#include "sai_lag_api.h"
#include "sai_oid_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

static std_dll_head global_lag_list;
static std_mutex_lock_create_static_init_fast(lag_lock);

void sai_lag_lock(void)
{
    std_mutex_lock(&lag_lock);
}

void sai_lag_unlock(void)
{
    std_mutex_unlock(&lag_lock);
}

std_dll_head* sai_lag_list_get(void)
{
    return &global_lag_list;
}
void sai_lag_cache_init(void)
{
    std_dll_init_sort(&global_lag_list,sai_port_node_compare,
                 SAI_LAG_ID_OFFSET, SAI_LAG_ID_SIZE);
}

sai_status_t sai_lag_node_add(sai_object_id_t lag_id)
{
    sai_lag_node_t *lag_node = NULL;
    lag_node = (sai_lag_node_t *)
                         calloc(1, sizeof(sai_lag_node_t));
    if(lag_node == NULL) {
        SAI_LAG_LOG_CRIT("Unable to add port 0x%"PRIx64" memory %ld unavailable",
                          lag_id, sizeof(sai_lag_node_t));
        return SAI_STATUS_NO_MEMORY;

    }
    lag_node->sai_lag_id = lag_id;
    std_dll_init_sort((&lag_node->port_list),sai_port_node_compare,
                 SAI_LAG_PORT_ID_OFFSET, SAI_LAG_PORT_ID_SIZE);

    std_dll_insert(&global_lag_list,&(lag_node->node));
    return SAI_STATUS_SUCCESS;
}

sai_lag_node_t* sai_lag_node_get(sai_object_id_t lag_id)
{
    sai_lag_node_t *lag_node = NULL;
    std_dll *node = NULL;

    for(node = std_dll_getfirst(&global_lag_list);
        node != NULL;
        node = std_dll_getnext(&global_lag_list,node)) {
        lag_node = (sai_lag_node_t *)node;
        if(lag_node->sai_lag_id == lag_id) {
            return lag_node;
        }else if (lag_node->sai_lag_id > lag_id){
            break;
        }
    }
    return NULL;
}

sai_lag_node_t* sai_lag_get_first_node (void)
{
    std_dll *node = NULL;

    node = std_dll_getfirst(&global_lag_list);
    return (sai_lag_node_t *)node;
}

sai_lag_node_t* sai_lag_get_next_node (sai_lag_node_t* lag_node)
{
    std_dll *node = NULL;

    node = std_dll_getnext(&global_lag_list,&(lag_node->node));
    return (sai_lag_node_t *)node;
}

sai_status_t sai_lag_node_remove (sai_object_id_t lag_id)
{
    sai_lag_node_t *lag_node = NULL;
    lag_node = sai_lag_node_get(lag_id);
    if(lag_node != NULL) {
        std_dll_remove(&global_lag_list,&(lag_node->node));
        free(lag_node);
        return SAI_STATUS_SUCCESS;
    }
    return SAI_STATUS_ITEM_NOT_FOUND;
}

sai_status_t sai_lag_port_count_get(sai_object_id_t sai_lag_id,
                                    unsigned int *port_count)
{
    sai_lag_node_t *lag_node = sai_lag_node_get(sai_lag_id);

    if(lag_node != NULL) {
        *port_count = lag_node->port_count;
        return SAI_STATUS_SUCCESS;
    }
    return SAI_STATUS_ITEM_NOT_FOUND;
}

bool sai_is_lag_created(sai_object_id_t lag_id)
{
   return ((sai_lag_node_get(lag_id) != NULL) ? true : false);
}

sai_status_t sai_lag_port_node_add(sai_object_id_t  lag_id,
                                   sai_object_id_t  port_id,
                                   sai_object_id_t  member_id)
{
    sai_lag_port_node_t *lag_port_node = NULL;
    sai_lag_node_t *lag_node = sai_lag_node_get(lag_id);

    if(lag_node == NULL) {
        return SAI_STATUS_ITEM_NOT_FOUND;
    }
    lag_port_node = (sai_lag_port_node_t *)
                         calloc(1, sizeof(sai_lag_port_node_t));
    if(lag_port_node == NULL) {
        SAI_LAG_LOG_CRIT("Unable to add port 0x%"PRIx64" memory %ld unavailable",
                         port_id, sizeof(sai_lag_port_node_t));
        return SAI_STATUS_NO_MEMORY;

    }

    lag_port_node->lag_id      = lag_id;
    lag_port_node->port_id     = port_id;
    lag_port_node->member_id   = member_id;
    lag_port_node->ing_disable = false;
    lag_port_node->egr_disable = false;

    std_dll_insert(&(lag_node->port_list),&(lag_port_node->node));
    lag_node->port_count++;
    return SAI_STATUS_SUCCESS;
}

static sai_lag_port_node_t* sai_lag_port_node_get(sai_object_id_t lag_id,
                                                  sai_object_id_t port_id)
{
    sai_lag_node_t *lag_node = sai_lag_node_get(lag_id);
    sai_lag_port_node_t *lag_port_node = NULL;
    std_dll *node = NULL;

    if(lag_node == NULL) {
        return NULL;
    }

    for(node = std_dll_getfirst(&(lag_node->port_list));
        node != NULL;
        node = std_dll_getnext(&(lag_node->port_list),node)) {
        lag_port_node = (sai_lag_port_node_t *)node;
        if(lag_port_node->port_id == port_id) {
            return lag_port_node;
        }else if (lag_port_node->port_id > port_id){
            break;
        }
    }
    return NULL;
}

sai_status_t sai_lag_get_port_id_from_member_id (sai_object_id_t  lag_id,
                                                 sai_object_id_t  member_id,
                                                 sai_object_id_t *port_id)
{
    sai_lag_node_t      *lag_node;
    sai_lag_port_node_t *lag_port_node = NULL;
    std_dll             *node = NULL;

    lag_node = sai_lag_node_get (lag_id);

    if (lag_node == NULL) {
        return SAI_STATUS_INVALID_PARAMETER;
    }

    for (node = std_dll_getfirst (&(lag_node->port_list));
         node != NULL;
         node = std_dll_getnext (&(lag_node->port_list), node)) {

        lag_port_node = (sai_lag_port_node_t *) node;

        if (lag_port_node->member_id == member_id) {
            *port_id = lag_port_node->port_id;
            return SAI_STATUS_SUCCESS;
        }
    }

    return SAI_STATUS_ITEM_NOT_FOUND;
}

sai_status_t sai_lag_get_info_from_member_id (sai_object_id_t  member_id,
                                              sai_object_id_t *lag_id,
                                              sai_object_id_t *port_id)
{
    sai_lag_node_t      *lag_node;
    sai_lag_port_node_t *lag_port_node = NULL;
    std_dll             *node1 = NULL;
    std_dll             *node2 = NULL;

    for (node1 = std_dll_getfirst (&global_lag_list);
         node1 != NULL;
         node1 = std_dll_getnext (&global_lag_list, node1)) {

        lag_node = (sai_lag_node_t *) node1;

        for (node2 = std_dll_getfirst (&(lag_node->port_list));
             node2 != NULL;
             node2 = std_dll_getnext (&(lag_node->port_list), node2)) {

            lag_port_node = (sai_lag_port_node_t *)node2;

            if (lag_port_node->member_id == member_id) {
                *port_id = lag_port_node->port_id;
                *lag_id  = lag_port_node->lag_id;
                return SAI_STATUS_SUCCESS;
            }
        }
    }

    return SAI_STATUS_ITEM_NOT_FOUND;
}

sai_status_t sai_lag_port_node_remove (sai_object_id_t lag_id,
                                       sai_object_id_t port_id)
{
    sai_lag_node_t *lag_node = sai_lag_node_get(lag_id);
    sai_lag_port_node_t *lag_port_node = sai_lag_port_node_get(lag_id, port_id);
    if((lag_node != NULL) && (lag_port_node != NULL)) {
        std_dll_remove(&(lag_node->port_list),&(lag_port_node->node));
        free(lag_port_node);
        lag_node->port_count--;
        return SAI_STATUS_SUCCESS;
    }
    return SAI_STATUS_ITEM_NOT_FOUND;
}

sai_status_t sai_remove_all_lag_port_nodes(sai_object_id_t lag_id)
{
    sai_lag_node_t *lag_node = sai_lag_node_get(lag_id);
    sai_lag_port_node_t *lag_port_node = NULL;
    std_dll *node = NULL;

    if(lag_node == NULL) {
        return SAI_STATUS_ITEM_NOT_FOUND;
    }

    for(node = std_dll_getfirst(&(lag_node->port_list));
        node != NULL;
        node = std_dll_getfirst(&(lag_node->port_list))) {
        lag_port_node = (sai_lag_port_node_t *)node;
        std_dll_remove(&(lag_node->port_list),&(lag_port_node->node));
        free(lag_port_node);
    }
    lag_node->port_count = 0;
    return SAI_STATUS_SUCCESS;
}

bool sai_is_port_lag_member(sai_object_id_t lag_id,sai_object_id_t port_id)
{
   return ((sai_lag_port_node_get(lag_id, port_id) != NULL) ? true : false);
}

sai_status_t sai_lag_port_list_get(sai_object_id_t lag_id,
                                   sai_object_list_t *lag_port_list)
{
    sai_lag_node_t *lag_node = sai_lag_node_get(lag_id);
    sai_lag_port_node_t *lag_port_node = NULL;
    std_dll *node = NULL;
    unsigned int port_index = 0;

    if(lag_node == NULL) {
        return SAI_STATUS_ITEM_NOT_FOUND;
    } else if(lag_node->port_count > lag_port_list->count) {
        lag_port_list->count = lag_node->port_count;
        return SAI_STATUS_BUFFER_OVERFLOW;
    }

    for(node = std_dll_getfirst(&(lag_node->port_list));
        node != NULL;
        node = std_dll_getnext(&(lag_node->port_list),node)) {
        lag_port_node = (sai_lag_port_node_t *)node;
        lag_port_list->list[port_index] = lag_port_node->port_id;
        port_index++;
    }
    return SAI_STATUS_SUCCESS;
}

sai_status_t sai_lag_portcount_cache_read(sai_object_id_t lag_id,
                                          unsigned int *port_count)
{
    sai_status_t ret_val;

    sai_lag_lock();
    ret_val = sai_lag_port_count_get(lag_id, port_count);
    sai_lag_unlock();

    return ret_val;
}
sai_status_t sai_lag_portlist_cache_read(sai_object_id_t lag_id,
                                         sai_object_list_t *lag_port_list)
{
    sai_status_t ret_val;

    sai_lag_lock();
    ret_val = sai_lag_port_list_get(lag_id, lag_port_list);
    sai_lag_unlock();

    return ret_val;
}

bool sai_is_port_part_of_different_lag(sai_object_id_t lag_id, sai_object_id_t port_id)
{
    sai_lag_node_t *lag_node = NULL;
    std_dll *node = NULL;

    for(node = std_dll_getfirst(&global_lag_list);
        node != NULL;
        node = std_dll_getnext(&global_lag_list,node)) {
        lag_node = (sai_lag_node_t *)node;
        if(lag_node->sai_lag_id != lag_id) {
            if(sai_is_port_lag_member(lag_node->sai_lag_id, port_id)) {
                return true;
            }
        }
    }
    return false;
}

sai_status_t sai_lag_npu_object_id_get (sai_object_id_t lag_id,
                                       sai_npu_object_id_t* npu_lag_id)
{
    if(sai_is_obj_id_lag(lag_id)) {
        *npu_lag_id = sai_uoid_npu_obj_id_get(lag_id);
        return SAI_STATUS_SUCCESS;
    }
    return SAI_STATUS_INVALID_OBJECT_TYPE;
}

sai_status_t sai_lag_update_rif_id (sai_object_id_t lag_id, sai_object_id_t rif_id)
{
    sai_lag_node_t *lag_node = NULL;

    sai_lag_lock();
    lag_node = sai_lag_node_get(lag_id);
    if(lag_node == NULL) {
        sai_lag_unlock();
        SAI_LAG_LOG_ERR("Unable to find LAG Object for ID 0x%"PRIx64"", lag_id);
        return SAI_STATUS_ITEM_NOT_FOUND;
    }
    lag_node->rif_id = rif_id;
    sai_lag_unlock();
    return SAI_STATUS_SUCCESS;
}
sai_object_id_t sai_lag_get_rif_id (sai_object_id_t lag_id)
{
    sai_object_id_t rif_id = 0;
    sai_lag_node_t *lag_node = NULL;

    lag_node = sai_lag_node_get(lag_id);
    if(lag_node != NULL) {
        rif_id = lag_node->rif_id;
    }
    return rif_id;
}

sai_status_t sai_lag_member_get_disable_status (sai_object_id_t  lag_id,
                                                sai_object_id_t  port_id,
                                                bool             is_ingress,
                                                bool            *status)
{
    sai_lag_port_node_t *lag_port_node;

    lag_port_node = sai_lag_port_node_get (lag_id, port_id);

    if (lag_port_node == NULL) {
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (is_ingress) {
        *status = lag_port_node->ing_disable;
    }
    else {
        *status = lag_port_node->egr_disable;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t sai_lag_member_set_disable_status (sai_object_id_t lag_id,
                                                sai_object_id_t port_id,
                                                bool            is_ingress,
                                                bool            status)
{
    sai_lag_port_node_t *lag_port_node;

    lag_port_node = sai_lag_port_node_get (lag_id, port_id);

    if (lag_port_node == NULL) {
        return SAI_STATUS_INVALID_PARAMETER;
    }

    if (is_ingress) {
        lag_port_node->ing_disable = status;
    }
    else {
        lag_port_node->egr_disable = status;
    }

    return SAI_STATUS_SUCCESS;
}

sai_status_t sai_lag_member_get_member_id (sai_object_id_t  lag_id,
                                           sai_object_id_t  port_id,
                                           sai_object_id_t *member_id)
{
    sai_lag_port_node_t *lag_port_node;

    lag_port_node = sai_lag_port_node_get (lag_id, port_id);

    if (lag_port_node == NULL) {
        return SAI_STATUS_INVALID_PARAMETER;
    }

    *member_id = lag_port_node->member_id;

    return SAI_STATUS_SUCCESS;
}

sai_status_t sai_lag_member_set_member_id (sai_object_id_t lag_id,
                                           sai_object_id_t port_id,
                                           sai_object_id_t member_id)
{
    sai_lag_port_node_t *lag_port_node;

    lag_port_node = sai_lag_port_node_get (lag_id, port_id);

    if (lag_port_node == NULL) {
        return SAI_STATUS_INVALID_PARAMETER;
    }

    lag_port_node->member_id = member_id;

    return SAI_STATUS_SUCCESS;
}
