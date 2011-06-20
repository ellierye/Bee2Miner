/*****************************************************************
 * coredrv.h
 *****************************************************************/

#ifndef _COREDRV_H_
#define _COREDRV_H_

/*****************************************************************
 * Included files
 *****************************************************************/

#ifdef __KERNEL__
#include <linux/config.h> /* DEVFS_FS         */
#include <linux/ioctl.h>  /* for ioctl macros */
#include <asm/atomic.h>   /* for atomic flag  */

#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
#else
typedef void * devfs_handle_t;
#endif /* CONFIG_DEVFS_FS */

#endif /* __KERNEL__ */

/*****************************************************************
 * Configuration defaults
 *****************************************************************/

#define COREDRV_NAME      "coredrv"
#define COREDRV_MAX_NAME  32
#define COREDRV_NUM_DEVS  256
#define COREDRV_MAX_DEVS  1024
#define COREDRV_MAJOR     0           /* dynamic */

#define COUNT_REG_WIDTH   4

/*****************************************************************
 * Debugging macros
 *****************************************************************/

#undef PDEBUG             /* undef it, just in case */
#ifdef COREDRV_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "COREDRV: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

/*****************************************************************
 * Ioctl constants
 *****************************************************************/

#define COREDRV_IOC_MAGIC  0xBA

#ifdef __KERNEL__
#define COREDRV_IOC_BASE_RD     _IOR(COREDRV_IOC_MAGIC, 1, unsigned long)
#define COREDRV_IOC_LEN_RD      _IOR(COREDRV_IOC_MAGIC, 2, unsigned long)
#define COREDRV_IOC_WIDTH_RD    _IOR(COREDRV_IOC_MAGIC, 3, unsigned long)
#define COREDRV_IOC_WC_BASE_RD  _IOR(COREDRV_IOC_MAGIC, 4, unsigned long)
#define COREDRV_IOC_RC_BASE_RD  _IOR(COREDRV_IOC_MAGIC, 5, unsigned long)
#define COREDRV_IOC_BASE_WR     _IOW(COREDRV_IOC_MAGIC, 1, unsigned long)
#define COREDRV_IOC_LEN_WR      _IOW(COREDRV_IOC_MAGIC, 2, unsigned long)
#define COREDRV_IOC_WIDTH_WR    _IOW(COREDRV_IOC_MAGIC, 3, unsigned long)
#define COREDRV_IOC_WC_BASE_WR  _IOW(COREDRV_IOC_MAGIC, 4, unsigned long)
#define COREDRV_IOC_RC_BASE_WR  _IOW(COREDRV_IOC_MAGIC, 5, unsigned long)


#define COREDRV_IOC_MINNR  1
#define COREDRV_IOC_MAXNR  5

#else /* __KERNEL__ */

#include <sys/ioctl.h>

#define IOC_BASE_WR     _IOC(_IOC_WRITE, COREDRV_IOC_MAGIC, 1, sizeof(unsigned long))
#define IOC_BASE_RD     _IOC(_IOC_READ, COREDRV_IOC_MAGIC, 1, sizeof(unsigned long))
#define IOC_LENGTH_WR   _IOC(_IOC_WRITE, COREDRV_IOC_MAGIC, 2, sizeof(unsigned long))
#define IOC_LENGTH_RD   _IOC(_IOC_READ, COREDRV_IOC_MAGIC, 2, sizeof(unsigned long))
#define IOC_WIDTH_WR    _IOC(_IOC_WRITE, COREDRV_IOC_MAGIC, 3, sizeof(unsigned long))
#define IOC_WIDTH_RD    _IOC(_IOC_READ, COREDRV_IOC_MAGIC, 3, sizeof(unsigned long))
#define IOC_WC_BASE_WR  _IOC(_IOC_WRITE, COREDRV_IOC_MAGIC, 4, sizeof(unsigned long))
#define IOC_WC_BASE_RD  _IOC(_IOC_READ, COREDRV_IOC_MAGIC, 4, sizeof(unsigned long))
#define IOC_RC_BASE_WR  _IOC(_IOC_WRITE, COREDRV_IOC_MAGIC, 5, sizeof(unsigned long))
#define IOC_RC_BASE_RD  _IOC(_IOC_READ, COREDRV_IOC_MAGIC, 5, sizeof(unsigned long))

#endif /* __KERNEL__ */

/*****************************************************************
 * Data structures
 *****************************************************************/

#ifdef __KERNEL__
typedef struct coredrv {
    int base_mapped;       /* Range currently mapped flag     */
    unsigned long base;    /* Base address of memory region   */
    void *vbase;           /* Virtual base address of region  */
    unsigned long length;  /* Length of memory region (bytes) */
    unsigned long width;   /* Width of access (bytes)         */
    int wc_base_mapped;    /* WC range currently mapped flag  */
    unsigned long wc_base; /* Base address of write count reg */
    void *wc_vbase;        /* Virtual write count reg base    */
    int rc_base_mapped;    /* RC range currently mapped flag  */
    unsigned long rc_base; /* Base address of read count reg  */
    void *rc_vbase;        /* Virtual read count reg base     */
    atomic_t available;    /* Single open flag                */
    devfs_handle_t handle; /* DevFS handle                    */
} coredrv_t;
#endif /* __KERNEL__ */

#endif /* _COREDRV_H_ */
