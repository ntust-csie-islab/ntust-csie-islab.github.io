

/* **********************************
   Program : KBFNN Rev: NVV.C 
   version : 6.0
   Author : En-Chieh Chang
   Date: Jan, 20, 1995
   ********************************** */
/* A modified version of KBFNN in ~m8202004/v50/vv/kbfnn.m.c
   Fuzzy target is used */
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define maxterm 7
#define maxlay 8
#define maxlay_nod 11
#define maxnod 100
#define max_minterms 1000
#define maxwei 1
#define max 600

float p1=1.5;              /* importance factor of m   */
float sim_threshold=0.95;  /* used in fuzzy_tabulation   */
float def_term[6][4]={
                      {   0,   0,  0,  0 },
                      {  .0, .05, .0, .1 },
                      {  .2,  .3, .1, .1 },
                      { .45, .55, .1, .1 },
                      {  .7,  .8, .1, .1 },
                      { .95, 1.0, .1, .0 } };


int laynum=0,patnum=0,tranum,tstnum;/*lay num.,pattern num.,training pattern 
					num., and test pattern number*/
int seed,no_pat,noepcoh,no_link;/* random seed, no of pattern, 
				no of epcoh, no of connection link*/
int first[maxnod],laynods[maxnod],patodr[max],bg_fg=0;/*bg_fg=1 for bg */
int sc_no[maxlay][maxnod];
int variable_mark[maxlay][maxnod];
char buff[512];
char netfile[12],wfile[12],bfile[12],chkfile[12],trafile[12],patfile[12],
	rep_file[12],cwfile[12];
float lrate,lr_con,lr_pre,momentum,threshold,lambda=1.0,delta=1.0;
float outm[maxlay][maxnod],outn[maxlay][maxnod],outa[maxlay][maxnod],outb[maxlay][maxnod],outf[maxlay][maxnod];
float pata[maxnod][max],patb[maxnod][max],patm[maxnod][max],
      patn[maxnod][max],patf[maxnod][max],pattagm[maxnod][max],pattagn[maxnod][max],
      pattaga[maxnod][max],pattagb[maxnod][max],pattagf[maxnod][max];
float errec[max];
char cflag[max];
int lmode,sfact;
long scount=0;

struct CT_NOD /*network's connection structure*/
{
  struct CT_NOD *next;
  int ct_id;
};
typedef struct CT_NOD ct_node;

/*declaration head pointer for each node*/
ct_node *ct_head[maxlay][maxnod], *ct_tail[maxlay][maxnod],
          *ct_head_temp[maxterm], *ct_tail_temp[maxterm],
          *ct_head_ft_tmp1[maxterm],  *ct_head_ft_tmp2[maxterm];

/*declaration connection table structure*/
struct ct
  {
    int lh,head,lt,tail;/* lh: lay_no of connection head, head: node_no of connection head,
			   lt: lay_no of connection tail, tail: node_no od connection tail*/ 
    float cm,cn,ca,cb; /*LR_typed fuzzy weights and current similarity_overlap_cofficient*/
   };	/* 4 */
struct ct cotab[max];

struct tm *newtime;
long ltime;

int debug_flag=0;
struct t_rule
{
	int layer,node;
	float m,n,a,b;
	struct t_rule *next;
};
typedef struct t_rule RULE;
RULE *premise[maxnod], *conclusion[maxnod];
struct t_group
{
	int number;
	struct t_group *next;
};
typedef struct t_group GROUP;
GROUP *group[maxnod];

static int lcompare(i,j)
struct ct *i,*j;
{
  return(i->lh<j->lh||(i->lh=j->lh&&i->head<j->head));
}
/*
struct PRECONDITION_ADDRESS
{
 struct PRECONDITION_ADDRESS *next;
 int    ind_lay;
 int    ind_nod;
 float  fz_term[4];
};
typedef struct PRECONDITION_ADDRESS pa_node;*/

struct INCLUDE_NUMBER
{
       struct INCLUDE_NUMBER *next;
       int    inc_num;
};
typedef struct INCLUDE_NUMBER inc_node;

struct SAME_SG
{
       struct SAME_SG *next;
       
       /* int    include[max_minterms];               */

       float  pre_condition[maxlay][maxlay_nod][4]; 
       struct INCLUDE_NUMBER       *inc_list;
       int    included_flag;
    
};
typedef struct SAME_SG ssg_node;
ssg_node *ft_det_list[2], *ft_sel_list, *ft_trouble_list, *ft_essential_list,
         *expand_list; 

struct DIGIT_NODE
{
       struct DIGIT_NODE *next;
       struct DIGIT_NODE *front;
       struct SAME_SG    *stl_ind;
};
typedef struct DIGIT_NODE dig_node;
dig_node *dig_list;

/* The main program Jan. 1995 */
main()
/*int argc;
char **argv[];*/
{
	int i,j,k,loop=0;
	char select;
	time(&ltime);
	newtime=gmtime(&ltime);
	i=newtime->tm_sec;
	j=newtime->tm_min;
	k=newtime->tm_hour;
	seed = i*i+j*k+1995;
	srand(seed);
	/*printf("start file: %s...\n",argv[1]);
	starfile(argv[1]);
	readnet (netfile);*/
	init_weights();
	do
	{
		printf("\n");
		printf("**********************\n");
		printf("*    NVV Rev 0.1     *\n");
		printf("*    main menu       *\n");
		printf("**********************\n");
		printf("1. load file\n");
		printf("2. save file\n");
		printf("3. learning\n");
		printf("4. test\n");
		printf("5. initialization\n");
		printf("6. reset weights\n");
		printf("7. set parameters\n");
		printf("8. on-line test\n");
		printf("9. building training data\n");
		printf("0. quit\n");
		printf("b. batch test\n");   /* new function */
		printf("d. debug\n"); 	/* print debug info */
		printf("c. check\n");	/* check consistency */
		printf("l. list network structure\n"); /* list links & node */
               /* printf("s. sort links\n"); */
                printf("f. fuzzy tabulation\n");
                printf("e. efficiency evoluation\n");
                printf("s. separate G nauron\n");
                printf("t. transitive (validation)\n");
                printf("p. build original algorithm training data\n ");
		printf("select a function...");
		scanf("%s",&select);
		switch(select)
		{
		case '1':
			load();
			break;
		case '2':
			save();
			break;
		case '3':
			learning();
			if(bg_fg==1)
			  loop=1;
			break;
		case '4':
			test();
			break;
		case '5':
			init();
			break;
		case '6':
			init_weights();
			break;
		case '7':
			set_par();
			break;
		case '8':
			on_line_test();
			break;
		case '9':
			build_data();
			break;
		case 'b':
		        batest();
		        break;
		case 'd':
			debug();
			break;
		case 'c':
			check();
			break;
		case 'l':
		        list_net_structure();
		        break;
		case 'f':
		        fuzzy_tabulation();
		        break;        
		case 'e':
		        efficiency_evoluation();
		        break;        	
		case 's':
		        /* qsort(cotab,max,sizeof(struct ct),lcompare); 
		        writewts("nvvtmp1");
		        printf("1\n");
		        system("sort -o nvvtmp2 nvvtmp1\n");
		        printf("2\n");
		        readwfile("nvvtmp2"); */
		        separate_g_neuron();
		        break;        
		case 't':
		        transitive();
		        break;          
		case '0':
			loop=1;
			break;
		case 'p':
		        build_cpr_data();
		        break;	
		default:
			printf("error!\n");
		}
	}  while(loop==0);
}



debug (){
int i,j;
  if (debug_flag == 0)
   {  printf("debug on\n");
      debug_flag= 1;
    }
   else 
   {  printf("debug off\n");
      debug_flag= 0;
    };
 for(i=1;i <= laynum; i++)
    for (j=1;j<=laynods[i];j++)
       printf("%d:%d om=%1.5f, on=%1.5f, oa=%1.5f, ob=%1.5f\n",
	i,j,outm[i][j],outn[i][j],outa[i][j],outb[i][j]);
 for (i=1;i<=laynods[laynum];i++)
     printf("target[%d]=%1.2f %1.2f %1.2f %1.2f\n", i,pattagm[i][no_pat],pattagn[i][no_pat],pattaga[i][no_pat],
		pattagb[i][no_pat]);


}

/* Reading start file for setting environment */
starfile(filename)
char *filename;
{
	FILE *fptr;
	int i;
	char name[10],value[12];
	char string[20],tmp;
	fptr=fopen(filename,"r");
	while(fscanf(fptr,"%s %s", name,value) != EOF)
	{
		switch(name[0])
		{
		case 'b':
			if (value != "no")
			{
				for(i=0;i<10;i++)
					bfile[i]=value[i];
				printf("biasfile: %s\n",bfile);
			}
			else
				break;
		case 'c':
			for(i=0;i<12;i++)
				cwfile[i]=value[i];
			printf("connection file: %s\n",cwfile);
			readwfile(cwfile);
			break;
		case 'e':
			noepcoh=atoi(value);
			printf("no of epcoh: %d\n",noepcoh);
			break;
		case 'l':
			lr_con=lr_pre=atof(value);
			printf("lrate: %0.2f\n",lrate);
			break;
		case 'm':
			momentum = atof(value);
			printf("momentum: %0.2f\n",momentum);
			break;
		case 'n':
			for(i=0;i<12;i++)
				netfile[i]=value[i];
			printf("netfile: %s\n",netfile);
			readnet(netfile);
			break;
		case 'p':
			threshold=atof(value);
			printf("threshold: %0.2f\n",threshold);
			break;
		case 't':
			for(i=0;i<12;i++)
				trafile[i]=value[i];
			printf("trafile: %s\n",trafile);
			break;

		case 'w':
			if (value != "no")
			{
				for(i=0;i<12;i++)
					wfile[i]=value[i];
				printf("weightfile: %s\n",wfile);
				readwts(wfile);
				break;
			}
			else
				break;

		default: 
			printf("no match!\n");
		}
	}
	fclose(fptr);
}

/* Reading network definition file for initializing network*/
readnet(netfile)
{
	FILE *fptr;
	int i,j;
	int lay;
	laynum=0;
	fptr=fopen(netfile,"r");
	printf("network information-\n");
	while(fscanf(fptr,"%u %u", &i, &j) != EOF)
	{
		lay = i;
		laynods[lay] = j;
		laynum++;
		printf("laynum: %d  laynods: %d\n", laynum,
		    laynods[laynum]);
	}
	fclose(fptr);
}

/* Reading connection table as weights file */
readwts(wfile)
{
	FILE *fptr;
	int i,j,k,l,index=1;
	float m,n,a,b;	/* 4 */
	char weight[3];
	fptr=fopen(wfile,"r");
	printf("connection weights-\n");
	while(fscanf(fptr,"%d %d %d %d %s", &i, &j, &k, &l, &weight) != EOF)
	{
		cotab[index].lh = i;
		cotab[index].head = j;
		cotab[index].lt = k;
		cotab[index].tail =l;
		/*translating fuzzy terms to
		  LR-type fuzzy number*/
		if (strcmp(weight,"NB")==0)  {m=.0; n=.05; a=.0; b=.1;}   /* 4 */
		if (strcmp(weight,"NM")==0)  {m=.2; n=.3; a=.1; b=.1;}
		if (strcmp(weight,"Z")==0)   {m=.45; n=.55; a=.1; b=.1;}
		if (strcmp(weight,"PM")==0)  {m=.7; n=.8; a=.1; b=.1;}
		if (strcmp(weight,"PB")==0)  {m=.95; n=1.0; a=.1; b=.0;}
		cotab[index].cm = m;
		cotab[index].cn = n;/* 4 */
		cotab[index].ca = a;
		cotab[index].cb = b;

	printf("%d:%d-->%d:%d weight: %1.3f %1.3f %1.3f %1.3f %s\n",
			cotab[index].lh,
			cotab[index].head,
			cotab[index].lt,
			cotab[index].tail,
			cotab[index].cm,
			cotab[index].cn,/* 4 */
			cotab[index].ca,
			cotab[index].cb,
			weight);
		index++;

	}
	printf("number of connections: %d\n", index-1);
	no_link=index-1;
	fclose(fptr);
}

/* Reading weight file as connection table */
readwfile(wfile)
{
	FILE *fptr;
	int i,j,k,l,index=1;
	float m,n,a,b;
	char weight[3];
	fptr=fopen(wfile,"r");
	printf("connection weights-\n");
	while(fscanf(fptr,"%d %d %d %d %f %f %f %f",&i,&j,&k,&l,&m,&n,&a,&b) != EOF)
	{
		cotab[index].lh = i;
		cotab[index].head = j;
		cotab[index].lt = k;
		cotab[index].tail =l;
 		cotab[index].cm = m;
 		cotab[index].cn = n;/* 4 */
		cotab[index].ca = a;
		cotab[index].cb = b;

	printf("%d:%d-->%d:%d weight: %1.3f %1.3f %1.3f %1.3f\n",
			cotab[index].lh,
			cotab[index].head,
			cotab[index].lt,
			cotab[index].tail,
			cotab[index].cm,
			cotab[index].cn,/* 4 */
			cotab[index].ca,
			cotab[index].cb);
		index++;
	}
	printf("number of connections: %d\n", index-1);
	no_link=index-1;
	fclose(fptr);
}

/*************************
 * Build Connection Link *
 *************************/
