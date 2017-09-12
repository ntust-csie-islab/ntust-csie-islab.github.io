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
   time_t Ourtime;    
   FILE *fp, *fopen();
   printf("Content-type: text/html%c%c",10,10);
   len = atoi(getenv("CONTENT_LENGTH"));

   for(i=0; len && (!feof(stdin)); i++) { 
	m=i; 
       inputs[i].val = ReadStdin(stdin,'&',&len); 
       AddToSpace(inputs[i].val); 
       Convert(inputs[i].val);  
       inputs[i].name = ReadData(inputs[i].val,'='); 
   } 

   fp = fopen(inputs[0].val,"a");  /* �}�ǥ͸���� */
   fputs(inputs[1].val, fp);  /* �g�J�ǥ;Ǹ� */
   fputs(" ", fp);
   fputs(inputs[2].val, fp);  /* �g�J��妨�Z */
   fputs(" ", fp);
   fputs(inputs[3].val, fp);  /* �g�J�^�妨�Z */
   fputs(" ", fp);
   fputs(inputs[4].val, fp);  /* �g�J�ƾǦ��Z */
   fputs(" ", fp);
   fputs(inputs[5].val, fp);  /* �g�J�T���D�q���Z */
   fputs(" ", fp);
   fputs(inputs[6].val, fp);  /* �g�J����q�v���Z */
   fputs("\n ", fp);
   fclose(fp);
   printf("<H1>�Ǹ� %s ���Z��J����!</H1>", inputs[1].val); 

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
	    data[i] = ConvertToHex(&data[j+1]); /* ��S���r�� */
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