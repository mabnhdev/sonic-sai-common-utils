

include_HEADERS = \
inc/sai_acl_npu_api.h    inc/sai_gen_utils.h      inc/sai_lag_callback.h    inc/sai_npu_mirror.h        inc/sai_port_common.h        inc/sai_shell_npu.h           inc/sai_vlan_api.h \
inc/sai_acl_type_defs.h  inc/sai_hash_object.h    inc/sai_lag_common.h      inc/sai_npu_port.h          inc/sai_port_utils.h         inc/sai_stp_defs.h            inc/sai_vlan_common.h \
inc/sai_acl_utils.h      inc/sai_hostif_common.h  inc/sai_mirror_defs.h     inc/sai_npu_qos.h           inc/sai_qos_buffer_util.h    inc/sai_stp_util.h \
inc/sai_common_utils.h   inc/sai_infra_api.h      inc/sai_mirror_util.h     inc/sai_npu_samplepacket.h  inc/sai_qos_common.h         inc/sai_switch_common.h \
inc/sai_debug_utils.h    inc/sai_l3_api.h         inc/sai_npu_api_plugin.h  inc/sai_npu_stp.h           inc/sai_qos_util.h           inc/sai_switch_init_config.h \
inc/sai_event_log.h      inc/sai_l3_common.h      inc/sai_npu_fdb.h         inc/sai_npu_switch.h        inc/sai_samplepacket_defs.h  inc/sai_switch_utils.h \
inc/sai_fdb_api.h        inc/sai_l3_util.h        inc/sai_npu_hostif.h      inc/sai_npu_vlan.h          inc/sai_samplepacket_util.h  inc/sai_udf_common.h \
inc/sai_fdb_common.h     inc/sai_lag_api.h        inc/sai_npu_lag.h         inc/sai_oid_utils.h         inc/sai_shell.h              inc/sai_udf_npu_api.h 



#The logging library
lib_LTLIBRARIES = libsai-common-utils.la



libsai_common_utils_la_SOURCES = \
				       src/sai_gen_utils.c  src/switchinfra/sai_switch_debug.c   src/switchinfra/sai_switch_utils.c \
				       src/port/sai_port_attributes.c  src/port/sai_port_debug.c       src/port/sai_port_utils.c \
				       src/routing/sai_l3_debug.c  src/routing/sai_l3_init.c   src/routing/sai_l3_util.c \
				       src/switching/sai_fdb_debug.c   src/switching/sai_fdb_utils.c   src/switching/sai_lag_debug.c   \
				       src/switching/sai_lag_utils.c   src/switching/sai_vlan_debug.c  src/switching/sai_vlan_utils.c  \
				       src/qos/sai_qos_buffer_util.c   src/qos/sai_qos_debug.c   src/qos/sai_qos_maps_debug.c   \
				       src/qos/sai_qos_policer_debugs.c  src/qos/sai_qos_util.c     src/qos/sai_qos_wred_debugs.c \
				       src/acl/sai_acl_utils.c 


libsai_common_utils_la_LDFLAGS= -lsonic_common -lsonic_logging -lpthread -lrt -version-info 1:1:0

libsai_common_utils_la_CFLAGS= -I$(top_srcdir)/inc -I$(includedir)/sonic