build_link()
{
  int i,j,lt,tail,lh,head;
  ct_node *p;
  for(i=1;i<=laynum;i++)
	for(j=1;j<=laynods[i];j++)
        {
		if((ct_head[i][j]=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
                 {
                        ct_head[i][j]->next=NULL;
                        ct_head[i][j]->ct_id=0;
                  }
		if((ct_tail[i][j]=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
                 {
			ct_tail[i][j]->next=NULL;
			ct_tail[i][j]->ct_id=0;
                  }
        }
/*
  for(i=2;i<=laynum;i++)
*/
       for(i=1;i<=no_link;i++)
        {
		lt=cotab[i].lt;
		tail=cotab[i].tail;
		lh=cotab[i].lh;
		head=cotab[i].head;
		if((p=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
		 {
                        p->ct_id=i;
                        p->next=ct_head[lt][tail];
                        ct_head[lt][tail]=p;

		  } else printf("error allocating memory\n");
		if((p=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
		 {
                        p->ct_id=i;
                        p->next=ct_tail[lh][head];
			ct_tail[lh][head]=p;
                  } else printf("error allocating memory\n");
         }
        for(i=2;i<=laynum;i++)
	for(j=1;j<=laynods[i];j++)
	{
	  p=ct_head[i][j];
	  while(p!=NULL)
	  {

	    printf("lay %d node %d ct %d",i,j,p->ct_id);
	    printf(" H_lay:%d H_nod:%d T_lay:%d T_nod:%d\n",
		cotab[p->ct_id].lh,
		cotab[p->ct_id].head,
		cotab[p->ct_id].lt,
		cotab[p->ct_id].tail);
	
	    p=p->next;
	   } 	
	  }


}
/* Initializing weights between [+0.5,-0.5] */
init_weights()
{
	int i,j,k,range=50;
	float temp;
	float rnd();
	printf("Reset weights...\n");
	for(i=1;i<=no_link;i++)
	 {
		cotab[i].cm = rnd(range);
		cotab[i].cn = rnd(range);	/* 4 */
		if (cotab[i].cm > cotab[i].cn)
			{temp=cotab[i].cm;
			cotab[i].cm=cotab[i].cn;
			cotab[i].cn=temp;}
		cotab[i].ca = fabs(rnd(range*3/5));
		cotab[i].cb = fabs(rnd(range*3/5));
		printf("link %d  m:%0.3f n:%0.3f a:%0.3f b:%0.3f\n",
			i,cotab[i].cm,cotab[i].cn,cotab[i].ca,cotab[i].cb);
           }
}

/* Random number generator */
float rnd(range)
float range;
{
	float x,y,z;
	x=rand();
	x=x/10000;
	y=modf(x,&z);
	y= range*y;
	return(y);
}

/* Reading pattern data file */
int readpat(patfile)
{
	int i=1,j=1,k=1;
	float m,n,a,b,f,total_f;
	FILE *fptr;
	fptr=fopen(patfile,"r");
	patnum = 0;
	total_f=0;
	printf("%s patterns-\n",patfile);

	while(fscanf(fptr,"%f %f %f %f %f",&m,&n,&a,&b,&f) != EOF)
	{
         printf("%f %f %f %f %f\n",m,n,a,b,f);
		if ( i>laynods[1]) /* output node */
		{
			pattagm[k][j]=m;
			pattagn[k][j]=n;
			pattaga[k][j]=a;
			pattagb[k][j]=b;
			pattagf[k][j]=f;
			total_f+=f;
			k++;
		}
		else
		{
			patm[i][j]=m;
			patn[i][j]=n;
			pata[i][j]=a;
			patb[i][j]=b;
			patf[i][j]=f;
			i++;	
		}
		if ( k>laynods[laynum])
		{
			i=k=1;
			j++;
		}
	 
	}
	patnum = j-1;
	printf("patnum: %d\n",patnum);
	printf("total degree = %1.5f \n",total_f);
	return(patnum);
}
writewts(wfile)
{
	FILE *fptr;
	int i,j,k;
	char weight[4];
	fptr=fopen(wfile,"w");
	printf("write to connection table-\n");
	for(i=1;i<=no_link;i++)
	{	/* 4 */
 		fprintf(fptr,"%d %d %d %d %1.2f %1.2f %1.2f %1.2f\n",
 			cotab[i].lh,cotab[i].head,cotab[i].lt,cotab[i].tail,
			cotab[i].cm,cotab[i].cn,cotab[i].ca,cotab[i].cb);
	}
	fclose(fptr);
  return;
}

get_pat()
{
	int i,tmp;
	double j,k,l;
	for (i=1;i<=patnum;i++)
		patodr[i]=i;

	for (i=patnum;i>1;i--)
	{
		j=fabs(rnd((i-1)*100))+1;
		k=modf(j,&l);
		tmp=patodr[i];
		patodr[i]= patodr[(int) l];
		patodr[(int) l]= tmp;
	} 
	/*for (i=patnum;i>0;i--)
		printf("%d %d\n",i,patodr[i]);*/
}

float sigmoid(input)
double input;
{
	double result,exp();
	/*if(input>16.0)
		result=1.0;
	else if(input<-16.0)
float	result=0.0;
	else*/ result =1/(1.0+exp(-lambda*input));

	return(result);
}


float similarity(m1,n1,a1,b1,m2,n2,a2,b2)
float m1,n1,a1,b1,m2,n2,a2,b2;
{  float s;
	if ((s= (1-(p1*(m1-m2)*(m1-m2)+p1*(n1-n2)*(n1-n2)+(a1-a2)*(a1-a2)+(b1-b2)*(b1-b2)))) >0) 
		return s;
	return 0;
}


float getmin(a,b)
float a,b;
{
if(a>b)
  return(b);
else
  return(a);
}
    
float getmax(a,b)
float a,b;
{
if(a>b)
  return(a);
else
  return(b);
}


float matching(wm,wn,wa,wb,im,in,ia,ib)
float wm,wn,wa,wb,im,in,ia,ib;
{
float term_button,term_core, overlap_area,input_area,
      epsilon=0.01;      /* an parameter */
      
   /*make ia ib not to be zero*/
if(ia==0)
  ia=getmin(epsilon,im);
if(ib==0)
  ib=getmin(epsilon,1-in);
                             /*calculate the overlap of two fuzzy terms
                                           by their button and core*/
term_button=getmin(in+ib,wn+wb) - getmax(im-ia,wm-wa);
term_core=getmin(in,wn) - getmax(im,wm);
                                                                                                                  
                        /* calaulate the area of the two fuzzy terms */
overlap_area=getmax(term_button,0)*(getmax(term_button,0)+getmax(term_core,0))
             / (2*(term_button-getmin(term_core,0)));
  /*    printf("overlap_area=%f\n",overlap_area);  */
  

                                    /*calculate the area of input fuzzy term
                                                     and matching degree*/
input_area=(in-im)+0.5*(ia+ib);
/*    printf("input_area=%f\n",input_area);  */

return(overlap_area/input_area);
                                                                                 
}                                                                                                                                                                       

float similarity_1(m1,n1,a1,b1,m2,n2,a2,b2)
float m1,n1,a1,b1,m2,n2,a2,b2;
{  float s;
   	
	if ((s= (1-(p1*(m1-m2)*(m1-m2)+p1*(n1-n2)*(n1-n2)+(a1-a2)*(a1-a2)+(b1-b2)*(b1-b2)))) >0) 
		return s;
	return 0;
}


compute_output_ori(no_pat)
{
	int i,j,c_no;
	float m1,n1,a1,b1,m2,n2,a2,b2;
	float MIN,MAX,s_degree,sum_ce,sum_so,centroid,total_sdeg;
	float um=.6,ua=.2,ub=.2;
	ct_node *p;

	for(i=1;i<=laynods[1];i++)      /*compute output value of input nodes*/
	{
		outm[1][i]=patm[i][no_pat];
		outn[1][i]=patn[i][no_pat];
		outa[1][i]=pata[i][no_pat];
		outb[1][i]=patb[i][no_pat];
	/*	printf("m:%1.2f a:%1.2f b:%1.2f\n",outm[1][i],outa[1][i],outb[1][i]);*/
	}
	for(i=2;i<=laynum;i++)
	    for(j=1;j<=laynods[i];j++) /*compute activation of S-neuron*/
		{
		 p=ct_head[i][j];
		 if(i % 2 ==0)
		 {
		  MIN=1;
		  total_sdeg=c_no=0;
		  while(p->next != NULL)
		  {
 			m1=cotab[p->ct_id].cm;
 			n1=cotab[p->ct_id].cn;
			a1=cotab[p->ct_id].ca;
			b1=cotab[p->ct_id].cb;
			m2=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			n2=outn[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			a2=outa[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			b2=outb[cotab[p->ct_id].lh][cotab[p->ct_id].head];
 			s_degree=similarity(m1,n1,a1,b1,m2,n2,a2,b2);
 		/*	printf("m1=%1.5f n1=%1.5f a1=%1.5f b1=%1.5f\n",m1,n1,a1,b1);
 			printf("m2=%1.5f n2=%1.5f a2=%1.5f b2=%1.5f\n",m2,n2,a2,b2); 
			printf("s_degree ==> %1.5f\n",s_degree);  9/22 */
			if (MIN > s_degree) 
			{
				MIN=s_degree;
				sc_no[i][j]=p->ct_id;
			};
 		  	 p=p->next;
		   }/*while */
			outm[i][j]=MIN;
		/*	printf("MIN  => %1.5f\n",MIN); */
			if(outm[i][j]>1) outm[i][j]=1;
			if(outm[i][j]<0) outm[i][j]=0;

		  }/* if */
		 else
		 {
		   sum_so=0;
		   outm[i][j]=0;
		   outn[i][j]=0;
		   outa[i][j]=0;
		   outb[i][j]=0;
		   p=ct_head[i][j];
		   while(p->next !=NULL)
		   {
			sum_so += outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			outm[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cm;
			outn[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cn;
			outa[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].ca;
			outb[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cb;
			p=p->next; 
	       	    } 
		/*	printf("outm[%d][%d]=%1.5f  sum_so=%1.5f\n",i,j,outm[i][j],sum_so); */
		    if (sum_so !=0)
			{ outm[i][j]/=sum_so;outn[i][j]/=sum_so;outa[i][j]/=sum_so;outb[i][j]/=sum_so; }
/*
		     else
			{ outm[i][j]=0.5;outn[i][j]=0.5;outa[i][j]=0;outb[i][j]=0; } 
		     if (i==laynum)
		         outm[i][j]= (outn[i][j]+outm[i][j])/2 + (outb[i][j]-outa[i][j])/3;
			 centroid for output G-neurons
*/
		   }/* else */
		 }/* for j */	
/*		 printf("outm=%1.5f\n",outm[laynum][1]); */
 					 
}


compute_output_ori_1(no_pat)
{
	int i,j,c_no;
	float m1,n1,a1,b1,m2,n2,a2,b2;
	float MIN,MAX,s_degree,sum_ce,sum_so,centroid,total_sdeg;
	float um=.6,ua=.2,ub=.2;
	ct_node *p;

	for(i=1;i<=laynods[1];i++)      /*compute output value of input nodes*/
	{
		outm[1][i]=patm[i][no_pat];
		outn[1][i]=patn[i][no_pat];
		outa[1][i]=pata[i][no_pat];
		outb[1][i]=patb[i][no_pat];
		printf("m:%1.2f a:%1.2f b:%1.2f\n",outm[1][i],outa[1][i],outb[1][i]);
	}
	for(i=2;i<=laynum;i++)
	    for(j=1;j<=laynods[i];j++) /*compute activation of S-neuron*/
		{
		 p=ct_head[i][j];
		 if(i % 2 ==0)
		 {
		  MIN=1;
		  total_sdeg=c_no=0;
		  while(p->next != NULL)
		  {
 			m1=cotab[p->ct_id].cm;
 			n1=cotab[p->ct_id].cn;
			a1=cotab[p->ct_id].ca;
			b1=cotab[p->ct_id].cb;
			m2=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			n2=outn[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			a2=outa[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			b2=outb[cotab[p->ct_id].lh][cotab[p->ct_id].head];
 			s_degree=similarity(m1,n1,a1,b1,m2,n2,a2,b2);
			if (MIN > s_degree) 
			{
				MIN=s_degree;
				sc_no[i][j]=p->ct_id;
			};
 		  	 p=p->next;
		   }/*while */
			outm[i][j]=MIN;
			if(outm[i][j]>1) outm[i][j]=1;
			if(outm[i][j]<0) outm[i][j]=0;

		  }/* if */
		 else
		 {
		   sum_so=0;
		   outm[i][j]=0;
		   outn[i][j]=0;
		   outa[i][j]=0;
		   outb[i][j]=0;
		   p=ct_head[i][j];
		   while(p->next !=NULL)
		   {
			sum_so += outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			outm[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cm;
			outn[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cn;
			outa[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].ca;
			outb[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cb;
			p=p->next; 
	       	    } 
		/*	printf("outm[%d][%d]=%1.5f\n",i,j,outm[i][j]); */
		    if (sum_so !=0)
			{ outm[i][j]/=sum_so;outn[i][j]/=sum_so;outa[i][j]/=sum_so;outb[i][j]/=sum_so; }

/*		     else
			{ outm[i][j]=0;outn[i][j]=1;outa[i][j]=0;outb[i][j]=0; } 
		     if (i==laynum)
		         outm[i][j]= (outn[i][j]+outm[i][j])/2 + (outb[i][j]-outa[i][j])/3;
			 centroid for output G-neurons
*/
		   }/* else */
		 }/* for j */	

                printf("output outm=%1.5f\n",outm[laynum][1]); 					 
}



compute_output(no_pat)
{
	int i,j,c_no;
	float m1,n1,a1,b1,m2,n2,a2,b2,f2;
	float MIN,MAX,s_degree,sum_ce,sum_so,centroid,total_sdeg;
	float um=.6,ua=.2,ub=.2;
	ct_node *p;

	for(i=1;i<=laynods[1];i++)      /*compute output value of input nodes*/
	{
		outm[1][i]=patm[i][no_pat];
		outn[1][i]=patn[i][no_pat];
		outa[1][i]=pata[i][no_pat];
		outb[1][i]=patb[i][no_pat];
		outf[1][i]=patf[i][no_pat];
		printf("input of compute_output\n");
		printf("m:%1.2f a:%1.2f b:%1.2f\n",outm[1][i],outa[1][i],outb[1][i]);
	
	}
	for(i=2;i<=laynum;i++)
	    for(j=1;j<=laynods[i];j++) /*compute activation of S-neuron*/
		{
		 p=ct_head[i][j];
		 if(i % 2 ==0)
		 {
		  MIN=1;
		  total_sdeg=c_no=0;
		  while(p->next != NULL)
		  {
 			m1=cotab[p->ct_id].cm;
 			n1=cotab[p->ct_id].cn;
			a1=cotab[p->ct_id].ca;
			b1=cotab[p->ct_id].cb;
			m2=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			n2=outn[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			a2=outa[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			b2=outb[cotab[p->ct_id].lh][cotab[p->ct_id].head];
                        f2=outf[cotab[p->ct_id].lh][cotab[p->ct_id].head];	
 			s_degree=matching(m1,n1,a1,b1,m2,n2,a2,b2)*f2;
			if(s_degree<0.00001)
			  s_degree=0;
			if (MIN > s_degree) 
			{
				MIN=s_degree;
				sc_no[i][j]=p->ct_id;
			};
 		  	 p=p->next;
		   }/*while */
			outm[i][j]=MIN;
			if(outm[i][j]>1) outm[i][j]=1;
			if(outm[i][j]<0) outm[i][j]=0; 
		       printf("The out of S-neuron %d %d is %1.5f [ %1.5f ]\n",i,j,outm[i][j],MIN); 
                  
		  }/* if */
		 else           /* G - neuron process */
		 {
		   sum_so=0;
		   outm[i][j]=0;
		   outn[i][j]=0;
		   outa[i][j]=0;
		   outb[i][j]=0;
		   outf[i][j]=0;
		   p=ct_head[i][j];
		   while(p->next !=NULL)
		   {
			sum_so += outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			outm[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]
			              *cotab[p->ct_id].cm;
			outn[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]
			              *cotab[p->ct_id].cn;
			outa[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]
			              *cotab[p->ct_id].ca;
			outb[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]
			              *cotab[p->ct_id].cb;
                        outf[i][j]+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]
                                      *outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];			
			p=p->next; 
	       	    } 
		/*	printf("outm[%d][%d]=%1.5f\n",i,j,outm[i][j]); */

                     if (sum_so >=0.00001)  /*  means sum_so!=0  */
                     {
		       outm[i][j]/=sum_so;
		       outn[i][j]/=sum_so;
		       outa[i][j]/=sum_so;
		       outb[i][j]/=sum_so; 
                       outf[i][j]/=sum_so;
                     }
                     else
                     {
                       outm[i][j]=0;
                       outn[i][j]=1;
                       outa[i][j]=0;
                       outb[i][j]=0;
                       outf[i][j]=0;
                     }
                    printf("The out of G-neuron %d %d is %1.5f %1.5f %1.5f %1.5f %1.2f\n"
                             ,i,j,outm[i][j],outn[i][j],outa[i][j],outb[i][j],outf[i][j]);
                     
/*		     if (i==laynum)
		         outm[i][j]= (outn[i][j]+outm[i][j])/2 + (outb[i][j]-outa[i][j])/3;
			 centroid for output G-neurons
*/
		   }/* else */
		 }/* for j */	
}

float weight_change(no_pat)
int no_pat;
{
	int i,j,k,no_sc;
	float mse=0.0,err,sum_so,ce_sum,x,sign,ov,min_deg;
	float m1,n1,a1,b1,m2,n2,a2,b2,s_degree,sum_cfm,sum_cfn,sum_cfa,sum_cfb;
	float errm[maxlay][maxnod],errn[maxlay][maxnod],erra[maxlay][maxnod],errb[maxlay][maxnod];
	float delm[maxlay][maxnod],dela[maxlay][maxnod],delb[maxlay][maxnod];
 	ct_node *p,*q;  
	for(i=1;i<=laynum;i++)
	   for(j=1;j<=laynods[i];j++)
		errm[i][j]=errn[i][j]=erra[i][j]=errb[i][j]=0.0;
	mse=0.0; 
	for(i=1;i<=laynods[laynum];i++)
	{	/* fuzzy target */
	        
/*match_test   if(!((outm[laynum][i] > pattagm[i][no_pat])
	                &&(outm[laynum][i] < pattagn[i][no_pat])))           
	 	  errm[laynum][i]=(outm[laynum][i]-pattagm[i][no_pat]);
		else
	          errm[laynum][i]=0;
	          
               if(!((outn[laynum][i] > pattagm[i][no_pat])
	                &&(outn[laynum][i] < pattagn[i][no_pat])))        	
		  errn[laynum][i]=(outn[laynum][i]-pattagn[i][no_pat]);
		else
	          errn[laynum][i]=0;
	          	
	*/      
	    	errm[laynum][i]=(outm[laynum][i]-pattagm[i][no_pat]);
		errn[laynum][i]=(outn[laynum][i]-pattagn[i][no_pat]);
		erra[laynum][i]=(outa[laynum][i]-pattaga[i][no_pat]);
		errb[laynum][i]=(outb[laynum][i]-pattagb[i][no_pat]);
		err=(errn[laynum][i]*errn[laynum][i]+errm[laynum][i]*errm[laynum][i]+
		     erra[laynum][i]*erra[laynum][i]+errb[laynum][i]*errb[laynum][i]);
		mse+=err/2; 
		
		min_deg=pattagf[i][no_pat];
		errm[laynum][i]*=min_deg;
		errn[laynum][i]*=min_deg;
		erra[laynum][i]*=min_deg;
		errb[laynum][i]*=min_deg;
	/*	printf("outm=%1.5f  targetm=%1.5f\n",outm[laynum][i],pattagm[i][no_pat]);
		printf("init error is errm=%1.5f errn=%1.5f erra=%1.5f errb=%1.5f \n",errm[laynum][i],errn[laynum][i],erra[laynum][i],errb[laynum][i]);    
                getchar(); 	9/22 */
	 }
	for(i=laynum;i>1;i--)
	  if(i % 2 != 0) /* G-neurons */
	    { 
	    if (i != laynum)
	      for(j=1;j<=laynods[i];j++)
    		{
		  q=ct_tail[i][j];
		  while(q->next != NULL)
		    { 
		      /* x=errm[cotab[q->ct_id].lt][cotab[q->ct_id].tail]*(-2)/(p1+2); 9/20 */
		       x=errm[cotab[q->ct_id].lt][cotab[q->ct_id].tail]*(-2)*outf[i][j]; 		
		/*       x=errm[cotab[q->ct_id].lt][cotab[q->ct_id].tail]*(-2);        */ 
	        
/*match_error*/	       if(!((outm[i][j] > cotab[q->ct_id].cm)&&(outm[i][j] < cotab[q->ct_id].cn)))	    
		          errm[i][j]+=x*(outm[i][j]-cotab[q->ct_id].cm)*p1;
/*match_error*/        if(!((outn[i][j] > cotab[q->ct_id].cm)&&(outn[i][j] < cotab[q->ct_id].cn)))	       
		          errn[i][j]+=x*(outn[i][j]-cotab[q->ct_id].cn)*p1;
		       erra[i][j]+=x*(outa[i][j]-cotab[q->ct_id].ca);
		       errb[i][j]+=x*(outb[i][j]-cotab[q->ct_id].cb);
			q=q->next;
		    }
		 }/* for j ,if i*/
	      for(j=1;j<=laynods[i];j++)
    		{
		  sum_so=0;
		  p=ct_head[i][j];
		  while(p->next != NULL)
		    {
		      sum_so += outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]; 
		      p=p->next;
		    }
		   p=ct_head[i][j];
  		   while(p->next != NULL)
  		     { 
		       if(sum_so !=0)
		        {
			x=lr_con*outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]/sum_so;
 		       cotab[p->ct_id].cm -= errm[i][j]*x;
 		       cotab[p->ct_id].cn -= errn[i][j]*x;
	               cotab[p->ct_id].ca -= erra[i][j]*x;
                       cotab[p->ct_id].cb -= errb[i][j]*x;
			if(cotab[p->ct_id].cm>1.0)
			   cotab[p->ct_id].cm=1.0;
			if(cotab[p->ct_id].cm<.0)
			   cotab[p->ct_id].cm=.0; 
			if(cotab[p->ct_id].cn>1.0)
			   cotab[p->ct_id].cn=1.0;
			if(cotab[p->ct_id].cn<.0)
			   cotab[p->ct_id].cn=.0; 
		        if(cotab[p->ct_id].ca>.5)
			   cotab[p->ct_id].ca=.5;
			if(cotab[p->ct_id].ca<.0)
			   cotab[p->ct_id].ca=.0;
			if(cotab[p->ct_id].cb>.5)
			   cotab[p->ct_id].cb=.5;
			if(cotab[p->ct_id].cb<.0)
			   cotab[p->ct_id].cb=.0;
		          }
 		       p=p->next; 

		      } /* while */
		 }/* for j */
		} /* if i */
		else   /* S-neurons */
	          for(j=1;j<=laynods[i];j++)
		     if(outm[i][j] != 0)
		     {
			q=ct_tail[i][j];
		        while(q->next != NULL)
			{
		          p=ct_head[cotab[q->ct_id].lt][cotab[q->ct_id].tail];
			  sum_so=sum_cfm=sum_cfn=sum_cfa=sum_cfb=0.0;
		          while(p->next != NULL)
		          { 
			    sum_so+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			    sum_cfm+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cm;
			    sum_cfn+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cn;
			    sum_cfa+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].ca;
			    sum_cfb+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cb;
			    p=p->next;
			   }
/*
printf("so=%1.5f cfm=%1.5f cfa=%1.5f cfb=%1.5f\n",sum_so,sum_cfm,sum_cfa,sum_cfb);
*/
			  if (sum_so != 0)
			  errm[i][j]+=(cotab[q->ct_id].cm*sum_so-sum_cfm)/(sum_so*sum_so)*
					errm[cotab[q->ct_id].lt][cotab[q->ct_id].tail]+
			  		(cotab[q->ct_id].cn*sum_so-sum_cfn)/(sum_so*sum_so)*
					errn[cotab[q->ct_id].lt][cotab[q->ct_id].tail]+
			  		(cotab[q->ct_id].ca*sum_so-sum_cfa)/(sum_so*sum_so)*
					erra[cotab[q->ct_id].lt][cotab[q->ct_id].tail]+
			  		(cotab[q->ct_id].cb*sum_so-sum_cfb)/(sum_so*sum_so)*
					errb[cotab[q->ct_id].lt][cotab[q->ct_id].tail];
			  else errm[i][j]=0;
			  q=q->next;
			 }
			 erra[i][j]=errn[i][j]=errb[i][j]=errm[i][j];
			 p=ct_head[i][j];
			 while (p->next != NULL)
			   if(p->ct_id == sc_no[i][j])
			 {
			    /* x=errm[i][j]*lr_pre*2/(p1+2); 9/20 */
			    x=errm[i][j]*lr_pre*2;
	
	/*match test*/	    if(!((outm[cotab[p->ct_id].lh][cotab[p->ct_id].head] > cotab[p->ct_id].cm )
	                       &&(outm[cotab[p->ct_id].lh][cotab[p->ct_id].head] < cotab[p->ct_id].cn )))
			    cotab[p->ct_id].cm -= 
			      (outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cm)*p1*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;

	/*match test*/	    if(!((outn[cotab[p->ct_id].lh][cotab[p->ct_id].head] > cotab[p->ct_id].cm )
	                       &&(outn[cotab[p->ct_id].lh][cotab[p->ct_id].head] < cotab[p->ct_id].cn )))			    
			    cotab[p->ct_id].cn -= 
			      (outn[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cn)*p1*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;

			    cotab[p->ct_id].ca -= 
			      (outa[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].ca)*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;
			    cotab[p->ct_id].cb -= 
			      (outb[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cb)*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;
  	
			    
		/*          cotab[p->ct_id].cm -= 
			      (outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cm)*p1*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;
			    cotab[p->ct_id].cn -= 
			      (outn[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cn)*p1*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;
			    cotab[p->ct_id].ca -= 
			      (outa[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].ca)*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;
			    cotab[p->ct_id].cb -= 
			      (outb[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cb)*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*x;
  		*/
  		 
  		/*	    cotab[p->ct_id].cm -= 
			      (outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cm)*p1*x;
			    cotab[p->ct_id].cn -= 
			      (outn[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cn)*p1*x;
			    cotab[p->ct_id].ca -= 
			      (outa[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].ca)*x;
			    cotab[p->ct_id].cb -= 
			      (outb[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cb)*x;
  		*/	 
  			   if(cotab[p->ct_id].cm>1.0)
			     cotab[p->ct_id].cm=1.0;
			   if(cotab[p->ct_id].cm<.0)
			     cotab[p->ct_id].cm=.0;  
			   if(cotab[p->ct_id].cn>1.0)
			     cotab[p->ct_id].cn=1.0;
			   if(cotab[p->ct_id].cn<.0)
			     cotab[p->ct_id].cn=.0; 
		           if(cotab[p->ct_id].ca>.5)
			     cotab[p->ct_id].ca=.5;
			   if(cotab[p->ct_id].ca<.0)
			     cotab[p->ct_id].ca=.0;
			   if(cotab[p->ct_id].cb>.5)
			     cotab[p->ct_id].cb=.5;
			   if(cotab[p->ct_id].cb<.0)
			     cotab[p->ct_id].cb=.0;
			   p=p->next;
		         } else p=p->next;/* while p ,if */
		      }; /* if outm */
if(debug_flag != 0)
{
  for(i=1;i <= laynum; i++)
    for (j=1;j<=laynods[i];j++)
       printf("%d:%d om=%1.2f, on:%1.2f, oa=%1.2f, ob=%1.2f, dm=%1.2f, dn=%1.2f, da=%1.2f, db=%1.2f\n",
	i,j,outm[i][j],outn[i][j],outa[i][j],outb[i][j],errm[i][j],errn[i][j],erra[i][j],errb[i][j]);
  for (i=1;i<=laynods[laynum];i++)
     printf("target[%d]=%1.2f %1.2f %1.2f %1.2f\n", i,pattagm[i][no_pat],pattagn[i][no_pat],pattaga[i][no_pat],
		pattagb[i][no_pat]);
  for(i=1;i<=no_link;i++)
     printf("%d:%d->%d:%d  m:%0.3f n:%0.3f a:%0.3f b:%0.3f\n",cotab[i].lh,cotab[i].head,
cotab[i].lt,cotab[i].tail,cotab[i].cm,cotab[i].cn,cotab[i].ca,cotab[i].cb);
 }; 
	return(mse);
}


#ifdef 0

float weight_change1(no_pat)
int no_pat;
{
	int i,j,k,no_sc;
	float mse=0.0,err,sum_so,ce_sum,x,sign,ov,min_deg;
	float m1,n1,a1,b1,m2,n2,a2,b2,s_degree,sum_cfm,sum_cfn,sum_cfa,sum_cfb;
	float errm[maxlay][maxnod],errn[maxlay][maxnod],erra[maxlay][maxnod],errb[maxlay][maxnod];
	float delm[maxlay][maxnod],dela[maxlay][maxnod],delb[maxlay][maxnod];
 	ct_node *p,*q;
	for(i=1;i<=laynum;i++)
	   for(j=1;j<=laynods[i];j++)
		errm[i][j]=errn[i][j]=erra[i][j]=errb[i][j]=0.0;
	mse=0.0; 
	for(i=1;i<=laynods[laynum];i++)
	{	/* fuzzy target */
		errm[laynum][i]=(outm[laynum][i]-pattagm[i][no_pat]);
		errn[laynum][i]=(outn[laynum][i]-pattagn[i][no_pat]);
		erra[laynum][i]=(outa[laynum][i]-pattaga[i][no_pat]);
		errb[laynum][i]=(outb[laynum][i]-pattagb[i][no_pat]);
		err=(errn[laynum][i]*errn[laynum][i]+errm[laynum][i]*errm[laynum][i]+
		     erra[laynum][i]*erra[laynum][i]+errb[laynum][i]*errb[laynum][i]);
		mse+=err/2; 
		
	/*	printf("outm = %1.5f patm = %1.5f eerm = %1.5f\n",outm[laynum][i],pattagm[i][no_pat],errm[laynum][i]);
		printf("outn = %1.5f patn = %1.5f eern = %1.5f\n",outn[laynum][i],pattagn[i][no_pat],errn[laynum][i]);
		printf("outa = %1.5f pata = %1.5f eera = %1.5f\n",outa[laynum][i],pattaga[i][no_pat],erra[laynum][i]);
		printf("outb = %1.5f patb = %1.5f eerb = %1.5f\n",outb[laynum][i],pattagb[i][no_pat],errb[laynum][i]);
		printf("outf = %1.5f\n",outf[laynum][i]); 
		printf(" mse is : %1.5f",mse);
	*/	
	/*	min_deg=min(outf[laynum][i],pattagf[i][no_pat]);  */ 
	        min_deg=pattagf[i][no_pat];   
		errm[laynum][i]*=min_deg;
		errn[laynum][i]*=min_deg;
		erra[laynum][i]*=min_deg;
                errb[laynum][i]*=min_deg;	
                
	 }	 
	for(i=laynum;i>1;i--)
	  if(i % 2 != 0) /* G-neurons */
	    { 
	    if (i != laynum)
	      for(j=1;j<=laynods[i];j++)
    		{
		  q=ct_tail[i][j];
		  while(q->next != NULL)
		    { 
		      /* x=errm[cotab[q->ct_id].lt][cotab[q->ct_id].tail]*(-2)/(p1+2); 9/20 */
		       x=errm[cotab[q->ct_id].lt][cotab[q->ct_id].tail]*(-2)*outf[i][j];
		       errm[i][j]+=x*(outm[i][j]-cotab[q->ct_id].cm)*p1;
		       errn[i][j]+=x*(outn[i][j]-cotab[q->ct_id].cn)*p1;
		       erra[i][j]+=x*(outa[i][j]-cotab[q->ct_id].ca);
		       errb[i][j]+=x*(outb[i][j]-cotab[q->ct_id].cb);
			q=q->next;
		    }
		 }/* for j ,if i*/
	      for(j=1;j<=laynods[i];j++)
    		{
		  sum_so=0;
		  p=ct_head[i][j];
		  while(p->next != NULL)
		    {
		      sum_so += outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]; 
		      p=p->next;
		    }
		   clear(); 
		   p=ct_head[i][j];
  		   while(p->next != NULL)
  		     { 
		       if(sum_so !=0)
		        {
			x=lr_con*outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]/sum_so;
 		        errec[p->ct_id]=x;
 		        }
 		      p=p->next;  
 		     }
 		     /* real-change */
	              esort();
		      p=ct_head[i][j];
                      while(p->next != NULL)
  		     {
  		       if(sum_so !=0 && cflag[p->ct_id]) {
 		       cotab[p->ct_id].cm -= errm[i][j]*errec[p->ct_id];
 		       cotab[p->ct_id].cn -= errn[i][j]*errec[p->ct_id];
	               cotab[p->ct_id].ca -= erra[i][j]*errec[p->ct_id];
                       cotab[p->ct_id].cb -= errb[i][j]*errec[p->ct_id];
			if(cotab[p->ct_id].cm>1.0)
			   cotab[p->ct_id].cm=1.0;
			if(cotab[p->ct_id].cm<.0)
			   cotab[p->ct_id].cm=.0; 
			if(cotab[p->ct_id].cn>1.0)
			   cotab[p->ct_id].cn=1.0;
			if(cotab[p->ct_id].cn<.0)
			   cotab[p->ct_id].cn=.0; 
		        if(cotab[p->ct_id].ca>.5)
			   cotab[p->ct_id].ca=.5;
			if(cotab[p->ct_id].ca<.0)
			   cotab[p->ct_id].ca=.0;
			if(cotab[p->ct_id].cb>.5)
			   cotab[p->ct_id].cb=.5;
			if(cotab[p->ct_id].cb<.0)
			   cotab[p->ct_id].cb=.0;
 		         }
 		       p=p->next; 

		      } /* while */
		 }/* for j */
		} /* if i */
		else   /* S-neurons */
	          for(j=1;j<=laynods[i];j++)
		     if(outm[i][j] != 0)
		     {
			q=ct_tail[i][j];
		        while(q->next != NULL)
			{
		          p=ct_head[cotab[q->ct_id].lt][cotab[q->ct_id].tail];
			  sum_so=sum_cfm=sum_cfn=sum_cfa=sum_cfb=0.0;
		          while(p->next != NULL)
		          { 
			    sum_so+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head];
			    sum_cfm+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cm;
			    sum_cfn+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cn;
			    sum_cfa+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].ca;
			    sum_cfb+=outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]*cotab[p->ct_id].cb;
			    p=p->next;
			   }
/*
printf("so=%1.5f cfm=%1.5f cfa=%1.5f cfb=%1.5f\n",sum_so,sum_cfm,sum_cfa,sum_cfb);
*/
			  if (sum_so != 0)
			  errm[i][j]+=(cotab[q->ct_id].cm*sum_so-sum_cfm)/(sum_so*sum_so)*
					errm[cotab[q->ct_id].lt][cotab[q->ct_id].tail]+
			  		(cotab[q->ct_id].cn*sum_so-sum_cfn)/(sum_so*sum_so)*
					errn[cotab[q->ct_id].lt][cotab[q->ct_id].tail]+
			  		(cotab[q->ct_id].ca*sum_so-sum_cfa)/(sum_so*sum_so)*
					erra[cotab[q->ct_id].lt][cotab[q->ct_id].tail]+
			  		(cotab[q->ct_id].cb*sum_so-sum_cfb)/(sum_so*sum_so)*
					errb[cotab[q->ct_id].lt][cotab[q->ct_id].tail];
			  else errm[i][j]=0;
			  q=q->next;
			 }
			 erra[i][j]=errn[i][j]=errb[i][j]=errm[i][j];
			 clear();
			 p=ct_head[i][j];
			 while (p->next != NULL)
			   if(p->ct_id == sc_no[i][j])
			 {
			    /* x=errm[i][j]*lr_pre*2/(p1+2); 9/20 */
			    x=errm[i][j]*lr_pre*2;
			    errec[p->ct_id]=x;
			    p=p->next;
			  } else p=p->next;
			  
			  esort();
			  p=ct_head[i][j];
			  while (p->next != NULL)
			    if(p->ct_id == sc_no[i][j]&&cflag[p->ct_id]) 
			     {
			    cotab[p->ct_id].cm -= 
			      (outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cm)*p1*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*errec[p->ct_id];
			    cotab[p->ct_id].cn -= 
			      (outn[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cn)*p1*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*errec[p->ct_id];
			    cotab[p->ct_id].ca -= 
			      (outa[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].ca)*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*errec[p->ct_id];
			    cotab[p->ct_id].cb -= 
			      (outb[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cb)*outf[cotab[p->ct_id].lh][cotab[p->ct_id].head]*errec[p->ct_id];    
			  
			/*  cotab[p->ct_id].cm -= 
			      (outm[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cm)*p1*errec[p->ct_id];
			    cotab[p->ct_id].cn -= 
			      (outn[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cn)*p1*errec[p->ct_id];
			    cotab[p->ct_id].ca -= 
			      (outa[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].ca)*errec[p->ct_id];
			    cotab[p->ct_id].cb -= 
			      (outb[cotab[p->ct_id].lh][cotab[p->ct_id].head]-cotab[p->ct_id].cb)*errec[p->ct_id];        
  			 */  
  			   if(cotab[p->ct_id].cm>1.0)
			     cotab[p->ct_id].cm=1.0;
			   if(cotab[p->ct_id].cm<.0)
			     cotab[p->ct_id].cm=.0;  
			   if(cotab[p->ct_id].cn>1.0)
			     cotab[p->ct_id].cn=1.0;
			   if(cotab[p->ct_id].cn<.0)
			     cotab[p->ct_id].cn=.0; 
		           if(cotab[p->ct_id].ca>.5)
			     cotab[p->ct_id].ca=.5;
			   if(cotab[p->ct_id].ca<.0)
			     cotab[p->ct_id].ca=.0;
			   if(cotab[p->ct_id].cb>.5)
			     cotab[p->ct_id].cb=.5;
			   if(cotab[p->ct_id].cb<.0)
			     cotab[p->ct_id].cb=.0;
			   p=p->next;
		         } else p=p->next;/* while p ,if */
		      }; /* if outm */
if(debug_flag != 0)
{
  for(i=1;i <= laynum; i++)
    for (j=1;j<=laynods[i];j++)
       printf("%d:%d om=%1.2f, on:%1.2f, oa=%1.2f, ob=%1.2f, dm=%1.2f, dn=%1.2f, da=%1.2f, db=%1.2f\n",
	i,j,outm[i][j],outn[i][j],outa[i][j],outb[i][j],errm[i][j],errn[i][j],erra[i][j],errb[i][j]);
  for (i=1;i<=laynods[laynum];i++)
     printf("target[%d]=%1.2f %1.2f %1.2f %1.2f\n", i,pattagm[i][no_pat],pattagn[i][no_pat],pattaga[i][no_pat],
		pattagb[i][no_pat]);
  for(i=1;i<=no_link;i++)
     printf("%d:%d->%d:%d  m:%0.3f n:%0.3f a:%0.3f b:%0.3f\n",cotab[i].lh,cotab[i].head,
cotab[i].lt,cotab[i].tail,cotab[i].cm,cotab[i].cn,cotab[i].ca,cotab[i].cb);
 } 
	return(mse);
}

#endif

load()
{
	int flag=0;
	char select;
	while(flag==0)
	{
		printf("\n");
		printf("**********************\n");
		printf("*      load          *\n");
		printf("**********************\n");
		printf("1.load network file\n");
		printf("2.load pattern file\n");
		printf("3.load symbolic weight file\n");
		printf("4.load numerical weight file\n");
		printf("0.quit\n");
		printf("select a function...");
		scanf("%s",&select);
		switch(select)
		{
		case '1':
			printf("Please input network filename(*.net)..");
			scanf("%s",&netfile);
			printf("\nloading %s...\n",netfile);
			readnet(netfile);
			break;
		case '2':
			printf("Please input pattern filename..");
			scanf("%s",&patfile);
			printf("\nloading %s...\n",patfile);
			readpat(patfile);
			break;
		case '3':
			printf("Please input symbolic weighe filename(*.wts)..");
			scanf("%s",&wfile);
			printf("\nloading %s...\n",wfile);
			readwts(wfile);
			break;
		case '4':
			printf("Please input numerical weight filename..");
			scanf("%s",&wfile);
			printf("\nloading %s...\n",wfile);
			readwfile(wfile);
			break;
		case '0':
			flag=1;
			break;
		default:
			printf("error!");
		}
	}
}

save()
{
	char select;
	int flag=0;
	printf("\n");
	printf("**********************\n");
	printf("*      save          *\n");
	printf("**********************\n");
	printf("1.save current file\n");
	printf("2.save as new file\n");
	printf("select a function...");
	scanf("%s",&select);
	if (select =='2')
	{
		printf("Please input connection table filename...");
		scanf("%s",wfile);
	}
	printf("saving files...\n");
	writewts(wfile);
	
}

learning()
{
	float pss,last_pss,pssa[5001];
	int i,current_pat,cycno=0,iteration,ratio,npoints=100;
	char key,outfile[30];
 	FILE *mfile;	

	printf("learning...\n");
	printf("threshold %1.2f noepcoh %d\n",threshold,noepcoh);
	iteration=noepcoh;

	do
	{
	  do
	  {
		pss=0.0;
		get_pat();
		for(i=1;i<=patnum;i++)
		{ 
		/*	current_pat=patodr[i];
			compute_output_ori(current_pat); 
			pss+=weight_change(current_pat); */
			compute_output(i);
			pss+=weight_change(i);
		/*  	printf(" pss = %1.5f \n",pss+1);*/
		}
		pss/=patnum;
		pssa[cycno]=pss;
		cycno++;
		if(bg_fg!=1)
		{
		 /*  if(cycno % 50 ==0) */
		      printf("cycle no.%d pss:%1.5f \n",cycno,pss);
		}
	   } while(pss>threshold && cycno<=iteration);
	if (bg_fg==1)
	{
		write_rep(cycno,pss);
		writewts(wfile);
		cycno--;
		ratio=cycno/npoints;
		if ( cycno < npoints ) ratio=1;
		strcpy(outfile,rep_file);
		strcat(outfile,".m");
		if ((mfile=fopen(outfile,"w"))==NULL) {printf("open file error\n"); exit(0);}
		/* fprintf(mfile,"a=["); */
		for (i=0;i < cycno-ratio;i+=ratio)
  		  fprintf(mfile,"%4d %1.5f\n",i,pssa[i]);
  		/* remark begin
		fprintf(mfile,"%1.5f]\n",pssa[i]);
		fprintf(mfile,"x=1:%d:%d\nplot(x,a)\n",ratio,cycno);
		fprintf(mfile,"title('lr_pre=%1.3f, lr_con=%1.3f, pat_num=%d')\n",lr_pre,lr_con,patnum);
		fprintf(mfile,"xlabel('iteration')\n");
		fprintf(mfile,"ylabel('pss')\n");
		fclose(mfile);
                remark end */
	 }
	else	
	{
	if(bg_fg!=1) /*foreground running*/
	 {
	  printf("scount:%d\n",scount);
	  printf("continue(y/n)?");
	  scanf("%s",&key);
          }
         
	}
	iteration+=noepcoh;
	if(bg_fg==1) 
		break;
	}while(key=='y');
	
	   
}


test()
{
	int i,j,pat_num,exist=0;
	float mse=0.0,err;
	printf("***testing***\n");
	while(1)
	{
		mse=0.0;
		printf("Please input test pattern number(0 for exit)...");
		scanf("%d",&pat_num);
		if (pat_num == 0)
			break;
		else
		{
 			compute_output(pat_num);
			for(i=1;i<=laynods[laynum];i++)
			{ 	/* fuzzy target */
				err=(pattagm[i][pat_num]-outm[laynum][i])*
				    (pattagm[i][pat_num]-outm[laynum][i])+
				    (pattagn[i][pat_num]-outn[laynum][i])*
				    (pattagn[i][pat_num]-outn[laynum][i])+
				    (pattaga[i][pat_num]-outa[laynum][i])*
				    (pattaga[i][pat_num]-outa[laynum][i])+
				    (pattagb[i][pat_num]-outb[laynum][i])*
				    (pattagb[i][pat_num]-outb[laynum][i]);
				mse+=err/2;
				printf("o%d: %1.2f %1.2f %1.2f %1.2f d: %1.2f %1.2f %1.2f %1.2f \n",
					i,outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i],
					pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],
					pattagb[i][pat_num]);
			}
			printf("Pss= %3.6f\n",mse);
		}
	}
}


init()
{
	char filename[10];
	printf("\n");
	printf("**********************\n");
	printf("*    initalization   *\n");
	printf("**********************\n");
	printf("Please input start filename(*.str)..");
	scanf("%s",&filename);
	printf("loading %s...\n",filename);
	starfile(filename);
	build_link();
	readpat(trafile);

}

set_par()
{
 char key;
  printf("Please input parameter value...\n");
  printf("Conclusion learning rate:(%1.5f) ",lr_con);
  scanf("%f",&lr_con);
  printf("Premise learning rate:(%1.5f) ",lr_pre);
  scanf("%f",&lr_pre);
  printf("no. of epcoh %d ",noepcoh);
  scanf("%d",&noepcoh);
  printf("background running(y/n)?");
  scanf("%s",&key);
  bg_fg=0;
  if(key=='y')
    bg_fg=1;
  if(bg_fg==1)
  {
  	printf("report file name:(%s)",rep_file);
  	scanf("%s",&rep_file);
  	printf("weight file name:(%s)",wfile);
  	scanf("%s",&wfile);
   }
}

on_line_test()
{
 int i,j;
 char ch;
 float mse,err;
 printf("On-LINE test\n");
 while(1)
 {
   for(i=1;i<=laynods[1];i++)
	{
	  printf("Please input data(m n a b f) for node %d...",i);
	  scanf("%f %f %f %f %f", &patm[i][0], &patn[i][0], &pata[i][0], 
	          &patb[i][0], &patf[i][0]);
	}  
  for(i=1;i<=laynods[laynum];i++)
	{
	  printf("Please input target output %d...",i);
	  scanf("%f %f %f %f", &pattagm[i][0],&pattagn[i][0],
	                                &pattaga[i][0],&pattagb[i][0]);
	}
	
   compute_output(0);
   for(i=1;i<=laynods[laynum];i++)
			{
			     /* j=i-first[laynum]+1;
				err=pattag[j][0]-outm[laynum][i];
				mse+=err*err/2;
				printf("out %d: %1.2f  target: %1.2f\n",
				        i,outm[laynum][i], pattag[j][0]); */
				        
				printf("o%d: %1.2f %1.2f %1.2f %1.2f %1.2f\n",
				               i,outm[laynum][i],outn[laynum][i],
					        outa[laynum][i],outb[laynum][i],
					        outf[laynum][i]);
			}
  /* printf("Pss= %3.3f\n",mse);*/
   printf("Continue (y/n)?",ch);
   scanf("%s",&ch);
   if(ch=='n')
	break;
  }
}

write_rep(cycle,pss)
int cycle;
float pss;
{
	FILE *fptr;
	int i,j,k;
	char weight[4];
	fptr=fopen(rep_file,"w");
	printf("write to report table-\n");
 	fprintf(fptr,"cycle no: %d pss: %1.6f conclusion learning rate %1.5f premise learning rate %1.5f\npat=%s",cycle-1,pss,lr_con,lr_pre,patfile);

	fclose(fptr);

}


build_data()
{
 int i,j,no_instance;
 char ch,mod,train_file[10],buffer1[80],buffer[256];
 float mse,err,range_l[10],range_h[10];
 FILE *fptr;

 printf("Please input training filename_new:");
 scanf("%s",train_file);
 fptr=fopen(train_file,"w");
 
 
 printf("manual or automatic (m/a)?");
 scanf("%s",&mod);
 if(mod=='m')
 {
   while(1)
   {
   for(i=1;i<=laynods[1];i++)
	{
	  printf("Please input data(m n a b f) for node %d: ",i);
	  scanf("%f %f %f %f %f", &patm[i][0], &patn[i][0], &pata[i][0], &patb[i][0], &patf[i][0]);
	/* scanf("%f", &patm[i][0]);
	  pata[i][0]=.2;
	  patb[i][0]=.2;  */
	fprintf(fptr,"%1.2f %1.2f %1.2f %1.2f %1.2f ",patm[i][0],patn[i][0], pata[i][0], patb[i][0], patf[i][0]);
 	} 
   compute_output(0); 
   
   for(i=1;i<=laynods[laynum];i++)
	{
	  fprintf(fptr,"%1.2f %1.2f %1.2f %1.2f %1.2f ",outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i],outf[laynum][i]);
 	/*  fprintf(fptr,"%1.2f ",outm[laynum][i]);  */
          printf("%1.2f %1.2f %1.2f %1.2f %1.2f\n",outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i],outf[laynum][i]); 
	/*  printf("out[%d]=%1.3f\n",i,outm[laynum][i]);  */
	}
	 fprintf(fptr,"\n");
    printf("Continue (y/n)?");
   scanf("%s",&ch);
   if(ch=='n')
	break;
    }
  }
  else
  {  
      printf("How many instances?");
      scanf("%d",&no_instance);
      for(i=1;i<=laynods[1];i++)
	{
/*
    	  printf("Please input range for node %d(lower and upper)",i);
          scanf("%f %f",&range_l[i],&range_h[i]);
*/
	  range_l[i]=0;
 	  range_h[i]=1;
	}
      for(i=1;i<=no_instance;i++)
      {
         buffer[0]='\0';
	 for(j=1;j<=laynods[1];j++)
	{
	   patm[j][0]=rnd(range_h[j]-range_l[j])+range_l[j];
	   patn[j][0]=rnd(.3);
	   patm[j][0]-=patn[j][0]/2;
	   if (patm[j][0]<0) patm[j][0]=0;
 	   patn[j][0]+=patm[j][0];
	   if (patn[j][0]>1) patn[j][0]=1;
	   pata[j][0]=rnd(.3);
	   patb[j][0]=rnd(.3);
	   patf[j][0]=1;
          /* printf("read input = %1.4f %1.4f %1.4f %1.4f %1.4f \n",patm[j][0],patn[j][0],pata[j][0],patb[j][0],patf[j][0]); */
           sprintf(buffer1,"%1.4f %1.4f %1.4f %1.4f %1.4f ",patm[j][0],patn[j][0],pata[j][0],patb[j][0],patf[j][0]);          /*  i ---> 0 */
           strcat(buffer, buffer1);
	 }
         compute_output(0);  
         if((outm[laynum][1]==0.0) && (outn[laynum][1]==1.0)) 
   /*      if(((outm[laynum][1]==0.0) && (outn[laynum][1]==1.0)) || (outf[laynum][1]<=0.01) ) */
         	i--;
         else   
         {
         fprintf(fptr,"%s",buffer);
         for(j=1;j<=laynods[laynum];j++)
	    {
 	    fprintf(fptr,"%1.4f %1.4f %1.4f %1.4f %1.4f ",outm[laynum][j],outn[laynum][j],
 	  		outa[laynum][j],outb[laynum][j],outf[laynum][j]);
	    printf("instance %d out[%d]=%1.3f %1.3f %1.3f %1.3f %1.3f\n",i,j,outm[laynum][j],outn[laynum][j],outa[laynum][j],outb[laynum][j],outf[laynum][j]);
	    }
	 fprintf(fptr,"\n");
	 
	 }
	 
	 
	 
       }
   }
	fclose(fptr);
}

build_cpr_data()
{
 int i,j,no_instance;
 char ch,mod,train_file[10],buffer1[80],buffer[256];
 float mse,err,range_l[10],range_h[10];
 FILE *fptr;

 printf("Build training data for old_algorithm \n");
 printf("Please input training filename_new:");
 scanf("%s",train_file);
 fptr=fopen(train_file,"w");
 
 
    
      for(i=1;i<=patnum;i++)
      {
         buffer[0]='\0';
         for(j=1;j<=laynods[1];j++)
         {
         printf("read input = %1.4f %1.4f %1.4f %1.4f \n",patm[j][i],patn[j][i],pata[j][i],patb[j][i],patf[j][i]); 	   
         sprintf(buffer1,"%1.4f %1.4f %1.4f %1.4f ",patm[j][i],patn[j][i],pata[j][i],patb[j][i],patf[j][i]);         
	 strcat(buffer, buffer1);
	 }                 
         compute_output_ori(i);  
         fprintf(fptr,"%s",buffer);
         for(j=1;j<=laynods[laynum];j++)
	    {
 	    fprintf(fptr,"%1.2f %1.2f %1.2f %1.2f ",outm[laynum][j],outn[laynum][j],
 	  		outa[laynum][j],outb[laynum][j]);
	    printf("instance %d out[%d]=%1.3f %1.3f %1.3f %1.3f \n",i,j,outm[laynum][j],outn[laynum][j],outa[laynum][j],outb[laynum][j]);
	    }
	 fprintf(fptr,"\n");
	 
       }
   
	fclose(fptr);
}

/****************************************
 * compare the sim. degree of two rules *
 ****************************************/
float comp_rule(ra,rb)
RULE *ra,*rb; 
{   RULE *ta,*tb,*oa=ra,*ob=rb,*tmp;
    float min_sim,s_deg,s1;
    float m1,n1,a1,b1,m2,n2,a2,b2;
    int found;
    tmp=rb;
    while(ra->next != NULL)
    {
        ta=ra->next;
	found=0;
	min_sim=1;
	rb=tmp;
	while(rb->next != NULL)
	{
    	    tb=rb->next;
	    if((ta->layer == tb->layer) && (ta->node == tb->node))
	    {
		m1=ta->m;n1=ta->n;
		a1=ta->a;b1=ta->b;
		m2=tb->m;n2=tb->n;
		a2=tb->a;b2=tb->b;
	      	s_deg=similarity(m1,n1,a1,b1,m2,n2,a2,b2);
		if(min_sim > s_deg) {
		  min_sim = s_deg;
		  found=1;
 		  printf("ref:found %d\n",found);
 		}  
	     }
	     rb=rb->next;
	 }
/*
	 if((found ==0) || (min_sim == 0.0))
	 {
	     ra=NULL;
	  }
	 else 
*/
	 ra=ra->next;
      }
    if (found == 0)  s1=0;
	else s1=min_sim;
    ra=ob;rb=oa;
    while(ra->next != NULL)
    {
        ta=ra->next;
	found=0;
	min_sim=1;
	rb=oa;
	while(rb->next != NULL)
	{
    	    tb=rb->next;
	    if((ta->layer == tb->layer) && (ta->node == tb->node))
	    {
		m1=ta->m;n1=ta->n;
		a1=ta->a;b1=ta->b;
		m2=tb->m;n2=tb->n;
		a2=tb->a;b2=tb->b;
	      	s_deg=similarity(m1,n1,a1,b1,m2,n2,a2,b2);
		if(min_sim > s_deg) {
		  min_sim = s_deg;
		  found=1;
		}  
	     }
	     rb=rb->next;
	 }
	 ra=ra->next;
      }
      printf("s1=%1.2f;min_sim=%1.2f\n",s1,min_sim);
      if(found == 0) min_sim=0;
      if (s1 > min_sim ) return s1;
      else return min_sim;
}

/***************************************
 * return the dimension of the premise *
 ***************************************/
int dim_rule(p)
RULE *p;
{
	int i=0;
	while(p->next != NULL)
	{
	    i++;
	    p=p->next;
	}
	return i;
}

/***************************************
 * return the dimension of the premise *
 ***************************************/
list_rule(p)
RULE *p;
{	RULE *tmp;
	while(p->next != NULL)
	{
	    tmp=p->next;
	    printf("%d: %d m=%1.2f, n=%1.2f, a=%1.2f, b=%1.2f\n",tmp->layer,tmp->node,
		tmp->m,tmp->n,tmp->a,tmp->b);
	    p=p->next;
	}
}

/******************
 * rb = (ra+rb)/2 *
 ******************/
combine_rule(ra,rb,weight)
RULE *ra,*rb ;
int weight;
{   RULE *ta,*tb;
    while(ra->next != NULL)
    {
    	ta=ra->next;
	while(rb->next != NULL)
	{
    	    tb=rb->next;
	    if((ta->layer == tb->layer) && (ta->node = tb->node))
	    {
		tb->m=(tb->m*weight+ta->m)/(weight+1);
		tb->n=(tb->n*weight+ta->n)/(weight+1);
		tb->a=(tb->a*weight+ta->a)/(weight+1);
		tb->b=(tb->b*weight+ta->b)/(weight+1);
	     }
	      rb=rb->next;
	 }
	  ra=ra->next;
      }
}


check()
{
 	char buffer[80],buffer1[20];
	int i,j,k,r_no=1,g_no=0,g_max;
	int numgc[maxnod],lindex[maxlay],lrecord[maxnod];
	char select;
	float max_sim,presim,consim;
	ct_node *p,*q;
	RULE *r,*tmp,*tmp2,*gcpre[maxnod],*gccon[maxnod];
	GROUP *g;
	float threshold=0.8;
	/* addedd by torch */
	int wi,wj,wk,wl,index=1;
	float m,n,a,b;	/* 4 */
	char weight[3];

        for (i=1;i<maxlay;i++)
          lindex[i]=1;
	for(i=1;i<maxnod;i++)
	    {
		premise[i]=(RULE *)malloc(sizeof(RULE));
		conclusion[i]=(RULE *)malloc(sizeof(RULE));
		group[i]=(GROUP *)malloc(sizeof(GROUP));
		gcpre[i]=(RULE *)malloc(sizeof(RULE));
		gccon[i]=(RULE *)malloc(sizeof(RULE));
		numgc[i]=0;
		premise[i]->next=NULL;
		conclusion[i]->next=NULL;
		group[i]->next=NULL; /* recording the member of each group */
		gcpre[i]->next=NULL; /* center of group */
		gccon[i]->next=NULL;
	     }
	printf("Threshold? (from 0 to 1) ");
	scanf("%f",&threshold);     
	if (threshold==0)
	  threshold=0.000001;
	if (threshold>1)
	  threshold=1;  
	for(i=2;i<=laynum;i+=2)  /* for each conjunction layer */
	    for(j=1;j<=laynods[i];j++)    /* for each node */
            {
		 p=ct_head[i][j];
		 q=ct_tail[i][j];

		 /* premise part */
		 sprintf(buffer,"\n  IF ");
		 while(p->next != NULL) /* for each link lead to the node */
		  {
		    	r=(RULE *)malloc(sizeof(RULE));
			r->next = premise[r_no]->next;
			premise[r_no]->next = r;
			r->node = cotab[p->ct_id].head;
			r->layer = cotab[p->ct_id].lh;
			r->m = cotab[p->ct_id].cm;
			r->n = cotab[p->ct_id].cn;
			r->a = cotab[p->ct_id].ca;
			r->b = cotab[p->ct_id].cb;
 		  	p=p->next;
		  }
		 while(q->next != NULL)  /* concluseion part */
		  {
			r=(RULE *)malloc(sizeof(RULE));
			r->next = conclusion[r_no]->next;
			conclusion[r_no]->next = r;
			r->node = cotab[q->ct_id].tail;
			r->layer = cotab[q->ct_id].lt;
			r->m = cotab[q->ct_id].cm;
			r->n = cotab[q->ct_id].cn;
			r->a = cotab[q->ct_id].ca;
			r->b = cotab[q->ct_id].cb;
 		  	q=q->next;
		  }
		  r_no++;
	     }

	for(i=1;i<r_no;i++)  /* list all rules */
{
		printf("Rule #%3d\n",i);
		list_rule(premise[i]);
		list_rule(conclusion[i]);
}

	for(i=1;i<r_no;i++)
	   {
		max_sim=0;
	        g_max=0;
		consim=0;
		g=(GROUP *)malloc(sizeof(GROUP));
		for(j=1;j<g_no+1;j++)
		{
		   printf("ref: rule %2d, gc %2d\n",i,j);
		   presim= comp_rule(premise[i],gcpre[j]);
		   if (max_sim < presim)
		   {
			max_sim = presim;
			g_max = j;
			consim=comp_rule(conclusion[i],gccon[j]);
		   }
		    
		};
	printf("test point %d,g_max:%d,max_sim=%1.2f, %1.2f\n",i,g_max,max_sim,consim);
		if(max_sim < threshold) /*no cluster near this premise, */
		{			/* form a new cluster */
		    g_no++;
		    group[g_no]->next=g;
		    g->next=NULL;
		    g->number=i;
		    assign_rule(premise[i], &gcpre[g_no]);
		    assign_rule(gcpre[g_no],&gcpre[g_no]);
		    assign_rule(conclusion[i], &gccon[g_no]);
		    numgc[g_no]=1;
		printf ("new group %2d formed by rule %3d\n",g_no,i);    
		list_rule(premise[i]);
		list_rule(conclusion[i]);
		printf("dimension: %2d\n",dim_rule(gcpre[g_no]));
		} /* add the premise to the cluster which it is closest to, */
		else /* and adjust the cluster center */
		{
		    printf("candidate: rule %2d\n",i);
		    list_rule(premise[i]);
		    list_rule(conclusion[i]);
		    if (consim<=threshold) {
		      g_no++;
		      group[g_no]->next=g;
		      g->next=NULL;
		      g->number=i;
		      assign_rule(premise[i], &gcpre[g_no]);
		      assign_rule(gcpre[g_no],&gcpre[g_no]);
		      assign_rule(conclusion[i], &gccon[g_no]);
		      numgc[g_no]=1;
		      printf("conflict rule!\n");
	              printf ("new group %2d formed by rule %3d\n",g_no,i);    
		      list_rule(premise[i]);
		      list_rule(conclusion[i]);
		      printf("dimension: %2d\n",dim_rule(gcpre[g_no])); 
		    }
		    else {
		      g->next=group[g_max]->next;
		      g->number=i; /* premise i belongs to group g_max */
		      group[g_max]->next=g;
		      if (dim_rule(premise[i]) == dim_rule(gcpre[g_max])) {
			combine_rule(premise[i],gcpre[g_max],numgc[g_max]);
			combine_rule(conclusion[i],gccon[g_max],numgc[g_max]);
                        numgc[g_max]++;
                        printf("Deleted: redundant\n");
                      }    
		      else if (dim_rule(premise[i]) < dim_rule(gcpre[g_max])) {
			/* free the memory of gc[g_max], not implemented */
			/* assign premise i to gc[g_max] */
			assign_rule(premise[i],&gcpre[g_max]);
	                assign_rule(conclusion[i],&gccon[g_max]);
	                numgc[g_max]=1;
	                printf("Replace current group center\n");		
                      } 
                      else 
                      printf("Deleted: subsumed\n"); 
                      printf("rule:%d  group:%d\n",dim_rule(premise[i]),dim_rule(gcpre[g_max]));  
		      };
		    };  
	    }
	for(i=1;i<=g_no;i++)
        {
		printf("Group %2d\n",i);
		printf("premise:\n");
		list_rule(gcpre[i]);
		printf("conclusion:\n");
		list_rule(gccon[i]);
        }
  printf("Perform change to weight?");
  scanf("%s",&select);
  if (select=='Y'||select=='y') {
    printf("Change weight according to CC\n");
    printf("Available: Remove redundency\n");
    index=1;
  /* under construction begin */

	for (i=1;i<=g_no;i++)    /* build premise links */
	{
	  while(gcpre[i]->next != NULL) {
	    tmp=gcpre[i]->next;
	    tmp2=gccon[i]->next;
	    cotab[index].lh=tmp->layer;
	    cotab[index].head=tmp->node;
	    cotab[index].lt=tmp2->layer-1;
	    cotab[index].tail =lindex[tmp2->layer-1];
	    cotab[index].cm = tmp->m;
	    cotab[index].cn = tmp->n;
	    cotab[index].ca = tmp->a;
	    cotab[index].cb = tmp->b;

	printf("%d:%d-->%d:%d weight: %1.3f %1.3f %1.3f %1.3f %s\n",
			cotab[index].lh,
			cotab[index].head,
			cotab[index].lt,
			cotab[index].tail,
			cotab[index].cm,
			cotab[index].cn,/* 4 */
			cotab[index].ca,
			cotab[index].cb,
			weight);
		gcpre[i]=gcpre[i]->next;
		index++;
	  }
	  lrecord[i]=lindex[tmp2->layer-1];
	  lindex[tmp2->layer-1]++;
	}
	 /* build conclusion links */
	 for (i=1;i<=g_no;i++) {	
	     tmp=gccon[i]->next;
		cotab[index].lh=tmp->layer-1;
		cotab[index].head=lrecord[i];
		cotab[index].lt=tmp->layer;
		cotab[index].tail =tmp->node;
		cotab[index].cm = tmp->m;
		cotab[index].cn = tmp->n;
		cotab[index].ca = tmp->a;
		cotab[index].cb = tmp->b;

	printf("%d:%d-->%d:%d weight: %1.3f %1.3f %1.3f %1.3f %s\n",
			cotab[index].lh,
			cotab[index].head,
			cotab[index].lt,
			cotab[index].tail,
			cotab[index].cm,
			cotab[index].cn,/* 4 */
			cotab[index].ca,
			cotab[index].cb,
			weight);
	index++;		
      }			
	printf("number of connections: %d\n", index-1);
	no_link=index-1;
  /* under construction end*/
  } 
  /* qsort(cotab,max,sizeof(struct ct),lcompare); */
  return;       
}

/***************************
 * assign rule a to rule b *
 ***************************/
assign_rule(ra,rb)
RULE *ra,**rb; 
{   RULE *ta,*tb;
    *rb=(RULE *)malloc(sizeof(RULE));
    (*rb)->next=NULL;
    while (ra->next != NULL)
    {
    	ta=ra->next;
	tb=(RULE *)malloc(sizeof(RULE));
	tb->next = (*rb)->next;
	tb->layer= ta->layer ;
	tb->node= ta->node ;
	tb->m= ta->m;
	tb->n= ta->n;
	tb->a= ta->a;
	tb->b= ta->b;
	(*rb)->next = tb;
	ra=ra->next;
     }
}

/* batch test of network model*/
batest()
{
  int i,j,pat_num,exist=0,
      ind_sim_d,ind_sim_o,term_num=5,
      term_distance[term_num],
      result_term_count[term_num];
      
  float mse=0.0,err,tt_mse,av_mse;
  float max_sim_o,max_sim_d,sim;
  char term_flag_o[5],term_flag_d[5];
  
  tt_mse=0;
  if (patnum==0)
    return;
  printf("***batch testing***\n");
  printf("patnum= %d\n",patnum);
  printf("laynum = %d\n",laynum);
  printf("laynods[laynum] = %d\n",laynods[laynum]);
  printf("\n");
  
  for(i=0;i<term_num;i++)   /* initiate term_distance */
     {
     term_distance[i]=0;
     result_term_count[i]=0;
     }
  for (pat_num=1;pat_num<=patnum;pat_num++) {
    mse=0.0;
    printf(" pattern number : %d \n",pat_num);      
    compute_output(pat_num);
    for(i=1;i<=laynods[laynum];i++) {
      /* fuzzy target */
      err=3*(pattagm[i][pat_num]-outm[laynum][i])*
        (pattagm[i][pat_num]-outm[laynum][i])+
        3*(pattagn[i][pat_num]-outn[laynum][i])*
        (pattagn[i][pat_num]-outn[laynum][i])+
        (pattaga[i][pat_num]-outa[laynum][i])*
        (pattaga[i][pat_num]-outa[laynum][i])+
        (pattagb[i][pat_num]-outb[laynum][i])*
        (pattagb[i][pat_num]-outb[laynum][i]);
      mse+=err/2;
      
      /* defuzzification begin */
      /* compute result */
  
       max_sim_d=0;
       ind_sim_d=0; 
       sim=similarity(.0, .05, .0, .1, pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],pattagb[i][pat_num]);
        if(sim > max_sim_d)
          {
             max_sim_d=sim;
             ind_sim_d=1;
             term_flag_d[0]='\0';
             strcat(term_flag_d,"NB");
          }             
        sim=similarity(.2, .3, .1, .1, pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],pattagb[i][pat_num]);
        if(sim > max_sim_d)
          {
             max_sim_d=sim;
             ind_sim_d=2;
             term_flag_d[0]='\0';
             strcat(term_flag_d,"NM");
          }        
        sim=similarity(.45, .55, .1, .1, pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],pattagb[i][pat_num]);
        if(sim > max_sim_d)
          {
             max_sim_d=sim;
             ind_sim_d=3;
             term_flag_d[0]='\0';
             strcat(term_flag_d," Z");
          }   
        sim=similarity(.7, .8, .1, .1, pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],pattagb[i][pat_num]);
        if(sim > max_sim_d)
          {
             max_sim_d=sim;
             ind_sim_d=4;
             term_flag_d[0]='\0';
             strcat(term_flag_d,"PM");
          }   
        sim=similarity(.95, 1.0, .1, .0, pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],pattagb[i][pat_num]);
        if(sim > max_sim_d)
          {
             max_sim_d=sim;
             ind_sim_d=5;
             term_flag_d[0]='\0';
             strcat(term_flag_d,"PB");
          }   
          
       /* defuzzification of desired output */
        
       max_sim_o=0;
       ind_sim_o=0;
       sim=similarity(.0, .05, .0, .1, outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i]);
        if(sim > max_sim_o)
          {
             max_sim_o=sim;
             ind_sim_o=1;
             term_flag_o[0]='\0';
             strcat(term_flag_o,"NB");
          }             
        sim=similarity(.2, .3, .1, .1, outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i]);
        if(sim > max_sim_o)
          {
             max_sim_o=sim;
             ind_sim_o=2;
             term_flag_o[0]='\0';
             strcat(term_flag_o,"NM");
          }        
        sim=similarity(.45, .55, .1, .1, outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i]);
        if(sim > max_sim_o)
          {
             max_sim_o=sim;
             ind_sim_o=3;
             term_flag_o[0]='\0';
             strcat(term_flag_o," Z");
          }   
        sim=similarity(.7, .8, .1, .1, outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i]);
        if(sim > max_sim_o)
          {
             max_sim_o=sim;
             ind_sim_o=4;
             term_flag_o[0]='\0';
             strcat(term_flag_o,"PM");
          }   
        sim=similarity(.95, 1.0, .1, .0, outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i]);
        if(sim > max_sim_o)
          {
             max_sim_o=sim;
             ind_sim_o=5;
             term_flag_o[0]='\0';
             strcat(term_flag_o,"PB");
          }   
          
          
          
      
      /* defuzzification end */
      
      term_distance[abs( ind_sim_d - ind_sim_o )]++;
      result_term_count[ind_sim_o-1]++;
      
      /*  this is an entire print  **************************************
      
      printf(" pattern number : %d \n",pat_num);
      if(ind_sim_d != ind_sim_o)
            printf("**************** term_distance > 0 ****************\n");
      printf("o%d: %1.2f %1.2f %1.2f %1.2f %s sim %1.2f  fac %1.2f \n",
           i,outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i],term_flag_o,max_sim_o,outf[laynum][i]);
      printf("d%d: %1.2f %1.2f %1.2f %1.2f %s sim %1.2f  fac %1.2f \n",
           i,pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],pattagb[i][pat_num],term_flag_d,max_sim_d,pattagf[i][pat_num]);
      printf("\n");    
      
      ***************************************************************** */

      /* partial result that term_distance >0 */
      printf(" pattern number : %d \n",pat_num);      
      if(ind_sim_d != ind_sim_o)
        {
     /*   printf(" pattern number : %d \n",pat_num);  0115 */
        printf("**************** term_distance > 0 ****************\n");
        printf("o%d: %1.4f %1.4f %1.4f %1.4f %s => %1.4f  fac %1.2f \n",
           i,outm[laynum][i],outn[laynum][i],outa[laynum][i],outb[laynum][i],term_flag_o,max_sim_o,outf[laynum][i]);
        printf("d%d: %1.4f %1.4f %1.4f %1.4f %s => %1.4f  fac %1.2f \n",
           i,pattagm[i][pat_num],pattagn[i][pat_num],pattaga[i][pat_num],pattagb[i][pat_num],term_flag_d,max_sim_d,pattagf[i][pat_num]);
        printf("\n");
        }
      
    }
    tt_mse+=mse;
  }
  for(i=0;i<term_num;i++)
     printf("total %d term %d \n",result_term_count[i],i);
  av_mse=tt_mse/patnum;
  printf("Average Pss= %3.6f\n",av_mse);
  
  printf("TOTAL :  %d test patterns \n",patnum);
  printf("   where  \n");
  for(i=0;i<term_num;i++)
     printf("   %d  patterns, the term_distance is %d \n",term_distance[i],i);
}

