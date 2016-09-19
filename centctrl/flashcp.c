/* vi: set sw=4 ts=4: */
/*
 * busybox reimplementation of flashcp
 *
 * (C) 2009 Stefan Seyfried <seife@sphairon.com>
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */

//usage:#define flashcp_trivial_usage
//usage:       "-v FILE MTD_DEVICE"
//usage:#define flashcp_full_usage "\n\n"
//usage:       "Copy an image to MTD device\n"
//usage:     "\n	-v	Verbose"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <errno.h>

#include "public.h"
#include "printdef.h"

#define IH_MAGIC                0x27051956      /* Image Magic Number           */

#define IH_OS_LINUX             5       /* Linux        */
#define IH_ARCH_ARM             2       /* ARM          */

#define IH_TYPE_INVALID         0       /* Invalid Image                */
#define IH_TYPE_STANDALONE      1       /* Standalone Program           */
#define IH_TYPE_KERNEL          2       /* OS Kernel Image              */
#define IH_TYPE_RAMDISK         3       /* RAMDisk Image                */
#define IH_TYPE_MULTI           4       /* Multi-File Image             */
#define IH_TYPE_FIRMWARE        5       /* Firmware Image               */

typedef struct image_header {
    uint32_t        ih_magic;       /* Image Header Magic Number    */
    uint32_t        ih_hcrc;        /* Image Header CRC Checksum    */
    uint32_t        ih_time;        /* Image Creation Timestamp     */
    uint32_t        ih_size;        /* Image Data Size              */
    uint32_t        ih_load;        /* Data  Load  Address          */
    uint32_t        ih_ep;          /* Entry Point Address          */
    uint32_t        ih_dcrc;        /* Image Data CRC Checksum      */
    uint8_t         ih_os;          /* Operating System             */
    uint8_t         ih_arch;        /* CPU architecture             */
    uint8_t         ih_type;        /* Image Type                   */
    uint8_t         ih_comp;        /* Compression Type             */
    uint8_t         ih_name[32];    /* Image Name           */
} image_header_t;
#define image2cpu(v)  \
    ((((v)&0xFF)<<24) | (((v)&0xFF00)<<8) | (((v)&0xFF0000)>>8) | (((v)&0xFF000000)>>24))

// read first data for check
int check_image(unsigned char *buf, unsigned int fsize)
{
    unsigned int magic, size;
    image_header_t *head = (image_header_t*)buf;

    magic  = image2cpu(head->ih_magic);
    size   = image2cpu(head->ih_size);

    if(IH_OS_LINUX != head->ih_os || IH_ARCH_ARM != head->ih_arch 
	    || IH_MAGIC != magic || size > fsize) {
	PERROR("Head,magic:0x%x,size:%d,os=%d,arch=%d,type=%d,name=%s\n",
		magic, size, head->ih_os, head->ih_arch,
		head->ih_type, head->ih_name);

	return -10;
    }
    return 0;
}

static void progress(int mode, int count, int total)
{
    int percent;

    percent = count * 100;
    if (total) percent = (unsigned) (percent / total);

    printf("\r%s: %u/%u (%u%%) ",
	    (mode == 0) ? "Erasing block" : ((mode == 1) ? "Writing kb" : "Verifying kb"),
	    count, total, (unsigned)percent);
    fflush(stdout);
}

ssize_t safe_write(int fd, const void *buf, size_t count)
{       
    ssize_t n;      

    do {                    
	n = write(fd, buf, count);
    } while (n < 0 && errno == EINTR);

    return n;
}  

/*              
 Write all of the supplied buffer out to a file.
 This does multiple writes as necessary.
 Returns the amount written, or -1 on an error.
*/                     
ssize_t full_write(int fd, const void *buf, size_t len)
{                       
    ssize_t cc;             
    ssize_t total;  

    total = 0;      

    while (len) {           
	if((cc = safe_write(fd, buf, len)) < 0) {
	    if (total) {            
		/* we already wrote some! */
		/* user can do another write to know the error code */
		return total;
	    }       
	    return cc;  /* write() returns -1 on failure. */
	}                       

	total += cc;
	buf = ((const char *)buf) + cc;
	len -= cc;
    }       

    return total;
}       

int blockisbad(int fd, long long start)
{
    return ioctl(fd, MEMGETBADBLOCK, &start);
}

