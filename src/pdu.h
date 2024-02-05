#include <stdint.h>

/*
	Taken from linux sources with elixir-bootlin
*/

#define struct_group(NAME, MEMBERS) \
    union { \
        struct { MEMBERS }; \
        struct { MEMBERS } NAME; \
    }

/**
 * struct nvme_tcp_hdr - nvme tcp pdu common header
 *
 * @type:          pdu type
 * @flags:         pdu specific flags
 * @hlen:          pdu header length
 * @pdo:           pdu data offset
 * @plen:          pdu wire byte length
 */
struct nvme_tcp_hdr {
    uint8_t     type;
    uint8_t     flags;
    uint8_t     hlen;
    uint8_t     pdo;
    uint32_t    plen;
};

/**
 * struct nvme_tcp_icreq_pdu - nvme tcp initialize connection request pdu
 *
 * @hdr:           pdu generic header
 * @pfv:           pdu version format
 * @hpda:          host pdu data alignment (dwords, 0's based)
 * @digest:        digest types enabled
 * @maxr2t:        maximum r2ts per request supported
 */
struct nvme_tcp_icreq_pdu {
    struct nvme_tcp_hdr hdr;
    uint16_t            pfv;
    uint8_t             hpda;
    uint8_t             digest;
    uint32_t            maxr2t;
    unsigned char       rsvd2[112];
};


/**
 * struct nvme_tcp_data_pdu - nvme tcp data pdu
 *
 * @hdr:           pdu common header
 * @command_id:    nvme command identifier which this relates to
 * @ttag:          transfer tag (controller generated)
 * @data_offset:   offset from the start of the command data
 * @data_length:   length of the data stream
 */
struct nvme_tcp_data_pdu {
    struct nvme_tcp_hdr hdr;
    uint16_t            command_id;
    uint16_t            ttag;
    uint32_t            data_offset;
    uint32_t            data_length;
    uint8_t             rsvd[4];
};


/**
 * struct nvme_tcp_icresp_pdu - nvme tcp initialize connection response pdu
 *
 * @hdr:           pdu common header
 * @pfv:           pdu version format
 * @cpda:          controller pdu data alignment (dowrds, 0's based)
 * @digest:        digest types enabled
 * @maxdata:       maximum data capsules per r2t supported
 */
struct nvme_tcp_icresp_pdu {
	struct nvme_tcp_hdr	hdr;
	uint16_t			pfv;
	unsigned char			cpda;
	unsigned char			digest;
	uint32_t			maxdata;
	unsigned char			rsvd[112];
};

struct nvme_completion {
	/*
	 * Used by Admin and Fabrics commands to return data:
	 */
	union nvme_result {
		uint16_t	u16;
		uint32_t	u32;
		uint64_t	u64;
	} result;
	uint16_t	sq_head;	/* how much of this queue may be reclaimed */
	uint16_t	sq_id;		/* submission queue that generated this entry */
	uint16_t	command_id;	/* of the command which completed */
	uint16_t	status;		/* did the command fail, and if so, why? */
};

/**
 * struct nvme_tcp_rsp_pdu - nvme tcp response capsule pdu
 *
 * @hdr:           pdu common header
 * @hdr:           nvme-tcp generic header
 * @cqe:           nvme completion queue entry
 */
struct nvme_tcp_rsp_pdu {
	struct nvme_tcp_hdr	hdr;
	struct nvme_completion	cqe;
};

/**
 * struct nvme_tcp_term_pdu - nvme tcp terminate connection pdu
 *
 * @hdr:           pdu common header
 * @fes:           fatal error status
 * @fei:           fatal error information
 */
struct nvme_tcp_term_pdu {
	struct nvme_tcp_hdr	hdr;
	uint16_t			fes;
	uint16_t			feil;
	uint16_t			feiu;
	uint8_t			rsvd[10];
};

