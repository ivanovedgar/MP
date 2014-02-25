/****************************************************************************
*
*   Copyright (c) 2006 Dave Hylands     <dhylands@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   Alternatively, this software may be distributed under the terms of BSD
*   license.
*
*   See README and COPYING for more details.
*
****************************************************************************/


// ---- Include Files -------------------------------------------------------

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


#include "i2c.h"


// ---- Public Variables ----------------------------------------------------

// ---- Private Constants and Types -----------------------------------------

// ---- Private Variables ---------------------------------------------------

static  I2C_Addr_t  gI2cAddr;
int i2cDev;

// ---- Private Function Prototypes -----------------------------------------

// ---- Functions -----------------------------------------------------------


//***************************************************************************
/**
*
*   Sets the I2C address that we'll be communicating with, as well as whether
*   the device uses smbus PEC (CRC).
*/

void I2cSetSlaveAddress( int i2cDev, I2C_Addr_t i2cAddr)
{
    gI2cAddr = i2cAddr;

    // Indicate which slave we wish to speak to

    if ( ioctl( i2cDev, I2C_SLAVE, gI2cAddr ) < 0 )
    {
        fprintf(stderr, "I2cSetSlaveAddress: Error trying to set slave address to 0x%02x (%d %s)\n", gI2cAddr, errno, strerror( errno ));
    }

} // I2cSetSlaveAddress

//***************************************************************************
/**
*   Transfer data to/from an i2c device.
*
*   This function implements the equivalent of the smbus functions using
*   I2C_RDWR.
*
*   The PXA driver doesn't support the smbus transfers.
*
*   This function can perform the following SMBUS transactions:
*
*       Write Byte:     wrLen == 1,                 rdLen == 0
*       Read Byte:      wrLen == 0,                 rdLen == 1
*       Write Word:     wrLen == 2,                 rdLen == 0
*       Read Word:      wrLen == 0,                 rdLen == 2
*       Process Call:   wrLen == 2,                 rdLen == 2
*       Write Block:    wrLen == 0x80 + numBytes,   rdLen == 0
*       Read Block:     wrLen == 0,                 rdLen == 0x80 + numBytes
*       Process Block:  wrLen == 0x80 + numBytes,   rdLen == 0x80 + numBytes
*/

int I2cTransfer
(
    int         i2cDev,        ///< Handle to i2c-dev file
    uint8_t     cmd,           ///< Command to send
    const void *wrData,        ///< Data to write
    uint8_t     wrLen,         ///< Number of bytes to write (or in 0x80 for a block write)
    void       *rdData,        ///< Place to store data read
    uint8_t     rdLen,         ///< Number of bytes to read  (or in 0x80 for a block read)
    uint8_t    *bytesReadp     ///< Place to store number of bytes read 
)
{
    struct i2c_rdwr_ioctl_data  rdwr;
    struct i2c_msg              msg[ 2 ];
    uint8_t                     wrBuf[ I2C_MAX_DATA_LEN + 3 ];  // +1 for cmd, +1 for len, +1 for CRC
    uint8_t                     rdBuf[ I2C_MAX_DATA_LEN + 2 ];  // +1 for len, +1 for CRC
    uint8_t                     wrBlock = (( wrLen & 0x80 ) != 0 );
    uint8_t                     rdBlock = (( rdLen & 0x80 ) != 0 );
    int                         rc = 0;


    rdLen &= 0x7f;
    wrLen &= 0x7f;

    if ( bytesReadp != NULL )
    {
        *bytesReadp = 0;
    }

    if ( wrLen > I2C_MAX_DATA_LEN ) 
    {
        fprintf(stderr, "I2cTransfer: wrLen too big: %d, max is %d\n",
                  wrLen, I2C_MAX_DATA_LEN );
        errno = ENOBUFS;
        return -1;
    }

    if ( rdLen > I2C_MAX_DATA_LEN )
    {
        fprintf(stderr, "I2cTransfer: rdLen too big: %d, max is %d\n",
                  rdLen, I2C_MAX_DATA_LEN );
        errno = ENOBUFS;
        return -1;
    }

    // Whether we're doing a read or a write, we always send
    // the command.

    msg[ 0 ].addr  = gI2cAddr;
    msg[ 0 ].flags = 0;
    msg[ 0 ].len   = wrLen + 1 + wrBlock;    // +1 for cmd
    msg[ 0 ].buf   = &wrBuf[ 0 ];


    wrBuf[ 0 ] = cmd;

    if ( wrLen > 0 )
    {
        // We have some data to send down to the device

        if ( wrBlock )
        {
            wrBuf[ 1 ] = wrLen;
            memcpy( &wrBuf[ 2 ], wrData, wrLen );
            wrLen++;    // Add in cmd to the length
        }
        else
        {
            memcpy( &wrBuf[ 1 ], wrData, wrLen );
        }
    }

    rdwr.msgs = msg;
    rdwr.nmsgs = 1;

    if ( rdLen > 0 )
    {
        // We're expecting some data to come back

        msg[ 1 ].addr  = gI2cAddr;
        msg[ 1 ].flags = I2C_M_RD;
        msg[ 1 ].len   = rdLen + rdBlock;
        msg[ 1 ].buf   = &rdBuf[ 0 ];

        rdwr.nmsgs = 2;

    }

    if ( ioctl( i2cDev, I2C_RDWR, &rdwr ) < 0 )
    {
        fprintf(stderr, "I2cTransfer: ioctl failed to 0x%02x: %s (%d)\n",gI2cAddr, strerror( errno ), errno );
        return -1;
    }

    if ( rdLen > 0 )
    {
        if ( rdBlock )
        {
            if ( rdBuf[ 0 ] > rdLen )
            {
                fprintf(stderr, "I2cTransfer: length is too big: %d max: %d\n", rdBuf[ 0 ], rdLen );

                rc = EMSGSIZE;
            }
            else
            {
                rdLen = rdBuf[ 0 ];
            }
        }
        
        memcpy( rdData, &rdBuf[ rdBlock ], rdLen );

        if ( bytesReadp != NULL )
        {
            *bytesReadp = rdLen;
        }
    }
    return rc;

} // I2cTransfer



