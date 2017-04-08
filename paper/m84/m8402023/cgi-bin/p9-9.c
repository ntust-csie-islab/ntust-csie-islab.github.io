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
	input inputs[10000]; 
	register int i,m=0; 
	int len; 
    	time_t Ourtime;    
    	FILE *fp, *fopen();
  	fp = fopen("/Guest/guestbook.html","a");  /* 開留言檔 */

	printf("Content-type: text/html%c%c",10,10);
	len = atoi(getenv("CONTENT_LENGTH"));

	for(i=0; len && (!feof(stdin)); i++) { 
		m=i; 
		inputs[i].val = ReadStdin(stdin,'&',&len); 	/* 讀 stdin 資料 */
		AddToSpace(inputs[i].val); 			/* 加號改空白符號 */
		Convert(inputs[i].val);  			/* 特殊字元改成 16 進位符號 */
		inputs[i].name = ReadData(inputs[i].val,'='); 	/* 讀用戶資料 */
	} 

	printf("<H1>謝謝你的光臨, %s! </H1>", inputs[0].val); 
    	printf("<H3>你的電傳地址是 %s </H3>", inputs[1].val);
    	printf("<H3>你的意見是 %s </H3>", inputs[2].val); 

       	fputs("\n<p>姓名: ", fp);
       	fputs(inputs[0].val, fp);  /* 寫入使用者姓名 */
       	fputs(", 電傳地址: ", fp); 
       	fputs(inputs[1].val, fp);  /* 寫入使用者電傳地址 */
       	fputs(", 意見留言:", fp);
       	fputs(inputs[2].val, fp);  /* 寫入使用者意見 */
             
    	fclose(fp);
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
