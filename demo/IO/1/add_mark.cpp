
#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]){

	
	int len,len_new;		   //ԭʼ���ȣ��͸��ĺ�ĳ���
	unsigned int Checksum = 0; //Ч��λ
	unsigned  char x = 0xFF;

	unsigned char DataBuffer[30<<10];
	unsigned int *ptr = (unsigned int*)DataBuffer;

	unsigned char prompt[44];
	memset(prompt, 0xff, sizeof(prompt));

	char *edition ; //�̼��汾��
	char *date ;	   //����
	char *oldpath="mBox.hex";
	char *newpath="_mBox.hex";

	//sleep(100);     //˯��100ms�ȴ�fromelf����bin�ļ����
	
	//��ȡ����
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
	
	//���ļ�
	FILE * file;
	file=fopen( oldpath, "rb+");  
	if(file == NULL){
		printf("file open error!\n");
		return 0;
	}
	
	len = fread(DataBuffer,1,sizeof(DataBuffer),file); //��ȡ�ļ���DataBuffer��
	printf("file size ->%d \n",len);
	fclose(file);

	//����16�ֽ�
	if (len % 16 != 0){
		//�޸Ĺ����ֽ��ܳ���
		len_new=len + (16 - (len % 16) );
		for (int j = len ; j<len_new; j++){
			DataBuffer[j] = x;
		}
	}else{
		len_new = len;
	}

	//����Ч��λ
	for (int i = 0; i<len_new / 4; i++){      //����ʱ������0xFFҲ�����ȥ
		Checksum += ptr[i];
	}
	printf("Checksum ->%d \n",Checksum);

	//���浽�µ��ļ���
	FILE *fpDest;
	fpDest = fopen(newpath, "wb");
	if(fpDest==NULL){
		printf("Destination file open failure.\n");
		return 0;
	}
	fwrite(DataBuffer, 1, len_new, fpDest);

	
	//д�����ڰ汾��
	for (int h = 0; h<leng ; h++){   
		unsigned char y = n_str[h];
		fwrite((unsigned char *)&y,sizeof(y),1,fpDest);
	}
	
	//д��δ���Ĺ����ļ���С
	fwrite((unsigned char *)&len,sizeof(len),1,fpDest);
	
	//д��Ч��λ
	fwrite((unsigned char *)&Checksum,sizeof(Checksum),1,fpDest);
	
	//д��0xFF��ȫ64λ
	fwrite((unsigned char *)&prompt,sizeof(prompt),1,fpDest);
	
	//�ر��ļ�
	fclose(fpDest);
	printf("OK!!!\n");
	return 0;	
}


