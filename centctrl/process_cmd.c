#include "public.h"
#include "printdef.h"

static const char tpfname[] = "/tmp/fireware.bin";
// 命令处理函数
typedef int (*fcmd_proc)(ProgIPtr prog, int n, NetPtoPtr pcmd);

struct PROCESS_CMD{   // 命令处理的结构体
    unsigned short cmd_num;
    unsigned char command[MAX_CMD_NUM];
    fcmd_proc process_cmd[MAX_CMD_NUM];
}proc_cmd;

// 注册命令处理函数
static void reg_cmd_proc(unsigned char command, fcmd_proc fproc)
{
    proc_cmd.command[proc_cmd.cmd_num] = command;
    proc_cmd.process_cmd[proc_cmd.cmd_num] = fproc;
    proc_cmd.cmd_num += 1;
}

#if 0
static void print_cmd(NetProto cmd) 
{
    PERROR ("cmd.magic0:\t0x%x\n", cmd.magic0);
    PERROR ("cmd.magic1:\t0x%x\n", cmd.magic1);
    PERROR ("cmd.cmdlen:\t%d\n", cmd.cmdlen);
    PERROR ("cmd.command:\t0x%x\n", cmd.command);
    PERROR ("cmd.cmdseq:\t0x%x\n", cmd.cmdseq);
    if (cmd.command > 0x80) {
        PERROR ("cmd.dat[0]\t0x%x\n", cmd.dat[0]);
    }else {
        cmd.dat[cmd.cmdlen-NPTO_HEAD_SIZE] = 0;
        PERROR ("cmd.dat:\t%s\n", cmd.dat);
    }
}
#endif

// 发送反馈命令         connfd: 为连接socket
// pcmd: 是接收到的命令 result: 为接收的结果
// extdat, extlen: 命令的载体数据和长度
static int send_cmd(int connfd, NetPtoPtr pcmd, int result,
                    unsigned char *extdat, int extlen) 
{
    
    NetProto cmd;
    int buflen = NPTO_MAX_SIZE;

    npto_GenerateResp(&cmd, &buflen, pcmd, result, extdat, extlen);

    if(connfd == gProgInfo.ttyfd) {
	if(write(connfd, &cmd, buflen) == buflen)
	    return 0;
	return -1;
    }

    if (send(connfd, &cmd, buflen, 0) == -1) {
        PERROR("Send Error: ");
        return -1;
    }

    return 0;
}


static int get_device_info(unsigned char dat[])
{
    unsigned char hwversion[32];
    unsigned char hwdate[12];
    unsigned char swversion[32];
    unsigned char swdate[12];

    bzero(hwversion, sizeof(hwversion));
    bzero(hwdate, sizeof(hwdate));
    bzero(swversion, sizeof(swversion));
    bzero(swdate, sizeof(swdate));
    
    strcpy((char *)hwversion, "1.1.1");
    strcpy((char *)hwdate, "2012");
    strcpy((char *)swversion, "2.2.2");
    strcpy((char *)swdate, "2011");

    strcpy((char *)dat, (char *)hwversion);
    strcpy((char *)dat+32, (char *)hwdate);
    strcpy((char *)dat+44, (char *)swversion);
    strcpy((char *)dat+76, (char *)swdate);

    return 88;
}

