#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>


static unsigned long sdbm(unsigned char *str) ;
typedef unsigned char BYTE;

static unsigned long sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;
	while (c = *str++)
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}

int main(int argc, char *argv[]) {
	//open and validate files
	if(argc !=4){
			fprintf(stderr, "incorrect number of arguments\n");
			return 1;
	}
	//get password
	unsigned char *password = argv[1];

	//open plain file
	
	int x, numRead;
    BYTE buffer[16]; 	
    //cipher
    FILE *fp = fopen(argv[2], "rb");
    if(fp == NULL){
    	fprintf(stderr, "cant open file1\n");
		return 1;
    }
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	//calculate blocks:
	int blocks = fileSize/16;
	printf("blocks: %d file/16: %d\n", blocks, fileSize/16);
	int isFirst =1;
	int blockCount= 0;
	
	//open plain
	FILE * file2= NULL;
	file2= fopen(argv[3], "w+");
	if(file2 == NULL){
		fprintf(stderr, "cant create/open output file/arg[3]\n");
		return 1;
	}
	
	//get seed & IV
	unsigned long seed= sdbm(password);
	printf("using seed: %ld \n", seed);		
	unsigned long iv[16];
	//read 16 bytes from keystream
	for(int i =0; i<16; i++){
		seed = (1103515245*seed +12345)%256;
		iv[i] = seed;
	}
	printf("\nIV: ");
	for(int i=0; i < 16; i++){
		// prints all 16 bytes
		printf(" %ld ", iv[i]);
	}
	/*
		Step1: read block
		Step 2: get 16 bytes keystream data and xor with block
		Step 3: 16 byte swaps in reverse based on keystream bytes
		Step 4: undo chaining by xor block with previous ciphertext 
		
	*/
	//step 1
	int padding;
	BYTE plain[16];
	BYTE cbc[16]; // prev cipher
	unsigned char plaintemp[16];
	for(int blockCount =0; blockCount<blocks; blockCount++){
		printf("\n\n\n\n");
		BYTE temp[16];
		BYTE scrambled[16];
	    unsigned long key[16];
	    
	    // get keystream:
		printf("\nseed:%ld keystream: ", seed);
		for(int v=0; v<16;v++){	
			key[v] = seed = (1103515245*seed +12345)%256;
		    printf(" %lx ", key[v]);
		}
		key[16]='\0' ;
		seed = key[15];
	    
	    //read from file
 		numRead = fread(buffer, 1, 16, fp);
 		printf("\nx:%d/%d numread:%d \n", blockCount,blocks, numRead);
 		printf("encrypted block: ");
 		for(int i=0; i<16; i++){
 			printf(" %x ", buffer[i]);
 		}
   		buffer[16] = '\0';	
	
		// xor with buffer block
		printf("\nXOR:\n");
		for(int i =0; i<16;i++){
			scrambled[i]= buffer[i]^key[i];
			//printf("buf: %x key: %lx scramb: %x\n", buffer[i], key[i], scrambled[i]);
		}
		scrambled[16]='\0';
		printf("\nscrambled: ");
		for(int i =0 ; i<16;i++){
			printf(" %x ", scrambled[i]);
		 }
		printf("\n");
		//swap
		for (int i=15; i >=0; i=i-1){
			BYTE first= key[i] & 0xf;   // lower 4 bits of the keystream
			BYTE second = (key[i] >> 4) & 0xf ;   // top 4 bits of the keystream
			//printf(" first: %d second: %d, scramb[first]: %x scramb[second]: %x\n", first, second, scrambled[first],scrambled[second]);
			//swap
			unsigned char tempByte = scrambled[first];
			scrambled[first] = scrambled[second];
			scrambled[second] = tempByte;
			//printf(" After Swap	      scramb[first]: %x scramb[second]: %x\n", scrambled[first],scrambled[second]);
			
		}
		printf("\nScramb Now: ");
		    for(int m =0; m<16; m++){
		    	printf(" %x ", scrambled[m]);
		   	}
		   	printf("\n");
		//fwrite(scrambled, 1, sizeof(scrambled), file2);
		
		//first block
		if(blockCount==0){
			printf("\n\nFirst Block\n");
			//get seed & IV
			unsigned long seed1= sdbm(password);
			printf("using seed: %ld \n", seed1);		
			unsigned long iv[16];
			//read 16 bytes from keystream
			for(int i =0; i<16; i++){
				seed1 = (1103515245*seed1 +12345)%256;
				iv[i] = seed1;
				printf(" %lu ", iv[i]);
				plain[i]=iv[i]^scrambled[i];
				//printf("plain[%d]: %c scramb[]: %x iv:%lu \n", i, plain[i], scrambled[i], iv[i]);
				
			}
			for(int i=0; i<16; i++){
				cbc[i]= buffer[i];
			}
			//fwrite(plain, 1, sizeof(plain), file2);
			//for(int i =0; i< 16; i++){
			//plaintemp[i] = plain[i];
			//}
		}
		else{
			/*for(int i=0; i< 16; i++){
			printf(" scram[%x] plain[%c]\n", scrambled[i], scrambled[i]^plaintemp[i]);
			
			
			}
			printf("\n plain:");
			for(int i =0 ; i<16;i++){
		    	printf(" %d:[%c] ", i, plaintemp[i]);
		    }*/
		    
		    printf("Decryption block %d\n", blockCount);
			for(int i =0; i< 16;i++){
				plain[i]= cbc[i]^scrambled[i];
				printf("\nplain[%d]: %c scramb[]: %x, buffer[]:%x", i, plain[i], scrambled[i], buffer[i]);
			}
			for(int i=0; i<16; i++){
				cbc[i]= buffer[i];
			}
		}
		
		//last block - remove padding
		if(blockCount +1 == blocks){
			printf("\n\nLast Block\n");
				
			padding = plain[15];
			printf("padding: %d\n", padding);
			if(padding ==16){
				break; // so it doesn't write this block
			}
			else{
				int until = 16 -padding;
				BYTE plain2[until];
				for(int i =0; i<until; i++){
					plain2[i] = plain[i];
					printf("%c", plain2[i]);
				}
				fwrite(plain2, 1, sizeof(plain2), file2);
				return 1;
				
			}
		
		}
		fwrite(plain, 1, sizeof(plain), file2);
		
		
		
	}
	
		



return 1;


}
