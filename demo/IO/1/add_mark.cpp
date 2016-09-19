
#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]){

	
	int len,len_new;		   //原始长度，和更改后的长度
	unsigned int Checksum = 0; //效验位
	unsigned  char x = 0xFF;

	unsigned char DataBuffer[30<<10];
	unsigned int *ptr = (unsigned int*)DataBuffer;

	unsigned char prompt[44];
	memset(prompt, 0xff, sizeof(prompt));

	char *edition ; //固件版本号
	char *date ;	   //日期
	char *oldpath="mBox.hex";
	char *newpath="_mBox.hex";

	//sleep(100);     //睡眠100ms等待fromelf生成bin文件完成
	
	//获取参数
	if (3 == argc){
		edition = argv[1];
		date = argv[2];
	}
	else{
		printf("Incorrect parameter\n");
		return 0;
	} 
	const int leng = strlen(date) + strlen(edition);
	char n_str[13];
	strcpy(n_str, edition);
	strcat(n_str, date);
	
	//打开文件
	FILE * file;
	file=fopen( oldpath, "rb+");  
	if(file == NULL){
		printf("file open error!\n");
		return 0;
	}
	
	len = fread(DataBuffer,1,sizeof(DataBuffer),file); //读取文件到DataBuffer中
	printf("file size ->%d \n",len);
	fclose(file);

	//对齐16字节
	if (len % 16 != 0){
		//修改过后字节总长度
		len_new=len + (16 - (len % 16) );
		for (int j = len ; j<len_new; j++){
			DataBuffer[j] = x;
		}
	}else{
		len_new = len;
	}

	//计算效验位
	for (int i = 0; i<len_new / 4; i++){      //计算时将对齐0xFF也算如进去
		Checksum += ptr[i];
	}
	printf("Checksum ->%d \n",Checksum);

	//保存到新的文件中
	FILE *fpDest;
	fpDest = fopen(newpath, "wb");
	if(fpDest==NULL){
		printf("Destination file open failure.\n");
		return 0;
	}
	fwrite(DataBuffer, 1, len_new, fpDest);

	
	//写入日期版本号
	for (int h = 0; h<leng ; h++){   
		unsigned char y = n_str[h];
		fwrite((unsigned char *)&y,sizeof(y),1,fpDest);
	}
	
	//写入未更改过的文件大小
	fwrite((unsigned char *)&len,sizeof(len),1,fpDest);
	
	//写入效验位
	fwrite((unsigned char *)&Checksum,sizeof(Checksum),1,fpDest);
	
	//写入0xFF补全64位
	fwrite((unsigned char *)&prompt,sizeof(prompt),1,fpDest);
	
	//关闭文件
	fclose(fpDest);
	printf("OK!!!\n");
	return 0;	
}