static int store_file(ProgIPtr prog, unsigned char *dat, int len, int offset) 
{
    int i;
    if (prog->sumsize == 0 || offset == 0) { // first time open
        if (prog->fp != NULL) { // 处理传输中途断传的情况
            fclose(prog->fp);
        }
        if ((prog->fp = fopen(tpfname, "w+")) == NULL) {
            PERROR("open file failed:%s\n", tpfname);
            return -1;
        }
        prog->checksum = 0;
        prog->sumsize = 0;
    }
    if(fseek(prog->fp, offset, SEEK_SET) == -1) {
        PERROR("file seek failed!\n");
        return -1;
    }
    if(fwrite(dat, 1, len, prog->fp) != len) {
        PERROR("write file failed!\n");
        return -1;
    }
    //fflush(fp);

    prog->sumsize += len;
//    PERROR("recvlen: %d\toffset:%d, len: %d, reallen: %ld\n", prog->sumsize, offset, len, ftell(prog->fp));
    if (prog->sumsize == offset + len) {
        for (i = 0; i < len; ++i) {
            prog->checksum += dat[i];
        } // 计算sum值
    }
    else {
        prog->sumsize = ftell(prog->fp);
    }
    
    return 0;
}

static void * start_upgrade(void *ptr)
{
    flashcp_main(FIRM_USER_PATH, 0, tpfname, 1, (int *)ptr);
    
    return (void *)0;
}

/*-----------------------Process Command Functions ---------------------------*/ 
// 0x00 0x02
static int proc_resp_succ(ProgIPtr prog, int n, NetPtoPtr pcmd) {
    // 处理接收正常的命令
    //PERROR("recv success!\n");
    return send_cmd(prog->users[n].sockfd, pcmd, DO_CMD_SUCC, NULL, 0);
}

// 0x82
static int proc_heart_resp(ProgIPtr prog, int n, NetPtoPtr pcmd) {
    // 处理心跳反馈命令，更新时间
    //PERROR("Heart beat is responsed! Time is updated!\n");
    prog->users[n].lastheart = time(NULL);
    return 0;
}

// 0x01
static int proc_verify_cmd(ProgIPtr prog, int n, NetPtoPtr pcmd) {

    unsigned char errstr[] = "verify failed";
    // 验证设备ID 
    if (pcmd->command != VERIFY_CMD || strncmp((char *)pcmd->dat, prog->param.sn, 16)) {
        // 验证失败或者命令字不对   
        if (send_cmd(prog->users[n].sockfd, pcmd, VERIFY_ERR,  \
                             errstr, strlen((char *)errstr)) == -1) {
            PERROR("send command error!\n");
        }
        close(prog->users[n].sockfd); 
        bzero(prog->users + n, sizeof(prog->users[n]));

        return -1;
    }

    // 验证成功, 设置验证状态
    prog->users[n].verified = 1;

    // 发送反馈命令
    return send_cmd(prog->users[n].sockfd, pcmd, DO_CMD_SUCC, NULL, 0);
}

// 0x03
static int proc_device_info (ProgIPtr prog, int n, NetPtoPtr pcmd) {
    //print_cmd(*pcmd);
    unsigned char extdat[100];
    unsigned char errstr[] = "get device info failed!";
    //unsigned char typeinfo = pcmd->dat[0];
//fprintf(stderr, "typeinfo: %d\n", pcmd->dat[0]);
    int extlen = 0;
    if ((extlen = get_device_info(extdat)) == -1) {
         return send_cmd(prog->users[n].sockfd, pcmd, DEVICE_ERR, errstr, strlen((char *)errstr));
    }

    return send_cmd(prog->users[n].sockfd, pcmd, DO_CMD_SUCC, extdat, extlen);
}

// 0x04
static int proc_upgrade_data(ProgIPtr prog, int n, NetPtoPtr pcmd) {

    //print_cmd(*pcmd);
    int offset = 0; 
    short length = 0;
    memcpy(&offset, pcmd->dat, 4);
    memcpy(&length, pcmd->dat + 4, 2);

    if (length <= 0) {
        return send_cmd(prog->users[n].sockfd, pcmd, UP_DATA_ERR, NULL, 0);
    }
    
    if (store_file(prog, pcmd->dat+8, length, offset) == -1) {
        return send_cmd(prog->users[n].sockfd, pcmd, UP_DATA_ERR, NULL, 0);
    }

    return send_cmd(prog->users[n].sockfd, pcmd, DO_CMD_SUCC, NULL, 0);
}

