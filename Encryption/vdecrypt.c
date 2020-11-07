#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef unsigned char BYTE;

void string2ByteArray(char* input, BYTE* output)
{
    int loop;
    int i;
    
    loop = 0;
    i = 0;
    
    while(input[loop] != '\0')
    {
        output[i++] = input[loop++];
    }
}

int main(int argc, char *argv[]) {
	if(argc !=4){
			fprintf(stderr, "incorrect number of arguments\n");
			return 1;
	}
	//open keyfile 
	FILE *keyfileptr;
	keyfileptr= fopen(argv[1], "r");
	if (keyfileptr == NULL){
		fprintf(stderr, "cant open keyfile\n");
		return 1;
	}
	//read it 
	fseek(keyfileptr, 0L, SEEK_END);
	long int keyfilelen=ftell(keyfileptr);
	char keyfile[keyfilelen]; 
	fseek(keyfileptr, 0, SEEK_SET);
	fread(keyfile, keyfilelen+1, 1,keyfileptr);
	keyfile[keyfilelen] = '\0';
	//printf("key: %s %ld\n", keyfile,keyfilelen); //del
	
	
	//open encrypted file
	FILE *encryptfp ;
	encryptfp = fopen(argv[2], "r");
	if(encryptfp  == NULL){
		fprintf(stderr, "cant open encrypted file\n");
		return 1;
	}
	//read
	fseek(encryptfp , 0L, SEEK_END);
	long int encryptmsglen=ftell(encryptfp );
	unsigned char encryptmsg[encryptmsglen]; 
	fseek(encryptfp , 0, SEEK_SET);
	fread(encryptmsg, encryptmsglen+1, 1,encryptfp );
	encryptmsg[encryptmsglen]='\0';
	//printf("plain: %s %ld\n", encrptmsg,encrptmsglen); //del
	
	FILE * fp = NULL;
	fp= fopen(argv[3], "w+");
	if(fp == NULL){
		fprintf(stderr, "cant create/open output file\n");
		return 1;
	}
	int i,j;
	//int msglen = strlen(encryptmsg);
	char newkey[encryptmsglen];
	//printf("%s %s\n", keyfile, message);
	for(i =0, j=0; i<encryptmsglen; ++i,++j ){
		if (j==keyfilelen){
			j=0;
		}
		newkey[i] = keyfile[j];		
		
	}
	newkey[encryptmsglen] = '\0';
	
	BYTE keystreambyte[encryptmsglen];
	char msg[encryptmsglen] ;
    string2ByteArray(newkey, keystreambyte);
    for(i =0; i<encryptmsglen; i++){
		msg[i] = ((encryptmsg[i] - keystreambyte[i]))%256;
		//int sum = messagebyte[i] + keystreambyte[i];		
		//printf("sum: %d encrypted: %d\n", messagebyte[i] + keystreambyte[i], encryptedmsg[i]);
		//printf("%s\n", msg);		
	}
	msg[encryptmsglen] = '\0';
	fwrite(msg, 1, sizeof(msg), fp);
	fclose(fp);
	return 1;
	}
	
	
	
	
