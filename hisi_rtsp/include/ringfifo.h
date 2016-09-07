struct ringbuf {
    unsigned char *buffer;
	int frame_type;
    int size;
};
int addring (int i);
int ringget(int ch,struct ringbuf *getinfo);
void ringput(int ch,unsigned char *buffer,int size,int encode_type);
void ringfree();
void ringmalloc(int size);
void ringreset();