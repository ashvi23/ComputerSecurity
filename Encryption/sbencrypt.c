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
	FILE *plainfptr ;
	plainfptr= fopen(argv[2], "r");
	if(plainfptr == NULL){
		fprintf(stderr, "cant open arg[2] file\n");
		return 1;
	}
	//read
	fseek(plainfptr, 0L, SEEK_END);
	long int plainmsglen=ftell(plainfptr);
	//printf( "Message len %ld", plainmsglen) ;
	unsigned char plain[plainmsglen]; 
	fseek(plainfptr, 0, SEEK_SET);
	fread(plain, plainmsglen, 1,plainfptr);
	plain[plainmsglen]='\0';
	printf("plain: %s", plain);
	
	//open cipher
	FILE * fp1= NULL;
	fp1= fopen(argv[3], "w+");
	if(fp1 == NULL){
		fprintf(stderr, "cant create/open output file/arg[3]\n");
		return 1;
	}

	//calculate byte size of file so u know when you will be at last block
	
        printf("File size is: %ld\n",plainmsglen);
        int padding_size = 16 - (plainmsglen%16);
        //find # of blocks
        int blocks = (plainmsglen/16) + 1;

        printf("padding: %d\nblocks: %d\n", padding_size, blocks);
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
				printf("%d:%ld ", i, iv[i]);
		 }
		int x, numRead;
    	BYTE buffer[16];
    	//get size
    	FILE *fp = fopen(argv[2], "rb");
		fseek(fp, 0, SEEK_END);
		int fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int isFirst =1;
		int blockCount= 0;
    
		for(x = 0; x < blocks; x ++) {
		    numRead = fread(buffer, 1, 16, fp);
		    unsigned char temp[16];
		    unsigned long key[16];
    		unsigned char cipherblock[16];
		    printf("x:%d numread:%d \n", x, numRead);
		    
		    blockCount++;
		    if (numRead < 16) {
		        memset(&buffer[numRead], 0, 16-numRead);
		        printf("last block potentially numread: %d\n", numRead);
		        
		        // need to add block counter, last block could be first block, last block could be exactly 16
		    }
		    printf("blockCount %d blocks %d\n", blockCount, blocks);
		    if(blockCount == blocks){
		    //add padding to buffer
		    //if (numRead == 16){}
				//if(numRead <16){
					for(int i =numRead; i<16; i++ ){
						printf("padding %d \n", 16 - (numRead%16));
						if(numRead == 0){
						buffer[i] = 16;
						}
						else{
						buffer[i] = (16 - (numRead%16));
						}
						
					}
				//}
				printf("\n cipher: ");
				for(int i =0 ; i<16;i++){
			    	printf(" %x ", cipherblock[i]);
			    }
		    }
		    if(isFirst ==1){
		    	isFirst =0;
		    	printf("1st. before shuffle: "); // shuffles specifically with IV
		    	for(int j=0; j<16; j++){
					temp[j] = buffer[j]^iv[j];
					printf(" %x ", temp[j]);
		    	}
		    	seed= iv[15];
		    }
		    else {
		    	/*printf("\nbuffer: ");
				for(int i =0; i<16; i++ ){
					printf(" %x ", buffer[i]);
					
				}*/
				printf("\n cipher: ");
				for(int i =0 ; i<16;i++){
			    	printf(" %x ", cipherblock[i]);
			    }
		    	printf("\nNth. before shuffle: ");
		    	for(int j=0; j<16; j++){
		    		
					temp[j] = buffer[j]^cipherblock[j];
					printf(" %x ", temp[j]);
		    	}
		    }
		    	temp[16]='\0';
		    	BYTE temp2[1+numRead];
		    	//print temp block
		    	printf("\ntemp: ");	
		    	for(int i =0; i< 16; i++){
		    		temp2[i]=temp[i];
		    		printf(" %x ", temp[i]);
		    	}
		    	temp2[numRead] = '\0';
		    	//generate 16 keys aka keystream[] = generatelcg()*16
		    	printf("\nkeystream: ");
		    	
		    	int v;
		    	for(v=0; v<16;v++){	
		    		key[v] = seed = (1103515245*seed +12345)%256;
		    		printf(" %lx ", key[v]);
		    	}
		    	key[16]='\0' ;
		    	//shuffle
		    	//print temp block
		    	printf("\ntemp: ");	
		    	for(int i =0; i< 16; i++){
		    		printf(" %x ", temp2[i]);
		    	}
		    	//take temp[] block and use key[]
		    	int blocksize = sizeof(temp)/sizeof(temp[0]);
		    	printf("\nblocksize: %d\n", blocksize);
		    	for (int i=0; i < 16; i=i+1){
					BYTE first = key[i] & 0xf;   // lower 4 bits of the keystream
					BYTE second = (key[i] >> 4) & 0xf ;   // top 4 bits of the keystream
					printf(" first: %d second: %d, temp2[first]: %x temp2[second]: %x\n", first, second, temp2[first],temp2[second]);
					//swap
					unsigned char tempByte = temp[first];
					temp[first] = temp[second];
					temp[second] = tempByte;
					printf(" After Swap	      temp2[first]: %x temp2[second]: %x\n", temp2[first],temp2[second]);
					printf("\nTemp2 Now: ");
		    		for(int m =0; m<numRead; m++){
		    		printf(" %x ", temp[m]);
		    		}
			    }
			    printf("\n after shuffling: ");
			    for(int i =0 ; i< 16;i++){
			    	printf(" %x ", temp[i]);
			    }
			    
			    for(int i =0; i<16;i++){
			    	cipherblock[i]= temp[i]^key[i];
			    	//printf(" %x ", cipherblock[i]);
			    }
			    cipherblock[16]='\0';
			    printf("\n cipher: ");
			    for(int i =0 ; i<16;i++){
			    	printf(" %x ", cipherblock[i]);
			    }	
		       fwrite(cipherblock, 1, sizeof(cipherblock), fp1);
        	printf("\n num bytes read: %.*s\n", numRead,buffer);
        	
    }
    fclose(fp1);


return 1;


}
