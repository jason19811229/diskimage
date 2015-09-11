#ifndef _TGRAM_IMAGE_H_
#define _TGRAM_IMAGE_H_

#define VENDOR_LABEL "tangram"
#define MAX_LABEL_SIZE 8
#define MAX_FILENAME_SIZE 16

#define ENABLE_HEADER_CRC 0x01
#define ENABLE_IMAGE_CRC 0x80


#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define SECTOR_SIZE             (512)
#define START_SECTOR            (8192)
#define PATH_MAX                (256)

#pragma pack(1)
struct tangram_image_header {
	unsigned char vendor_label[MAX_LABEL_SIZE];
	unsigned int header_crc;
	unsigned int header_size;
	unsigned int image_size;
	unsigned int load_addr;
	unsigned int entry_addr;
	unsigned int created_clock;
	unsigned int created_day;
	unsigned int created_year;
	unsigned int image_crc;
	unsigned char enable_crc;
	unsigned char flash_type;
	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char image_name[MAX_FILENAME_SIZE];
};
struct partition_item {
	unsigned int index;
	unsigned int sector_size_in_bytes;
	unsigned int start_sector;
	unsigned int size_in_kb;
	unsigned int size_in_sector;
	unsigned int is_emcc;
	unsigned int ecc_strategy;
	unsigned int flags;
};
#pragma pack()

#define IMAGE_HEADER_SIZE (64)

#if IMAGE_HEADER_SIZE > 512
#error "struct tangram_image_header size out of range !"
#endif

enum flash_enum {
	FLASH_EMMC      = 0x00,
	FLASH_NAND      = 0x01,
	FLASH_SD        = 0x02,
	FLASH_RAM       = 0x03,
	FLASH_UNKNOW    = 0xff,
};

#define PART_TABLE_NAME         "partition.img"
#define PART_MCU_PBL_NAME       "mcu-pbl.img"
#define PART_CPU_PBL_NAME       "cpu-pbl.img"
#define PART_RTOS_NAME          "rtos.img"
#define PART_BL_NAME            "bootloader.img"
#define PART_SHARED_NAME        "FLASH_S"
#define PART_RESERVED_NAME      "RESERVED"
#define PART_KERNEL_NAME        "boot.img"
#define PART_FILESYS_NAME       "system.img"
#define PART_PERSIST_NAME       "persist.img"
#define PART_CACHE_NAME         "cache.img"
#define PART_RECOVERY_NAME      "recovery.img"
#define PART_USERDATA_NAME      "userdata.img"
#define PART_MBR_NAME           "mbr.img"
#define PART_PTA_NAME           "gpt.img"

enum partition_num {
	PART_TABLE      = 0,
	PART_TABLE_B    = 1,
	PART_MCU_PBL    = 2,
	PART_MCU_PBL_B  = 3,
	PART_CPU_PBL    = 4,
	PART_CPU_PBL_B  = 5,
	PART_RTOS       = 6,
	PART_RTOS_B     = 7,
	PART_BL         = 8,
	PART_BL_B       = 9,
	PART_SHARED     = 10,
	PART_RESERVED   = 11,
	PART_KERNEL     = 12,
	PART_FILESYS    = 13,
	PART_PERSIST    = 14,
	PART_CACHE      = 15,
	PART_RECOVERY   = 16,
	PART_USERDATA   = 17,
	PART_END_NUM    = 18,
};


#define PARTITION_ITEM_SIZE (sizeof(struct partition_item))


void trace( const char *fmt,...);

//UINT32 crc32(UINT32 crc, const UINT8 *buf, UINT32 len);

//int burn_sd(FILE *outstream, const char *path, int part, UINT32 *sector);

#endif  /* _TGRAM_IMAGE_H_ */