/* clear record of weight change */
clear()
{
  int i;
  for (i=0;i<max;i++) {
    errec[i]=0;
    cflag[i]=0;
  }
  return;
}

static int fcompare(i,j)
float *i,*j;
{
  return(*i<*j);
}



esort()
{
  int rank[max],i,j,limit,flag;
  float temp[max];
  
  scount++; 
  if (!lmode) {
    for (i=1;i<max;i++) {
      cflag[i]=1;
    }
    return;
  }    
  
  limit=0;
  for (i=0;i<max;i++) { 
    temp[i]=errec[i];
    if (errec[i]>0)
      limit++;
  } 
  qsort(temp,max,sizeof(float),fcompare);
  for (i=0;i<max;i++) { 
    flag=0;
    for (j=0;j<max&&!flag;j++) 
      if (errec[i]>=temp[j]) { 
        rank[i]=j+1;
        flag=1;
      }
    if (rank[i]<=(limit/sfact+1)||!lmode)  
      cflag[i]=1;
  }  
  return;
}

list_link()
{
  int l;
  float max_sim,sim;
  char term_flag[5];
  
  for (l=1;l<=no_link;l++) {
        max_sim=0;
        term_flag[0]='\0';
        sim=similarity(.0, .05, .0, .1, cotab[l].cm,cotab[l].cn,cotab[l].ca,cotab[l].cb);
        if(sim > max_sim)
          {
             max_sim=sim;
             term_flag[0]='\0';
             strcat(term_flag,"NB");
          }             
        sim=similarity(.2, .3, .1, .1, cotab[l].cm,cotab[l].cn,cotab[l].ca,cotab[l].cb);
        if(sim > max_sim)
          {
             max_sim=sim;
             term_flag[0]='\0';
             strcat(term_flag,"NM");
          }        
        sim=similarity(.45, .55, .1, .1, cotab[l].cm,cotab[l].cn,cotab[l].ca,cotab[l].cb);
        if(sim > max_sim)
          {
             max_sim=sim;
             term_flag[0]='\0';
             strcat(term_flag," Z");
          }   
        sim=similarity(.7, .8, .1, .1, cotab[l].cm,cotab[l].cn,cotab[l].ca,cotab[l].cb);
        if(sim > max_sim)
          {
             max_sim=sim;
             term_flag[0]='\0';
             strcat(term_flag,"PM");
          }   
        sim=similarity(.95, 1.0, .1, .0, cotab[l].cm,cotab[l].cn,cotab[l].ca,cotab[l].cb);
        if(sim > max_sim)
          {
             max_sim=sim;
             term_flag[0]='\0';
             strcat(term_flag,"PB");
          }   
        printf("%d %d %d %d %1.2f %1.2f %1.2f %1.2f => %s  %1.4f\n",
 			cotab[l].lh,cotab[l].head,cotab[l].lt,cotab[l].tail,
			cotab[l].cm,cotab[l].cn,cotab[l].ca,cotab[l].cb
			,term_flag,max_sim);
  }  
  printf("Total links %d\n",no_link);
  return;
}

