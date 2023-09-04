/****************************************************************

  Generated by Autocore DDS IDL to C Translator
  File name: SampleEvent_cmcg.c
  Source: /home/ubuntu/pangolin/HHEV version/build/cmcg_generated/SampleEvent_cmcg.idl
  AutoCore DDS: V2.0.18.1

*****************************************************************/
#include "SampleEvent_cmcg.h"

#include "dds/dds.h"
#include "dds/ddsrt/sync.h"
#include <string.h>

static const uint32_t sample_SampleEvent_ops [] =
{
  /* SampleEvent */
  DDS_OP_ADR | DDS_OP_TYPE_8BY | DDS_OP_FLAG_SGN, offsetof (sample_SampleEvent, f_ghmSessionId),
  DDS_OP_ADR | DDS_OP_TYPE_ARR | DDS_OP_SUBTYPE_1BY | DDS_OP_FLAG_SGN, offsetof (sample_SampleEvent, datetime), 256u,
  DDS_OP_ADR | DDS_OP_TYPE_8BY, offsetof (sample_SampleEvent, seqNum),
  DDS_OP_RTS
};

/* Type Information:
  [MINIMAL 1dfd9684794edd487ebef5a33f10] (#deps: 0)
  [COMPLETE f794c0c45c2d20a94dd6e24749f2] (#deps: 0)
*/
#define TYPE_INFO_CDR_sample_SampleEvent (unsigned char []){ \
  0x60, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x40, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, \
  0x14, 0x00, 0x00, 0x00, 0xf1, 0x1d, 0xfd, 0x96, 0x84, 0x79, 0x4e, 0xdd, 0x48, 0x7e, 0xbe, 0xf5, \
  0xa3, 0x3f, 0x10, 0x00, 0x57, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, \
  0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x00, 0x40, 0x28, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, \
  0x14, 0x00, 0x00, 0x00, 0xf2, 0xf7, 0x94, 0xc0, 0xc4, 0x5c, 0x2d, 0x20, 0xa9, 0x4d, 0xd6, 0xe2, \
  0x47, 0x49, 0xf2, 0x00, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, \
  0x00, 0x00, 0x00, 0x00\
}
#define TYPE_INFO_CDR_SZ_sample_SampleEvent 100u
#define TYPE_MAP_CDR_sample_SampleEvent (unsigned char []){ \
  0x6b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xf1, 0x1d, 0xfd, 0x96, 0x84, 0x79, 0x4e, 0xdd, \
  0x48, 0x7e, 0xbe, 0xf5, 0xa3, 0x3f, 0x10, 0x00, 0x53, 0x00, 0x00, 0x00, 0xf1, 0x51, 0x01, 0x00, \
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, \
  0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x05, 0x3b, 0x7a, 0x7d, 0x8f, 0x00, \
  0x19, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x91, 0xf3, 0x01, 0x00, 0x00, 0x00, \
  0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0xdf, 0xea, 0xae, 0xb4, 0x00, 0x00, 0x00, \
  0x0b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0xef, 0x3e, 0xfd, 0x50, 0x00, \
  0xad, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xf2, 0xf7, 0x94, 0xc0, 0xc4, 0x5c, 0x2d, 0x20, \
  0xa9, 0x4d, 0xd6, 0xe2, 0x47, 0x49, 0xf2, 0x00, 0x95, 0x00, 0x00, 0x00, 0xf2, 0x51, 0x01, 0x00, \
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x73, 0x61, 0x6d, 0x70, \
  0x6c, 0x65, 0x3a, 0x3a, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x45, 0x76, 0x65, 0x6e, 0x74, 0x00, \
  0x6d, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
  0x01, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x66, 0x5f, 0x67, 0x68, 0x6d, 0x53, 0x65, 0x73, \
  0x73, 0x69, 0x6f, 0x6e, 0x49, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, \
  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x91, 0xf3, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
  0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x64, 0x61, 0x74, 0x65, \
  0x74, 0x69, 0x6d, 0x65, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, \
  0x01, 0x00, 0x08, 0x00, 0x07, 0x00, 0x00, 0x00, 0x73, 0x65, 0x71, 0x4e, 0x75, 0x6d, 0x00, 0x00, \
  0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xf2, 0xf7, 0x94, 0xc0, \
  0xc4, 0x5c, 0x2d, 0x20, 0xa9, 0x4d, 0xd6, 0xe2, 0x47, 0x49, 0xf2, 0xf1, 0x1d, 0xfd, 0x96, 0x84, \
  0x79, 0x4e, 0xdd, 0x48, 0x7e, 0xbe, 0xf5, 0xa3, 0x3f, 0x10\
}
#define TYPE_MAP_CDR_SZ_sample_SampleEvent 330u
const dds_topic_descriptor_t sample_SampleEvent_desc =
{
  .m_size = sizeof (sample_SampleEvent),
  .m_align = dds_alignof (sample_SampleEvent),
  .m_flagset = DDS_TOPIC_FIXED_SIZE | DDS_TOPIC_XTYPES_METADATA,
  .m_nkeys = 0u,
  .m_typename = "sample::SampleEvent",
  .m_keys = NULL,
  .m_nops = 4,
  .m_ops = sample_SampleEvent_ops,
  .m_meta = "",
  .type_information = { .data = TYPE_INFO_CDR_sample_SampleEvent, .sz = TYPE_INFO_CDR_SZ_sample_SampleEvent },
  .type_mapping = { .data = TYPE_MAP_CDR_sample_SampleEvent, .sz = TYPE_MAP_CDR_SZ_sample_SampleEvent }
};