enum nvme_tcp_pdu_type {
    nvme_tcp_icreq      = 0x0,
    nvme_tcp_icresp     = 0x1,
    nvme_tcp_h2c_term   = 0x2,
    nvme_tcp_c2h_term   = 0x3,
    nvme_tcp_cmd        = 0x4,
    nvme_tcp_rsp        = 0x5,
    nvme_tcp_h2c_data   = 0x6,
    nvme_tcp_c2h_data   = 0x7,
    nvme_tcp_r2t        = 0x9,
};


/* COMMANDS */
struct nvme_sgl_desc {
	uint64_t	addr;
	uint32_t	length;
	uint8_t	rsvd[3];
	uint8_t	type;
};

struct nvme_keyed_sgl_desc {
	uint64_t	addr;
	uint8_t	length[3];
	uint8_t	key[4];
	uint8_t	type;
};

union nvme_data_ptr {
	struct {
		uint64_t	prp1;
		uint64_t	prp2;
	};
	struct nvme_sgl_desc	sgl;
	struct nvme_keyed_sgl_desc ksgl;
};

struct nvmf_connect_command {
	uint8_t		opcode;
	uint8_t		resv1;
	uint16_t	command_id;
	uint8_t		fctype;
	uint8_t		resv2[19];
	union nvme_data_ptr dptr;
	uint16_t	recfmt;
	uint16_t	qid;
	uint16_t	sqsize;
	uint8_t		cattr;
	uint8_t		resv3;
	uint16_t    kato;
	uint8_t		resv4[12];
};


struct nvme_get_log_page_command {
	uint8_t			opcode;
	uint8_t			flags;
	uint16_t			command_id;
	uint32_t			nsid;
	uint64_t			rsvd2[2];
	union nvme_data_ptr	dptr;
	uint8_t			lid;
	uint8_t			lsp; /* upper 4 bits reserved */
	uint16_t			numdl;
	uint16_t			numdu;
	uint16_t			rsvd11;
	union {
		struct {
			uint32_t lpol;
			uint32_t lpou;
		};
		uint64_t lpo;
	};
	uint8_t			rsvd14[3];
	uint8_t			csi;
	uint32_t			rsvd15;
};

struct nvme_create_cq {
	uint8_t			opcode;
	uint8_t			flags;
	uint16_t			command_id;
	uint32_t			rsvd1[5];
	uint64_t			prp1;
	uint64_t			rsvd8;
	uint16_t			cqid;
	uint16_t			qsize;
	uint16_t			cq_flags;
	uint16_t			irq_vector;
	uint32_t			rsvd12[4];
};

struct nvmf_property_set_command {
	uint8_t		opcode;
	uint8_t		resv1;
	uint16_t		command_id;
	uint8_t		fctype;
	uint8_t		resv2[35];
	uint8_t		attrib;
	uint8_t		resv3[3];
	uint32_t		offset;
	uint64_t		value;
	uint8_t		resv4[8];
};

struct nvmf_property_get_command {
	uint8_t		opcode;
	uint8_t		resv1;
	uint16_t		command_id;
	uint8_t		fctype;
	uint8_t		resv2[35];
	uint8_t		attrib;
	uint8_t		resv3[3];
	uint32_t		offset;
	uint8_t		resv4[16];
};

struct nvme_common_command {
	uint8_t			opcode;
	uint8_t			flags;
	uint16_t			command_id;
	uint32_t			nsid;
	uint32_t			cdw2[2];
	uint64_t			metadata;
	union nvme_data_ptr	dptr;
    
    struct_group(cdws,
    	uint32_t			cdw10;
    	uint32_t			cdw11;
    	uint32_t			cdw12;
    	uint32_t			cdw13;
    	uint32_t			cdw14;
    	uint32_t			cdw15;
    );

};

struct nvmf_auth_send_command {
	uint8_t		opcode;
	uint8_t		resv1;
	uint16_t		command_id;
	uint8_t		fctype;
	uint8_t		resv2[19];
	union nvme_data_ptr dptr;
	uint8_t		resv3;
	uint8_t		spsp0;
	uint8_t		spsp1;
	uint8_t		secp;
	uint32_t		tl;
	uint8_t		resv4[16];
};