// 0x05
static int proc_upgrade_ctrl(ProgIPtr prog, int n, NetPtoPtr pcmd) {
    //print_cmd(*pcmd);
    if (prog->sumsize != 0) {
        prog->sumsize = 0;
        fclose(prog->fp); // close the temp file
        prog->fp = NULL;
    }

    static pthread_t tpid = 0;
    static int percent = 0;

    unsigned char perc = 0;
    unsigned char control = pcmd->dat[4];
    unsigned int checksum = 0;
    memcpy(&checksum, pcmd->dat, 4);
//    PERROR("control: %d,checksum: %d,cksum: %d\n", (int)control, checksum, prog->checksum);

    if (control != 1 && control != 0) { // unknown control command
        return send_cmd(prog->users[n].sockfd, pcmd, UP_CTRL_ERR, &perc, 1);
    }
    if (control == 1) {  // start upgrade
        if (tpid != 0) { // if client click OK buttun twice or more
            pthread_cancel(tpid);
        }
        percent = 0;
        tpid = 0;
        
        if (checksum != prog->checksum) { // checksum is wrong!
//fprintf(stderr, "ckecksum: %d\tprog->checksum%d\n", checksum, prog->checksum);
            return send_cmd(prog->users[n].sockfd, pcmd, UP_CTRL_ERR, &perc, 1);
        }
        if (pthread_create(&tpid, /* create thread failed!*/
                NULL, start_upgrade, (void *)&percent) != 0) {
            return send_cmd(prog->users[n].sockfd, pcmd, UP_CTRL_ERR, &perc, 1);
        }
    }
    
    perc = (unsigned char) percent;
//fprintf (stderr, "percent: %d%%\n", (int)perc);
    return send_cmd(prog->users[n].sockfd, pcmd, DO_CMD_SUCC, &perc, 1);
}

// 查找并处理命令
int process_command(ProgIPtr prog, int n, NetPtoPtr pcmd) {
    int i;
    for (i = 0; i < proc_cmd.cmd_num; ++i) {
        if (proc_cmd.command[i] == pcmd->command) {
//            PFUNC("cmd: 0x%x\n", pcmd->command);
            return proc_cmd.process_cmd[i](prog, n, pcmd);
        }
    }

    return -1;
}


static int proc_mcu_version(ProgIPtr prog, int n, NetPtoPtr pcmd)
{
	FILE * file;
	int len;
//	int i;
//	float num,recv_num;
//	int  recv_time,time;
	unsigned char DataBuffer[25];
	unsigned char version[12];
//	unsigned char date[9];
	unsigned char recv_version[12];
//	unsigned char recv_date[9];
	printf("-----------------\n");
	file = fopen("/lib/ko/version.txt", "r");
	if(file == NULL)
	{
		printf("file open error!\n");
		return -1;
	}
	len = fread(DataBuffer,1,sizeof(DataBuffer),file); //读取文件到DataBuffer中
	printf("file size ->%d \n",len);
	fclose(file);

	memcpy(version, DataBuffer,12);
	memcpy(recv_version, pcmd->dat,12);
/*	for(i =0 ; i<6 ;i++)
	{
		date[i] = DataBuffer[i+6];
	}
	
	for(i =0 ; i<6 ;i++)
	{
		recv_date[i] = pcmd->dat[i+6];
	}
	recv_num = atof(recv_date);
	
	num = atof((char *)version);
	recv_num = atof((char *)recv_version);
	time = atoi((char *)date);
	recv_time = atoi((char *)recv_date);

	printf("num=%f  recv_num=%f  %d  %d\n",num,recv_num,time,recv_time);
*/
	if(memcmp(version,recv_version,12) !=0)
	{
		printf("update wait ok \n");
		prog->iMainState = STATE_UPGRADE;
		prog->upg_offset = 0;
	}

	return 0;
}

