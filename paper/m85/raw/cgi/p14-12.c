/* ���v�n��: ���{���O�ѽ��_�ɩҼ��g, �w��Ū�̨� Copy �ϥ� */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
typedef struct {        	/* �w�q stdin ��Ƶ��c */
    char *name;  		/* ��ƦW�� */
    char *val;   		/* ��Ƥ��e */
} input;  

char *ReadStdin(FILE *f, char stop, int *len);
void AddToSpace(char *str);
void Convert(char *url);
char *ReadData(char *line, char stop);

main(int argc, char *argv[])
{ 
	input inputs[10000]; 
	register int i,m=0; 
	int len; 
	FILE *fp, *fp1, *fopen();
	char filename1[50], filename2[50], title[50];
	char subject[10][10];
	char id[100][10], password[100][10];
        int score[100][20], sum[100], allsum;
	float avg[100], alltotalavg;
	int rank, number, index, find, j, kind;

	printf("Content-type: text/html%c%c",10,10);
	     len = atoi(getenv("CONTENT_LENGTH"));

	for(i=0; len && (!feof(stdin)); i++) { 
		m=i; 
		inputs[i].val = ReadStdin(stdin,'&',&len); 
		AddToSpace(inputs[i].val); 
		Convert(inputs[i].val); 
		inputs[i].name = ReadData(inputs[i].val,'='); 
	} 
     	strcpy(filename1, "/score/setup/"); /* �ŧi���| */
     	strcpy(filename2, "/score/data/");

     	if (strcmp(inputs[0].val, "�@�~��")==0 && strcmp(inputs[1].val, "�үZ")==0) {        
 	   	strcat(filename1, "1Asubject.dat);
           	strcat(filename2, "1Ascore.dat");
       	}
       	if (strcmp(inputs[0].val, "�@�~��")==0 && strcmp(inputs[1].val, "�A�Z")==0) {      
           	strcat(filename1, "1Bsubject.dat);
           	strcat(filename2, "1Bscore.dat");
        }
       	if (strcmp(inputs[0].val, "�@�~��")==0 && strcmp(inputs[1].val, "���Z")==0) {        
	   	strcat(filename1, "1Csubject.dat);
           	strcat(filename2, "1Cscore.dat");
       	}
       	if (strcmp(inputs[0].val, "�G�~��")==0 && strcmp(inputs[1].val, "�үZ")==0) {        
	        strcat(filename1, "2Asubject.dat);
           	strcat(filename2, "2Ascore.dat");
       	}
       	if (strcmp(inputs[0].val, "�G�~��")==0 && strcmp(inputs[1].val, "�A�Z")==0) {        
	        strcat(filename1, "2Bsubject.dat);
           	strcat(filename2, "2Bscore.dat");
       	}
       	if (strcmp(inputs[0].val, "�G�~��")==0 && strcmp(inputs[1].val, "���Z")==0) {        
	   	strcat(filename1, "2Csubject.dat);
           	strcat(filename2, "2Cscore.dat");
       	}
       	if (strcmp(inputs[0].val, "�T�~��")==0 && strcmp(inputs[1].val, "�үZ")==0) {        
		strcat(filename1, "3Asubject.dat);
           	strcat(filename2, "3Ascore.dat");
       	}
       	if (strcmp(inputs[0].val, "�T�~��")==0 && strcmp(inputs[1].val, "�A�Z")==0) {        
	  	strcat(filename1, "3Bsubject.dat);
           	strcat(filename2, "3Bscore.dat");
       	}
       	if (strcmp(inputs[0].val, "�T�~��")==0 && strcmp(inputs[1].val, "���Z")==0) {        
	   	strcat(filename1, "3Csubject.dat);
           	strcat(filename2, "3Cscore.dat");
       	}
       	if (strcmp(inputs[0].val, "�|�~��")==0 && strcmp(inputs[1].val, "�үZ")==0) {        
		strcat(filename1, "4Asubject.dat);
           	strcat(filename2, "4Ascore.dat");
       	}
       	if (strcmp(inputs[0].val, "�|�~��")==0 && strcmp(inputs[1].val, "�A�Z")==0) {        
		strcat(filename1, "4Bsubject.dat);
           	strcat(filename2, "4Bscore.dat");
       	}
       	if (strcmp(inputs[0].val, "�|�~��")==0 && strcmp(inputs[1].val, "���Z")==0) {        
		strcat(filename1, "4Csubject.dat);
           	strcat(filename2, "4Cscore.dat");
       	}

	fp = fopen(filename1,"r");
	kind= 0;
	while(fscanf(fp, "%s", subject[kind])!= EOF)
		kind=kind+1;
	fclose(fp);

	fp1 = fopen(filename2,"r");
       	allsum = 0;
	for (i=0; i<100; i++)
	{
	   	number = i;
	   	if ( fscanf(fp1, "%s %s", id[i], password[i]) != EOF ) 
		{
              		sum[i]=0;
	      		for (j=0; j < kind; j++) {
		   		fscanf(fp1, "%d", &score[i][j] );
                   		sum[i] += score[i][j];
			}
             		avg[i]= sum[i]/kind;
             		allsum += sum[i];
		}
		else
		     break;
   	}

	fclose(fp1);
	find = 0;

	for (i=0; i < number; i++)
	{
	   if ( strcmp(inputs[2].val, id[i]) == 0 && strcmp(inputs[3].val, password[i])==0 ){
			index = i;
			find = 1;
	   }
	}
	if (find == 0)
	{
	   	printf("<H1>��p! �d�L���H�αK�X���~!</H1>\n");
	   	printf("<H1>�Э��s�A��J�@��!</H1>\n");
	}
	else
	{
	   	rank = 1;
	   	for (i=0; i< number; i++)
			if (sum[index] < sum[i] )
				rank = rank + 1;           
           	alltotalavg = allsum / number;
           	printf("<center><H1>�ǥͦ��Z�d�ߨt��</H1></center>"); 
	   	printf("\n<center>�d�߾ǥ;Ǹ�: %s</center><br>", inputs[2].val);
	   	printf("\n<center>�A�ثe�����Z�p�U:</center><hr>"); 

           	for (i=0; i< kind; i++)
               		printf("%s\t", subject[kind]);
           	printf("�`��<br>\n");

           	for (i=0; i< kind; i++)
               		printf("%d\t", score[index][i]);

          	printf("%d<br>\n", sum[index]);

	   	printf("<hr>�ثe�A���`�����O %0.2f. ���Z�`�����O %0.2f, �A���ƦW�O %d<hr>", avg[index], alltotalavg, rank);

      	}
}    


char *ReadStdin(FILE *f, char stop, int *len)
{ 
    int wsize; 
    char *word; 
    int x;
 
    wsize = 102400; 
    x=0; 
    word = (char *) malloc(sizeof(char) * (wsize + 1)); 

    while(1) { 
	word[x] = (char)fgetc(f); 
	if( x==wsize) { 
	    word[x+1] = '\0'; 
	    wsize+=102400; 
	    word = (char *)realloc(word,sizeof(char)*(wsize+1)); 
	} 
	--(*len); 
	if((word[x] == stop) || (feof(f)) || (!(*len))) { 
	    if(word[x] != stop) x++; 
	    word[x] = '\0'; 
	    return word; 
	} 
	++x; 
    } 
} 

char *ReadData(char *line, char stop) 
{ 
    int i = 0, j; 
    char *word = (char *) malloc(sizeof(char) * (strlen(line) + 1)); 

    for(i=0; ((line[i]) && (line[i] != stop)); i++) 
	word[i] = line[i]; 

    word[i] = '\0'; 
    if(line[i]) ++i; 
    j=0; 

    while(line[j++] = line[i++]); 
    return word; 
} 

char ConvertToHex(char *change) 
{ 
    register char hexdigit;  

    hexdigit = (change[0] >= 'A' ? ((change[0] & 0xdf) - 'A')+10 : (change[0] - '0')); 

    hexdigit *= 16; 

    hexdigit += (change[1] >= 'A' ? ((change[1] & 0xdf) - 'A')+10 : (change[1] - '0')); 

    return(hexdigit); 
} 
 
void Convert(char *data) 
{ 
    register int i,j; 
 
    for(i=0, j=0; data[j]; ++i, ++j) { 
	if((data[i] = data[j]) == '%') { 
	    data[i] = ConvertToHex(&data[j+1]); /* ��S��r�� */
	    j+=2; 
	} 
    } 
    data[i] = '\0'; 
}

void AddToSpace(char *str) 
{ 
    register int i; 
 
    for(i=0; str[i]; i++) 
       if(str[i] == '+') 
	   str[i] = ' ';  /* ��[���令�ťղŸ� */
} 
