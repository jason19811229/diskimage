#include "stdafx.h"
#include "windows.h"



int g_bDebug = 1;

void trace( const char *fmt,...)
{
	FILE *log_fp;


	if(!g_bDebug)
		return;

	if( (log_fp = fopen("c:\\commit\\application.log","a+")) == NULL  ){
		int result  =  GetLastError();
		perror("open c:\\application.log err");
		return ;
	}
	va_list args;
	va_start(args, fmt);
	time_t now;
	time(&now);
//	char ch_time[30];
 //	struct tm* time;
//	time = localtime(&now);
//	sprintf(ch_time,"%04d-%02d-%02d %02d:%02d:%02d",time->tm_year+1900,time->tm_mon+1,time->tm_mday, time->tm_hour,time->tm_min,time->tm_sec);
//	fprintf(log_fp,"%s,",ch_time);		
	vfprintf(log_fp,fmt,args);
//	fprintf(log_fp,"\n");
	va_end(args);
	fclose(log_fp);
}
/*
int main(int argc, char **argv)
{
	int i = 0;
	int fd = -1;
	UINT32 sector = START_SECTOR;
	UINT32 size = 0;
	UINT32 chunk = 0;
	UINT32 off = 0;
	UINT32 count = 0;
	if (argc != 3) {
		printf("tgram_sd_tool /dev/sdx image_path\n");
		return -1;
	}
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("open %s error : %d\n", argv[1], fd);
		return -2;
	}
	for (i = 0; i < PART_END_NUM; i++) {
		if (burn_sd(fd, argv[2], i, &sector) != 0) {
			printf("burn sd error path : %s\n", argv[2]);
			close(fd);
			return -3;
		}
	}
	strncpy(partition_image.header.vendor_label, VENDOR_LABEL, MAX_LABEL_SIZE);
	partition_image.header.header_crc = 0xFFFFFFFF;
	partition_image.header.header_size = sizeof(partition_image.header);
	partition_image.header.image_size = sizeof(partition_image.item);
	partition_image.header.image_crc = crc32(0xFFFFFFFF, (UINT8 *)(&(partition_image.item[0])), sizeof(partition_image.item));
	partition_image.header.image_crc ^= 0xFFFFFFFF;
	partition_image.header.enable_crc = ENABLE_HEADER_CRC | ENABLE_IMAGE_CRC;
	strncpy(partition_image.header.image_name, partition_name[PART_TABLE], MAX_FILENAME_SIZE);
	partition_image.header.header_crc = crc32(0xFFFFFFFF, (UINT8 *)(&(partition_image.header)), sizeof(partition_image.header));
	partition_image.header.header_crc ^= 0xFFFFFFFF;
	sector = START_SECTOR;
	while ((chunk = MIN(sizeof(partition_image) - size, SECTOR_SIZE)) > 0) {
		off = SECTOR_SIZE * (sector + count);
		if (off != lseek(fd, off, SEEK_SET)) {
			printf("burn sd error off : %u", off);
			close(fd);
			return -1;
		}
		if (write(fd, ((UINT8 *)(&partition_image)) + size, chunk) != chunk) {
			printf("burn sd error chunk : %u", chunk);
			close(fd);
			return -1;
		}
		size += chunk;
		count++;
	}
	close(fd);
	return 0;
}
*/