struct nvme_command {
    union {
        struct nvme_common_command common;
        struct nvmf_connect_command connect;
        struct nvme_get_log_page_command get_log_page;
        struct nvme_create_cq create_cq;
        struct nvmf_property_set_command prop_set;
		struct nvmf_property_get_command prop_get;
        struct nvmf_auth_send_command auth_send;
    };
};


/**
 * struct nvme_tcp_cmd_pdu - nvme tcp command capsule pdu
 *
 * @hdr:           pdu common header
 * @cmd:           nvme command
 */
struct nvme_tcp_cmd_pdu {
	struct nvme_tcp_hdr	hdr;
	struct nvme_command	cmd;
};


/* admin */
enum nvme_admin_opcode {
	nvme_admin_delete_sq		= 0x00,
	nvme_admin_create_sq		= 0x01,
	nvme_admin_get_log_page		= 0x02,
	nvme_admin_delete_cq		= 0x04,
	nvme_admin_create_cq		= 0x05,
	nvme_admin_identify		= 0x06,
	nvme_admin_abort_cmd		= 0x08,
	nvme_admin_set_features		= 0x09,
	nvme_admin_get_features		= 0x0a,
	nvme_admin_async_event		= 0x0c,
	nvme_admin_ns_mgmt		= 0x0d,
	nvme_admin_activate_fw		= 0x10,
	nvme_admin_download_fw		= 0x11,
	nvme_admin_dev_self_test	= 0x14,
	nvme_admin_ns_attach		= 0x15,
	nvme_admin_keep_alive		= 0x18,
	nvme_admin_directive_send	= 0x19,
	nvme_admin_directive_recv	= 0x1a,
	nvme_admin_virtual_mgmt		= 0x1c,
	nvme_admin_nvme_mi_send		= 0x1d,
	nvme_admin_nvme_mi_recv		= 0x1e,
	nvme_admin_dbbuf		= 0x7C,
	nvme_admin_format_nvm		= 0x80,
	nvme_admin_security_send	= 0x81,
	nvme_admin_security_recv	= 0x82,
	nvme_admin_sanitize_nvm		= 0x84,
	nvme_admin_get_lba_status	= 0x86,
	nvme_admin_vendor_start		= 0xC0,
};


enum {
	NVME_QUEUE_PHYS_CONTIG	= (1 << 0),
	NVME_CQ_IRQ_ENABLED	= (1 << 1),
	NVME_SQ_PRIO_URGENT	= (0 << 1),
	NVME_SQ_PRIO_HIGH	= (1 << 1),
	NVME_SQ_PRIO_MEDIUM	= (2 << 1),
	NVME_SQ_PRIO_LOW	= (3 << 1),
	NVME_FEAT_ARBITRATION	= 0x01,
	NVME_FEAT_POWER_MGMT	= 0x02,
	NVME_FEAT_LBA_RANGE	= 0x03,
	NVME_FEAT_TEMP_THRESH	= 0x04,
	NVME_FEAT_ERR_RECOVERY	= 0x05,
	NVME_FEAT_VOLATILE_WC	= 0x06,
	NVME_FEAT_NUM_QUEUES	= 0x07,
	NVME_FEAT_IRQ_COALESCE	= 0x08,
	NVME_FEAT_IRQ_CONFIG	= 0x09,
	NVME_FEAT_WRITE_ATOMIC	= 0x0a,
	NVME_FEAT_ASYNC_EVENT	= 0x0b,
	NVME_FEAT_AUTO_PST	= 0x0c,
	NVME_FEAT_HOST_MEM_BUF	= 0x0d,
	NVME_FEAT_TIMESTAMP	= 0x0e,
	NVME_FEAT_KATO		= 0x0f,
	NVME_FEAT_HCTM		= 0x10,
	NVME_FEAT_NOPSC		= 0x11,
	NVME_FEAT_RRL		= 0x12,
	NVME_FEAT_PLM_CONFIG	= 0x13,
	NVME_FEAT_PLM_WINDOW	= 0x14,
	NVME_FEAT_HOST_BEHAVIOR	= 0x16,
	NVME_FEAT_SANITIZE	= 0x17,
	NVME_FEAT_SW_PROGRESS	= 0x80,
	NVME_FEAT_HOST_ID	= 0x81,
	NVME_FEAT_RESV_MASK	= 0x82,
	NVME_FEAT_RESV_PERSIST	= 0x83,
	NVME_FEAT_WRITE_PROTECT	= 0x84,
	NVME_FEAT_VENDOR_START	= 0xC0,
	NVME_FEAT_VENDOR_END	= 0xFF,
	NVME_LOG_ERROR		= 0x01,
	NVME_LOG_SMART		= 0x02,
	NVME_LOG_FW_SLOT	= 0x03,
	NVME_LOG_CHANGED_NS	= 0x04,
	NVME_LOG_CMD_EFFECTS	= 0x05,
	NVME_LOG_DEVICE_SELF_TEST = 0x06,
	NVME_LOG_TELEMETRY_HOST = 0x07,
	NVME_LOG_TELEMETRY_CTRL = 0x08,
	NVME_LOG_ENDURANCE_GROUP = 0x09,
	NVME_LOG_ANA		= 0x0c,
	NVME_LOG_DISC		= 0x70,
	NVME_LOG_RESERVATION	= 0x80,
	NVME_FWACT_REPL		= (0 << 3),
	NVME_FWACT_REPL_ACTV	= (1 << 3),
	NVME_FWACT_ACTV		= (2 << 3),
};

