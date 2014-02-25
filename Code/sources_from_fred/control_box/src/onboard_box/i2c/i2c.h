/****************************************************************************
 * *
 * *   Copyright (c) 2006 Dave Hylands     <dhylands@gmail.com>
 * *
 * *   This program is free software; you can redistribute it and/or modify
 * *   it under the terms of the GNU General Public License version 2 as
 * *   published by the Free Software Foundation.
 * *
 * *   Alternatively, this software may be distributed under the terms of BSD
 * *   license.
 * *
 * *   See README and COPYING for more details.
 * *
 * ****************************************************************************/

#if !defined( I2C_H )
#define I2C_H               /**< Include Guard                             */

#if !defined( I2C_API_H )
#define I2C_API_H
#endif

/* ---- Include Files ---------------------------------------------------- */

#include <inttypes.h>
#include <linux/types.h>
#include <sys/ioctl.h>

#if !defined( CONFIG_H )
#   include "Config.h"
#endif



/* ---- Constants and Types ---------------------------------------------- */

/**
 *  *  Error Codes
 *   */

#define I2C_ERROR_NONE               0  // No Error
#define I2C_ERROR_ADDR_NACK         -1  // No response to SLA+R/W
#define I2C_ERROR_DATA_NACK         -2  // NACK during data transmission
#define I2C_ERROR_ARBITRATION_LOST  -3  // Lost arbitration
#define I2C_ERROR_BAD_LEN           -4  // Length is wonky
#define I2C_ERROR_BAD_CRC           -5  // CRC failed
#define I2C_ERROR_BUS_ERROR         -6  // Someting weird on the i2c bus

typedef int8_t  I2C_Error_t;

/**
 *  *  Since we're loosely following the SMBus spec, we restrict the amount
 *   *  of data in each transaction to 32 bytes.
 *    */

#define I2C_MAX_DATA_LEN    32

/**
 *  *  I2C_Addr_t can contain the address of any device on the bus. This
 *   *  module only supports 7 bit addressing.
 *    */

typedef uint8_t I2C_Addr_t;

/**
 *  *  We use the high bit of the slave address to determine if CRC's should be
 *   *  used when communicating with a particular slave.
 *    */

#define I2C_USE_CRC 0x80

/**
 *  *  The I2C_CRC macro can be used to remove all CRC support at compile time.
 *   */

#if CFG_I2C_USE_CRC
#   define  I2C_CRC(x)  x
#else

#   define  I2C_CRC(x)
#endif


/**
 *  *  I2C_Data_t encapsulates the data being read or written on the i2c bus.
 *   *  This module follows the SMBus spec, whihch specifies a maximum payload
 *    *  of 32 bytes.
 *     */

typedef struct
{
#if CFG_I2C_USE_CRC
	    uint8_t     m_crc;
#endif

//  For reads, m_len is the number of bytes actually read (doesn't include
//  the CRC - if present). If a block transfer was performed which has a 
 //  length byte, this length will include the length byte.
  uint8_t     m_len;

  // Note: Under SMBus, a block write can consist of a command, a length,
  //       32 bytes of payload, and a CRC.
  // A read response can consist of a length, 32 bytes of data, and a CRC.
    
  uint8_t     m_data[ I2C_MAX_DATA_LEN  + 2]; // +1 for the command, +1 for length
    
} I2C_Data_t;

// ---- Function Prototypes -------------------------------------------------

void I2cSetSlaveAddress
(
    int         i2cDev,     ///< Handle to i2c-dev file
    I2C_Addr_t  i2cAddr);    ///< 7 bit i2c address to use

int I2cTransfer
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    const void *wrData,     ///< Data to write
    uint8_t     wrLen,      ///< Number of bytes to write (or in 0x80 for a block write)
    void       *rdData,     ///< Place to store data read
    uint8_t     rdLen,      ///< Number of bytes to read  (or in 0x80 for a block read)
    uint8_t    *bytesReadp  ///< Place to store number of bytes read 
);

