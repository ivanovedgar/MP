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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/timeb.h>


#include "i2c.h"

int gI2cAddr    = -1;
int gByteCount  = 1;

const char *gAddrStr    = NULL;
const char *gDataStr    = NULL;


#define TRUE    1
#define FALSE   0

#define I2C_NO_CRC 0

// ---- Private Function Prototypes -----------------------------------------

static  void ProcessReadByteCommand( int i2cDev, uint8_t addrStr );
static  void ProcessWriteByteCommand( int i2cDev, uint8_t addrStr, const char *dataStr );

// ---- Functions -----------------------------------------------------------

//***************************************************************************
/**
*   Main entry point
*/

int main( int argc, char **argv )
{
    const char         *i2cDevName = "/dev/i2c-0";
    int                 i2cDev;

    // Try to open the i2c device

    if (( i2cDev = open( i2cDevName, O_RDWR )) < 0 )
    {
        fprintf(stderr,"Error  opening '%s': %s\n", i2cDevName, strerror( errno ));
        exit( 1 );
    }

    // Indicate which slave we wish to speak to
    I2cSetSlaveAddress( i2cDev, gI2cAddr);

    close( i2cDev );

    return 0;

} // main


//***************************************************************************
/**
*   Issues a read byte command to read a byte from a particular address.
*/

void ProcessReadByteCommand( int i2cDev, uint8_t addr )
{
    uint8_t dataByte[ I2C_MAX_DATA_LEN ];
    int     rc;

    if (( rc = I2cReadBytes( i2cDev, addr, dataByte, gByteCount )) != 0 )
    {
        fprintf(stderr, "I2cReadByte failed: %d\n", rc );
        return;
    }

} // ProcessReadByteCommand

//***************************************************************************
/**
*   Issues a read byte command to read a byte from a particular address.
*/

void ProcessWriteByteCommand( int i2cDev, uint8_t addr, const char *dataStr )
{
    uint8_t dataByte[ I2C_MAX_DATA_LEN ];
    uint8_t bytesParsed;
    int     rc;

    if (( rc = I2cWriteBytes( i2cDev, addr, dataByte, bytesParsed )) != 0 )
    {
        fprintf(stderr, "I2cWriteBytes failed: %d\n", rc );
        return;
    }

} // ProcessWriteByteCommand