/*
 * Fabrics subcommands.
 */
enum nvmf_fabrics_opcode {
	nvme_fabrics_command		= 0x7f,
};

enum nvmf_capsule_command {
	nvme_fabrics_type_property_set	= 0x00,
	nvme_fabrics_type_connect	= 0x01,
	nvme_fabrics_type_property_get	= 0x04,
	nvme_fabrics_type_auth_send	= 0x05,
	nvme_fabrics_type_auth_receive	= 0x06,
};


enum {
	NVME_REG_CAP	= 0x0000,	/* Controller Capabilities */
	NVME_REG_VS	= 0x0008,	/* Version */
	NVME_REG_INTMS	= 0x000c,	/* Interrupt Mask Set */
	NVME_REG_INTMC	= 0x0010,	/* Interrupt Mask Clear */
	NVME_REG_CC	= 0x0014,	/* Controller Configuration */
	NVME_REG_CSTS	= 0x001c,	/* Controller Status */
	NVME_REG_NSSR	= 0x0020,	/* NVM Subsystem Reset */
	NVME_REG_AQA	= 0x0024,	/* Admin Queue Attributes */
	NVME_REG_ASQ	= 0x0028,	/* Admin SQ Base Address */
	NVME_REG_ACQ	= 0x0030,	/* Admin CQ Base Address */
	NVME_REG_CMBLOC	= 0x0038,	/* Controller Memory Buffer Location */
	NVME_REG_CMBSZ	= 0x003c,	/* Controller Memory Buffer Size */
	NVME_REG_BPINFO	= 0x0040,	/* Boot Partition Information */
	NVME_REG_BPRSEL	= 0x0044,	/* Boot Partition Read Select */
	NVME_REG_BPMBL	= 0x0048,	/* Boot Partition Memory Buffer
					 * Location
					 */
	NVME_REG_CMBMSC = 0x0050,	/* Controller Memory Buffer Memory
					 * Space Control
					 */
	NVME_REG_CRTO	= 0x0068,	/* Controller Ready Timeouts */
	NVME_REG_PMRCAP	= 0x0e00,	/* Persistent Memory Capabilities */
	NVME_REG_PMRCTL	= 0x0e04,	/* Persistent Memory Region Control */
	NVME_REG_PMRSTS	= 0x0e08,	/* Persistent Memory Region Status */
	NVME_REG_PMREBS	= 0x0e0c,	/* Persistent Memory Region Elasticity
					 * Buffer Size
					 */
	NVME_REG_PMRSWTP = 0x0e10,	/* Persistent Memory Region Sustained
					 * Write Throughput
					 */
	NVME_REG_DBS	= 0x1000,	/* SQ 0 Tail Doorbell */
};