//***************************************************************************
/**
*   Uses the SMBUS Read-Byte protocol to read a byte.
*/

int I2cReadByte(
    int      i2cDev,        ///< Handle to i2c-dev file
    uint8_t  cmd,           ///< Command to send
    uint8_t *rdByte         ///< Place to store byte read
)
{
    return I2cTransfer( i2cDev, cmd, NULL, 0, rdByte, 1, NULL );

} // I2cReadByte

//***************************************************************************
/**
*   Reads an array of bytes usinng i2c (not compatible with SMBUS)
*/

int I2cReadBytes
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    uint8_t    *rdByte,     ///< Place to store bytes read
    uint8_t     rdLen       ///< Number of bytes to read
)
{
    return I2cTransfer( i2cDev, cmd, NULL, 0, rdByte, rdLen, NULL );

} // I2cReadBytes


//***************************************************************************
/**
*   Uses the SMBUS Write-Byte protocol to write a byte.
*/

int I2cWriteByte
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    uint8_t     wrByte      ///< Byte to write
)
{
    return I2cTransfer( i2cDev, cmd, &wrByte, 1, NULL, 0, NULL );

} // I2cWriteByte

//***************************************************************************
/**
*   Writes an array of bytes using i2c (not compatible with SMBUS)
*/

int I2cWriteBytes
(
    int         i2cDev,     ///< Handle to i2c-dev file
    uint8_t     cmd,        ///< Command to send
    const void *wrByte,     ///< Bytes to write
    uint8_t     wrLen       ///< Number of bytes to write
)
{
    return I2cTransfer( i2cDev, cmd, wrByte, wrLen, NULL, 0, NULL );

} // I2cWriteBytes

/*----------easy to use versions of functions--------------*/

void i2c_init(int address)
{
    const char *i2cDevName = "/dev/i2c-0";

    // Try to open the i2c device
    if (( i2cDev = open( i2cDevName, O_RDWR )) < 0 )
    {
        fprintf(stderr,"Error  opening '%s': %s\n", i2cDevName, strerror( errno ));
        exit( 1 );
    }

    // Indicate which slave we wish to speak to
    I2cSetSlaveAddress( i2cDev, address);
}

void i2c_deinit()
{
    close(i2cDev);
}

void i2c_write_byte(uint8_t cmd,uint8_t data)
{
   uint8_t rc;
   if (( rc = I2cWriteByte ( i2cDev, cmd, data)) != 0 )
   {
        fprintf(stderr, "\n\nI2cWriteBytes failed: %d\n\n\n", rc );
        //exit(1);
   }
}

uint8_t i2c_read_byte(uint8_t cmd)
{
    uint8_t dataByte;
    uint8_t rc;
    if (( rc = I2cReadByte( i2cDev, cmd, &dataByte)) != 0 )
    {
        fprintf(stderr, "\n\nI2cReadByte failed: %d\n\n\n", rc );
        //exit(1);
        return -1;
    }
    return dataByte;
}

void i2c_read_bytes(uint8_t cmd, uint8_t *data, int numOfBytes)
{
    uint8_t rc;
    if (( rc = I2cReadBytes( i2cDev, cmd, data,numOfBytes)) != 0 )
    {
        fprintf(stderr, "\n\nI2cReadByte failed: %d\n\n\n", rc );
        //exit(1);
        return -1;
    }
}
