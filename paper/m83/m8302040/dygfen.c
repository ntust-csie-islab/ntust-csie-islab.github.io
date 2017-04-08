
/* file: multinnc.c  v1.0

         file for multiclasses neural network classifier with disjunctive fuzzy information.

         input : trapezoidal fuzzy number (m1, m2, al, b )
         author: Jiang I-Feng
         tel   : (02) 951-3388
         date  : 6/29/96 



/* file: expert.c

        file for hyperplane-hyperbox hybrid learning.
        input : fuzzy interval
        author: wade wang
        tel: 6667252
        Date of last revision:  8-23-92
*/

#include <math.h>
#include <time.h>
#include "general.h"
#include "variable.h"
#include "patterns.h"
#include "command.h"
#include "cl.h"

unsigned time1,time2,time3,time4;
clock_t  tick1,tick2;

char   *str = NULL;
char   *Prompt = "cl: ";
char   *Default_step_string = "epoch";

boolean System_Defined = FALSE;

boolean lflag = 1;		/* 1 = learning,     at learning phase 
				   0 = not learning, at testing phase  */
				
boolean proto_train = 1;	/* pocket algorithm learning flag */

int     class = 0;
int     count = 0;
int     tra_no = 0;
int     mis_flag = 0;
int     proto_no = 0;
int     iteration = 0;

int     nepochs = 0;
int     epochno = 0;

int     iris = 0;
int     pal = 0;      	/* pecfuh 0;   avg 1;   lvq 2;   */
int     look = 0;
int     test_flag = 0;
int     pass = 2;
int     holap = 1;    	/* the exemplars with same class can overlap */
int     no_phase1 = 0;
int     max_run_len = 1;
float   max_size = 0.3;	/* size condition during pass2 -- gfence */
float   alpha = 0.002;	/* initial learning rate in dynpro       */
float   t_rate = 1.0;	/* the proportion of training set (%)    */



float   min_dist,dist;

float   lrate = 1.0;	/* learning rate during dynpro. decreasing when epoch ++ */ 

int     nunits = 0;
int     nhiddens = 0;           /* number of hidden units */ 
int     noutputs = 0;		/* number of output units */
int     ninputs = 0;		/* number of input units*/

float  **mean_value1_weight;	/* pointers to vectors of maximun weights */
float  **mean_value2_weight;	/* pointers to vectors of minimun weights */
float  **spread_a_weight;
float  **spread_b_weight;




float  *desired_out = NULL;
float  *activation = NULL;	/* activations for all units */

float  defuzzify_step = 0.0;
float  defuzzify_count = 0.0;
float  membershipvalue = 0.0;
float  defuzzification_result = 0.0;

float  *mean_value1 = NULL;	/* activations for all units */
float  *mean_value2 = NULL;	/* activations for all units */
float  *spread_a = NULL;	/* activations for all units */
float  *spread_b = NULL;	/* activations for all units */





float  **w_mean_value1 = NULL;	/* activations for all units */
float  **w_mean_value2 = NULL;	/* activations for all units */
float  **w_spread_a = NULL;	/* activations for all units */
float  **w_spread_b = NULL;	/* activations for all units */
float  *netinput = NULL;	/* sum of inputs for pool units */


float  **old_mean_value1_weight;   
float  **old_mean_value2_weight; 
float  **old_spread_a;
float  **old_spread_b; 


int   **p_node;
int   **old_p_node;
int   *p_node_index;
 
int    *old_use_flag=NULL;


float  **pnode_weight;


 
int    patno = 0;
int    winner = 0;
int    nonwin = 0;
int    tallflag = 0;            /* test all flag */

float  *classification_out = NULL;
float  *size = NULL;

int    *err_pattern = NULL;
int    *tra_pattern = NULL;
int    *class_flag = NULL;
int    *enode_win = NULL;
int    *use_train = NULL;
int    *use_flag = NULL;

int     no_of_misclassification = 0;
int     winner_no;
int     nhyperbox;		/* number of created hyperboxes */
int     mistake;
int     no_win;

int      train_error_number;
int      find_prototype_number =0 ;
int      max_prototype_node  ;
float    total_distant = 999 ;/* initial value of the total_distance */
float    old_distant   = 999 ; 

