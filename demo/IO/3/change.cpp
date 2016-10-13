#include<stdio.h>

int main()
{
	char buffer[1024];
	unsigned char hex[16];
	unsigned char num1,num2,num3,num4,num5,num6,num7,num8,num9,num10,num11,num12,num13,num14,num15,num16;
	char bs[6];
	char bs1[20];

	FILE * file;
	file=fopen( "boot1.read", "rb+");  
	if(file == NULL){
		printf("file open error!\n");
		return 0;
	}
	FILE * file1;
	file1=fopen( "tt.read", "ab+");  
	if(file1 == NULL){
		printf("file open error!\n");
		fclose(file);
		return 0;
	}
	while(fgets(buffer, 1024, file) != NULL){
		sscanf(buffer,"%bs %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x    %s",s,&num1,&num2,&num3,&num4,&num5,&num6,&num7,&num8,&num9,&num10,&num11,&num12,&num13,&num14,&num15,&num16,bs1);
		hex[0]=num1;
		hex[1]=num2;
		hex[2]=num3;
		hex[3]=num4;
		hex[4]=num5;
		hex[5]=num6;
		hex[6]=num7;
		hex[7]=num8;
		hex[8]=num9;
		hex[9]=num10;
		hex[10]=num11;
		hex[11]=num12;
		hex[12]=num13;
		hex[13]=num14;
		hex[14]=num15;
		hex[15]=num16;
		
		fwrite((unsigned char *)&hex,sizeof(hex),1,file1);
	}
	
	fclose(file1);
	fclose(file);
	return 0;
	
}