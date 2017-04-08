/* 版權聲明: 本程式是由蔡奇玉所撰寫, 歡迎讀者來 Copy 使用 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {        	/* 定義 stdin 資料結構 */
    char *name;  		/* 資料名稱 */
    char *val;   		/* 資料內容 */
} input;  

char *ReadStdin(FILE *f, char stop, int *len);
void AddToSpace(char *str);
void Convert(char *url);
char *ReadData(char *line, char stop);

main(int argc, char *argv[])
{ 
	input inputs[10000];  /* 設定最大輸入是 10000 筆資料 */
	register int i,m=0; 
	int len; 

	FILE *fp, *fp1, *fopen();

	/* 設定查詢系統上所需要的資料變數 */

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
	fp = fopen(filename,"r");   /* 開成績設定檔 */
	kind= 0;
	while(fscanf(fp, "%s %d %d", readrule[kind], &num[kind], &percent[kind])!= EOF) /* 讀取成績項目設定資料 */
		kind=kind+1;
	fclose(fp);

	strcpy(filename, "/score/data/");
	strcat(filename, inputs[2].val);

	fp1 = fopen(filename,"r");   /* 開成績資料檔 */

	alltotalscore = 0;

	for (i=0; i<100; i++)
	{
		number = i;
		if ( fscanf(fp1, "%s %s", id[i], password[i]) != EOF )
		{
			for (j=0, t_home=0.0; j < num[0]; j++)
			{ /* 讀取作業成績 */
				 fscanf(fp1, "%d", &home[i][j] );  
				 t_home += home[i][j];
			}
			for (j=0, t_small=0.0; j < num[1]; j++)
			{ /* 讀取小考成績 */
			 	fscanf(fp1, "%d", &small[i][j] );  
			 	t_small += small[i][j];
			}
			for (j=0, t_test=0.0; j < num[2]; j++)
			{ /* 讀取抽考成績 */
			 	fscanf(fp1, "%d", &test[i][j] );  			
	            		t_test += test[i][j];
			}
			for (j=0, t_middle=0.0; j < num[3]; j++)
			{ /* 讀取期中考成績*/ 
				fscanf(fp1, "%d", &middle[i][j] );  
				t_middle += middle[i][j];
			}
			for (j=0,t_final=0.0;j < num[4]; j++)
			{ /* 讀取期末考成績 */
				fscanf(fp1, "%d", &final[i][j] ); 
				t_final += final[i][j];
			}

          		/* 計算學生總分成績 */
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
	{  /* 檢查學生姓名及密碼 */
		if ( strcmp(inputs[3].val, id[i]) == 0 && strcmp(inputs[4].val, password[i])==0 )  		{
			index = i;
			find = 1;
		}
	}
	if (find == 0) /* 若與資料不符， 則顯示查無此人或密碼錯誤 */
	{
		printf("<H1>抱歉! 查無此人或密碼錯誤1</H1>\n");
		printf("<H1>請重新輸入一次!</H1>\n");
	}
	else
	{
		rank = 1;
		for (i=0; i< number; i++)
			if (totalscore[index] < totalscore[i] )
				rank = rank + 1;  /* 計算該生在全班的成績排名 */
		homeavg = 0;
		for (i=0; i < num[0]; i++) /* 計算作業平均 */
			homeavg = homeavg + home[index][i]; 
		homeavg = homeavg / num[0];
		homescore =((homeavg * percent[0])/100); 
		smallavg = 0;
		for (i=0; i < num[1]; i++) /* 計算小考平均 */
			smallavg = smallavg + small[index][i];
		smallavg = smallavg / num[1];
		smallscore =((smallavg * percent[1])/100); 
		testavg = 0;
		for (i=0; i < num[2]; i++) /* 計算抽考平均 */
			testavg = testavg + test[index][i];
		testavg = testavg / num[2];
		testscore =((testavg * percent[2])/100); 
		midscore = 0;
		for (i=0; i < num[3]; i++) /*計算期中考平均*/
			midscore = midscore + middle[index][i];
		midscore = midscore / num[3];
		totmidscore =((midscore * percent[3])/100); 
		finalscore = 0;
		for (i=0; i < num[4]; i++) /*計算期末考平均*/
			finalscore = finalscore + final[index][i];
		finalscore = finalscore / num[4];
		finalscore =((finalscore * percent[4])/100);

 		alltotalavg = alltotalscore / number; /* 計算總平均 */
		
		strcpy(title, inputs[0].val);
		printf("\n<center><H1>學生成績查詢系統</H1></center>"); 
		printf("\n<center><H3>查詢科目: %s</H3></center>", title);
		printf("\n<HR><center><H3>你目前的成績如下: </H3></center>");

		if ( num[0] > 0)
		{   /* 顯示該生作業成績 */
			printf("\n<H3>%s:</H3><pre>\n", readrule[0]);
			for (i=0; i< num[0]; i++)
				printf("第%d次\t", i+1);
			printf("平均\t共得分數(佔總分 %d)\n", percent[0]);
			for (i=0; i< num[0]; i++)
				printf(" %d \t", home[index][i]);
			printf(" %0.2f \t %0.2f", homeavg, homescore);
			printf("\n</pre>");
		}
		if (num[1] > 0)
		{   /* 顯示該生小考成績 */
			printf("\n<H3>%s:</H3><pre>\n", readrule[1]);
			for (i=0; i< num[1]; i++)
				printf("第%d次\t", i+1);
			printf("平均\t共得分數(佔總分 %d)\n", percent[1]);
			for (i=0; i< num[1]; i++)
				printf(" %d \t", small[index][i]);
			printf(" %0.2f \t %0.2f", smallavg, smallscore);
			printf("\n</pre>");
		}
		if (num[2]>0)
		{   /* 顯示該生抽考成績 */
			printf("\n<H3>%s:</H3><pre>\n", readrule[2]);
			for (i=0; i< num[2]; i++)
				printf("第%d次\t", i+1);
			printf("平均\t共得分數(佔總分 %d)\n", percent[2]);
			for (i=0; i< num[2]; i++)
				printf(" %d \t", test[index][i]);
			printf("%0.2f \t %0.2f", testavg, testscore );
			printf("\n</pre>");
		}
		if (num[3]>0)
		{   /* 顯示該生期中考成績 */
			printf("\n<H3>%s成績:</H3><pre>\n", readrule[3]);
			for (i=0; i< num[3]; i++)
				printf("第%d次\t", i+1);
			printf("平均\t共得分數(佔總分 %d)\n", percent[3]);
			for (i=0; i< num[3]; i++)
				printf(" %d \t", middle[index][i]);
			printf("%0.2f \t %0.2f", midscore, totmidscore);
			printf("\n</pre>");
		}
		if (num[4]>0)
		{   /* 顯示該生期末考成績 */
			printf("\n<H3>期末考成績:</H3><pre>");
			printf("\n期末考\t共得分數(佔總分 %d)\n",percent[4]);
			for (i=0; i< num[4]; i++)
				printf(" %d \t", final[index][i]);
			printf(" %0.2f", finalscore);
			printf("\n</pre>");
			printf("\n<HR>");
		}
		printf("<hr><H3>目前你的總平均是 %0.2f. 全班總平均是 %0.2f, 你的排名是 %d</H3><hr>", totalscore[index], alltotalavg, rank);
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
	    data[i] = ConvertToHex(&data[j+1]); /* 轉特殊字元 */
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
	   str[i] = ' ';  /* 把加號改成空白符號 */
} 