int I2cProcessBlock
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    const void *wrData,     ///< Data to write
    uint8_t     wrLen,      ///< Number of bytes to write
    void       *rdData,     ///< Place to store data read
    uint8_t     rdLen,      ///< Number of bytes to read
    uint8_t    *bytesReadp  ///< Place to store number of bytes read 
);

int I2cReadBlock
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    void       *rdData,     ///< Place to store data read
    uint8_t     rdLen,      ///< Number of bytes to read
    uint8_t    *bytesReadp  ///< Place to store number of bytes read 
);

int I2cReadByte
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    uint8_t    *rdByte      ///< Place to store byte to read
);

int I2cReadBytes
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    uint8_t    *rdByte,     ///< Place to store bytes read
    uint8_t     rdLen       ///< Number of bytes to read
);

int I2cWriteBlock
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    const void *wrData,     ///< Data to write
    uint8_t     wrLen       ///< Number of bytes to write
);

int I2cWriteByte
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    uint8_t     wrByte      ///< Byte to write
);

int I2cWriteBytes
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    const void *wrByte,     ///< Bytes to write
    uint8_t     wrLen       ///< Number of bytes to write
);

//initialises I2C to talk to the specified slave address
void i2c_init(int address);
//closes I2C descriptors
void i2c_deinit();
//write's a single byte
void i2c_write_byte(uint8_t cmd,uint8_t data);
//read's a single byte
uint8_t i2c_read_byte(uint8_t cmd);
//read's multiple bytes
void i2c_read_bytes(uint8_t cmd, uint8_t *data, int numOfBytes);

#ifndef LIB_I2CDEV_H
#define LIB_I2CDEV_H



/* -- i2c.h -- */


/*
 * I2C Message - used for pure i2c transaction, also from /dev interface
 */
struct i2c_msg {
	__u16 addr;	/* slave address			*/
	unsigned short flags;		
#define I2C_M_TEN	0x10	/* we have a ten bit chip address	*/
#define I2C_M_RD	0x01
#define I2C_M_NOSTART	0x4000
#define I2C_M_REV_DIR_ADDR	0x2000
#define I2C_M_IGNORE_NAK	0x1000
#define I2C_M_NO_RD_ACK		0x0800
	short len;		/* msg length				*/
	char *buf;		/* pointer to msg data			*/
};

/* ----- commands for the ioctl like i2c_command call:
 * note that additional calls are defined in the algorithm and hw 
 *	dependent layers - these can be listed here, or see the 
 *	corresponding header files.
 */
				/* -> bit-adapter specific ioctls	*/
#define I2C_RETRIES	0x0701	/* number of times a device address      */
				/* should be polled when not            */
                                /* acknowledging 			*/
#define I2C_TIMEOUT	0x0702	/* set timeout - call with int 		*/


/* this is for i2c-dev.c	*/
#define I2C_SLAVE	0x0703	/* Change slave address			*/
				/* Attn.: Slave address is 7 or 10 bits */
#define I2C_SLAVE_FORCE	0x0706	/* Change slave address			*/
				/* Attn.: Slave address is 7 or 10 bits */
				/* This changes the address, even if it */
				/* is already taken!			*/
#define I2C_TENBIT	0x0704	/* 0 for 7 bit addrs, != 0 for 10 bit	*/

#define I2C_FUNCS	0x0705	/* Get the adapter functionality */
#define I2C_RDWR	0x0707	/* Combined R/W transfer (one stop only)*/
#define I2C_PEC		0x0708	/* != 0 for SMBus PEC                   */

#if 0
#define I2C_ACK_TEST	0x0710	/* See if a slave is at a specific address */
#define I2C_SMBUS	0x0720	/* SMBus-level access */
#endif

/* -- i2c.h -- */

/* This is the structure as used in the I2C_RDWR ioctl call */
struct i2c_rdwr_ioctl_data {
	struct i2c_msg *msgs;	/* pointers to i2c_msgs */
	int nmsgs;		/* number of i2c_msgs */
};

#endif /* LIB_I2CDEV_H */


#endif /* I2C_H */