/****************/
define_system() {
/****************/
    int     i,j;

    if (noutputs <= 0) {
        put_error("cannot initialize weights without noutputs");
        return(FALSE);
    }

    if (ninputs <= 0) {
        put_error("cannot initialize weights without ninputs");
        return(FALSE);
    }

    nunits = ninputs + nhiddens + noutputs;
    no_of_misclassification = 0;
    nhyperbox = 0;
    
    if (activation != NULL) {
        free((char *) activation);
    }
    
    if (desired_out != NULL) {
        free((char *) desired_out);
    }

    if (netinput != NULL) {
        free((char *) netinput);
    }
    
    activation = (float *) emalloc((unsigned)(nunits * sizeof(float)));
    install_var("activation", Vfloat,(int *) activation, nunits,0,SETSVMENU);
    mean_value1 = (float *) emalloc((unsigned)(nunits * sizeof(float)));
    install_var("mean_value1", Vfloat,(int *) mean_value1, nunits,0,SETSVMENU);
    mean_value2 = (float *) emalloc((unsigned)(nunits * sizeof(float)));
    install_var("mean_value2", Vfloat,(int *) mean_value2, nunits,0,SETSVMENU);
    spread_a = (float *) emalloc((unsigned)(nunits * sizeof(float)));
    install_var("spread_a", Vfloat,(int *) spread_a, nunits,0,SETSVMENU);
    spread_b = (float *) emalloc((unsigned)(nunits * sizeof(float)));
    install_var("spread_b", Vfloat,(int *) spread_b, nunits,0,SETSVMENU);
    install_var("defuzzification_result", Float,(int *) & defuzzification_result, 0,0,NOMENU);
    netinput = (float *) emalloc((unsigned)(nunits * sizeof(float)));
    install_var("netinput", Vfloat,(int *) netinput, nunits,0,SETSVMENU);
    size = (float *) emalloc((unsigned)(nunits * sizeof(float)));
    install_var("size", Vfloat,(int *) size, nunits,0,SETSVMENU);
    use_flag = (int *) emalloc((unsigned)(nunits * sizeof(int)));
    install_var("use_flag", Vint,(int *) use_flag, nunits, 0, SETSVMENU);
    use_train = (int *) emalloc((unsigned)(noutputs * sizeof(int)));
    install_var("use_train", Vint,(int *) use_train, noutputs, 0, SETSVMENU);

    class_flag = (int *) emalloc((unsigned)(nunits * sizeof(int)));
    install_var("class_flag", Vint,(int *) class_flag, nunits, 0, SETSVMENU);

    p_node_index= (int *) emalloc((unsigned)(nunits * sizeof(int)));
    install_var("p_node_index", Vint,(int *) p_node_index, nunits, 0, SETSVMENU);

    


    desired_out = (float *) emalloc((unsigned)(noutputs * sizeof(float)));    
    install_var("desired_out", Vfloat,(int *)  desired_out, noutputs, 0, NOMENU);
    classification_out = (float *) emalloc((unsigned)(noutputs * sizeof(float)));    
    install_var("classification_out", Vfloat,(int *) classification_out, 0, 0, NOMENU);
    install_var("defuzzify_step", Float,(int *) & defuzzify_step, 0, 0, NOMENU);

    mean_value1_weight = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    mean_value2_weight = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    spread_a_weight = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    spread_b_weight = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));


   
    old_mean_value1_weight = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    old_mean_value2_weight = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    old_spread_a=((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    old_spread_b=((float **) emalloc((unsigned)(nunits * sizeof(float *))));


    old_use_flag = (int *) emalloc((unsigned)(nunits * sizeof(int)));
    install_var("old_use_flag", Vint,(int *) old_use_flag, nunits, 0, SETSVMENU);
     
    pnode_weight = (( float ** ) emalloc((unsigned)(nhiddens * sizeof(float *)))); 
    p_node=( (int **) emalloc((unsigned)(nunits * sizeof(int))));
    old_p_node=( (int **) emalloc((unsigned)(nunits * sizeof(int))));


    w_mean_value1 = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    w_mean_value2 = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    w_spread_a = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));
    w_spread_b = ((float **) emalloc((unsigned)(nunits * sizeof(float *))));

    install_var("mean_value1_weight", PVweight,(int *) 
                 mean_value1_weight, nunits, nunits,SETSVMENU);
    install_var("mean_value2_weight", PVweight,(int *) 
                 mean_value2_weight, nunits, nunits,SETSVMENU);
    install_var("spread_a_weight", PVweight,(int *) 
                 spread_a_weight, nunits, nunits,SETSVMENU);
    install_var("spread_b_weight", PVweight,(int *) 
                 spread_b_weight, nunits, nunits,SETSVMENU);



    install_var("pnode_weight",Vfloat,(int *) pnode_weight,nhiddens,noutputs,SETSVMENU);
    install_var("p_node", Vint,(int *) p_node, nunits, 150, SETSVMENU);
    install_var("old_p_node", Vint,(int *) old_p_node, nunits, 150, SETSVMENU);

    

    
    install_var("old_mean_value1_weight", PVweight,(int *) 
		 old_mean_value1_weight, nunits, nunits,SETSVMENU);
    install_var("old_mean_value2_weight", PVweight,(int *) 
		 old_mean_value2_weight, nunits, nunits,SETSVMENU);

    
    install_var("old_spread_a", PVweight,(int *) 
		 old_spread_a, nunits, nunits,SETSVMENU);
    install_var("old_spread_b", PVweight,(int *) 
		 old_spread_b, nunits, nunits,SETSVMENU);



    install_var("w_mean_value1", PVweight,(int *) 
                 w_mean_value1, nunits, nunits,SETSVMENU);
    install_var("w_mean_value2", PVweight,(int *) 
                 w_mean_value2, nunits, nunits,SETSVMENU);
    install_var("w_spread_a", PVweight,(int *) 
                 w_spread_a, nunits, nunits,SETSVMENU);
    install_var("w_spread_b", PVweight,(int *) 
                 w_spread_b, nunits, nunits,SETSVMENU);

    tra_pattern = (int *) emalloc((unsigned)(npatterns * sizeof(int)));
    err_pattern = (int *) emalloc((unsigned)(npatterns * sizeof(int)));
    enode_win = (int *) emalloc((unsigned)(nhiddens * sizeof(int)));
    install_var("err_pattern", Vint,(int *) err_pattern, npatterns, 0, SETSVMENU);
    install_var("tra_pattern", Vint,(int *) tra_pattern, npatterns, 0, SETSVMENU);
    install_var("enode_win", Vint,(int *) enode_win,nhiddens , 0, SETSVMENU);

    
    for (i = ninputs; i < nunits; i++) {
      mean_value1_weight[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      mean_value2_weight[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      spread_a_weight[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      spread_b_weight[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));

      w_mean_value1[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      w_mean_value2[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      w_spread_a[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      w_spread_b[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));

      old_mean_value1_weight[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      old_mean_value2_weight[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      old_spread_a[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));
      old_spread_b[i] = ((float *) emalloc((unsigned) (ninputs / 4) * sizeof (float)));

            for (j = 0; j < (ninputs / 4); j++) {
          w_mean_value1[i][j] = 0.0;
          w_mean_value2[i][j] = 0.0;
          w_spread_a[i][j] = 0.0;
          w_spread_b[i][j] = 0.0;
          old_mean_value1_weight[i][j] = 0.0;
          old_mean_value2_weight[i][j] = 0.0;
          old_spread_a[i][j] = 0.0;
          old_spread_b[i][j] =0.0;

       }
    }
    
                  
    for (i=0 ; i< nhiddens ; i++)
      {
        pnode_weight[i]=((float *) emalloc((unsigned) (noutputs) * sizeof (float)));
        for(j=0; j< noutputs ;j++)
         pnode_weight[i][j]= 0.0;
      }

                   
    for (i=0 ; i< nunits ; i++)
      {
        p_node[i]=((int *) emalloc((unsigned) (150) * sizeof (int)));
        old_p_node[i]=((int *) emalloc((unsigned) (150) * sizeof (int)));
        for(j=0; j< 150 ;j++)
         {
           p_node[i][j]= 0;
           old_p_node[i][j]= 0;
         }

           
      }


    for (i = 0; i < nunits; i++) {
        netinput[i] = 0.0;        
        activation[i] = 0.0;
        mean_value1[i] = 0.0;
        mean_value2[i] = 0.0;
        spread_a[i] = 0.0;
        spread_b[i] = 0.0;
    }
    
    max_prototype_node=noutputs;

    new1();
    
    System_Defined = TRUE;
    return(TRUE);
}

/********************/
membership_value(m1,m2,a,b,y) float m1,m2,a,b,y; {
/********************/
   if  (y <= m2 && y >= m1)
       membershipvalue = 1.0;
   else {
     if  (y > m2) {
      if ((1 - (y - m2 ) / b) > 0.0)
        membershipvalue = (1 - ( y - m2 ) / b);
      else
        membershipvalue = 0.0;
     }
     else
      if  (y < m1) {

       if ((1 - (m1 - y ) / a) > 0.0)
        membershipvalue = (1 - ( m1 - y ) / a);
       else
        membershipvalue = 0.0;
     }
   }
   update_display();
}

/*****************************************/
defuzzification(t1,t2,u,v) float t1,t2,u,v; {
/*****************************************
float x,y,z;
    defuzzify_count = t1 - u;
    x = 0.0;
    y = 0.0;

    while ( (defuzzify_count >= t1 - u)  &&  (defuzzify_count <= t2 + v)){
       membership_value(t1,t2,u,v,defuzzify_count);
       x += defuzzify_count * membershipvalue;
       y += membershipvalue;
       defuzzify_count += defuzzify_step;
    }
    if (y != 0.0)
    defuzzification_result = x / y;
******************************************/
    defuzzification_result = (t1 + t2) / 2;
}


/************/
float min(c,d) float c,d; {
/************/
   if (c > d)  return(d);
   else        return(c);
}

/************/
float max(c,d) float c,d; {
/************/
   if (c < d)  return(d);
   else        return(c);
}

/**********************/
float absolute_dist(x,y) float x,y; {
/**********************/
   float dist;
  
   dist = ( x - y ) * ( x - y );
   return ( dist );
}





/*************/    
disp_weight() {
/*************/
   int    i,j,k,test_error_number;
   float  train_correct_rate,test_correct_rate;
   
   update_display();
   printf("\n\npal = %d  ",pal);
   printf("    winner =%3d",winner);   
   printf("    t_rate =%6.3f",t_rate);   
   printf("    msize =%6.3f",max_size);
   printf("    alpha=%6.3f",alpha);

if (look > 4) {
   printf("\n\nlook =%3d",look);   
   printf("    min_dist =%6.3f",min_dist);   
   printf("    dist =%6.3f",dist);
}
   printf("\n\nno_box=%3d   base=%5u     ",nhyperbox,time4/1000);
   printf("pass1=%8u   p2.1=%8u     all=%8u",time1/1000,time2/1000,time3/1000);

/*
   printf("\n\ntra_pattern = ");
   for ( i = 0; i < tra_no; i++) 
         printf("%4d",tra_pattern[i]);
   
   printf("\nuse_train   = ");
   for ( i = 0; i < noutputs; i++) 
         printf("%4d",use_train[i]);
*/
   printf("\n\ndesired_out = ");
   for ( i = 0; i < noutputs; i++) 
         printf("%5.0f",desired_out[i]*1000);

   printf("\nhidden_out  = ");
   for ( i = ninputs; i < ninputs + nhiddens/2; i++) 
         printf("%5.0f",activation[i]*1000);

   printf("\nhidden_size = ");
   for ( i = ninputs; i < ninputs + nhiddens/2; i++) 
         printf("%5.0f",size[i] * 4000 / ninputs);
   
   printf("\nuse_flag_no = ");
   for ( i = ninputs; i < ninputs + nhiddens/2; i++) 
         printf(" %4d",use_flag[i]);

   printf("\nnet_inputs  = ");
   for ( i = 0; i < ninputs; i++) 
         printf("%5.0f",activation[i]*1000);

   if (look == 2) {
      printf("\n\newin =%3d   match node =",no_win);
      for ( i = 0; i < nhiddens - noutputs; i++) 
            printf("%5d",enode_win[i]);
      printf("\n");
   }   

   k = ninputs; 
   for ( i = k; i < k + max_prototype_node + nhyperbox; i++) {
       printf("\n m1[%d]= ",use_flag[i]);
       for ( j = 0; j < ninputs/4; j++)
           printf("%4.0f",mean_value1_weight[i][j] * 1000);
       printf("\n m2[c%d]  ",class_flag[i]);
       for ( j = 0; j < ninputs/4; j++)
           printf("%4.0f",mean_value2_weight[i][j] * 1000);
       printf("\n a       ");
       for ( j = 0; j < ninputs/4; j++)
           printf("%4.0f",spread_a_weight[i][j] * 1000);
       printf("\n b       ");
       for ( j = 0; j < ninputs/4; j++)
           printf("%4.0f",spread_b_weight[i][j] * 1000);
       printf("\n");
   }
     
   
   if ( lflag )  /* Caluate train set error number during training phase (lflag = 1) */ 
    { 
       printf("\n********************************************************************\n"); /* train correct rate */
       train_error_number=no_of_misclassification;
       printf("\n Train_error_pattern [ Total : %d ] \n",no_of_misclassification);

       /*for ( i = 0; i < no_of_misclassification; i++) 
	 printf(" %4d",err_pattern[i]);*/

       train_correct_rate= ( tra_no - train_error_number ) *  100.0  / tra_no ;
       printf( "\n Training correcting rate is : %.2f  \n",train_correct_rate);
       printf("\n********************************************************************\n");
    } 
   else
    {                 
       printf("\n********************************************************************\n"); /* test correct rate */ 
       test_error_number=no_of_misclassification;
       printf("\n Test_error_pattern [ Total : %d ]  \n",no_of_misclassification - train_error_number);
       test_correct_rate= ( npatterns - tra_no - test_error_number + train_error_number ) * 100.0 / (npatterns -tra_no )  ;
       printf( "\n Testing correcting rate is : %.2f  \n",test_correct_rate);
       printf("\n*********************************************************************\n");
    }

  }           


/********************/
compute_proto_output() {
/********************/
    int i,j,k;
    float ss,cx,cw,cw1,cw2;
        
       for (j = ninputs; j < ninputs + max_prototype_node; j++) {
        activation[j] = 0.0;
        ss = 0.0;
        for ( k = 0; k < (ninputs / 4 ) ; k++) {
           cx = ( activation[k*4] + activation[k*4+1] ) / 2.0 
              + ( activation[k*4+3] - activation[k*4+2] ) / 4.0;
           cw = ( mean_value1_weight[j][k] + mean_value2_weight[j][k] ) / 2.0 
              + ( spread_b_weight[j][k] - spread_a_weight[j][k] ) / 4.0;
           ss += absolute_dist(cx,cw);  
        }
        activation[j] = 1.0 - sqrt( ss / (float) k ); 
    }    
}


/*********************/
avg_proto_change_weights() {
/*********************/
    int     i,j,k,n;

    j = ninputs + class;
    use_flag[j]++;
    
    k = use_flag[j] - 1;
    for ( i = 0; i < ninputs / 4; i++) {
        mean_value1_weight[j][i] = (mean_value1_weight[j][i] * k + activation[i*4]) / (k+1);
        mean_value2_weight[j][i] = (mean_value2_weight[j][i] * k + activation[i*4+1]) / (k+1);
        spread_a_weight[j][i] = (spread_a_weight[j][i] * k + activation[i*4+2]) / (k+1);
        spread_b_weight[j][i] = (spread_b_weight[j][i] * k + activation[i*4+3]) / (k+1);
    }
      
    for ( j = ninputs; j < ninputs + noutputs; j++ ) {
    size[j] = 0.0;
    for ( k = 0; k < ninputs / 4; k++) 
      size[j] += ( mean_value2_weight[j][k] - mean_value1_weight[j][k]
              +  ( spread_a_weight[j][k] + spread_b_weight[j][k] ) / 2.0);
    }    
}

/*********************/
lvq_proto_change_weights() {
/*********************/
       int     i,j,k,n,a;
       float   r,s,t,x,y,z;

              
        j = winner; 
        p_node[winner][p_node_index[winner]]=patno;
        p_node_index[winner]++;
        use_flag[winner]++;
        lrate = ( 1.0 - ((float) (iteration) / (float) max_run_len));
        r = alpha; 
	  for ( i = 0; i < ninputs / 4; i++) 
          {
 
             t =  mean_value1_weight[j][i];
             mean_value1_weight[j][i] += (r*lrate*(activation[i*4] - mean_value2_weight[j][i]));
             mean_value2_weight[j][i] += (r*lrate*(activation[i*4+1] - t));
             t = spread_a_weight[j][i];
             spread_a_weight[j][i] += (r*lrate*(activation[i*4+2] + spread_b_weight[j][i]));
             spread_b_weight[j][i] += (r*lrate*(activation[i*4+3] + t));
             
             if (mean_value1_weight[j][i] <= 0.0 ) 
                {
                 mean_value1_weight[j][i]=0.0;
                 spread_a_weight[j][i]=0.0;
                }

             if (mean_value2_weight[j][i] >= 1.0 ) 
                {
                  mean_value2_weight[j][i]=1.0 ;
                  spread_b_weight[j][i]=0.0;
                } 

             z= mean_value1_weight[j][i] - spread_a_weight[j][i];
             if( z < 0.0 ) spread_a_weight[j][i]+=z;  

             z= mean_value2_weight[j][i] + spread_b_weight[j][i];
             if ( z > 1.0) spread_b_weight[j][i] += ( 1.0 - z );

       

          }    
       
          

       size[j] = 0.0;
       for ( i = 0; i < ninputs / 4; i++) 
         size[j] += ( mean_value2_weight[j][i] - mean_value1_weight[j][i]
                 +  ( spread_a_weight[j][i] + spread_b_weight[j][i] ) / 2.0 );
}


/*********************/
pecfuh_proto_change_weights() {
/*********************/
     int     d,i,j,k,ii,jj,kk,n;
     float   r,s,t,x,y,z;
   
     j = ninputs + class;
     use_flag[j]++;
    
     ii = jj = 0;
     if (use_flag[j] == 1)
     for ( k = 0; k < ninputs / 4; k++ ) {
           mean_value1_weight[j][k] = activation[k*4];
           mean_value2_weight[j][k] = activation[k*4+1];
           spread_a_weight[j][k] = activation[k*4+2];
           spread_b_weight[j][k] = activation[k*4+3];
     }
     else {
        for ( k = 0; k < ninputs / 4; k++ ) {
/* m1 */     
          x = mean_value1_weight[j][k];
          y = activation[k*4];
          if ( x > y ) {
             mean_value1_weight[j][k] = y;
             spread_a_weight[j][k] = activation[k*4+2];
          }
          else ii++;   
          if (( x == y ) && ( spread_a_weight[j][k] < activation[k*4+2])) 
             spread_a_weight[j][k] = activation[k*4+2];
           
          z = mean_value1_weight[j][k] - spread_a_weight[j][k];
          if ( z < 0.0 ) spread_a_weight[j][k] += z;
/* m2 */                  
          x = mean_value2_weight[j][k];
          y = activation[k*4+1];
          if ( x < y ) {
             mean_value2_weight[j][k] = y;
             spread_b_weight[j][k] = activation[k*4+3];
          }   
          else jj++;
          if (( x == y ) && ( spread_b_weight[j][k] < activation[k*4+3])) 
             spread_b_weight[j][k] = activation[k*4+3];
               
          z = mean_value2_weight[j][k] + spread_b_weight[j][k];
          if ( z > 1.0) spread_b_weight[j][k] += ( 1.0 - z);
        }
  
        if ( (ii != k) || (jj != k) ) {  
          for (i = ninputs; i < ninputs + noutputs; i++) {        
            if ((use_flag[j] > 0 ) && (use_flag[i] > 0 ) && (j != i)) {
              kk = ninputs / 4;
              min_dist = 1.0;
              dist = 1.0;
              for ( k = 0; k < ninputs / 4; k++ ) {
                x = mean_value1_weight[j][k];
                y = mean_value2_weight[j][k];
                s = mean_value1_weight[i][k];
                t = mean_value2_weight[i][k];
               
                if (look > 4)  disp_weight();

/* x s y t */   if ((x <= s) && (s < y) && (y <= t)) {
/*   x y t */      dist = y - s;
/* x s   y */      if (look > 4) str=get_command("tt1");
/* s     t */   }

/* s x t y */   else if ((s <= x) && (x < t) && (t <= y)) {
/*   s t y */      dist = t - x;
/* s x   t */      if (look > 4) str=get_command("tt2");
/* x     y */   }

/* s x y t */   else if (((s < x) && (y < t)) || ((x < s) && (t < y))) {
/* x s t y */      dist = min (y-s , t-x);
                   if (look > 4) str=get_command("tt3 || tt4");
                }

                else {
                   dist = 1.0;
                   kk = ninputs / 4;
                   k = ninputs / 4;
                   if (look > 4) str=get_command("telse");
                } 
              
                if (min_dist > dist) {
                   min_dist = dist;
                   kk = k;
                   if (look > 4) {
                      disp_weight();
                      str=get_command("min_dist");
                   }   
                }   
              }   /* end for k : for each feature */  
            
              if (kk < ninputs / 4) {
                 if (look > 4) str=get_command("contracting.....");
                 k = kk;
                 x = mean_value1_weight[j][k];
                 y = mean_value2_weight[j][k];
                 s = mean_value1_weight[i][k];
                 t = mean_value2_weight[i][k];
               
                 if (( x <= s ) && ( s < y ) && ( y <= t )) {
/* x s   y t */     r = ( s + y ) / 2.0;
/*     r     */     mean_value2_weight[j][k] = r;
                    mean_value1_weight[i][k] = r;
                    spread_b_weight[j][k] += (y - r);
                    spread_a_weight[i][k] += (r - s);
                    d = 1;
                 }
               
                 else if (( s <= x ) && ( x < t ) && ( t <= y )) {
/* s x   t y */     r = ( x + t ) / 2.0;
/*     r     */     mean_value1_weight[j][k] = r;
                    mean_value2_weight[i][k] = r;
                    spread_a_weight[j][k] += (r - x);
                    spread_b_weight[i][k] += (t - r);
                    d = 2; 
                 } 
               
                 else if (( s > x ) && ( y > t )) {
/* x   s ty */      if (( t - x ) > ( y - s )) {
                       mean_value2_weight[j][k] = s;
                       spread_b_weight[j][k] += ( y - s);
                       d = 3; 
                    }
                    else {
/* xs t   y */         mean_value1_weight[j][k] = t;
                       spread_a_weight[j][k] += ( t - x);
                       d = 4; 
                    }
                 }   
               
                 else if (( x > s ) && ( t > y )) {
/* s   x yt */      if (( y - s ) > ( t - x )) {
                       mean_value2_weight[i][k] = x;
                       spread_b_weight[i][k] += ( t - x);
                       d = 5; 
                    }
/* sx y   t */      else {
                       mean_value1_weight[i][k] = y;
                       spread_a_weight[i][k] += ( y - s);
                       d = 6; 
                    }
                 }

                 if (look > 4) {
                    disp_weight();
                    if (d == 1) str=get_command("modify weight m2");
                    else if (d == 2) str=get_command("modify weight m1");
                    else if (d == 3) str=get_command("modify covering weight near m2");
                    else if (d == 4) str=get_command("modify covering weight near m1");
                    else if (d == 5) str=get_command("modify covered weight near m2");
                    else if (d == 6) str=get_command("modify covered weight near m1");
                 }   
              }  /* if kk     */
            }    /* if j != i */
          }      /* for i     */
        }        /* for else ii || jj */  
/*      
        for ( j = ninputs; j < ninputs + noutputs; j++ ) 
        for ( k = 0; k < ninputs / 4; k++) {
          x = mean_value1_weight[j][k];
          y = mean_value2_weight[j][k];
          s = spread_a_weight[j][k];
          t = spread_b_weight[j][k];
          
          if (( x < 0.0 ) || ( y < 0.0 ) || ( s < 0.0 ) || ( t < 0.0 ) ||
              ( x > 1.0 ) || ( y > 1.0 ) || ( s > 1.0 ) || ( t > 1.0 )) {
             disp_weight();       
             str=get_command("under 0 or over 1 checked ....");
          }
        }

        for ( j = ninputs; j < ninputs + noutputs; j++ ) {
          size[j] = 0.0;
          for ( k = 0; k < ninputs / 4; k++) 
            size[j] += (mean_value2_weight[j][k] - mean_value1_weight[j][k]
                    +  (spread_a_weight[j][k] + spread_b_weight[j][k]) / 2.0);
        }
*/     
     }  /* end for else */  
}


/***********/
setinput() {
/***********/
    register int    i;
    register float  *pp;
    for ( i = 0, pp = ipattern[patno]; i < ninputs; i++, pp++)
          activation[i] = *pp;
    strcpy(cpname,pname[patno]);
}

/****************/
settarget() {
/****************/
    register int    i;
    register float  *pp;
    
    class = noutputs;
    for ( i = 0, pp = tpattern[patno]; i < noutputs; i++, pp++) {
          desired_out[i] = *pp;
          if ( desired_out[i] == 1.0 ) class = i;
          else desired_out[i] = 0.0;
    }      
}


/****************/
setup_pattern() {
/****************/
    setinput();
    settarget();
}

/****************/
proto_trial() {
/****************/
    setup_pattern();
    compute_proto_output();
    compute_classification_result(); 
}


/*********/
ptrain() {
/*********/
  return(train('p'));
}

/*********/
strain() {
/*********/
  return(train('s'));
}

/***************************/
compute_hyperboxes_output() {
/***************************/
   int   i,j,feature,k;
   float a,b,sum,cx,cw,ss;
   
  if ( nhyperbox != 0 )
  {
       for (i = (ninputs + max_prototype_node); i < (ninputs+max_prototype_node+nhyperbox); i++) 
        {
         sum = 0.0;
         feature = 0;
         for (j = 0; j < ninputs / 4; j++) 
           {
             a = activation[j*4] - mean_value1_weight[i][j];
             b = activation[j*4+1] - mean_value2_weight[i][j];
             if ((a >= 0) && (b <= 0))
              {
                sum += ( a * a + b * b);
                feature++;
              }
           }
         if (feature == (ninputs / 4))  activation[i] = 2.0 - sqrt ( sum / feature );
         /*else activation[i]=0.0; */

         else /* when input pattern falls outside an exemplar node */
           {
                 activation[i] = 0.0;
                 ss = 0.0;
                 for ( k = 0; k < (ninputs / 4 ) ; k++) 
                   {
                     cx = ( activation[k*4] + activation[k*4+1] ) / 2.0 
                        + ( activation[k*4+3] - activation[k*4+2] ) / 4.0;
                     cw = ( mean_value1_weight[i][k] + mean_value2_weight[i][k] ) / 2.0 
                        + ( spread_b_weight[i][k] - spread_a_weight[i][k] ) / 4.0;
                     ss += absolute_dist(cx,cw);  
                   }
                 activation[i] = 1.0 - sqrt( ss / (float) k ); 
                  
            } 

          }  /* for */
     } 
     else if( nhyperbox == 0 )
             for (i = ninputs + max_prototype_node ; i < nunits; i++) 
               activation[i] = 0.0;



   
}

/*******************************/
compute_classification_result() {
/*******************************/
     int i,j,k,m;
     float s;
   
     s = 0.0;
     winner = ninputs ;
     
     for (j = winner; j < ninputs + nhiddens; j++) {
         if (activation[j] > s) {
            s = activation[j]; 
            winner = j;
         }   
     }    
     
     if ( s == 0.0 ) {
        err_pattern[no_of_misclassification] = patno;
        no_of_misclassification++;      
        mis_flag = 1;
        nonwin = 1;   
     }   
     else {
        nonwin = 0;
        mis_flag = 0;  
        if ((class_flag[winner] == class) && (use_flag[winner] == 0))        
           mis_flag = 1; 
        else if (class_flag[winner] != class) {
               no_of_misclassification++;
               if (no_of_misclassification > mistake)
               mistake = no_of_misclassification; 

           mis_flag = 1;
        }
     } 
}


/***********************/
hyperbox_change_weight_1() {
/***********************/
int i,j,k,m,n,n1,n2,p,q,r,ecount=0,exp_no=0;
    float m1,m2,a,b,s,t,x,y,sz,z;
    
    no_win = 0;
    
    enode_win[0] = ninputs + max_prototype_node - 1;     

    if ( nhyperbox == 0 ) hyperbox_generate();
    else {

              p = ninputs + max_prototype_node;
              enode_win[0] = winner;
              ecount=1;
           

       for (r = 0; r < ecount; r++) {
           j = enode_win[r];
           no_win = enode_win[r];
           for ( i = p; i < p + nhyperbox; i++ ) {
             if ( i != j ) {
                n = 0;
                n1 = 0;
                 n2 = 0;
                for ( k = 0; k < ninputs / 4; k++) {
                  x = mean_value1_weight[j][k];
                  y = activation[k*4];
                  s = activation[k*4];
                  if ( x < y ) s = x;
                  x = mean_value2_weight[j][k];
                  y = activation[k*4+1];
                  t = activation[k*4+1];
                  if ( x > y ) t = x;
       
      /* ------ overlap test (include nesting ) ------------- */ 
                  if (((s < mean_value1_weight[i][k]) 
                   &&  (t < mean_value2_weight[i][k])
                   &&  (t > mean_value1_weight[i][k])) 
                   || ((s > mean_value1_weight[i][k]) 
                   &&  (t > mean_value2_weight[i][k])
                   &&  (s < mean_value2_weight[i][k]))
                   || ((s <= mean_value1_weight[i][k]) 
                   &&  (t >= mean_value2_weight[i][k]))
                   || ((s >= mean_value1_weight[i][k]) 
                   &&  (t <= mean_value2_weight[i][k]))) n++;
                  }    
                  
                /* ------ nesting test -------------------------------- */ 
                  if ((s <= mean_value1_weight[i][k]) 
                   && (t >= mean_value2_weight[i][k])) n1++;
                  if (n1 == k) {
                      disp_weight();
                      str=get_command("nested n1");
                  } 
                  if ((s >= mean_value1_weight[i][k]) 
                   && (t <= mean_value2_weight[i][k])) n2++; 
                  if (n2 == k) {
                      disp_weight();
                      str=get_command("nested n2");
                  } 

                  if ( (n == k) && (n1 != k) && (n2 != k) &&
                     (((holap == 1) && (class_flag[i] != class_flag[j]))
                     || (holap == 0)) ) {
                     if (look > 3) {
                       disp_weight();
                       str=get_command("overlap...");
                     }
                     i = p + nhyperbox;
                     enode_win[r] = ninputs + max_prototype_node - 1;
                     printf(" Overlap occurs !!!!\n");

                  }
             }   
           }
             }

if (look > 3) {
   disp_weight();
   str=get_command("enode win2"); 
}
       for (r = 0; r < ecount; r++) {
         j = enode_win[r];
         no_win = enode_win[r];
         if (j >= ninputs + max_prototype_node) {
             sz = 0.0;
             for ( k = 0; k < ninputs / 4; k++ ) {
               m1 = mean_value1_weight[j][k];
               m2 = mean_value2_weight[j][k];
               a = spread_a_weight[j][k];
               b = spread_b_weight[j][k];
   /* m1 */     
               x = m1;
               y = activation[k*4];
               if ( x > y ) {
                  m1 = y;
                  a = activation[k*4+2];
               }   
               if ((x == y) && (a < activation[k*4+2]))
                  a = activation[k*4+2];
           
               
               z = m1 - a;
               if ( z < 0.0 ) a += z;
   /* m2 */                  
               x = m2;
               y = activation[k*4+1];
               if ( x < y ) {
                  m2 = y;
                  b = activation[k*4+3];
               }   
               if ((x == y) && (b < activation[k*4+3]))
                  b = activation[k*4+3];
               
               z = m2 + b;
               if ( z > 1.0) b += ( 1.0 - z );
               
               sz += (m2 - m1 + ( a + b ) / 2.0);
            } 

           
             if (sz > max_size * k) 
               {
                enode_win[r] = ninputs + max_prototype_node - 1;
                printf(" size too large !!!!\n");
               }
         }    
       }

if (look > 3) {
   disp_weight();
   str=get_command("enode win3"); 
}   
              
       if( enode_win[0] == winner )  exp_no=no_win = winner;  
       else exp_no=no_win=0;  

if (look > 3) {
   disp_weight();
   str=get_command("enode win4"); 
}              

           if ( exp_no == 0 ) hyperbox_generate();
                     
             else {
             j = exp_no;
             size[j] = 0.0;
             for ( k = 0; k < ninputs / 4; k++ ) {
   /* m1 */     
               x = mean_value1_weight[j][k];
               y = activation[k*4];
                 if ( x > y ) {
                  mean_value1_weight[j][k] = y;
                  spread_a_weight[j][k] = activation[k*4+2];
               }   
               if ((x == y) && (spread_a_weight[j][k] < activation[k*4+2]))
                  spread_a_weight[j][k] = activation[k*4+2];
           
               z = mean_value1_weight[j][k] - spread_a_weight[j][k];
               if ( z < 0.0 ) spread_a_weight[j][k] += z;
   /* m2 */                  
               x = mean_value2_weight[j][k];
               y = activation[k*4+1];
               if ( x < y ) {
                  mean_value2_weight[j][k] = y;
                  spread_b_weight[j][k] = activation[k*4+3];
               }   
               if ((x == y) && (spread_b_weight[j][k] < activation[k*4+3]))
                  spread_b_weight[j][k] = activation[k*4+3];
               
               z = mean_value2_weight[j][k] + spread_b_weight[j][k];
                 
               size[j] += ( mean_value2_weight[j][k] - mean_value1_weight[j][k]
                       +  ( spread_a_weight[j][k] + spread_b_weight[j][k] ) / 2.0 );
             } 
           }
    }




}

 /************************/
hyperbox_change_weight_2() {
/************************/
    /* hyperbox learning */

    int i,j,k,m,n,n1,n2,p,q,r,ecount=0,exp_no=0;
    float m1,m2,a,b,s,t,x,y,sz,z;
    
    no_win = 0;
    for (j = 0; j < nhiddens - max_prototype_node; j++)
         enode_win[j] = ninputs + max_prototype_node - 1;     

    if ( nhyperbox == 0 ) hyperbox_generate();
    else {

/* --- find exemplar which has the same class of current pattern ----- */       

       p = ninputs + max_prototype_node;
       
       for ( j = p; j < p + nhyperbox; j++ ) 
           if (class_flag[j] == class) {
              enode_win[ecount] = j;
              ecount++;
           }   

       for (r = 0; r < ecount; r++) {
           j = enode_win[r];
           no_win = enode_win[r];
           for ( i = p; i < p + nhyperbox; i++ ) {
             if ( i != j ) {
                n = 0;
                n1 = 0;
                n2 = 0;
                for ( k = 0; k < ninputs / 4; k++) {
                  x = mean_value1_weight[j][k];
                  y = activation[k*4];
                  s = activation[k*4];
                  if ( x < y ) s = x;
                  x = mean_value2_weight[j][k];
                  y = activation[k*4+1];
                  t = activation[k*4+1];
                  if ( x > y ) t = x;
       
      /* ------ overlap test (include nesting ) ------------- */ 
                  if (((s < mean_value1_weight[i][k]) 
                   &&  (t < mean_value2_weight[i][k])
                   &&  (t > mean_value1_weight[i][k])) 
                   || ((s > mean_value1_weight[i][k]) 
                   &&  (t > mean_value2_weight[i][k])
                   &&  (s < mean_value2_weight[i][k]))
                   || ((s <= mean_value1_weight[i][k]) 
                   &&  (t >= mean_value2_weight[i][k]))
                   || ((s >= mean_value1_weight[i][k]) 
                   &&  (t <= mean_value2_weight[i][k]))) n++;
                    }    
                  
      /* ------ nesting test -------------------------------- */ 
                  if ((s <= mean_value1_weight[i][k]) 
                   && (t >= mean_value2_weight[i][k])) n1++;
                      if (n1 == k) {
                      disp_weight();
                      str=get_command("nested n1");
                  } 
                  if ((s >= mean_value1_weight[i][k]) 
                   && (t <= mean_value2_weight[i][k])) n2++; 
                  if (n2 == k) {
                      disp_weight();
                      str=get_command("nested n2");
                  } 

                  if ( (n == k) && (n1 != k) && (n2 != k) &&
                     (((holap == 1) && (class_flag[i] != class_flag[j]))
                     || (holap == 0)) ) {
                     if (look > 3) {
                       disp_weight();
                       str=get_command("overlap...");
                     }
                     i = p + nhyperbox;
                     enode_win[r] = ninputs + max_prototype_node - 1;
                  }
             }   
           }
       }

if (look > 3) {
   disp_weight();
   str=get_command("enode win2"); 
}
       for (r = 0; r < ecount; r++) {
         j = enode_win[r];
         no_win = enode_win[r];
         if (j >= ninputs + max_prototype_node) {
             sz = 0.0;
             for ( k = 0; k < ninputs / 4; k++ ) {
               m1 = mean_value1_weight[j][k];
               m2 = mean_value2_weight[j][k];
               a = spread_a_weight[j][k];
               b = spread_b_weight[j][k];
   /* m1 */     
               x = m1;
               y = activation[k*4];
               if ( x > y ) {
                  m1 = y;
                  a = activation[k*4+2];
               }   
               if ((x == y) && (a < activation[k*4+2]))
                  a = activation[k*4+2];
           
 
               z = m1 - a;
               if ( z < 0.0 ) a += z;
   /* m2 */                  
               x = m2;
               y = activation[k*4+1];
               if ( x < y ) {
                  m2 = y;
                  b = activation[k*4+3];
               }   
               if ((x == y) && (b < activation[k*4+3]))
                 
               z = m2 + b;
               if ( z > 1.0) b += ( 1.0 - z );
               
               sz += (m2 - m1 + ( a + b ) / 2.0);
             } 

             if (sz > max_size * k) enode_win[r] = ninputs + max_prototype_node - 1;
         }    
      }

if (look > 3) {
   disp_weight();
   str=get_command("enode win3"); 
}   
       exp_no = 0;
       min_dist = 1.0;
       for ( r = 0; r < ecount; r++ ) {
         j = enode_win[r];
         no_win = enode_win[r];
         if ( j >= ninputs + max_prototype_node ) {
              dist = 0.0;
              for ( k = 0; k < ninputs / 4; k++) {
                x = (mean_value1_weight[j][k] + mean_value2_weight[j][k]) / 2.0
                  + (spread_b_weight[j][k] - spread_a_weight[j][k]) / 4.0; 
                y = (activation[k * 4] + activation[k * 4 + 1]) / 2.0
                  + (activation[k * 4 + 3] - activation[k * 4 + 2]) / 4.0;
                dist += absolute_dist(x,y);
              }
              dist = sqrt ( dist / (float) k );
              if ( dist < min_dist) {
                 exp_no = j;
                 min_dist = dist;
              }   
         }
       }  
       
       no_win = exp_no;    

if (look > 3) {
   disp_weight();
   str=get_command("enode win4"); 
}              

  if ( exp_no == 0 ) hyperbox_generate();
                  
           else {
              
             j = exp_no;
             size[j] = 0.0;
             for ( k = 0; k < ninputs / 4; k++ ) {
   /* m1 */     
               x = mean_value1_weight[j][k];
               y = activation[k*4];
               if ( x > y ) {
                  mean_value1_weight[j][k] = y;
                  spread_a_weight[j][k] = activation[k*4+2];
               }   
               if ((x == y) && (spread_a_weight[j][k] < activation[k*4+2]))
                  spread_a_weight[j][k] = activation[k*4+2];
           
               z = mean_value1_weight[j][k] - spread_a_weight[j][k];
               if ( z < 0.0 ) spread_a_weight[j][k] += z;
   /* m2 */                  
               x = mean_value2_weight[j][k];
               y = activation[k*4+1];
               if ( x < y ) {
                  mean_value2_weight[j][k] = y;
                  spread_b_weight[j][k] = activation[k*4+3];
               }   
              if ((x == y) && (spread_b_weight[j][k] < activation[k*4+3]))
                  spread_b_weight[j][k] = activation[k*4+3];
               
               z = mean_value2_weight[j][k] + spread_b_weight[j][k];
               if ( z > 1.0) spread_b_weight[j][k] += ( 1.0 - z );
               
               size[j] += ( mean_value2_weight[j][k] - mean_value1_weight[j][k]
                       +  ( spread_a_weight[j][k] + spread_b_weight[j][k] ) / 2.0 );
             } 
           }
    }
}






/********************/
hyperbox_generate() {
/********************/
   int j,k;
   
   j = ninputs + max_prototype_node + nhyperbox;
   if (j > (nunits - max_prototype_node - 1)) {
        printf ("\n *** error %d***\n", nhyperbox);
   }
   else {
     size[j] = 0.0;
     for ( k = 0; k < ninputs / 4; k++) {
       mean_value1_weight[j][k] = activation[k * 4];
       mean_value2_weight[j][k] = activation[k * 4 + 1];
       spread_a_weight[j][k] = activation[k * 4 + 2];
       spread_b_weight[j][k] = activation[k * 4 + 3];
       size[j] += ( mean_value2_weight[j][k] - mean_value1_weight[j][k]
               +  ( spread_a_weight[j][k] + spread_b_weight[j][k] ) / 2.0 );
     }

     class_flag[j] = class;  
     use_flag[j] = 1;       
     nhyperbox++;
   }
}






/************************/
hyperbox_change_weight() {
/************************/
    /* hyperbox learning */

    int i,j,k,m,n,n1,n2,p,q,r,ecount=0,exp_no=0;
    float m1,m2,a,b,s,t,x,y,sz,z;
    
    no_win = 0;
    for (j = 0; j < nhiddens - max_prototype_node; j++)
         enode_win[j] = ninputs + max_prototype_node - 1;     

    if ( nhyperbox == 0 ) hyperbox_generate();
    else {

/* --- find exemplar which has the same class of current pattern ----- */       

       p = ninputs + max_prototype_node;
       
       for ( j = p; j < p + nhyperbox; j++ ) 
           if (class_flag[j] == class) {
              enode_win[ecount] = j;
              ecount++;
           }   

       for (r = 0; r < ecount; r++) {
           j = enode_win[r];
           no_win = enode_win[r];
           for ( i = p; i < p + nhyperbox; i++ ) {
             if ( i != j ) {
                n = 0;
                n1 = 0;
                n2 = 0;
                for ( k = 0; k < ninputs / 4; k++) {
                  x = mean_value1_weight[j][k];
                  y = activation[k*4];
                  s = activation[k*4];
                  if ( x < y ) s = x;
                  x = mean_value2_weight[j][k];
                  y = activation[k*4+1];
                  t = activation[k*4+1];
                  if ( x > y ) t = x;
       
      /* ------ overlap test (include nesting ) ------------- */ 
                  if (((s < mean_value1_weight[i][k]) 
                   &&  (t < mean_value2_weight[i][k])
                   &&  (t > mean_value1_weight[i][k])) 
                   || ((s > mean_value1_weight[i][k]) 
                   &&  (t > mean_value2_weight[i][k])
                   &&  (s < mean_value2_weight[i][k]))
                   || ((s <= mean_value1_weight[i][k]) 
                   &&  (t >= mean_value2_weight[i][k]))
                   || ((s >= mean_value1_weight[i][k]) 
                   &&  (t <= mean_value2_weight[i][k]))) n++;
                  }    
                  
      /* ------ nesting test -------------------------------- */ 
                  if ((s <= mean_value1_weight[i][k]) 
                   && (t >= mean_value2_weight[i][k])) n1++;
                  if (n1 == k) {
                      disp_weight();
                      str=get_command("nested n1");
                  } 
                  if ((s >= mean_value1_weight[i][k]) 
                   && (t <= mean_value2_weight[i][k])) n2++; 
                  if (n2 == k) {
                      disp_weight();
                      str=get_command("nested n2");
                  } 

                  if ( (n == k) && (n1 != k) && (n2 != k) &&
                     (((holap == 1) && (class_flag[i] != class_flag[j]))
                     || (holap == 0)) ) {
                     if (look > 3) {
                       disp_weight();
                       str=get_command("overlap...");
                     }
                     i = p + nhyperbox;
                     enode_win[r] = ninputs + max_prototype_node - 1;
                  }
             }   
           }
       }

if (look > 3) {
   disp_weight();
   str=get_command("enode win2"); 
}
       for (r = 0; r < ecount; r++) {
         j = enode_win[r];
         no_win = enode_win[r];
         if (j >= ninputs + max_prototype_node) {
             sz = 0.0;
             for ( k = 0; k < ninputs / 4; k++ ) {
               m1 = mean_value1_weight[j][k];
               m2 = mean_value2_weight[j][k];
               a = spread_a_weight[j][k];
               b = spread_b_weight[j][k];
   /* m1 */     
               x = m1;
               y = activation[k*4];
               if ( x > y ) {
                  m1 = y;
                  a = activation[k*4+2];
               }   
               if ((x == y) && (a < activation[k*4+2]))
                  a = activation[k*4+2];
           
               z = m1 - a;
               if ( z < 0.0 ) a += z;
   /* m2 */                  
               x = m2;
               y = activation[k*4+1];
               if ( x < y ) {
                  m2 = y;
                  b = activation[k*4+3];
               }   
               if ((x == y) && (b < activation[k*4+3]))
                  b = activation[k*4+3];
               
               z = m2 + b;
               if ( z > 1.0) b += ( 1.0 - z );
               
               sz += (m2 - m1 + ( a + b ) / 2.0);
             } 

             z = 1.0 * k;
             if ((nonwin == 0) && (winner >= ninputs + max_prototype_node)) {
                n1 = 0;
                n2 = 0;
                for ( k = 0; k < ninputs / 4; k++) {
                  m1 = w_mean_value1[j][k];
                  m2 = w_mean_value2[j][k];
                  if ((m1 <= mean_value1_weight[winner][k]) &&
                      (m2 >= mean_value2_weight[winner][k])) {
                      n1++;
                      if (look == 4) {
                         disp_weight();
                         if (k==0) str=get_command("covering expanded n1 0");
                         else if (k==1) str=get_command("covering expanded n1 1");
                         else if (k==2) str=get_command("covering expanded n1 2");
                         else if (k==3) str=get_command("covering expanded n1 3");
                         else if (k==4) str=get_command("covering expanded n1 4");
                         else if (k==5) str=get_command("covering expanded n1 5");
                      }
                  }        
                  if ((m1 >= mean_value1_weight[winner][k]) &&
                      (m2 <= mean_value2_weight[winner][k])) {
                      n2++; 
                      if (look == 4) {
                         disp_weight();
                         if (k==0) str=get_command("nesting expanded n2 0");
                         else if (k==1) str=get_command("nesting expanded n2 1");
                         else if (k==2) str=get_command("nesting expanded n2 2");
                         else if (k==3) str=get_command("nesting expanded n2 3");
                         else if (k==4) str=get_command("nesting expanded n2 4");
                         else if (k==5) str=get_command("nesting expanded n2 5");
                      }
                  }        
                }
             
                if ((n1 == k) && (size[winner] > max_size * sz)) {
                   enode_win[r] = ninputs + max_prototype_node - 1;
                   if (look > 3) {
                      disp_weight();
                      str=get_command("covering expanded");
                   }   
                }   
                else if (n2 == k) {
                   z = size[winner];
                   if (look > 3) {
                      disp_weight();
                      str=get_command("nesting expanded");
                   }   
                }
             }
             if (sz > max_size * z) enode_win[r] = ninputs + max_prototype_node - 1;
         }    
       }

if (look > 3) {
   disp_weight();
   str=get_command("enode win3"); 
}   
       exp_no = 0;
       min_dist = 1.0;
       for ( r = 0; r < ecount; r++ ) {
         j = enode_win[r];
         no_win = enode_win[r];
         if ( j >= ninputs + max_prototype_node) {
              dist = 0.0;
              for ( k = 0; k < ninputs / 4; k++) {
                x = (mean_value1_weight[j][k] + mean_value2_weight[j][k]) / 2.0
                  + (spread_b_weight[j][k] - spread_a_weight[j][k]) / 4.0; 
                y = (activation[k * 4] + activation[k * 4 + 1]) / 2.0
                  + (activation[k * 4 + 3] - activation[k * 4 + 2]) / 4.0;
                dist += absolute_dist(x,y);
              }
              dist = sqrt ( dist / (float) k );
              if ( dist < min_dist) {
                 exp_no = j;
                 min_dist = dist;
              }   
         }
       }  
       
       no_win = exp_no;    

if (look > 3) {
   disp_weight();
   str=get_command("enode win4"); 
}              

           if ( exp_no == 0 ) 
              {
               printf("\n *********** Generate a exempler to store a new pattern **************\n");
               hyperbox_generate();
             } 
           else {
               printf("\n ***********  Expand the exempler to include a apttern  ***************\n");
             j = exp_no;
             size[j] = 0.0;
             for ( k = 0; k < ninputs / 4; k++ ) {
   /* m1 */     
               x = mean_value1_weight[j][k];
               y = activation[k*4];
               if ( x > y ) {
                  mean_value1_weight[j][k] = y;
                  spread_a_weight[j][k] = activation[k*4+2];
               }   
               if ((x == y) && (spread_a_weight[j][k] < activation[k*4+2]))
                  spread_a_weight[j][k] = activation[k*4+2];
           
               z = mean_value1_weight[j][k] - spread_a_weight[j][k];
               if ( z < 0.0 ) spread_a_weight[j][k] += z;
   /* m2 */                  
               x = mean_value2_weight[j][k];
               y = activation[k*4+1];
               if ( x < y ) {
                  mean_value2_weight[j][k] = y;
                  spread_b_weight[j][k] = activation[k*4+3];
               }   
               if ((x == y) && (spread_b_weight[j][k] < activation[k*4+3]))
                  spread_b_weight[j][k] = activation[k*4+3];
               
               z = mean_value2_weight[j][k] + spread_b_weight[j][k];
               if ( z > 1.0) spread_b_weight[j][k] += ( 1.0 - z );
               
               size[j] += ( mean_value2_weight[j][k] - mean_value1_weight[j][k]
                       +  ( spread_a_weight[j][k] + spread_b_weight[j][k] ) / 2.0 );
             } 
           }
    }
}


/****************/
hyperbox_trial() {
/****************/
    setup_pattern();
    compute_proto_output();
    compute_hyperboxes_output();
    compute_classification_result(); 
}


/****************/
train(c) char c; {
/****************/
     int     i,j,k,t,old,npat,mis,pointer,index,current_node,dead_node,a,x,y; 
     float   cx,cw,ss,s;     
     char    *str;
   
     if (!System_Defined)
        if (!define_system())
           return(BREAK);

        proto_train = TRUE;


        if (look == 2) {
         disp_weight();
         str=get_command("initial weight");
     }        

         random_seed = rand();
         srand(random_seed);	

         for (i = 0; i <npatterns; i++) used[i] = i;	
         for (i = 0; i <npatterns ; i++) {
                npat = rnd() * (npatterns- i) + i;
                old = used[i];
                used[i] = used[npat];
                used[npat] = old;
         }
         for (i = 0; i < npatterns; i++) tra_pattern[i] = used[i];	
         
/* ------------ iris generalization ability test -------- */
      if (iris == 1) {
         j = 0;
         for (i = 0; i < npatterns; i++) {
             patno = used[i];
             setup_pattern();
             for ( k = 0; k < noutputs; k++ )
             if ( (class == k) && (use_train[k] < t_rate * npatterns) ) {
                tra_pattern[j] = patno;
                use_train[k]++;
                j++;
             }
         }    
      }   
      
/* --- base time --- */
         time4 = 0;
         if ( (tick1 = clock()) == (clock_t) - 1 ) {
            printf("\nclock error *****\n");
            exit(0);
         }
         for (i = 0; i < tra_no; i++) {
             if (Interrupt) {
                Interrupt_flag = 0;
                update_display();
                if (contin_test() == BREAK) return(BREAK);
             }
             patno = tra_pattern[i];
             setup_pattern();
             
/* delay */  if (tra_no < 1000) 
                 for (j = 0; j < 2 * (1000 - tra_no); j++) { j++; j--; }
             
             if (step_size == PATTERN) {
               update_display();
               if (single_flag) {
                 if (contin_test() == BREAK) return(BREAK);
               }
             }
         }   
         if ( (tick2 = clock()) == (clock_t) - 1 ) {
            printf("\nclock 2 error ******\n");
            exit(0);
         }
         time4 = (unsigned) (tick2 - tick1);
/* --- end --- */

  if (no_phase1 == 0) 
   {
    max_prototype_node = noutputs ; /* the initial number of prototype nodes = the number of class */

    do /* The first subpass of DYNPRO */
    {
     nhyperbox=0;
     for (t = 0; t < max_run_len; t++) {
         if (!tallflag) epochno++;
         no_of_misclassification = 0;
         for (i = 0; i < tra_no; i++) {
             npat = rnd() * (tra_no - i) + i;
             old = tra_pattern[i];
             tra_pattern[i] = tra_pattern[npat];
             tra_pattern[npat] = old;
         }
         
         time1 = 0;
         if ( (tick1 = clock()) == (clock_t) - 1 ) {
            printf("\nclock error *****\n");
            exit(0);
         }

         for (i = 0; i < tra_no; i++) {
             if (Interrupt) {
                Interrupt_flag = 0;
                update_display();
                if (contin_test() == BREAK) return(BREAK);
             }
             patno = tra_pattern[i]; 
             
             if (pal == 2) {
                 proto_trial();
                 lvq_proto_change_weights(); /* winner-take-all */
           
           }
             else {
                 setup_pattern();
                 if (pal == 0) pecfuh_proto_change_weights();
                 if (pal == 1) avg_proto_change_weights();
             }
/* delay */  if (tra_no < 1000) 
                 for (j = 0; j < 2 * (1000 - tra_no); j++) { j++; j--; }
            
             if (look == 2) {
                disp_weight();
                str=get_command("look weight");
             }   

             if (step_size == PATTERN) {
               update_display();
               if (single_flag) {
                 if (contin_test() == BREAK) return(BREAK);
               }
             }  
         }
         if ( (tick2 = clock()) == (clock_t) - 1 ) {
            printf("\nclock 2 error ******\n");
            exit(0);
         }
         time1 = (unsigned) (tick2 - tick1) - time4;
         iteration++;   

         if (look) {
            disp_weight();
            if (t == 0) str=get_command("proto_trained 1 epoch");
            else if (t == 1) str=get_command("proto_trained 2 epochs");
            else if (t == 2) str=get_command("proto_trained 3 epochs");
         }   

         if (step_size == EPOCH)  {
            if (single_flag)
               if (contin_test() == BREAK) return(BREAK);
         }
     } /* A training epoch end */
         


         
         printf("\n ***************************************************************************\n");
         printf(" Train pattern  in [%d]th node : \n",max_prototype_node); /* show current number of prototypes */
         
         old_distant=total_distant; /* save the total_distance calculated in the last training epoch */
         total_distant=0.0;

         for(i=0;i<tra_no;i++)  /* calculate the total_distance in the current network */
          {

            patno = tra_pattern[i];
            setup_pattern();
            for (j = ninputs; j < ninputs + max_prototype_node; j++) 
             {
                activation[j] = 0.0;
                ss = 0.0;
                for ( k = 0; k < (ninputs / 4 ) ; k++)
                 {
                  cx = ( activation[k*4] + activation[k*4+1] ) / 2.0 
                     + ( activation[k*4+3] - activation[k*4+2] ) / 4.0;
                  cw = ( mean_value1_weight[j][k] + mean_value2_weight[j][k] ) / 2.0 
                     + ( spread_b_weight[j][k] - spread_a_weight[j][k] ) / 4.0;
                  ss += absolute_dist(cx,cw);  
                 }
                activation[j] = 1.0 - sqrt( ss / (float) k ); 
              }    


             s = 0.0;
             winner = ninputs ;
     
             for (j = winner; j < ninputs + max_prototype_node; j++) 
              {
               if (activation[j] > s) 
                 {
                    s = activation[j]; 
                    winner = j;
                  }   
               }    

            total_distant=total_distant+(1.0-activation[winner]);
            
          }
               
          total_distant=total_distant/tra_no; 


          /* show the total_distnce of current network */                                      
          printf("\n ***************************************************************************\n");
          printf("The total distance in [%d]th node is %f \n", max_prototype_node, total_distant);
          printf("\n ***************************************************************************\n");
          
         /* show the distrbution of the existing ptrototype nodes */
         printf("\n");
         for ( i = ninputs; i < ninputs +max_prototype_node ; i++) {
                printf(" m1[%d]= ",i);
                for ( j = 0; j < ninputs/4; j++)
	        printf("%4.0f",mean_value1_weight[i][j] * 1000);
                printf(" m2[#%d]  ",use_flag[i]);
                for ( j = 0; j < ninputs/4; j++)
	        printf("%4.0f",mean_value2_weight[i][j] * 1000);
                printf(" a       ");
                for ( j = 0; j < ninputs/4; j++)
	        printf("%4.0f",spread_a_weight[i][j] * 1000);
                printf(" b       ");
                for ( j = 0; j < ninputs/4; j++)
	        printf("%4.0f",spread_b_weight[i][j] * 1000);
                printf("\n");
             }

        
         /* if total_distance discreases , save the current weights of the existing prototype nodes */

         if( old_distant > total_distant )
          { 
            for (i = ninputs; i < ninputs+max_prototype_node; i++) old_use_flag[i]=use_flag[i];
            for(i=ninputs;i<ninputs+max_prototype_node;i++)
               for(j=0;j<p_node_index[i];j++)
                  old_p_node[i][j]=p_node[i][j];
              
                  

           for (i = ninputs ; i < ninputs +max_prototype_node ; i++) 
              for (j = 0; j < (ninputs / 4); j++)
               { 
                  old_mean_value1_weight[i][j]=mean_value1_weight[i][j];
                  old_mean_value2_weight[i][j]=mean_value2_weight[i][j];
                  old_spread_a[i][j]= spread_a_weight[i][j];
                  old_spread_b[i][j]= spread_b_weight[i][j];
               }
            max_prototype_node++;
            find_prototype_number=1;
            new1();
            find_prototype_number=0;
         }
            
         printf("\n****** old_distant is %f and total_distant  is  %f***** \n",old_distant,total_distant );

        
       

    } while ( old_distant > total_distant ) ; /* if total_distance not discrease , the fist subpass of DYNPRO end */

     /* restore the weights of the prototypes trained in the last training epoch */

     find_prototype_number=0;
        
     printf("\n ************************* Find prototype node end ! *************************\n");
     max_prototype_node--;
     for (i = ninputs; i < nunits - noutputs; i++) use_flag[i]=0 ;
     for(i=0;i<nunits;i++) activation[i] = 0.0;

     for (i = ninputs; i < ninputs+max_prototype_node; i++) use_flag[i]=old_use_flag[i];
     for (i = ninputs ; i < ninputs + max_prototype_node; i++) 
        for (j = 0; j < (ninputs / 4); j++)
               { 
                  mean_value1_weight[i][j]=old_mean_value1_weight[i][j];
                  mean_value2_weight[i][j]=old_mean_value2_weight[i][j];
                  spread_a_weight[i][j]= old_spread_a[i][j];
                  spread_b_weight[i][j]= old_spread_b[i][j];
               }

      for (j = 0; j < (ninputs / 4); j++)
               { 
                  mean_value1_weight[ninputs+max_prototype_node][j]=0.0;
                  mean_value2_weight[ninputs+max_prototype_node][j]=0.0;
                  spread_a_weight[ninputs+max_prototype_node][j]= 0.0;
                  spread_b_weight[ninputs+max_prototype_node][j]=0.0;
               }
   

     printf("\n");
     for ( i = ninputs; i < ninputs +max_prototype_node+1 ; i++) {
         printf(" m1[%d]= ",i);
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",mean_value1_weight[i][j] * 1000);
       printf(" m2[#%d]  ",use_flag[i]);
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",mean_value2_weight[i][j] * 1000);
       printf(" a       ");
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",spread_a_weight[i][j] * 1000);
       printf(" b       ");
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",spread_b_weight[i][j] * 1000);
      printf("\n");
      }

      printf("\n****************************  Deleting the dead node *********************************\n");


      for (i = ninputs ; i < nunits; i++)
       { 
        for (j = 0; j < (ninputs / 4); j++) 
         {
          old_mean_value1_weight[i][j]=0.0;
	  old_mean_value2_weight[i][j]=0.0;
	  old_spread_a[i][j] = 0.0;
	  old_spread_b[i][j] =0.0;
         }
        old_use_flag[i]=0;
       }
 
      for(i=ninputs;i<ninputs+max_prototype_node;i++)
       {
        for (j = 0; j < (ninputs / 4); j++)
        {
          old_mean_value1_weight[i][j]=mean_value1_weight[i][j];
	  old_mean_value2_weight[i][j]=mean_value2_weight[i][j];
	  old_spread_a[i][j] =spread_a_weight[i][j];
	  old_spread_b[i][j] =spread_b_weight[i][j];
        }
        old_use_flag[i]=use_flag[i];
       }

      index=ninputs;
      for(i=ninputs;i<ninputs+max_prototype_node;i++)
         if ( old_use_flag[i] > 1 ) 
            {
              for(j=0;j<ninputs/4;j++)
               {
                 mean_value1_weight[index][j]=old_mean_value1_weight[i][j];
                 mean_value2_weight[index][j]=old_mean_value2_weight[i][j];
                 spread_a_weight[index][j]=old_spread_a[i][j];
                 spread_b_weight[index][j]=old_spread_b[i][j];
               }
              use_flag[index]=old_use_flag[i];
             
              index++; 
            }



     max_prototype_node=index-ninputs;

     
     printf("\n");
     for ( i = ninputs; i < ninputs +max_prototype_node ; i++) {
         printf(" m1[%d]= ",i);
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",mean_value1_weight[i][j] * 1000);
       printf(" m2[#%d]  ",use_flag[i]);
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",mean_value2_weight[i][j] * 1000);
       printf(" a       ");
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",spread_a_weight[i][j] * 1000);
       printf(" b       ");
       for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",spread_b_weight[i][j] * 1000);
      printf("\n");



   }

     printf("\n ********************************  Crossberg layer  ********************************* \n");

     pronode_label();
   

    printf("\n******************************* test pattern in phase 1 ************************\n"); 
    for (i = 0; i < tra_no; i++) {
	    patno = tra_pattern[i];
            printf(" %d",patno); 
      }
     printf(" Now there are %d prototype node \n",max_prototype_node);
     test_flag=1;
     nhyperbox = 0;
     printf("\n");
      no_of_misclassification = 0;
       if ( t_rate < 1.0 ) {
	no_of_misclassification = 0;
	for (i = 0; i < tra_no; i++) {
	    patno = tra_pattern[i]; 
	    proto_trial();
	}
	disp_weight();
	str=get_command("training set error in phase 1  .....\n");   


 
     }


    /* printf("\n**************************** The members of Each prototype node *****************************\n");
        
    for(i=ninputs;i<ninputs+max_prototype_node;i++)
           for(j=0;j<use_flag[i];j++)
                  p_node[i][j]=old_p_node[i][j]; 
              
 

    for(i=ninputs;i<ninputs+max_prototype_node;i++)
     {
        printf("\n P_node [%d] have [%d]:\n",i,use_flag[i]);
        for(j=0;j<use_flag[i];j++)
          printf("%d ",p_node[i][j]);
        printf("\n");
     }      */
     
                                 
     str=get_command("\n *************  Start exemplar training *************** \n");


    
  }
     test_flag=0; 
     nhyperbox = 0;
     time2 = time3 = 0; 
     for (i = 0; i < nhiddens - max_prototype_node; i++) enode_win[i] = 0;

     for (t = 0; t < pass; t++) {
         
         random_seed = rand();
         srand(random_seed);	

         for (i = 0; i < tra_no; i++) {
           npat = rnd() * (tra_no - i) + i;
           old = tra_pattern[i];
           tra_pattern[i] = tra_pattern[npat];
           tra_pattern[npat] = old;
         }
         no_of_misclassification = 0;  

         if ( (tick1 = clock()) == (clock_t) - 1 ) {
            printf("\nclock error *****\n");
            exit(0);
         }

         for (i = 0; i < tra_no; i++) {
             if (Interrupt) {
                Interrupt_flag = 0;
                update_display();
                if (contin_test() == BREAK) return(BREAK);
             }
             patno = tra_pattern[i]; 

             hyperbox_trial();


/* delay */  if (tra_no < 1000) 
                 for (j = 0; j < 2 * (1000 - tra_no); j++) { j++; j--; }
           

             /* when input pattern falls outside an exemplar node and is classified correctly by this exemplar */

             if ( (!mis_flag) && (ninputs+max_prototype_node <= winner) && ( activation[winner] <= 1.0) )
                 hyperbox_change_weight_1();
               
             /* when input pattern falls outside an exemplar node but is classified uncorrectly by this exemplar */

             else if ( (mis_flag) && (ninputs+max_prototype_node <= winner) && ( activation[winner] <= 1.0) )
                 hyperbox_change_weight_2();
                
  

             else if ( mis_flag ) {
               
                hyperbox_change_weight();
                if (look == 1) {
                   disp_weight();
                   str=get_command("box_training......");
                }   
             }     

             if (step_size == PATTERN) {
               update_display();
               if (single_flag) {
                 if (contin_test() == BREAK) return(BREAK);
               }
             }  
         }
         if ( (tick2=clock()) == (clock_t) - 1 ) {
            printf("\nclock 2 error *****\n");
            exit(0);
         }
         if (t == 0) time2 = (unsigned) (tick2 - tick1) - time4;
    if (t == 1) time3 = (unsigned) (tick2 - tick1) - time4 + time1 + time2;

         if (look == 1) {
/*            no_of_misclassification = 0;
            test_all_patterns('s');
*/            if (t == 0) str=get_command("box_trained 1 epoch");
            else if (t == 1) str=get_command("box_trained 2 epochs");
            else if (t == 2) str=get_command("box_trained 3 epochs");
         }
     }

     if ( t_rate < 1.0 ) {
        no_of_misclassification = 0;
        for (i = 0; i < tra_no; i++) {
            patno = tra_pattern[i]; 
            hyperbox_trial();
        }
        disp_weight();
        str=get_command("training set error .....");    
     }
     tall();
     return(CONTINUE);
}




/*****************/
pronode_label() {
/*****************/
   int i,npat,old,j,max; 
   float temp;

   for (i = 0; i < tra_no; i++) {
	     npat = rnd() * (tra_no - i) + i;
	     old = tra_pattern[i];
	     tra_pattern[i] = tra_pattern[npat];
	     tra_pattern[npat] = old;
	 }

   printf(" In Grossberg layer have :  ");
   for( i=0 ; i<tra_no;i++) printf(" %d ",tra_pattern[i]);
   printf("\n");
   
   for( i=0 ; i< tra_no;i++)
    {
     
      patno=tra_pattern[i];
      proto_trial();
      for(j=0;j<noutputs;j++)
       {
          temp=pnode_weight[winner-ninputs][j];
          if ( class == j )  pnode_weight[winner-ninputs][j]+=0.25*(1.0-temp);
          else pnode_weight[winner-ninputs][j]+=0.25*(0.0-temp); 
       }
    }

  printf(" \n *********************************************************************** \n");
     for(i=0;i<max_prototype_node;i++)
   {
     
     for(j=0;j<noutputs;j++) printf("p_weight[%d][%d] is %f   ",i,j,pnode_weight[i][j]);
     printf("\n");

         max=0;
     for(j=0;j<noutputs;j++)
      if ( pnode_weight[i][j] > pnode_weight[i][max] ) max=j;
     class_flag[i+ninputs]=max ;
   }

   printf(" \n *********************************************************************** \n");

   for(i=0;i<max_prototype_node;i++)  printf("\n ** P_node[%d] belong to [%d] class  ** \n",i+ninputs,class_flag[i+ninputs]); 

   printf(" \n *********************************************************************** \n");

}





/****************/
leaving_one_out_train(c,out_no) char c; int out_no; {
/****************/
}

/**************************/
test_all_patterns(c) char c; {
/**************************/
    int   j,k,t,i,old,npat;
    
    for (t = 0; t < 1; t++) {
        if (!tallflag) epochno++;

        for (i = 0; i < npatterns; i++) used[i] = i;
        if (c == 'p') {
          for (i = 0; i < npatterns; i++) {
            npat = rnd() * (npatterns - i) + i;
            old = used[i];
            used[i] = used[npat];
            used[npat] = old;
          }
        }

        if (look) {
           disp_weight();
           str=get_command("testing .....");
        }  
         
        for (i= ninputs; i < ninputs + nhiddens; i++) use_flag[i] = 0;        
        for (i = 0; i < npatterns; i++) {
            if (Interrupt) {
               Interrupt_flag = 0;
               update_display();
               if (contin_test() == BREAK) return(BREAK);
            }
            patno = used[i];
             
            hyperbox_trial();
            use_flag[winner]++;

            j = 0;
            for ( k = ninputs + noutputs; k < ninputs + nhiddens; k++ ) 
                if ( activation[k] > 0.0 ) j++;

            if ( (look) && ((class_flag[winner] != class) || ( j > 2 )) ) {
               disp_weight();
               str=get_command("error or nested");
            }   

            if (step_size == PATTERN) {
               update_display();
               if (single_flag) {
                 if (contin_test() == BREAK) return(BREAK);
               }
            }
        }
        if (step_size == EPOCH)  {
          update_display();
          if (single_flag)  {
                 if (contin_test() == BREAK) return(BREAK);
          }
        }
    }
    disp_weight();
}


/********/
tall() {
/********/
  int i,save_lflag;
  int save_single_flag;
  int save_nepochs;
  int save_step_size;

  save_lflag = lflag;  lflag = 0;
  save_single_flag = single_flag;
/*  if (in_stream == stdin) single_flag = 1;  */
  single_flag = 0;
  save_nepochs = nepochs;  nepochs = 1;
  save_step_size = step_size;
  if (step_size > PATTERN) step_size = PATTERN;
  tallflag = 1;
  no_of_misclassification = 0;
  proto_train = FALSE;
  test_all_patterns('s');
  /*train('s');*/
  
  proto_train = TRUE;
  tallflag = 0;
  lflag = save_lflag;
  nepochs = save_nepochs;
  single_flag = save_single_flag;
  step_size = save_step_size;
  return(CONTINUE);
}

/****************/
test_pattern() {
/****************/
    char   *str;
    int    j,k,t;
    
    if (!System_Defined)
        if (!define_system())
            return(BREAK);

    str = get_command("Test which pattern? ");
    if (str == NULL) return(CONTINUE);
    if ((patno = get_pattern_number(str)) < 0 ) {
        return(put_error("Invalid pattern specification."));
    }
    
    hyperbox_trial();
    disp_weight();
    
    return(CONTINUE);
}

/****************/
leaving_one_out_test()  {
/****************/
        return(CONTINUE);
}

/****************/
new() {
/****************/
   new1();
   disp_weight();
}

/***********/
new1() {
/***********/
   int i,j;
   
   nhyperbox = 0;
   iteration = 0;
   epochno = 0;
   winner = 0;
   lrate = 1.0;
   no_of_misclassification = 0;
   time1 = time2 = time3 = time4 = 0;
   
   tra_no = (int) ( (float) (npatterns) * t_rate);   
 
   for (i = ninputs; i < nunits - noutputs; i++) {

       if ( find_prototype_number == 0) old_use_flag[i]=0;

       use_flag[i] = 0;
       class_flag[i] = noutputs; 
   }
   for (j = 0; j < nunits; j++) size[j] = 0.0;
   for (j = 0; j < nunits; j++) activation[j] = 0.0;

   if( find_prototype_number==0 ) for (i = 0; i < npatterns; i++) tra_pattern[i] = i;

   for (i = 0; i < npatterns; i++) err_pattern[i] = npatterns;
   for (i = 0; i < nhiddens - max_prototype_node; i++) enode_win[i] = 0;
   for (i = 0; i < noutputs; i++) use_train[i] = 0;

    for (j = 0; j < nunits; j++) p_node_index[j] = 0;


   reset_weights();
} 


/****************/
reset_weights() {
/****************/

    int i,j;

   
     for (i=0 ; i< nhiddens ; i++)
      {
         for(j=0; j< noutputs ;j++)
         pnode_weight[i][j]=  0.5;
      }
 
    if ( (pal == 2) && ( find_prototype_number == 0) )
      {
       for (i = ninputs ; i < ninputs + max_prototype_node; i++) 
           for (j = 0; j < (ninputs / 4); j++) 
             {
               random_seed =rand();
               srand(random_seed);

	      mean_value1_weight[i][j] = mean_value2_weight[i][j] = rnd()/10.0;

              mean_value1_weight[i][j]+=0.45;
	      mean_value2_weight[i][j]+=0.45;
	      spread_a_weight[i][j] =0;
	      spread_b_weight[i][j] =0;
             }

          printf("\n ****************** initaial for prototype ************************\n");
          for ( i = ninputs; i < ninputs +max_prototype_node ; i++) {
           printf(" m1[%d]= ",i);
           for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",mean_value1_weight[i][j]*1000);
           printf(" m2[#%d]  ",use_flag[i]);
           for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",mean_value2_weight[i][j]*1000);
           printf(" a       ");
           for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",spread_a_weight[i][j]*1000 );
           printf(" b       ");
           for ( j = 0; j < ninputs/4; j++)
	   printf("%4.0f",spread_b_weight[i][j]*1000 );
           printf("\n");
       
         }



      }
    else if ( (pal == 2) && ( find_prototype_number == 1) )
          for (j = 0; j < (ninputs / 4); j++) 
             {
	      mean_value1_weight[ninputs+max_prototype_node-1][j] = mean_value2_weight[ninputs+max_prototype_node-1][j] = rnd()/10.0;

              mean_value1_weight[ninputs+max_prototype_node-1][j]+=0.45;
	      mean_value2_weight[ninputs+max_prototype_node-1][j]+=0.45;
	      spread_a_weight[ninputs+max_prototype_node-1][j] = 0.0;
	      spread_b_weight[ninputs+max_prototype_node-1][j] = 0.0 ;
             }



        for (i = ninputs+max_prototype_node ; i < nunits-max_prototype_node; i++) 
           for (j = 0; j < (ninputs / 4); j++) 
             {
              mean_value1_weight[i][j]=0.0;
	      mean_value2_weight[i][j]=0.0;
	      spread_a_weight[i][j] = 0.0;
	      spread_b_weight[i][j] =0.0;
             }


}


/****************/
get_weights() {
/****************/

   int i,j,old,npat;

   FILE  *fp;
   char  fstr[80];
   
   if (! System_Defined)
      if (! define_system())
         return(BREAK);
   
   new();      
   strcpy(fstr,"../ok/");
   str = get_command(" filename (get): ");                
   if (str == NULL) return(CONTINUE);
   strcat(fstr,str);

   if ((fp = fopen(fstr,"r")) == NULL ) {
      return(put_error("Cannot open file"));
   }

   nhyperbox = 0;
   for ( j = ninputs; j < nunits - noutputs; j++ ) { 
     size[j] = 0.0;
     for ( i = 0; i < ninputs / 4; i++) {
       (void) fscanf(fp,"  %d  %f  %f  %f  %f \n",
                           &class_flag[j],
                           &mean_value1_weight[j][i],
                           &mean_value2_weight[j][i],
                           &spread_a_weight[j][i],
                           &spread_b_weight[j][i]);
       if (class_flag[j] != noutputs) nhyperbox++;
       size[j] += ( mean_value2_weight[j][i] - mean_value1_weight[j][i]
               +  ( spread_a_weight[j][i] + spread_b_weight[j][i] ) / 2.0 );
     }
   }  
   fclose(fp);   
   nhyperbox /= i; 
   nhyperbox -= noutputs;  

   test_all_patterns('p');
   return(CONTINUE);                                            
}


/****************/
save_weights() {
/****************/

   int i,j;

   FILE  *fp;
   char  fstr[80];
   
   if (! System_Defined)
      if (! define_system())
         return(BREAK);
         
   strcpy(fstr,"../ok/");
   str = get_command(" filename(save): ");                
   if (str == NULL) return(CONTINUE);
   strcat(fstr,str);
   strcpy(str,"");      
   if ((fp = fopen(fstr,"w")) == NULL ) {
      return(put_error("Cannot open file"));
   }

   for ( j = ninputs; j < nunits - noutputs; j++ ) { 
       for ( i = 0; i < ninputs / 4; i++) 
           fprintf(fp,"  %3d  %6.3f  %6.3f  %6.3f  %6.3f  \n",
                                class_flag[j], 
                                mean_value1_weight[j][i],
                                mean_value2_weight[j][i],
                                spread_a_weight[j][i],            
                                spread_b_weight[j][i]);
       fprintf(fp,"\n");
   }                             

   fclose(fp);
   return(CONTINUE);                                            
}


/*************/
init_system() {
/*************/
    int     strain(), ptrain(), tall(), test_pattern(),reset_weights();
    int     get_unames();

    install_command("leaving_one_out_test", leaving_one_out_test, BASEMENU,(int *) NULL);
    install_command("strain", strain, BASEMENU,(int *) NULL);
    install_command("ptrain", ptrain, BASEMENU,(int *) NULL);
    install_command("tall", tall, BASEMENU,(int *) NULL);
    install_command("test", test_pattern, BASEMENU,(int *) NULL);
    install_command("new",new,BASEMENU,(int *)NULL);
    install_command("reset",reset_weights,BASEMENU,(int *)NULL);
    install_command("w", get_weights, GETMENU,(int *) NULL);
    install_command("w", save_weights, SAVEMENU,(int *) NULL);
    install_command("patterns", get_pattern_pairs, GETMENU,(int *) NULL);
    install_command("unames", get_unames, GETMENU,(int *) NULL);
    install_var("noutputs", Int,(int *) & noutputs, 0, 0, SETCONFMENU);
    install_var("ninputs", Int,(int *) & ninputs, 0, 0, SETCONFMENU);
    install_var("nunits", Int,(int *) & nunits, 0, 0, SETCONFMENU);
    install_var("nhiddens", Int,(int *) & nhiddens, 0, 0, SETCONFMENU);
    install_var("iris", Int,(int *) & iris, 0, 0, SETPARAMMENU);
    install_var("pal", Int,(int *) & pal, 0, 0, SETPARAMMENU);
    install_var("no_phase1", Int,(int *) & no_phase1, 0, 0, SETPARAMMENU);    
    install_var("alpha", Float,(int *) & alpha, 0, 0, SETPARAMMENU);
    install_var("lrate", Float,(int *) & lrate, 0, 0, SETPARAMMENU);
    install_var("max_size", Float,(int *) & max_size, 0, 0, SETPARAMMENU);
    install_var("t_rate", Float,(int *) & t_rate, 0, 0, SETPARAMMENU);    
    install_var("pass", Int,(int *) & pass, 0, 0, SETPARAMMENU);
    install_var("holap", Int,(int *) & holap, 0, 0, SETPARAMMENU);    
    install_var("max_run_len", Int,(int *) & max_run_len, 0, 0, SETPARAMMENU);
    install_var("look", Int,(int *) & look, 0, 0, SETPARAMMENU);    
    install_var("lflag", Int,(int *) & lflag, 0, 0, SETPCMENU);
    install_var("nepochs", Int,(int *) & nepochs, 0, 0, SETPCMENU);
    install_var("epochno", Int,(int *) & epochno, 0, 0, SETSVMENU);
    install_var("patno", Int,(int *) & patno, 0, 0, SETSVMENU);
    install_var("npatterns", Int,(int *) & npatterns, 0, 0,SETENVMENU);
    install_var("no_of_misclassification", Int,(int *) & no_of_misclassification, 0, 0, NOMENU);
    install_var("patno", Int,(int *) & patno, 0, 0, NOMENU);
    install_var("count", Int,(int *) & count, 0, 0, NOMENU);
    install_var("nhyperbox", Int,(int *) & nhyperbox, 0, 0, NOMENU);
    install_var("winner_no", Int,(int *) & winner_no, 0, 0, NOMENU);
    install_var("iteration", Int,(int *) & iteration, 0, 0, NOMENU);

    init_pattern_pairs();
}

