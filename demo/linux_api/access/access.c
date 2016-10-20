/********************************************************************
* $ID: access.c             Wed 2016-10-19 17:51:25 +0800  lz       *
*                                                                   *
* Description:                                                      *
*                                                                   *
* Maintainer:  (lizhu)  <lizhu@zhytek.com>                 *
*                                                                   *
* CopyRight (c) 2016 ZHYTEK                                         *
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
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int ret;
	ret = access("1.txt", F_OK);
	if(ret < 0)
	{
		printf("1.txt bu cun zai \n");
		return -1;
	}
	printf("1.txt ok\n");

	return 0;
}



/******************** End Of File: access.c ********************/