static int proc_mcu_rx(ProgIPtr prog, int n, NetPtoPtr pcmd)
{
	unsigned char check;
	check = pcmd->dat[0];

//	PFUNC("check=%x, ofsfet=%d, %d\n",check, prog->upg_offset, pcmd->cmdseq);
	if(check ==0 )
	{
		usleep(10000000);
//	PFUNC("check=%x, ofsfet=%d, %d\n",check, prog->upg_offset, pcmd->cmdseq);
		prog->iMainState = STATE_MCU_VER;
	}
	else if(check ==1)
	{
		prog->upg_offset += 512;
		prog->msecSend = 0;
	}
	PFUNC("check=%x, ofsfet=%d\n",check, prog->upg_offset);
	
	return 0;

}

// 所有的命令都在这里注册
void register_all_command(void) {
#if 0
    // 心跳处理!
    reg_cmd_proc(HEART_CMD,  proc_resp_succ);

    // 认证处理!
    reg_cmd_proc(VERIFY_CMD, proc_verify_cmd);

    // 获取设备信息
    reg_cmd_proc(DEVICE_INFO, proc_device_info);

    // 下发升级数据
    reg_cmd_proc(UPGRADE_DATA, proc_upgrade_data);

    // 固件升级控制
    reg_cmd_proc(UPGRADE_CTRL, proc_upgrade_ctrl);

    // heart response!
    reg_cmd_proc(HEART_RESP, proc_heart_resp);
#endif
	// 检验日期 判断是否升级
	reg_cmd_proc(CMD_GET_VERSION, proc_mcu_version);
	// 判断是否做好升级准备	
	//reg_cmd_proc(CMD_GET_UPGRADE, proc_imax6_wait_update);

	reg_cmd_proc(CMD_GET_UPDATARE, proc_mcu_rx);

}

int check_update_file(ProgIPtr prog)
{
	int ret;
	ret = access(VERSION_FILE, F_OK);
	if(ret < 0)
	{
		PFUNC("VERSION file not exist\n");
		prog->iMainState = STATE_NULL;
		return -1;
	}
	ret = access(HEX_FILE, F_OK);
	if(ret < 0)
	{
		PFUNC("HEX file not exist\n");
		prog->iMainState = STATE_NULL;
		return -2;
	}

	prog->iMainState = STATE_MCU_VER;
	return 0;

}

int SendSerialCmd(ProgIPtr prog, int command, unsigned char *extdat, int extlen)
{
    int ret;
    
    npto_GenerateRaw(prog->sendBuf, &prog->sendBufLen, command, prog->sendseq++, extdat, extlen);

    ret = write(prog->ttyfd, prog->sendBuf, prog->sendBufLen);
    PFUNC("Send %d ret=%d\n", prog->sendBufLen, ret);

    return ret;
}

int firmware_read(char *fname, int offset, void *obuf)
{
	int fd, rdlen = 0;

	if((fd = open(fname, O_RDONLY, NULL)) < 0) {
		PERROR("open fname %s\n",fname);
		return -1;
	}
	lseek(fd, offset, SEEK_SET);
	rdlen = read(fd, obuf, 512);
	PFUNC("offset=%d,rdlen=%d",offset,rdlen);
	close(fd);

	return rdlen;
}

int SendSerialUpgrade(ProgIPtr prog, int command, unsigned char *extdat, int extlen)
{
	char *fname = HEX_FILE;
    int ret;

	extdat = &prog->sendBuf[8];
	if((extlen = firmware_read(fname, prog->upg_offset, extdat)) < 0)
		extlen = 0;

    npto_GenerateRaw(prog->sendBuf, &prog->sendBufLen, command, prog->upg_offset, extdat, extlen);

    ret = write(prog->ttyfd, prog->sendBuf, prog->sendBufLen);
    PFUNC("Send %d ret=%d\n", prog->sendBufLen, ret);

    return ret;
}

