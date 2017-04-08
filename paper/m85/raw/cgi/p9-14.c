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
	input inputs[10000];  /* �]�w�̤j��J�O 10000 ����� */
	register int i,m=0; 
	int len; 

	FILE *fp, *fp1, *fopen();

	/* �]�w�d�ߨt�ΤW�һݭn������ܼ� */

	char filename[50], title[50];
	char readrule[10][10];
	int num[10], percent[10];
	char id[100][10], password[100][10];
	int home[100][10];
	float homeavg, homescore;
	int small[100][10];
	float smallavg, smallscore;
	int test[100][10];
	float testavg, testscore;
	int middle[100][10], final[100][10];
	float t_home,t_small, t_test, t_middle, t_final;
	float  midscore, totmidscore, finalscore, totalscore[100], alltotalscore, alltotalavg;
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
 
	strcpy(filename, "/score/setup/");
	strcat(filename, inputs[1].val);
	fp = fopen(filename,"r");   /* �}���Z�]�w�� */
	kind= 0;
	while(fscanf(fp, "%s %d %d", readrule[kind], &num[kind], &percent[kind])!= EOF) /* Ū�����Z���س]�w��� */
		kind=kind+1;
	fclose(fp);

	strcpy(filename, "/score/data/");
	strcat(filename, inputs[2].val);

	fp1 = fopen(filename,"r");   /* �}���Z����� */

	alltotalscore = 0;

	for (i=0; i<100; i++)
	{
		number = i;
		if ( fscanf(fp1, "%s %s", id[i], password[i]) != EOF )
		{
			for (j=0, t_home=0.0; j < num[0]; j++)
			{ /* Ū���@�~���Z */
				 fscanf(fp1, "%d", &home[i][j] );  
				 t_home += home[i][j];
			}
			for (j=0, t_small=0.0; j < num[1]; j++)
			{ /* Ū���p�Ҧ��Z */
			 	fscanf(fp1, "%d", &small[i][j] );  
			 	t_small += small[i][j];
			}
			for (j=0, t_test=0.0; j < num[2]; j++)
			{ /* Ū����Ҧ��Z */
			 	fscanf(fp1, "%d", &test[i][j] );  			
	            		t_test += test[i][j];
			}
			for (j=0, t_middle=0.0; j < num[3]; j++)
			{ /* Ū�������Ҧ��Z*/ 
				fscanf(fp1, "%d", &middle[i][j] );  
				t_middle += middle[i][j];
			}
			for (j=0,t_final=0.0;j < num[4]; j++)
			{ /* Ū�������Ҧ��Z */
				fscanf(fp1, "%d", &final[i][j] ); 
				t_final += final[i][j];
			}

          		/* �p��ǥ��`�����Z */
			if (num[0]>0)
			  	totalscore[i] += t_home/num[0] * percent[0]/100;
			if (num[1]>0)
			  	totalscore[i] += t_small/num[1] * percent[1]/100;
			if (num[2]>0)
			  	totalscore[i] += t_test/num[2] * percent[2]/100;
			if (num[3]>0)
			  	totalscore[i] += t_middle/num[3] * percent[3]/100;
			if (num[4]>0)
			  	totalscore[i] += t_final/num[4] * percent[4]/100;
			alltotalscore += totalscore[i];
		}
		else
		  break;
	}
	fclose(fp1);

	find = 0;
	for (i=0; i < number; i++)
	{  /* �ˬd�ǥͩm�W�αK�X */
		if ( strcmp(inputs[3].val, id[i]) == 0 && strcmp(inputs[4].val, password[i])==0 )  		{
			index = i;
			find = 1;
		}
	}
	if (find == 0) /* �Y�P��Ƥ��šA �h��ܬd�L���H�αK�X���~ */
	{
		printf("<H1>��p! �d�L���H�αK�X���~1</H1>\n");
		printf("<H1>�Э��s��J�@��!</H1>\n");
	}
	else
	{
		rank = 1;
		for (i=0; i< number; i++)
			if (totalscore[index] < totalscore[i] )
				rank = rank + 1;  /* �p��ӥͦb���Z�����Z�ƦW */
		homeavg = 0;
		for (i=0; i < num[0]; i++) /* �p��@�~���� */
			homeavg = homeavg + home[index][i]; 
		homeavg = homeavg / num[0];
		homescore =((homeavg * percent[0])/100); 
		smallavg = 0;
		for (i=0; i < num[1]; i++) /* �p��p�ҥ��� */
			smallavg = smallavg + small[index][i];
		smallavg = smallavg / num[1];
		smallscore =((smallavg * percent[1])/100); 
		testavg = 0;
		for (i=0; i < num[2]; i++) /* �p���ҥ��� */
			testavg = testavg + test[index][i];
		testavg = testavg / num[2];
		testscore =((testavg * percent[2])/100); 
		midscore = 0;
		for (i=0; i < num[3]; i++) /*�p������ҥ���*/
			midscore = midscore + middle[index][i];
		midscore = midscore / num[3];
		totmidscore =((midscore * percent[3])/100); 
		finalscore = 0;
		for (i=0; i < num[4]; i++) /*�p������ҥ���*/
			finalscore = finalscore + final[index][i];
		finalscore = finalscore / num[4];
		finalscore =((finalscore * percent[4])/100);

 		alltotalavg = alltotalscore / number; /* �p���`���� */
		
		strcpy(title, inputs[0].val);
		printf("\n<center><H1>�ǥͦ��Z�d�ߨt��</H1></center>"); 
		printf("\n<center><H3>�d�߬��: %s</H3></center>", title);
		printf("\n<HR><center><H3>�A�ثe�����Z�p�U: </H3></center>");

		if ( num[0] > 0)
		{   /* ��ܸӥͧ@�~���Z */
			printf("\n<H3>%s:</H3><pre>\n", readrule[0]);
			for (i=0; i< num[0]; i++)
				printf("��%d��\t", i+1);
			printf("����\t�@�o����(���`�� %d)\n", percent[0]);
			for (i=0; i< num[0]; i++)
				printf(" %d \t", home[index][i]);
			printf(" %0.2f \t %0.2f", homeavg, homescore);
			printf("\n</pre>");
		}
		if (num[1] > 0)
		{   /* ��ܸӥͤp�Ҧ��Z */
			printf("\n<H3>%s:</H3><pre>\n", readrule[1]);
			for (i=0; i< num[1]; i++)
				printf("��%d��\t", i+1);
			printf("����\t�@�o����(���`�� %d)\n", percent[1]);
			for (i=0; i< num[1]; i++)
				printf(" %d \t", small[index][i]);
			printf(" %0.2f \t %0.2f", smallavg, smallscore);
			printf("\n</pre>");
		}
		if (num[2]>0)
		{   /* ��ܸӥͩ�Ҧ��Z */
			printf("\n<H3>%s:</H3><pre>\n", readrule[2]);
			for (i=0; i< num[2]; i++)
				printf("��%d��\t", i+1);
			printf("����\t�@�o����(���`�� %d)\n", percent[2]);
			for (i=0; i< num[2]; i++)
				printf(" %d \t", test[index][i]);
			printf("%0.2f \t %0.2f", testavg, testscore );
			printf("\n</pre>");
		}
		if (num[3]>0)
		{   /* ��ܸӥʹ����Ҧ��Z */
			printf("\n<H3>%s���Z:</H3><pre>\n", readrule[3]);
			for (i=0; i< num[3]; i++)
				printf("��%d��\t", i+1);
			printf("����\t�@�o����(���`�� %d)\n", percent[3]);
			for (i=0; i< num[3]; i++)
				printf(" %d \t", middle[index][i]);
			printf("%0.2f \t %0.2f", midscore, totmidscore);
			printf("\n</pre>");
		}
		if (num[4]>0)
		{   /* ��ܸӥʹ����Ҧ��Z */
			printf("\n<H3>�����Ҧ��Z:</H3><pre>");
			printf("\n������\t�@�o����(���`�� %d)\n",percent[4]);
			for (i=0; i< num[4]; i++)
				printf(" %d \t", final[index][i]);
			printf(" %0.2f", finalscore);
			printf("\n</pre>");
			printf("\n<HR>");
		}
		printf("<hr><H3>�ثe�A���`�����O %0.2f. ���Z�`�����O %0.2f, �A���ƦW�O %d</H3><hr>", totalscore[index], alltotalavg, rank);
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
