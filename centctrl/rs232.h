/********************************************************************
* $ID: rs232.h    二, 04 10月 2011 22:29:03 +0800  feijianhf        *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  王刚(danny.wang)  <danny.wang@besovideo.com>         *
*                                                                   *
* CopyRight (c)  2011  besovideo                                    *
*   All rights reserved.                                            *
*                                                                   *
* This file is free software;                                       *
*   you are free to modify and/or redistribute it                   *
*   under the terms of the GNU General Public Licence (GPL).        *
*                                                                   *
* Last modified:                                                    *
*                                                                   *
* No warranty, no liability, use this at your own risk!             *
********************************************************************/
#ifndef  __RS232_DEF_H
#define  __RS232_DEF_H


//long GetBaudRate(long baudRate);
int OpenCom(const char *deviceName, long baudRate, int parity,
    int dataBits, int stopBits, int flow, int iqSize, int oqSize);
int CloseCom(int fd);

#endif// __RS232_DEF_H
/********************* End Of File: rs232.h *********************/