int blockmarkbad(int fd, long long start)
{
    int ret = ioctl(fd, MEMSETBADBLOCK, &start);
    return ret;
}

extern int g_check_image;
int flashcp_main(const char *devicename, int devoffset, const char *filename, int quiet, int *r_percent)
{
    int fd_f=-1, fd_d = -1, ret = -2;
    int err_verify = 0;
    int offsetfile, offsetdev;
    struct mtd_info_user mtd;
    struct erase_info_user e;
    struct stat statb;
    unsigned char *buf = NULL, *buf2 = NULL;

    /* open input file and mtd device and do sanity checks */
    if(((fd_f = open(filename, O_RDONLY)) < 0)
	    || ((fd_d = open(devicename, O_SYNC | O_RDWR)) < 0)) {
	PERROR("Open file:%s,%s\n", filename, devicename);
	ret = -2;
	goto errout;
    }

    fstat(fd_f, &statb);

    if (ioctl(fd_d, MEMGETINFO, &mtd) < 0) {
	PERROR("%s is not a MTD flash device", devicename);
	ret = -3;
	goto errout;
    }
    if (statb.st_size > mtd.size) {
	PERROR("%s bigger than %s", filename, devicename);
	ret = -4;
	goto errout;
    }

    if(mtd.erasesize > (4<<20)) {
	PERROR("Mtd Erasesize(%d) error\n", mtd.erasesize);
	ret = -14;
	goto errout;
    }

    buf = (unsigned char*)malloc(mtd.erasesize);
    buf2 = (unsigned char*)malloc(mtd.erasesize);
    if(!buf || !buf2) {
	ret = -15;
	PERROR("malloc buff[%d] for write data error\n", mtd.erasesize);
	goto errout;
    }

    printf("flash file:%s(%lx) to %s(0x%x-0x%x), start...\n", filename, statb.st_size, 
	    devicename, mtd.size, mtd.erasesize);

    if(g_check_image && ((read(fd_f, buf, 512) < 512) || (check_image(buf, statb.st_size) < 0))) {
	PERROR("Error Image\n");
	return -22;
    }

    offsetfile = offsetdev = 0;
    e.start = 0;  e.length = mtd.erasesize;
    err_verify = 0;
    while(1) {
	int count, wr_len, rem, two = 0;
	e.start = offsetdev;
#if 0     /* by danny ZHYTEK on 2013-10-16 */
	if (blockisbad(fd_d, e.start)) {
	    PFUNC("Skip bad block[0x%x]\n", e.start);
	    goto next_block;
	}
#endif

	for(two = 0; two < 2; two += 1) {
	    if (ioctl(fd_d, MEMERASE, &e) < 0) {
		PERROR("erase error at 0x%llx on %s\n", (long long)e.start, devicename);
		// do nothing, no using for markbadblock
	    }
	    else break;
	}

	lseek(fd_f, offsetfile, SEEK_SET);

	// read data
	rem = statb.st_size - offsetfile;
	count = (rem < mtd.erasesize) ? rem : mtd.erasesize;
	count = read(fd_f, buf, count);

	// write data
	lseek(fd_d, offsetdev, SEEK_SET);
	if((wr_len = full_write(fd_d, buf, mtd.erasesize)) < count) {
	    PERROR("write error at 0x%x[0x%x] on %s, "
		    "write returned %d\n", offsetfile, offsetdev, devicename, wr_len);
	}

	// verify data
	usleep(8000);
	lseek(fd_d, offsetdev, SEEK_SET);
	read(fd_d, buf2, count);
	if(memcmp(buf, buf2, count)) {
	    int i, k;

	    err_verify += 1;
	    if(err_verify <= 3) continue;

	    for(i = 0; i < count; i++) {
		if(buf[i] != buf2[i]) break;
	    }

	    PERROR("verification[%d] mismatch at 0x%x:0x%x,blockoffset=%d, last wrlen=%d\n", err_verify,
		    offsetfile, e.start, i, wr_len);

	    for(k = i-2;k < i+10; k++) printf("%02x ", buf[k]);
	    printf(":src\n");

	    for(k = i-2;k < i+10; k++) printf("%02x ", buf2[k]);
	    printf(":dst\n");
	    ioctl(fd_d, MEMERASE, &e);
	    blockmarkbad(fd_d, e.start);
	    goto next_block;
	}

	offsetfile += count;

	rem = ((offsetfile*100) / statb.st_size);
	if(r_percent) *r_percent = rem;
	else progress(1, offsetfile, statb.st_size);

next_block:
	err_verify = 0;
	//PFUNC("offsetfile:%d, devoffset:%d\n", offsetfile, e.start);
	if(offsetfile >= statb.st_size) {
	    ret = 0;
	    break;
	}
	offsetdev += mtd.erasesize;
	if(offsetdev >= mtd.size) {
	    PERROR("Upgrade Error of device size, maybe too much badblock\n");
	    break;
	}
    }

errout:
    if(buf) free(buf), buf = NULL;
    if(buf2)free(buf2), buf2 = NULL;

    if(fd_d > 0) close(fd_d), fd_d = -1;
    if(fd_f > 0) close(fd_f), fd_f = -1;

    return ret;
}

