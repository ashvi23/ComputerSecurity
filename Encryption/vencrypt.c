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
	char message[plainmsglen]; 
	fseek(plainfptr, 0, SEEK_SET);
	fread(message, plainmsglen+1, 1,plainfptr);
	message[plainmsglen]='\0';
	//printf("plain: %s %ld\n", message,plainmsglen); //del
	
	FILE * fp = NULL;
	fp= fopen(argv[3], "w+");
	if(fp == NULL){
		fprintf(stderr, "cant create/open output file\n");
		return 1;
	}
	//char message[] = "legnthy8";
	//char outputfile = argv[3];
	int msglen = strlen(message); 
	int keylen = strlen(keyfile);
	int i, j;
	char newkey[msglen];
	//printf("%s %s\n", keyfile, message);
	for(i =0, j=0; i<msglen; ++i,++j ){
		if (j==keylen){
			j=0;
		}
		newkey[i] = keyfile[j];		
		
	}
	newkey[msglen] = '\0';
	//printf("%s\n", newkey);
	
	//encryption
	unsigned char encryptedmsg[msglen];
	//convert keystream and message to bytes
    BYTE keystreambyte[msglen];
    BYTE messagebyte[msglen];
    string2ByteArray(newkey, keystreambyte);
    string2ByteArray(message, messagebyte);
     //printing
    //printf("ascii_str: %s\n", newkey);
    //printf("byte array is...\n");
   /* for(i=0; i<msglen; i++)
    {
        printf("%c - %d\n", newkey[i], keystreambyte[i]);
    }
    printf("\n");
    for(i=0; i<msglen; i++)
    {
        printf("%c - %d\n", message[i], messagebyte[i]);
    }
    printf("\n");
   */
	
	for(i =0; i<msglen; i++){
		encryptedmsg[i] = ((messagebyte[i] + keystreambyte[i]))%256;
		//int sum = messagebyte[i] + keystreambyte[i];		
		//printf("sum: %d encrypted: %d\n", messagebyte[i] + keystreambyte[i], encryptedmsg[i]);		
	}
	encryptedmsg[msglen] = '\0';
	
	//printf("encrypted: %s\n", encryptedmsg);
	//printf("done!\n");
	//fputs(encryptedmsg,fp);
	//fprintf(fp, "%hhn", encryptedmsg);
	fwrite(encryptedmsg, 1, sizeof(encryptedmsg), fp);
	fclose(fp);
	return 0;
}