list_node()
{
int lay_index;

for(lay_index=1;lay_index<=laynum;lay_index++)
   printf("layer  %d  has  %d  node \n",lay_index,laynods[lay_index]);
   
}
list_net_structure()
{

list_link();
list_node();

}




trans_rebuild()
  {
  int i,j,k,flag,count;
  int lt,tail,lh,head;
  ct_node *p;
  
  count=0;                         /* rebuild the sequence of node for each */
  for(i=1;i<=no_link;i++)          /*     layer  */
     {
     if (cotab[i].lh!=0)           /* check the number of link */
        {
        count++;
        cotab[count].lh=cotab[i].lh;
        cotab[count].head=cotab[i].head;
        cotab[count].lt=cotab[i].lt;
        cotab[count].tail=cotab[i].tail;
        cotab[count].cm=cotab[i].cm;
        cotab[count].cn=cotab[i].cn;
        cotab[count].ca=cotab[i].ca;
        cotab[count].cb=cotab[i].cb;
        }
     }
  no_link=count;
  
  for(i=1;i<=laynum;i++)            /*rebuild the sequence of node ina layer*/
     {
     count=0;
     for(j=1;j<=laynods[i];j++)
        {
        flag=0;
        for(k=1;k<=no_link;k++)
           {
           if((cotab[k].lh==i)&&(cotab[k].head==j)
              ||((cotab[k].lt==i)&&(cotab[k].tail==j) ))
                flag=1;
           }
        if (flag==1)
           {
           count++;
           for(k=1;k<=no_link;k++)
              {
              if((cotab[k].lh==i)&&(cotab[k].head==j))
                  cotab[k].head=count;
              if((cotab[k].lt==i)&&(cotab[k].tail==j))
                  cotab[k].tail=count;    
              }
           }  
           
        }
     laynods[i]=count;   
     }
     
  for(i=2;i<=laynum-3;i=i+2)           /* rebuild the sequence of layer */ 
     {
     if((laynods[i]==0)&&(laynods[i+1]==0))
       {
       printf("*********Layer Rebuilding Active*********\n");
       for(j=i;j<=laynum-2;j++)
          laynods[i]=laynods[i+2];
       laynum=laynum-2;   
       for(k=1;k<=no_link;k++)
          {
          if(cotab[k].lh>i) cotab[k].lh=cotab[k].lh-2;
          if(cotab[k].lt>i) cotab[k].lt=cotab[k].lt-2;
          }
       }
     }
  for(i=1;i<=laynum;i++)                       /* rebuild the link */
	for(j=1;j<=laynods[i];j++)
        {
		if((ct_head[i][j]=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
                 {
                        ct_head[i][j]->next=NULL;
                        ct_head[i][j]->ct_id=0;
                  }
		if((ct_tail[i][j]=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
                 {
			ct_tail[i][j]->next=NULL;
			ct_tail[i][j]->ct_id=0;
                  }
        }

       for(i=1;i<=no_link;i++)
        {
		lt=cotab[i].lt;
		tail=cotab[i].tail;
		lh=cotab[i].lh;
		head=cotab[i].head;
		if((p=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
		 {
                        p->ct_id=i;
                        p->next=ct_head[lt][tail];
                        ct_head[lt][tail]=p;

		  } else printf("error allocating memory\n");
		if((p=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
		 {
                        p->ct_id=i;
                        p->next=ct_tail[lh][head];
			ct_tail[lh][head]=p;
                  } else printf("error allocating memory\n");
         }
     
     
  }
  
  
/*  
  
trans_combine(addr_x,addr_y)
int addr_x,addr_y;
  {
  int lh,head,lt,tail,tlt,ttail,count,k; 
  ct_node *p,*q,*r,*s,*pt;
  
  q=ct_tail[addr_x][addr_y];
  lt=cotab[q->ct_id].lt;
  tail=cotab[q->ct_id].tail;
  
  s=ct_tail[lt][tail];
  tlt=cotab[s->ct_id].lt;
  ttail=cotab[s->ct_id].tail;
  
  p=ct_head[addr_x][addr_y];
  k=no_link;
  printf("check-1\n");
  while(p->next!=NULL)
    {
    k++;
    cotab[k].lh=lt;
    cotab[k].head=laynods[lt]++;
    cotab[k].lt=tlt;
    cotab[k].tail=ttail;
    cotab[k].cm=cotab[s->ct_id].cm;
    cotab[k].cn=cotab[s->ct_id].cn;
    cotab[k].ca=cotab[s->ct_id].ca;
    cotab[k].cb=cotab[s->ct_id].cb;
    pt=ct_head[cotab[p->ct_id].lh][cotab[p->ct_id].head];
    while(pt->next!=NULL)
    {
    cotab[pt->ct_id].lt=cotab[k].lh;
    cotab[pt->ct_id].tail=cotab[k].head;
    pt=pt->next;
    }
    r=ct_head[lt][tail];
    printf("check-3\n");
    while(r->next!=NULL)
       {
        if(r->ct_id != q->ct_id)
           {
           k++;
           cotab[k].lh=cotab[r->ct_id].lh;
           cotab[k].head=cotab[r->ct_id].head;
           cotab[k].lt=lt;
           cotab[k].tail=laynods[lt];
           cotab[k].cm=cotab[r->ct_id].cm;
           cotab[k].cn=cotab[r->ct_id].cn;
           cotab[k].ca=cotab[r->ct_id].ca;
           cotab[k].cb=cotab[r->ct_id].cb;
           }
        printf("check-4\n");   
        r=r->next;   
       }
    p=p->next;   
    }
  no_link=k;
  printf("check-2\n");
  p=ct_head[addr_x][addr_y];
  while(p->next!=NULL)
     {
     cotab[p->ct_id].lh=0;
     p=p->next;
     }
  q=ct_tail[addr_x][addr_y];
  cotab[q->ct_id].lh=0;  
  
  
  p=ct_head[lt][tail];    
  while(p->next!=NULL)
     {
     cotab[p->ct_id].lh=0;
     p=p->next;
     }
  q=ct_tail[lt][tail];
  cotab[q->ct_id].lh=0;
  printf("combine terminated\n");  
  trans_rebuild();
  }
  
  */

int trans_separate(addr_x,addr_y)  
int addr_x,addr_y;
{
int i,distr;
float max_sim,sim;
int term_num=5,sepa_able,dif_term_count;
float aft_connect, bef_connect;
ct_node *p,*q;

for(i=1;i<=term_num;i++)     /* allocate temp space to put separated neuron */
  {
  if((ct_head_temp[i]=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
       {
        ct_head_temp[i]->next=NULL;
        ct_head_temp[i]->ct_id=0;
       }
  if((ct_tail_temp[i]=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
       {
        ct_tail_temp[i]->next=NULL;
	ct_tail_temp[i]->ct_id=0;
       } 
  }
  
p=ct_head[addr_x][addr_y];  /* distribute similar head-weights to same */
while(p->next!=NULL)        /*   partial neuron                        */
     {
        distr=0;
        max_sim=0;
        sim=similarity(.0, .05, .0, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=1;
             }
        sim=similarity(.2, .3, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=2;
             }
        sim=similarity(.45, .55, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=3;
             }
        sim=similarity(.7, .8, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=4;
             }
        sim=similarity(.95, 1.0, .1, .0, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             { 
             max_sim=sim;
             distr=5;
             } 
             
        if((q=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
          {
           q->ct_id=p->ct_id;
           q->next=ct_head_temp[distr];     
           ct_head_temp[distr]=q;     
          } else printf("error locating memory\n");  
         
        p=p->next; 
     }  
     
p=ct_tail[addr_x][addr_y];  /* distribute similar tail-weights to same */
while(p->next!=NULL)        /*   partial neuron                        */
     {
        distr=0;
        max_sim=0;
        sim=similarity(.0, .05, .0, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=1;
             }
        sim=similarity(.2, .3, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=2;
             }
        sim=similarity(.45, .55, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=3;
             }
        sim=similarity(.7, .8, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=4;
             }
        sim=similarity(.95, 1.0, .1, .0, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=5;
             }
        if((q=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
          {
           q->ct_id=p->ct_id;
           q->next=ct_tail_temp[distr];     
           ct_tail_temp[distr]=q;     
          } else printf("error locating memory\n");   
     
        p=p->next;
     }  
sepa_able=1;
dif_term_count=0;
for(i=1;i<=term_num;i++)       /*check if separatable */
   {
   if(ct_head_temp[i]->next==NULL)
     bef_connect=0;
   else
     bef_connect=1;
   
   if(ct_tail_temp[i]->next==NULL)
     aft_connect=0;
   else
     aft_connect=1; 
        
   if((aft_connect-bef_connect)!=0  )   
     sepa_able=0;
   
   if((ct_head_temp[i]->next!=NULL)&&(ct_tail_temp[i]->next!=NULL)) 
     dif_term_count++;
     
   }     
   
return(dif_term_count*sepa_able);   

}
  
float trans_check_comb(ind)
int ind;
{
float delta_g, delta_s, delta_gs, delta_sg, delta_p;
int i, j, k, lh, head, lt, tail, tlt, ttail;
int count_gout, count_gin, s_bef_in, s_aft_in, count;
ct_node *p,*q,*pt,*r,*s;

delta_g=-1;  /* for seperate 1/4 */               /* delta_g */

count_gin=0;
p=ct_head_temp[ind];
while(p->next!=NULL)
  {
  count_gin++;
  p=p->next;
  }
  
count_gout=0;  
q=ct_tail_temp[ind];
while(q->next!=NULL)
  {
  count_gout++;
  q=q->next;
  }
                            /* calculate delta_s */    
delta_s=(count_gout-1) * count_gin - count_gout;

s_bef_in=0;
p=ct_head_temp[ind];  /* count the total input weight of the s-neurons */
while(p->next!=NULL)  /* that before the G_neuron */
    {
     pt=ct_head[cotab[p->ct_id].lh][cotab[p->ct_id].head];  
     count=0;
     while(pt->next!=NULL)
       {
       count++;
       pt=pt->next;
       }
     s_bef_in+=count;  
     p=p->next;  
    }
    
s_aft_in=0;    
q=ct_tail_temp[ind];  /* count the total input weight of the s-neurons */
while(q->next!=NULL)  /*    that after the G_neuron             */
    {
     pt=ct_head[cotab[q->ct_id].lt][cotab[q->ct_id].tail];  
     count=0;
     while(pt->next!=NULL)
          {
          count++;
          pt=pt->next;
          }
     s_aft_in+=count;
     q=q->next;
    }
                                     /* calculate delta_gs, delta_sg */
                                         
delta_gs=s_bef_in * (count_gout-1) + (s_aft_in-count_gout) * (count_gin-1) 
                  - count_gout;
                  
delta_sg=count_gout * count_gin-count_gout - count_gin;                         

                                     /* calculate delta_p */
printf("---------------------------\n");
printf("count_gout = %d\n",count_gout);
printf("count_gin = %d\n",count_gin);
printf("s_aft_in = %d\n",s_aft_in);
printf("s_bef_in = %d\n",s_bef_in);
printf("delta_sg = %f \n",delta_sg);
printf("delta_gs = %f \n",delta_gs);
printf("delta_g = %f \n",delta_g);                                     
printf("delta_s = %f \n",delta_s);

delta_p=14*delta_sg + 16*delta_gs - 14*delta_g - delta_s;

if(delta_p<0)                 /********** doing rule combination ***********/        
  {                             
  printf("condition is passed, doing  combination \n ");                  
  k=no_link;
  q=ct_tail_temp[ind];
  while(q->next!=NULL)
       {
       lt=cotab[q->ct_id].lt;
       tail=cotab[q->ct_id].tail;
  
       s=ct_tail[lt][tail];
       tlt=cotab[s->ct_id].lt;
       ttail=cotab[s->ct_id].tail;
  
       p=ct_head_temp[ind];
       while(p->next!=NULL)
            {                  /* build s-neurons and its after_weight */
            k++;               /*   after the g-neuron */
            cotab[k].lh=lt;
            laynods[lt]++;
            cotab[k].head=laynods[lt];
            cotab[k].lt=tlt;
            cotab[k].tail=ttail;
            cotab[k].cm=cotab[s->ct_id].cm;
            cotab[k].cn=cotab[s->ct_id].cn;
            cotab[k].ca=cotab[s->ct_id].ca;
            cotab[k].cb=cotab[s->ct_id].cb;
            pt=ct_head[cotab[p->ct_id].lh][cotab[p->ct_id].head];
            while(pt->next!=NULL)
                 {
                 k++;
                 cotab[k].lh=cotab[pt->ct_id].lh;
                 cotab[k].head=cotab[pt->ct_id].head;
                 cotab[k].lt=lt;
                 cotab[k].tail=laynods[lt];
                 cotab[k].cm=cotab[pt->ct_id].cm;
                 cotab[k].cn=cotab[pt->ct_id].cn;
                 cotab[k].ca=cotab[pt->ct_id].ca;
                 cotab[k].cb=cotab[pt->ct_id].cb;
                 pt=pt->next;
                 }
                                  /* process other connection between other */
            r=ct_head[lt][tail];  /* g-neuron an this s-neuron */
            while(r->next!=NULL)
                 {
                 if(r->ct_id != q->ct_id)
                   {
                   k++;
                   cotab[k].lh=cotab[r->ct_id].lh;
                   cotab[k].head=cotab[r->ct_id].head;
                   cotab[k].lt=lt;
                   cotab[k].tail=laynods[lt];
                   cotab[k].cm=cotab[r->ct_id].cm;
                   cotab[k].cn=cotab[r->ct_id].cn;
                   cotab[k].ca=cotab[r->ct_id].ca;
                   cotab[k].cb=cotab[r->ct_id].cb;
                   }
                 r=r->next;   
                }
            p=p->next;   
            }
       q=q->next;
       }
            
       no_link=k;
       
       p=ct_head_temp[ind];   /* delete the links before G neuron */
       while(p->next!=NULL)
            {
            lh=cotab[p->ct_id].lh;
            head=cotab[p->ct_id].head;
            pt=ct_head[lh][head];
            while(pt->next!=NULL)
                 {
                 cotab[pt->ct_id].lh=0;
                 pt=pt->next;
                 }
            cotab[p->ct_id].lh=0;     
            p=p->next;
            }
       
       q=ct_tail_temp[ind];  /* delete the links around the s neuron */
       while(q->next!=NULL)   /*   after G neuron      */
            {
            lt=cotab[q->ct_id].lt;
            tail=cotab[q->ct_id].tail;
            r=ct_head[lt][tail];
            while(r->next!=NULL)
                 {
                 cotab[r->ct_id].lh=0;
                 r=r->next;
                 }
            s=ct_tail[lt][tail];
            cotab[s->ct_id].lh=0;     
            q=q->next;
            }
            
       ct_head_temp[ind]->next=NULL; /* disable the partial G neuron */
       ct_tail_temp[ind]->next=NULL; 
               
       printf("the  %d th  term combine terminated \n",ind);  
  
  }
return(delta_p);
}

/*
trans_unseparate(addrx,addry)
int addx,addy;
{
int i; 
int term_num =5;
ct_node *p,*q,*r;



ct_head[addx][addy]->next=NULL;
ct_head[addx][addy]->ct_id=0;

for(i=1;i<=term_num;i++)
   {


   r=ct_head_temp[i];
   while(r->next!=NULL)
       {   
       p=ct_head[addx][addy];
       q=p;
       while(p->next!=NULL)
           {
           q=p;
           p=p->next;
           }
       q->next=ct_head_temp[i];  
       r=r->next;
       }
       
   r=ct_tail_temp[i];
   while(r->next!=NULL)
       {   
       p=ct_tail[addx][addy];
       q=p;
       while(p->next!=NULL)
           {
           q=p;
           p=p->next;
           }
       q->next=ct_tail_temp[i];  
       r=r->next;
       }     
   }
   
}

*/

/*       ************* this transitive() include the separate function ******
transitive()
{
  int i,j,k,m,sepa_able;
  int term_num=5,flag=0,dif_term;
  float delt_p;
  
  while(flag==0)
  {
  i=laynum-2;
  while((i>=3)&&(flag==0)) 
    {
    j=1;
    m=laynods[i];
    while((j<=m)&&(flag==0))
      {
         dif_term=0;
         dif_term=trans_separate(i,j);
         if(dif_term>0)
           {
            for(k=1;k<=term_num;k++)
                    {   
                     delt_p=trans_check_comb(k);
                     if(delt_p<0)
                       flag=1;
                     printf(" node %d %d - %d ==> %f \n",i,j,k,delt_p);
                    } 
            trans_rebuild();             
            printf("Rule combination around G-neiron %d  %d is processed\n",i,j);
            }
           else
             printf("G-neuron %d %d can not be separated\n",i,j); 
         j++;
      }
    i=i-2;  
    }
    if(flag==0)
           flag=1;
       else
           flag=0;      
  }
} 
*/

transitive()                 /* this trabsitive() is performed without */
                             /*    separate the G neuron               */
{
  int i,j,k,m,sepa_able;
  int term_num=5,flag=0,dif_term;
  float delt_p;
  
  while(flag==0)
  {
  i=laynum-2;
  while((i>=3)&&(flag==0)) 
    {
    j=1;
    m=laynods[i];
    while((j<=m)&&(flag==0))
      {
         dif_term=0;
         dif_term=trans_separate(i,j);
         if(dif_term==1)
           {
            for(k=1;k<=term_num;k++)
                    {   
                     delt_p=trans_check_comb(k);
                     if(delt_p<0)
                       flag=1;
                     printf(" node %d %d - %d ==> %f \n",i,j,k,delt_p);
                    } 
            trans_rebuild();             
            printf("Rule combination around G-neiron %d  %d is processed\n",i,j);
            }
           else
             {
             printf("G-neuron %d %d have not only one kind of terms\n",i,j);
             printf("    thus it cannot be combined\n");
             }
         j++;
      }
    i=i-2;  
    }
    if(flag==0)
           flag=1;
       else
           flag=0;      
  }
}
 

int realize_VG_neuron(lay_add,nod_add)
int lay_add,nod_add;
{
int i,j;
int term_num=5;
ct_node *p,*q;

for(i=1;i<=term_num;i++)
   {
   p=ct_head_temp[i];
   if(p->next!=NULL)     
        nod_add++;  
   while(p->next!=NULL)
        {
        cotab[p->ct_id].tail=nod_add;
        p=p->next;
        }
   q=ct_tail_temp[i];
   while(q->next!=NULL)
        {
        cotab[q->ct_id].head=nod_add;
        q=q->next;
        }
   }
return(nod_add);   
}

separate_g_neuron()
{
int i,j,sepa_able,laynode_index,laynode_ori;

   for(i=3;i<=laynum-2;i++)
      {
      laynode_ori=laynods[i];
      laynode_index=laynods[i];
      for(j=1;j<=laynode_ori;j++)
         {
         sepa_able=trans_separate(i,j);
         if(sepa_able)
            {
            laynode_index=realize_VG_neuron(i,laynode_index);
            printf(" G neuron  %d %d is separated\n",i,j);
            }
         else
            printf(" *** G neuron %d %d can not be separated **\n",i,j);  
         }
      laynods[i]=laynode_index;   /* 1224*/
      trans_rebuild();              
      }

}

efficiency_evoluation()
{
int i,g_num,s_num,gs_num,sg_num
    ,computational_load;


                         /* count the number of G neuron */
g_num=0;
for(i=3;i<=laynum;i=i+2)
   g_num=g_num+laynods[i];
   
                         /* count the number of S neuron */
s_num=0;
for(i=2;i<laynum;i=i+2)
    s_num=s_num+laynods[i];

                         /* count the number of GS and SG connections */
gs_num=0;
sg_num=0;
for(i=1;i<=no_link;i++) 
   {
   if(cotab[i].lh % 2)
     gs_num++;
   else
     sg_num++;       
   }
 
                         /* compute computational_load */
computational_load = 6*sg_num + 30*gs_num - 2*g_num - s_num;  
printf("computational_load = 6*sg_num + 30*gs_num - 2*g_num - s_num;   \n");
printf("The total number of G neuron is  : %d\n",g_num);
printf("The total number of S neuron is  : %d\n",s_num);
printf("The total number of GS connections is  : %d\n",gs_num);
printf("The total number of SG connections is  : %d\n",sg_num);
printf("The computational_load is :  %d\n",computational_load);

}


fztab_separate(fz_laynum,fz_node)
int fz_laynum,fz_node;
{
int i,distr,class_num;
float max_sim,sim;
int term_num=5;
ct_node *p,*q;

for(i=1;i<=term_num;i++)     /* allocate temp space to put separated neuron */
  {
  if((ct_head_ft_tmp1[i]=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
       {
        ct_head_ft_tmp1[i]->next=NULL;
        ct_head_ft_tmp1[i]->ct_id=0;
       }
  }
  
p=ct_head[fz_laynum][fz_node];  /* distribute similar head-weights together */
while(p->next!=NULL)            
     {
        distr=0;
        max_sim=0;
        sim=similarity(.0, .05, .0, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=1;
             }
        sim=similarity(.2, .3, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=2;
             }
        sim=similarity(.45, .55, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=3;
             }
        sim=similarity(.7, .8, .1, .1, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             {
             max_sim=sim;
             distr=4;
             }
        sim=similarity(.95, 1.0, .1, .0, cotab[p->ct_id].cm,
                    cotab[p->ct_id].cn,cotab[p->ct_id].ca,cotab[p->ct_id].cb);
        if(sim > max_sim)
             { 
             max_sim=sim;
             distr=5;
             } 
             
        if((q=(ct_node *)malloc(sizeof(ct_node)))!=NULL)
          {
           q->ct_id=p->ct_id;
           q->next=ct_head_ft_tmp1[distr];     
           ct_head_ft_tmp1[distr]=q;     
          } else printf("error locating memory\n");  
         
        p=p->next; 
     }
class_num=0;     
for(i=1;i<=term_num;i++)
   {
   p=ct_head_ft_tmp1[i];
   if(p->next!=NULL)
     class_num++;
   }
printf("\n");
printf("Node %d %d can be separated to %d part(s)\n",fz_laynum,fz_node,class_num);   
     
}


int initialize_ft_list(term_idx) 
int term_idx;
{
int i,j;
int term_count;
ct_node *p,*q;
ssg_node *r,*s;
inc_node *x,*y;

printf("  Initialize ft list of term %d\n",term_idx);

p=ct_head_ft_tmp1[term_idx];   /* build the initial list of the */
                               /*     same conclusion rules     */
if((ft_det_list[0]=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
   {
   ft_det_list[0]->next=NULL;           /* initialize on ft_det_list[0] */   
   ft_det_list[0]->inc_list=NULL;
   } else printf("error locating memory\n");

if((ft_sel_list=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
   {
   ft_sel_list->next=NULL;              /* initialize on ft_sel_list */   
   ft_sel_list->inc_list=NULL;
   } else printf("error locating memory\n");
   
while(p->next!=NULL)                     /* add a ssg_node*/            
     {
      if((s=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
        {
                                         /* initialize inc_list */ 
        if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
          {
          x->next=NULL;
          s->inc_list=x;
          }
        else printf("error locating memory of inc_node \n");
        /**********************************/
        /* for(i=1;i<max_minterms;i++)*/ /*initialize included[i] of ssg_node*/      
        /*    s->include[i]=0;            */ 
        /**********************************/
           
        s->included_flag=0;            /* initialize included_flag  */
              
        for(j=1;j<maxlay;j++)          /* initialize pre_condition         */
           {                                  
           for(i=1;i<maxlay_nod;i++)         
              {                            
              s->pre_condition[j][i][0]=0;          
              s->pre_condition[j][i][1]=1;          
              s->pre_condition[j][i][2]=0;          
              s->pre_condition[j][i][3]=0;          
              }                            
           }                               

        s->next=ft_det_list[0];           /* initialize next    */  
        ft_det_list[0]=s; 
                                          /* record the conclusion data */
                                          /*  to the ssg_node  */
        s->pre_condition[0][0][0]=cotab[p->ct_id].cm;
        s->pre_condition[0][0][1]=cotab[p->ct_id].cn;        
        s->pre_condition[0][0][2]=cotab[p->ct_id].ca;
        s->pre_condition[0][0][3]=cotab[p->ct_id].cb;
                                                          
                                          /* record the pre_condition */
                                          /*   data of the ssg_node   */
        q=ct_head[cotab[p->ct_id].lh][cotab[p->ct_id].head];                  
        while(q->next!=NULL)
             {
             s->pre_condition[cotab[q->ct_id].lh][cotab[q->ct_id].head][0]
                      =cotab[q->ct_id].cm;
             s->pre_condition[cotab[q->ct_id].lh][cotab[q->ct_id].head][1]
                      =cotab[q->ct_id].cn;
             s->pre_condition[cotab[q->ct_id].lh][cotab[q->ct_id].head][2]
                      =cotab[q->ct_id].ca;
             s->pre_condition[cotab[q->ct_id].lh][cotab[q->ct_id].head][3]
                      =cotab[q->ct_id].cb;
                      
             q=q->next;
             }
        } 
      else printf("error locating memory\n");  
      
      p=p->next;
     }

remove_same_minterms();      /* remove the same terms */     

term_count=0; 
r=ft_det_list[0];
while(r->next!=NULL)    /* count the number of the terms in ft_det_list[0] */
     {                  /*  and give a number to each of the terms         */
     term_count++;
     if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
          {
          x->inc_num=term_count;
          x->next=r->inc_list;
          r->inc_list=x;
          }
     else 
        printf("error locating memory of inc_node \n");
     r=r->next;
     }    
     
     
return(term_count);     
}

int is_not_minterm(ind_p)
ssg_node *ind_p;
{
int unminterm_flag;
int i,j;

unminterm_flag=0;
for(j=1;j<maxlay;j++)
   {
   for(i=1;i<maxlay_nod;i++)
      {
      if((variable_mark[j][i]==1)&&
         ((ind_p->pre_condition[j][i][0]==0)&&(ind_p->pre_condition[j][i][1]==1)))
      unminterm_flag=1;   
      }
   }
return(unminterm_flag);
}


expand_loop(ind_p)
ssg_node *ind_p;
{
ssg_node *s, *t, *u, *p, *q;
inc_node *x;
int i,j,k,all_minterm;
int unreach_lay,unreach_nod;
int term_num=5, expand_count;

printf("do expand_loop()\n");

                                        /* build expanding list */
if((expand_list=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
   {
   expand_list->next=NULL;           /* initialize on expanding_list */   
   expand_list->inc_list=NULL;
   } else printf("error locating memory\n");
   
                                        /* put the nonminterm to */
                                        /*    expand_list     */

if((s=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
        {
                                         /* initialize inc_list */ 
        if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
          {
          x->next=NULL;
          s->inc_list=x;
          }
        else printf("error locating memory of inc_node \n");
        
        /******************************/
        /*for(i=1;i<max_minterms;i++) */ /*initialize included[i] of ssg_node*/      
        /*   s->include[i]=0;         */
        /******************************/ 
           
        s->included_flag=0;            /* initialize included_flag         */      
        for(j=1;j<maxlay;j++)          /* copy pre_condition               */
           {
           for(i=1;i<maxlay_nod;i++)         
              { 
              s->pre_condition[j][i][0]=ind_p->pre_condition[j][i][0];          
              s->pre_condition[j][i][1]=ind_p->pre_condition[j][i][1];          
              s->pre_condition[j][i][2]=ind_p->pre_condition[j][i][2];          
              s->pre_condition[j][i][3]=ind_p->pre_condition[j][i][3];          
              }
           }
        s->next=expand_list;           /* initialize next    */  
        expand_list=s; 
                                          /* copy the conclusion data */
        s->pre_condition[0][0][0]=ind_p->pre_condition[0][0][0];
        s->pre_condition[0][0][1]=ind_p->pre_condition[0][0][1];        
        s->pre_condition[0][0][2]=ind_p->pre_condition[0][0][2];
        s->pre_condition[0][0][3]=ind_p->pre_condition[0][0][3];
                                                          
        } else printf("error locating memory\n");  

                /********* do loop to expanding *********/
printf(" /********* do loop to expanding *********/ \n");

all_minterm=0;
while(!all_minterm)
     {
     t=expand_list;
     while((!is_not_minterm(t))&&(t->next!=NULL))
          t=t->next;   
     if(t->next!=NULL)
       {
       /*find the one unreached variable*/
       for(j=1;j<maxlay;j++)
          {
          for(i=1;i<maxlay_nod;i++)
             {
             if((variable_mark[j][i]==1)&&
             ((t->pre_condition[j][i][0]==0)&&(t->pre_condition[j][i][1]==1)))
                {
                unreach_lay=j;
                unreach_nod=i;
                }
             }
          }
       for(k=1;k<=term_num;k++)
          {
          if((s=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
            {
                                             /* initialize inc_list */ 
            if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
              {
              x->next=NULL;
              s->inc_list=x;
              }
            else printf("error locating memory of inc_node \n");
        
            /*****************************/
            /*for(i=1;i<max_minterms;i++)*/ /*initialize included[i] of ssg_node*/      
            /*   s->include[i]=0;        */
            /*****************************/
            s->included_flag=0;            /* initialize included_flag         */      
            for(j=1;j<maxlay;j++)          /* copy pre_condition               */
               {
               for(i=1;i<maxlay_nod;i++)         
                  { 
                  s->pre_condition[j][i][0]=t->pre_condition[j][i][0];          
                  s->pre_condition[j][i][1]=t->pre_condition[j][i][1];          
                  s->pre_condition[j][i][2]=t->pre_condition[j][i][2];          
                  s->pre_condition[j][i][3]=t->pre_condition[j][i][3];          
                  }
               }
                                             /* add the unreach one */
            s->pre_condition[unreach_lay][unreach_nod][0]
                    =def_term[k][0];                         
            s->pre_condition[unreach_lay][unreach_nod][1]
                    =def_term[k][1];                                 
            s->pre_condition[unreach_lay][unreach_nod][2]
                    =def_term[k][2];                         
            s->pre_condition[unreach_lay][unreach_nod][3]
                    =def_term[k][3];                         
            
            s->next=expand_list;           /* initialize next    */  
            expand_list=s; 
                                          /* copy the conclusion data */
            s->pre_condition[0][0][0]=t->pre_condition[0][0][0];
            s->pre_condition[0][0][1]=t->pre_condition[0][0][1];        
            s->pre_condition[0][0][2]=t->pre_condition[0][0][2];
            s->pre_condition[0][0][3]=t->pre_condition[0][0][3];
                                                          
            } else printf("error locating memory---minterms\n");  
          
          }   
          
       /*** delete the t node in exapnding_list ***/
       /*printf(" delete the t node in exapnding_list  \n"); */
       
       u=expand_list;
       if(u==t)
         {
         expand_list=t->next;
         t->next=NULL;
         freelist(t);
         }
       else
         {
         while(u->next!=t)
             u=u->next;
         u->next=t->next;
         t->next=NULL;       
         freelist(t);  
         }  
       }
     else
       all_minterm=1;  
     }

/* appand to ft_det_list[0]  and  delete original term */      

expand_count=0;

/******************************
p=expand_list;
while(p->next!=NULL)  
     {
     q=p;
     p=p->next;
     q->next=ft_det_list[0];
     ft_det_list[0]=p;
     expand_count++;
     }
*******************************/
                                
while(expand_list->next!=NULL)  /* appand to ft_det_list[0] */
     {
     p=expand_list;
     expand_list=p->next;
     p->next=ft_det_list[0];
     ft_det_list[0]=p;
     expand_count++;
     }
freelist(expand_list);     
     
printf("This term is expanded to %d minterms\n",expand_count);     
                      
                      /* delete original term */   
q=ft_det_list[0];
if(q==ind_p)
  {
  ft_det_list[0]=q->next;
  q->next=NULL;
  freelist(q);
  }
else
  {
  while(q->next!=ind_p)
       q=q->next;
  q->next=ind_p->next;
  ind_p->next=NULL;       
  freelist(ind_p);
  }  
/**************************                      
q=ft_det_list[0];
while(q->next!=ind_p)
     q=q->next;
q->next=ind_p->next;       
****************************/                            

}

remove_same_minterms()
{
ssg_node *p, *q, *r;
int same_flag, delete_count;
int i,j;
float sim;

printf("          remove the duplicating minterms \n");
p=ft_det_list[0];

                               /* mark the duplicating terms */
while(p->next!=NULL)
     {
     q=p->next;
     while(q->next!=NULL)
          {
          same_flag=1;
          /*for(j=1;j<maxlay;j++)*/      
          j=1;
          while((j<maxlay)&&(same_flag==1))
             {
             /*for(i=1;i<maxlay_nod;i++)   */
             i=1;
             while((i<maxlay_nod)&&(same_flag==1))
                {
                sim=similarity(p->pre_condition[j][i][0],
                               p->pre_condition[j][i][1],
                               p->pre_condition[j][i][2],
                               p->pre_condition[j][i][3],
                        q->pre_condition[j][i][0],
                        q->pre_condition[j][i][1],
                        q->pre_condition[j][i][2],
                        q->pre_condition[j][i][3] );
                if(sim<sim_threshold)
                same_flag=0;
                i++;
                }
             j++;
             }   
          if(same_flag==1)
             q->included_flag=1;     
          q=q->next;       
          }
     p=p->next;
     }
delete_count=0;
printf("          delete the marked ones \n");          /* delete the marked ones */
p=ft_det_list[0]->next;
q=ft_det_list[0];
while(p->next!=NULL)
     {
     if(p->included_flag==1)
       {
       q->next=p->next;
       r=p;
       p=p->next;
       r->next=NULL;
       freelist(r);
       delete_count++;
       }
     else
       {  
       p=p->next;
       q=q->next;  
       }    
     }
printf("   Total %d same minterms are removed \n",delete_count);     

}

int minterms_expanding()
{
int i,j,term_count;
ssg_node *p,*q;
inc_node *x;

printf("do minterms_expanding()\n");
                            /********* find the total used variabes *********/
for(j=1;j<maxlay;j++)
   for(i=1;i<maxnod;i++)
      variable_mark[j][i]=0;      /* initialize variable_mark */
      
p=ft_det_list[0];
while(p->next!=NULL)
     {
     for(j=1;j<maxlay;j++)
        {
        for(i=1;i<maxlay_nod;i++)
           {
           if(!((p->pre_condition[j][i][0]==0)&&(p->pre_condition[j][i][1]==1)))
             {
             variable_mark[j][i]=1;
             }
           }
        }
     p=p->next;   
     }
     
                         /******** find the terms which are not minterms *****/
                         /********  and expand them                      *****/
p=ft_det_list[0];
while(p->next!=NULL)
     {
     q=p;
     p=p->next;
     if(is_not_minterm(q))
       expand_loop(q);
     }
     
printf("     remove the same minterms after expanding\n");     

remove_same_minterms();  /* remove the same minterms after expanding */ 
                         /*  at ft_det_list[0]                       */
                              
                         /* give a number to each terms and     */
term_count=0;            /* return the total number of minterms */
p=ft_det_list[0];
while(p->next!=NULL)
     {
     term_count++;
     free_inc_list(p->inc_list);     /* reset the inc_list of p */
                                      /* initialize inc_list */ 
     if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
        {
        x->next=NULL;
        p->inc_list=x;
        }
     else printf("error locating memory of inc_node \n");
     /************************************/
     /*for(i=1;i<max_minterms;i++)*/     /*initialize included[i] of ssg_node*/      
     /*      p->include[i]=0;     */
     /************************************/
     
     if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
        {
        x->next=p->inc_list;
        x->inc_num=term_count;
        p->inc_list=x;
        }
     else printf("error locating memory of inc_node \n");
     /****************************/
     /* p->include[term_count]=1;*/       /* assign a number to each ssg_node */   
     /****************************/
     p->included_flag=0;
     p=p->next;
     }
return(term_count);     
}

free_inc_list(inc_pt)
inc_node *inc_pt;
{
inc_node *x;
while(inc_pt!=NULL)
     {
     x=inc_pt;
     inc_pt=inc_pt->next;
     free(x);
     }
}


int premise_comp_comb(index_p,index_q,list_num)
ssg_node *index_p,*index_q;
int list_num;
{
int i,j,diff_count,diff_last_node,diff_last_layer,equal_flag;
float sim,term_distance;
float min_term_distance=.15;
ssg_node *s;
inc_node *x,*y,*z,*w;

/* compare these two noeds of index_p and index_q                     */
/* if active put the combined node to another ft_pre_list[ list_num ] */

/*printf("   compare and combine two fuzzy terms to list-%d\n",list_num);
*/
diff_count=0;
diff_last_node=0;
diff_last_layer=0;
/*for(j=1;j<maxlay;j++)*/      /* check if only one variable is different */ 
j=1;
while((j<maxlay)&&(diff_count<2))  
  {
  /*for(i=1;i<maxlay_nod;i++)   */
  i=1;
  while((i<maxlay_nod)&&(diff_count<2))  
     {
     sim=similarity(index_p->pre_condition[j][i][0],
                    index_p->pre_condition[j][i][1],
                    index_p->pre_condition[j][i][2],
                    index_p->pre_condition[j][i][3],
                index_q->pre_condition[j][i][0],
                index_q->pre_condition[j][i][1],
                index_q->pre_condition[j][i][2],
                index_q->pre_condition[j][i][3] );
     if(sim<sim_threshold)
       {
       /*printf("less sim %f %f %f %f, %f %f %f\n",
                  index_p->pre_condition[i][0],index_p->pre_condition[i][1],
                  index_p->pre_condition[i][2],index_p->pre_condition[i][3],
                index_q->pre_condition[i][0],index_q->pre_condition[i][1],
                index_q->pre_condition[i][2],index_q->pre_condition[i][3]);
       */
       diff_count++;
       diff_last_layer=j;
       diff_last_node=i;
       }
     i++;
     }                       
  j++;
  }                           /* if these two terms are near, */
                             /*      do combining            */   
term_distance=
   getmax(index_p->pre_condition[diff_last_layer][diff_last_node][0]
         ,index_q->pre_condition[diff_last_layer][diff_last_node][0])
  -getmin(index_p->pre_condition[diff_last_layer][diff_last_node][1]
         ,index_q->pre_condition[diff_last_layer][diff_last_node][1]);
         
 /* printf("    diff_count = %d\n",diff_count);         
    printf("    term_distance = %f\n",term_distance);         */

if((diff_count==1)&&(term_distance<=min_term_distance))          
  {
  index_p->included_flag=1;        /* set the included_flag of p and q */
  index_q->included_flag=1;                        
  
  if((s=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
        {                                  
                                         /* initialize inc_list */ 
        if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
          {
          x->next=NULL;
          s->inc_list=x;
          }
          else printf("error locating memory of inc_node \n");
          
        y=index_p->inc_list;
        z=index_q->inc_list;
        while(y->next!=NULL)    /*copy the inc_list of index_p to s */
             {
             if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
                {
                x->next=s->inc_list;
                x->inc_num=y->inc_num;
                s->inc_list=x;
                }
             else printf("error locating memory of inc_node \n");
             y=y->next;
             }
             
        while(z->next!=NULL)    /*copy the inc_list of index_q to s */
             {
             equal_flag=0;
             w=s->inc_list;
             while(w->next!=NULL) /*equal test*/
                  {
                  if(w->inc_num==z->inc_num)
                     equal_flag=1;
                  w=w->next;
                  }
             if(equal_flag!=1)
               {     
               if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
                  {
                  x->next=s->inc_list;
                  x->inc_num=z->inc_num;
                  s->inc_list=x;
                  }
               else printf("error locating memory of inc_node \n");
               }
             z=z->next;  
             }     
                                           /* set included[i] of ssg_node */
        /**********************************
        for(i=1;i<max_minterms;i++)            
           {
           if((index_p->include[i]==1)||(index_q->include[i]==1))
             s->include[i]=1;
           else   
             s->include[i]=0;
           }
        ************************************/
           
        s->included_flag=0;              /* set included_flag         */ 
        for(j=1;j<maxlay;j++)            /* set pre_condition         */
          {
          for(i=1;i<maxlay_nod;i++)            /* set pre_condition         */
             { 
             if((j!=diff_last_layer)||(i!=diff_last_node))
               {
               s->pre_condition[j][i][0]=
                        .5*(index_p->pre_condition[j][i][0]+index_q->pre_condition[j][i][0]);          
               s->pre_condition[j][i][1]=
                        .5*(index_p->pre_condition[j][i][1]+index_q->pre_condition[j][i][1]);          
               s->pre_condition[j][i][2]=
                        .5*(index_p->pre_condition[j][i][2]+index_q->pre_condition[j][i][2]);          
               s->pre_condition[j][i][3]=
                        .5*(index_p->pre_condition[j][i][3]+index_q->pre_condition[j][i][3]);          
               }
             else
               {
               s->pre_condition[j][i][0]=
                        getmin(index_p->pre_condition[j][i][0],index_q->pre_condition[j][i][0]);          
               s->pre_condition[j][i][1]=
                        getmax(index_p->pre_condition[j][i][1],index_q->pre_condition[j][i][1]);
                        
             /* the LEFT spread 'a' should be  ...  */
             /*                min(m1,m2)-min(m1-a1,m2-a2)               */
                                  
               s->pre_condition[j][i][2]=
                 getmin(index_p->pre_condition[j][i][0],
                        index_q->pre_condition[j][i][0])
                 -getmin(index_p->pre_condition[j][i][0]-index_p->pre_condition[j][i][2]
                        ,index_q->pre_condition[j][i][0]-index_q->pre_condition[j][i][2]);
             
             /* the RIGHT spread 'b' should be  ...  */
             /*                max(n1+b1,n2+b2)-max(n1,n2)               */                        
               s->pre_condition[j][i][3]=
                        getmax(index_p->pre_condition[j][i][1]+index_p->pre_condition[j][i][3],
                        index_q->pre_condition[j][i][1]+index_q->pre_condition[j][i][3])
                        -getmax(index_p->pre_condition[j][i][1],index_q->pre_condition[j][i][1]);   
               }
             }
          } 
        s->next=ft_det_list[list_num];           /* set ->next   */  
        ft_det_list[list_num]=s;                 
        } else printf("error locating memory\n");  
  return(1);
  }   
  else
  return(0);
}


freelist(p)
ssg_node *p;
{
ssg_node *s;
inc_node *x,*y;

while(p!=NULL)
     {
     s=p;
     p=p->next;
     x=s->inc_list;
     while(x!=NULL)
          {
          y=x;
          x=x->next;
          free(y);
          }
     free(s);
     }
}

copy_to_sel_list(list_numb)
int list_numb;
{
int i,j;
ssg_node *p,*q;
inc_node *x,*y;

p=ft_det_list[list_numb];
while(p->next!=NULL)
     {
     if(p->included_flag==0)
       {                                 /* do copying */
       if((q=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
          {
        
          if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
            {
            x->next=NULL;
            q->inc_list=x;
            }
          else printf("error locating memory of inc_node \n");
     
          y=p->inc_list;
          while(y->next!=NULL)
               {     
               if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
                 {
                 x->next=q->inc_list;
                 x->inc_num=y->inc_num;
                 q->inc_list=x;
                 }
               else printf("error locating memory of inc_node \n");
               y=y->next;
               }
          /*******************************/
          /* for(i=1;i<max_minterms;i++) */     /* copy included[i] from p */      
          /*   q->include[i]=p->include[i]; */
          /**********************************/
        
        
          q->included_flag=0;              /* copy included_flag from p */
          for(j=1;j<maxlay;j++)            /* copy pre_condition from p */
             {   
             for(i=1;i<maxlay_nod;i++)           
                { 
                q->pre_condition[j][i][0]=p->pre_condition[j][i][0];          
                q->pre_condition[j][i][1]=p->pre_condition[j][i][1];          
                q->pre_condition[j][i][2]=p->pre_condition[j][i][2];          
                q->pre_condition[j][i][3]=p->pre_condition[j][i][3];          
                }
             }
          q->next=ft_sel_list;             /* initialize next   */  
          ft_sel_list=q;                 
          } else printf("error locating memory\n");  
       }
     p=p->next;
     }
}

delete_duplicated_small_terms(det_list_ind)
int det_list_ind;
{
int i,same_flag,delete_count,get_same_one;
ssg_node *p, *q, *r;
inc_node *x,*y;

printf(" mark the duplicated and smaller terms \n");
p=ft_det_list[det_list_ind];            /* mark the duplicated terms */
while(p->next!=NULL)
     {
     if(p->included_flag==0)
       {
       /* q=p->next; */
       q=ft_det_list[det_list_ind];
       while(q->next!=NULL)
            {                          /* compare p with q */ 
            if((q->included_flag==0)&&(q!=p))
              {
              same_flag=1;               
            
              /**********************************
              x=p->inc_list;
              while(x->next!=NULL)
                 {
                 get_same_one=0;
                 y=q->inc_list;
                 while(y->next!=NULL)
                      {
                      if(x->inc_num==y->inc_num)
                      get_same_one=1;
                      y=y->next;
                      }
                 if(get_same_one==0)
                   same_flag=0;      
                 x=x->next;
                 }
              **************************************/   
                                         /* compare p with q */

              x=q->inc_list;            
              while(x->next!=NULL)
                 {
                 get_same_one=0;
                 y=p->inc_list;
                 while(y->next!=NULL)
                      {
                      if(x->inc_num==y->inc_num)
                        get_same_one=1;
                      y=y->next;
                      }
                 if(get_same_one==0)
                   same_flag=0;      
                 x=x->next;
                 }
              /************************************
              for(i=1;i<max_minterms;i++)
                 if(p->include[i]!=q->include[i])
                   same_flag=0;
              *************************************/     
          
              if(same_flag==1)
                 q->included_flag=1;     
                     
              }
            q=q->next;
            }
       }
     p=p->next;
     }
delete_count=0;
printf(" delete the marked ones \n");          /* delete the marked ones */
p=ft_det_list[det_list_ind]->next;
q=ft_det_list[det_list_ind];
while(p->next!=NULL)
     {
     if(p->included_flag==1)
       {
       q->next=p->next;
       r=p;
       p=p->next;
       r->next=NULL;
       freelist(r);
       delete_count++;
       }
     else
       {  
       p=p->next;
       q=q->next;  
       }    
     }
printf("   Total %d same minterms are deleted in this column \n",delete_count);     

}


det_prime_implicant(ori_rule_numb)
int ori_rule_numb;
{
int i, j, list_ind, combine_flag, combine_count, minterm_count, inc_count;
int one_cover_all, some_cover_all;            
ssg_node *p,*q,*r;
inc_node *x;

printf("Step 1. Determine the prime implicant \n");

combine_flag=1;
list_ind=0;
while(combine_flag==1)
     {   
                     
     delete_duplicated_small_terms(list_ind);
     
     p=ft_det_list[list_ind];   /*count how many minterms in this column */
     minterm_count=0;
     while(p->next!=NULL)
          {
          minterm_count++;
          p=p->next;            
          }      
     printf("There are %d minterms in this column\n",minterm_count);
     
     p=ft_det_list[list_ind];
    
                /******* initial another ft_det_list *******/
     
     if((ft_det_list[ (list_ind+1)%2 ]=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
       {
       ft_det_list[ (list_ind+1)%2 ]->next=NULL;
       ft_det_list[ (list_ind+1)%2 ]->inc_list=NULL;     
       } else printf("error locating memory\n");

     combine_flag=0;  
     combine_count=0;
     
     while(p->next!=NULL)         /* compare any two premises in a list    */
          {                       /*  and put the combined term to another */    
          q=p->next;              /*    list   */ 
          while(q->next!=NULL)
               {
               if(premise_comp_comb(p,q,(list_ind+1)%2))
                 {
                 combine_flag=1;
                 combine_count++;
                 }
               q=q->next;
               }
          p=p->next;
          }
     
     /* check if some node of ft_det_list cover all items */
     /* if YES --> stop */   
     
     some_cover_all=0;
     p=ft_det_list[list_ind];
     while(p->next!=NULL)
          {
          inc_count=0;
          x=p->inc_list;
          while(x->next!=NULL)
               {
               inc_count++;
               x=x->next;
               }
          if(inc_count==ori_rule_numb) 
            some_cover_all=1;     
          p=p->next;
          }
     if(some_cover_all==1)             /* if some cover all*/
       {
       combine_flag=0;                 /* 1. set to stop */     
       p=ft_det_list[list_ind];        /* 2. set included_flag of all  */
       while(p->next!=NULL)            /*    elements to ZERO */
            {
            p->included_flag=0;
            p=p->next;
            }
       delete_duplicated_small_terms(list_ind);     
       }
     
     printf("    %d combination(s) appear in this column \n",combine_count);     
     printf("    finish one column to determine prime implicant\n");     
     
     copy_to_sel_list(list_ind);
     
     freelist(ft_det_list[list_ind]);          
     
     list_ind++;  
     list_ind=list_ind%2;   
     }
}

sel_prime_implicant(ori_node_num)
int ori_node_num;
{
/* 1. find the essential prime_implicants(nodes) from ft_sel_list */
/* 2. select some of the remain nodes to cover the terms that are */
/*     not be included in the essential nodes                     */

int i,j,k,included_count,stop_flag,nodenum_to_check,complete_cover;
int check_if_cover_all,essential_cover_all;
int covered_ind[max_minterms];
ssg_node *p, *q, *r;
dig_node *s, *t, *u;
inc_node *x, *y;

printf("Step 2. Select the prime implicant\n");

                               /******* initial ft_essential_list *******/

if((ft_essential_list=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
  {
  ft_essential_list->next=NULL;
  ft_essential_list->inc_list=NULL;       
  } else printf("error locating memory\n");

printf("       1. get essential list\n");

for(i=1;i<=ori_node_num;i++)   /* find the essential nodes */
   {
   included_count=0;
   p=ft_sel_list;
   while(p->next!=NULL)
        {
        x=p->inc_list;
        while(x->next!=NULL)
             {
             if(x->inc_num==i)
               {
               included_count++;
               q=p;
               }
             x=x->next;
             }
        /***********************     
        if(p->include[i]==1)
          {
          included_count++;
          q=p;
          }
        ************************/          

        p=p->next;  
        }
   if((included_count==1)&&(q->included_flag==0)) /* copy the essential node */
     {                                            /*  to ft_essential_list   */
     if((r=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
       {
       printf("add one essential node \n");
       
       
       if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
          {
          x->next=NULL;
          r->inc_list=x;
          }
       else printf("error locating memory of inc_node \n");
     
       y=q->inc_list;
       while(y->next!=NULL)
             {     
             if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
               {
               x->next=r->inc_list;
               x->inc_num=y->inc_num;
               r->inc_list=x;
               }
             else printf("error locating memory of inc_node \n");
             y=y->next;
             }
        
       /*********************************/
       /*for(j=1;j<max_minterms;j++)*/      /* copy included[j] from q */      
       /*   r->include[j]=q->include[j];*/
       /*********************************/
       
       r->included_flag=0;              
       q->included_flag=1;              /* mark the essential node   */  
       for(k=1;k<maxlay;k++)            /* copy pre_condition from q */
          {  
          for(j=1;j<maxlay_nod;j++)          
             { 
             r->pre_condition[k][j][0]=q->pre_condition[k][j][0];          
             r->pre_condition[k][j][1]=q->pre_condition[k][j][1];          
             r->pre_condition[k][j][2]=q->pre_condition[k][j][2];          
             r->pre_condition[k][j][3]=q->pre_condition[k][j][3];          
             }
          }   
       r->next=ft_essential_list;             /* initialize next   */  
       ft_essential_list=r;                 
       } else printf("error locating memory\n");  
     }   
   }
   
                      /* copy the remain node to the ft_trouble_list */
                      /* initialize the ft_trouble_list              */
printf("       2. get trouble list\n");

if((ft_trouble_list=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
  {
  ft_trouble_list->next=NULL;     
  ft_trouble_list->inc_list=NULL;     
  } else printf("error locating memory\n");

p=ft_sel_list;
while(p->next!=NULL)
     {
     if(p->included_flag==0)
       {
       if((r=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
         {
         
         printf("Add a node to trouble list\n");
         
         if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
          {
          x->next=NULL;
          r->inc_list=x;
          }
          else printf("error locating memory of inc_node \n");
     
          y=p->inc_list;
          while(y->next!=NULL)
             {     
             if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
               {
               x->next=r->inc_list;
               x->inc_num=y->inc_num;
               r->inc_list=x;
               }
             else printf("error locating memory of inc_node \n");
             y=y->next;
             }
       
         
         /**************************************/
         /*for(j=1;j<max_minterms;j++)         */ /* copy included[j] from r */      
         /*   r->include[j]=p->include[j];     */
         /**************************************/
         
         r->included_flag=0;              /* copy included_flag from r */
         for(k=1;k<maxlay;k++)            /* copy pre_condition from r */
            {
            for(i=1;i<maxlay_nod;i++)       
               { 
               r->pre_condition[k][i][0]=p->pre_condition[k][i][0];          
               r->pre_condition[k][i][1]=p->pre_condition[k][i][1];          
               r->pre_condition[k][i][2]=p->pre_condition[k][i][2];          
               r->pre_condition[k][i][3]=p->pre_condition[k][i][3];          
               }
            }   
         r->next=ft_trouble_list;             /* initialize next   */  
         ft_trouble_list=r;                 
         } else printf("error locating memory\n");
       }
     p=p->next;
     }  

/*
p=ft_trouble_list;
if(p->next!=NULL)
  {
*/
  
  /*********** begin to deal with the ft_troube_list  ***********/                     
  printf("       3. begin to deal with the trouble list\n");

for(i=1;i<=ori_node_num;i++)           /* find the uncovered ones */
   covered_ind[i]=1;

p=ft_essential_list;
while(p->next!=NULL)   
     {
     x=p->inc_list;
     while(x->next!=NULL)
          {
          covered_ind[x->inc_num]=0;
          x=x->next;
          }
     /*******************************
     for(j=1;j<=ori_node_num;j++)
        {
        if(p->include[j]==1)
          covered_ind[j]=0;
        }
     *********************************/   
     p=p->next;
     }
     
essential_cover_all=1;                   /* check if essential nodes cover */
printf("    Before dealing with the trouble list\n");
for(i=1;i<=ori_node_num;i++)
   {
   /*printf("  covered_ind[%d]==%d\n",i,covered_ind[i]);
   */
   if(covered_ind[i]==1)
     essential_cover_all=0;
   }
if(essential_cover_all==0)
  {     
     
              /* find the least nodes of ft_troube_list */
              /*         to cover all                   */

  printf("       4. initialize dig_list\n");

  nodenum_to_check=0;
  complete_cover=0;
  while(complete_cover!=1)        
       {
       nodenum_to_check++;
                           /* initialize dig_list (a two-direct list)*/

       if((dig_list=(dig_node *)malloc(sizeof(dig_node)))!=NULL)
         {
         dig_list->next=NULL;           /* build its tail node */          
         s=dig_list;     
         s->stl_ind=NULL;
         } else printf("error locating memory\n");

       if((dig_list=(dig_node *)malloc(sizeof(dig_node)))!=NULL)
         {
         dig_list->next=s;              /* build its head node */
         dig_list->front=NULL;
         s->front=dig_list;
         dig_list->stl_ind=NULL;
         } else printf("error locating memory\n");
      
                                    /* insert nodenum_to_check dig_node */     
                                    /*  into dig_list                   */
       p=ft_trouble_list;
       for(i=1;i<=nodenum_to_check;i++)
          {
          if((s=(dig_node *)malloc(sizeof(dig_node)))!=NULL)
            {
            s->next=dig_list->next;
            t=dig_list->next;
            t->front=s;                    
            dig_list->next=s;
            s->front=dig_list;     
            s->stl_ind=p;
            } else printf("error locating memory\n");
          p=p->next;  
          }
     /***********************************************/   
     /********* begining the find_mininum algorithm */
     /***********************************************/
     
       printf("       5. the find_mininum algorithm \n");
          
       stop_flag=0;
       s=dig_list;
       for(i=1;i<=nodenum_to_check;i++)
          s=s->next;
       while(stop_flag==0)
            {
            printf("                check-1\n");
            u=dig_list;                     /* check if  cover all */
            u=u->next;
            while(u->next!=NULL)
                 {
                 /*printf("                       check-1.1\n");
                 */
                 p=u->stl_ind;
                 
                 x=p->inc_list;
                 while(x->next!=NULL)
                      {
                      covered_ind[x->inc_num]=0;
                      x=x->next;
                      }
                 /********************************
                 for(i=1;i<=ori_node_num;i++)
                    {
                    if(p->include[i]==1)
                      covered_ind[i]=0;
                    }
                 *********************************/                  
                 u=u->next;                   
                 }
            /*printf("                check-2\n");
            */   
            check_if_cover_all=1;     
            for(i=1;i<=ori_node_num;i++)
               {
               if(covered_ind[i]==1)
                 check_if_cover_all=0;     
               }  
            printf("check_if_cover_all=%d\n",check_if_cover_all);
            if(check_if_cover_all)   
              {
                                           /*add_to_esstetial_list*/
              u=dig_list; 
              u=u->next;
              while(u->next!=NULL)
                   {
                   p=u->stl_ind;
                   if((q=(ssg_node *)malloc(sizeof(ssg_node)))!=NULL)
                     {
                     /*printf("add a essential node \n");
                     */
                     
                     if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
                        {
                        x->next=NULL;
                        q->inc_list=x;
                        }
                     else printf("error locating memory of inc_node \n");
     
                     y=p->inc_list;
                     while(y->next!=NULL)
                          {     
                          if((x=(inc_node *)malloc(sizeof(inc_node)))!=NULL)
                            {
                            x->next=q->inc_list;
                            x->inc_num=y->inc_num;
                            q->inc_list=x;
                            }
                          else printf("error locating memory of inc_node \n");
                          y=y->next;
                          }
                   
                     /*****************************/
                     /*for(j=1;j<max_minterms;j++)*/ /* copy included[j] from r */      
                     /*   q->include[j]=p->include[j];*/
                     /*****************************/
                     q->included_flag=0;      /* copy included_flag from r */
                     for(k=1;k<maxlay;k++)    /* copy pre_condition from r */
                        {
                        for(j=1;j<maxlay_nod;j++)   
                           { 
                           q->pre_condition[k][j][0]=p->pre_condition[k][j][0];          
                           q->pre_condition[k][j][1]=p->pre_condition[k][j][1];          
                           q->pre_condition[k][j][2]=p->pre_condition[k][j][2];          
                           q->pre_condition[k][j][3]=p->pre_condition[k][j][3];
                        
                           /*if(!((p->pre_condition[j][0]==0)&&
                                (p->pre_condition[j][1]==1)))
                              {
                              printf("p->pre_condition[%d][0]=%f\n",j,p->pre_condition[j][0]); 
                              printf("p->pre_condition[%d][1]=%f\n",j,p->pre_condition[j][1]); 
                              }           
                           */
                           } 
                        }   
                     q->next=ft_essential_list;     /* initialize next   */  
                     ft_essential_list=q;                 
                     }else printf("error locating memory\n");
                   u=u->next;
                   }                              

              complete_cover=1;                     /* set control flag  */
              stop_flag=1;
              }
            else
              {
              t=s->next;
              p=s->stl_ind;                   /* if s can not move */
              if(((p->next)->next==NULL)||(p->next==t->stl_ind))
                {
                if((s->front)->front!=NULL)
                   s=s->front;
                else
                   stop_flag=1;           
                }
              else                             
                {
                p=p->next;
                s->stl_ind=p;                /* move s to next one */
                while((s->next)->next!=NULL) /*  initial the follows  */
                   {
                   s=s->next;
                   p=p->next;
                   s->stl_ind=p;
                   }
                                           /* reset s */
                s=dig_list;
                for(i=1;i<=nodenum_to_check;i++)
                   s=s->next;   
                }  
              }  
            }        
       }
  }
}

ft_structure_modify(layer_ind,node_ind,term_idx)
int layer_ind,node_ind,term_idx;
{
    /* 1. delect the original releated connections */                  
    /* 2. add the new connections                  */  
int i,j;
float delta_linknum;
ct_node *p, *q;                  
ssg_node *s, *t; 

printf("Do structure rebuild\n");
                           /* delete the original releated connections */
delta_linknum=0;
p=ct_head_ft_tmp1[term_idx];
/*printf("total link number : %d\n",no_link);
*/
while(p->next!=NULL)
     {
     q=ct_head[cotab[p->ct_id].lh][cotab[p->ct_id].head];
     while(q->next!=NULL)
          {
          cotab[q->ct_id].lh=0;
          delta_linknum--;
          /*printf("delete no.%d connection\n",q->ct_id);          
          */
          q=q->next;
          }
     cotab[p->ct_id].lh=0;
     delta_linknum--;     
     printf("delete no.%d connection\n",p->ct_id);          
     p=p->next;
     }
printf("flag -- after delete the original connection\n");     

printf("            delta_linknum=%f\n",delta_linknum);                           
                           /* add new connections */         
s=ft_essential_list;
while(s->next!=NULL)
     {
     no_link++;            /* add a conclusion connection */   
     delta_linknum++;
     laynods[layer_ind-1]++;
     cotab[no_link].lh=layer_ind-1;
     cotab[no_link].head=laynods[layer_ind-1];
     cotab[no_link].lt=layer_ind;
     cotab[no_link].tail=node_ind;
     cotab[no_link].cm=def_term[term_idx][0];
     cotab[no_link].cn=def_term[term_idx][1];
     cotab[no_link].ca=def_term[term_idx][2];
     cotab[no_link].cb=def_term[term_idx][3];
     /* for(i=1;i<=laynods[layer_ind-2];i++) */
     for(j=1;j<maxlay;j++)
        {
        for(i=1;i<maxlay_nod;i++)
           {
           if(!((s->pre_condition[j][i][0]==0)&&(s->pre_condition[j][i][1]==1)))
             {
             no_link++;
             delta_linknum++;
             cotab[no_link].lh=j;
             cotab[no_link].head=i;
             cotab[no_link].lt=layer_ind-1;
             cotab[no_link].tail=laynods[layer_ind-1];
             cotab[no_link].cm=s->pre_condition[j][i][0];
             cotab[no_link].cn=s->pre_condition[j][i][1];
             cotab[no_link].ca=s->pre_condition[j][i][2];
             cotab[no_link].cb=s->pre_condition[j][i][3];
             }
           }
        }   
     s=s->next;
     }
printf("            delta_linknum=%f\n",delta_linknum); 
}

fuzzy_tabulation()
{
int i,j,rule_numb;
int term_ind,term_num=5;

for(i=3;i<=laynum;i=i+2)
   {
   for(j=1;j<=laynods[i];j++)
      {
      fztab_separate(i,j);
      for(term_ind=1;term_ind<=term_num;term_ind++)
         {
         if(ct_head_ft_tmp1[term_ind]->next!=NULL)
           {
           rule_numb=initialize_ft_list(term_ind);
           
                            /* minterms_expanding is disabled to reduce the */
                            /*  computational resource as we can make sure  */
                            /*  that minterms_expanding cannot help to      */
                            /*  decrease more computational load   6/2 N.J. */
                            
           rule_numb=minterms_expanding();     
         
           
           printf("Total %d minterms\n",rule_numb);
           
           det_prime_implicant(rule_numb);
           sel_prime_implicant(rule_numb);
           ft_structure_modify(i,j,term_ind);
           }
         }
      trans_rebuild();
      printf("after deal with G-neuron(%d,%d) total link number : %d\n",
                         i,j,no_link);
      }
   }

}