int flashcp_memory_read(const char *devicename, int devoffset,
	char *bufFlag, int bufFlagsize,
	char *inBuf, int iSize)
{
    int fd_d = -1, ret = -2;

    /* open input file and mtd device and do sanity checks */
    if((fd_d = open(devicename, O_SYNC | O_RDWR)) < 0) {
	PERROR("Open device file:%s\n", devicename);
	return (ret = -2);
    }

    lseek(fd_d, devoffset, SEEK_SET);

    if((read(fd_d, bufFlag, bufFlagsize) <= 0)
	||(iSize > 0 && (read(fd_d, inBuf, iSize) <= 0)))
	    ret = -3;
    else ret = 0;

    if(fd_d > 0) close(fd_d), fd_d = -1;

    return ret;
}

int flashcp_memory_write(const char *devicename, int devoffset,
	const char *bufFlag, int bufFlagsize,
	const char *inBuf, int iSize)
{
    int fd_d = -1, ret = -1;
    struct mtd_info_user mtd;
    struct erase_info_user e;
    unsigned char *buf2 = NULL;

    /* open input file and mtd device and do sanity checks */
    if((fd_d = open(devicename, O_SYNC | O_RDWR)) < 0) {
	PERROR("Open device file:%s\n", devicename);
	ret = -2;
	goto errout;
    }

    if (ioctl(fd_d, MEMGETINFO, &mtd) < 0) {
	PERROR("%s is not a MTD flash device", devicename);
	ret = -3;
	goto errout;
    }
    if(mtd.erasesize > (4<<20)) {
	PERROR("Mtd Erasesize(%d) error\n", mtd.erasesize);
	ret = -14;
	goto errout;
    }

    buf2 = (unsigned char*)malloc(mtd.erasesize);
    if(!buf2) {
	ret = -15;
	PERROR("malloc buff[%d] for write data error\n", mtd.erasesize);
	goto errout;
    }

    printf("flash ram:%d to %s(0x%x-0x%x), start...\n", iSize,
	    devicename, mtd.size, mtd.erasesize);

    lseek(fd_d, devoffset, SEEK_SET);

    e.start = 0;  e.length = mtd.erasesize;
    do{
	int two = 0;

	e.start = devoffset;
	// erase
	for(two = 0; two < 2; two += 1) {
	    if (ioctl(fd_d, MEMERASE, &e) < 0) {
		PERROR("erase error at 0x%llx on %s\n", (long long)e.start, devicename);
		// do nothing, no using for markbadblock
	    }
	    else break;
	}

	lseek(fd_d, devoffset, SEEK_SET);
	full_write(fd_d, bufFlag, bufFlagsize);
	if(iSize > 0)
	    full_write(fd_d, inBuf, iSize);

	// verify data
	usleep(8000);

	lseek(fd_d, devoffset, SEEK_SET);
	read(fd_d, buf2, bufFlagsize + iSize);
	if(memcmp(buf2, bufFlag, bufFlagsize) ||
	    (iSize > 0 && memcmp(&buf2[bufFlagsize], inBuf, iSize))) {
	    ioctl(fd_d, MEMERASE, &e);
	    blockmarkbad(fd_d, e.start);
	    break;
	}

	ret = 0;
    }while(0);

errout:
    if(buf2)free(buf2), buf2 = NULL;
    if(fd_d > 0) close(fd_d), fd_d = -1;

    return ret;
}



