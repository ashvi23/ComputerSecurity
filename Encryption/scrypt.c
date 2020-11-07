#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned long generatelcg(unsigned long x);
static unsigned long sdbm(unsigned char *str) ;

static unsigned long sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;
	while (c = *str++)
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}

unsigned long generatelcg(unsigned long x){
	unsigned long next = (1103515245*x +12345)%256;
	return next;
}


int main(int argc, char *argv[]) {
	if(argc !=4){
			fprintf(stderr, "incorrect number of arguments\n");
			return 1;
	}
	//get password
	unsigned char *password = argv[1];

	//open plain file
	FILE *plainfptr ;
	plainfptr= fopen(argv[2], "r");
	if(plainfptr == NULL){
		fprintf(stderr, "cant open plain text file\n");
		return 1;
	}
	//read
	fseek(plainfptr, 0L, SEEK_END);
	long int plainmsglen=ftell(plainfptr);
	unsigned char plain[plainmsglen]; 
	fseek(plainfptr, 0, SEEK_SET);
	fread(plain, plainmsglen+1, 1,plainfptr);
	plain[plainmsglen]='\0';
	
	//open cipher
	FILE * fp = NULL;
	fp= fopen(argv[3], "w+");
	if(fp == NULL){
		fprintf(stderr, "cant create/open output file\n");
		return 1;
	}
	//get first seed
	unsigned long seed= sdbm(password);
	unsigned char xor[plainmsglen]; //initialize cipher array
	unsigned long key = generatelcg(seed);
	//get keystream
	for (int i =0 ; i< plainmsglen; i++){
		//xor key and plainmsg
		xor[i] = key^plain[i];
		key = generatelcg(key);
				
	}
	xor[plainmsglen]='\0';
	
	//write to file
	fwrite(xor, 1, sizeof(xor), fp);

	return 1;
}
