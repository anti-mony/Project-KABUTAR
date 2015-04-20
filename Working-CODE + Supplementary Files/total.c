#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#define MAX_NUM_USERS 131
#define MAX_NUM_CONNECTIONS (MAX_NUM_USERS*(MAX_NUM_USERS-1))
#define MAX_QUEUE_SIZE 100
#define DEBUG 0

typedef struct tweet
{
	char hash_tag[30];
	char *tweet_array[140];
	char tweet[140];
	struct tweet *next;
}tweet_t;


/*structure for hash table*/
typedef struct user
{
	char user_name[10];
	char full_name[35];
	char password[10];
	char answer_1[15];
	char answer_2[15];
	struct user *next;
  int vertex;
}user_t;

typedef struct user_connect
{
 int user1;
 int user2;
}connect;

typedef struct frame_work
{
   /* Number of users */
   int num_users;

   /* Adjacency lists*/
   user_t *adj_list[MAX_NUM_USERS];

   /* Vertex List */
   user_t list_of_users[MAX_NUM_USERS];

   /* Edge List */
   int num_of_connections;
   connect  connections[MAX_NUM_CONNECTIONS];
}fwrk;


#define NUM_BUCKETS 53
user_t *hash_tbl[NUM_BUCKETS];
user_t *hash_tbl_fulln[NUM_BUCKETS];
tweet_t *hash_tbl_tweet[NUM_BUCKETS];

int vertexReturn(fwrk *G,char *user_name);
int sign_up(char user_name[],char password[],char full_name[],char answer_1[],char answer_2[]);
int log_in(char user_name[],char password[],char full_name[],char answer_1[],char answer_2[]);
void hash_tbl_init(user_t *hash_tbl[NUM_BUCKETS]);
void print_hash_tbl(user_t *hash_tbl[NUM_BUCKETS]);
int search_hash_tbl(user_t *hash_tbl[NUM_BUCKETS], char user_name[],char full_name[], char password[], char answer_1[], char answer_2[]);
int insert_hash_tbl(user_t *hash_tbl[NUM_BUCKETS],char user_name[],char password[], char full_name[], char answer_1[], char answer_2[]);
int insert_hash_tbl_fulln(user_t *hash_tbl_fulln[NUM_BUCKETS],char user_name[],char full_name[]);
int graph_add_edge(fwrk *G, int vertex1, int vertex2 ,char *user , char *follow);
int insert_hash_tbl_tweet(tweet_t *hash_tbl_tweet[NUM_BUCKETS],char hash_tag[],char *tweet_array[],char tweet[],int counter);

int noOfWords=0;

/*function to create new user */
int sign_up(char user_name[],char password[],char full_name[],char answer_1[],char answer_2[])
{
	FILE *fp;
	int ret;

	fp=fopen("login.txt","a");

	if(fp == NULL)
	{
		printf("Error opening file! \n");
		exit(1);
	}

	printf("username (10 characters): \n");
	scanf("%s",user_name);

	printf("password (10 character): \n");
	scanf("%s",password);

	printf("first_name (35 characters): \n");
	scanf("%s",full_name);

	printf("(Q1)Your favourite music band? \n");
	scanf("%s",answer_1);

	printf("(Q2)Your nickname? \n");
	scanf("%s",answer_2);

	ret = search_hash_tbl(hash_tbl, user_name, password, full_name, answer_1, answer_2);
         if (ret == 1)
         {
           printf("Username alredy exists");
           return 0;
         }
         if (ret == 0)
         {
         	fprintf(fp,"%s \t %s \t %s \t %s \t %s \t \n ",user_name,password,full_name,answer_1,answer_2);
         	insert_hash_tbl(hash_tbl,user_name,password,full_name,answer_1,answer_2);
					return 1;
	 				}

	fclose(fp);
	return 0;
}

int log_in(char user_name[],char password[],char full_name[],char answer_1[],char answer_2[])
{

	char choice;
	char answer1[15];
	char answer2[15];
	char pass[11];

	printf("username (10 characters): \n");
	scanf("%s",user_name);

	printf("password (10 character): \n");
	scanf("%s",pass);

	 			if(search_hash_tbl(hash_tbl,user_name,full_name,pass,answer1, answer2)==1)
        	return 1;
		    else
         {
         	printf("username or password not correct\n");
         	printf("Forgot password? (y/n)");
         	scanf("%s",&choice);
         	if (choice == 'y' || choice == 'Y')
         	{
         		printf("(Q1)Your favourite music band? \n");
						scanf("%s",answer_1);

						printf(" The real :%s \n",answer1);

						printf("(Q2)Your nickname? \n");
						scanf("%s",answer_2);

						printf(" The real :%s \n",answer2);

						if(strcmp(answer_1,answer1) == 0 && strcmp(answer_2,answer2) == 0)
							return 1;
					}
					else
						return 0;
   	  	}
		return 0;
}

void add_hash_tbl(user_t *hash_tbl[NUM_BUCKETS],char user_name[], char password[], char full_name[], char answer_1[], char answer_2[])
{
	hash_tbl_init(hash_tbl);
	FILE *fp;

	fp=fopen("login.txt","r");
	if(fp == NULL)
	{
		printf("Error opening file! \n");
		exit(1);
	}

	while (!feof(fp))
   	{
      		fscanf(fp,"%s \t %s \t %s \t %s \t %s \n ",user_name,password,full_name,answer_1,answer_2);
					insert_hash_tbl(hash_tbl,user_name,password,full_name,answer_1,answer_2);
   	}
   	fclose(fp);
	//print_hash_tbl(hash_tbl);
}

int hash_func(char *user_name)
{
  int i=0,sum=0;
  while(user_name[i] != '\0')
  {
  	if(i%2==0)
			sum = sum  + 2*(int)user_name[i];
  	else
			sum = sum + i*(int)user_name[i];

		i++;
	}
  return (sum % NUM_BUCKETS) ;
}

void hash_tbl_init(user_t *hash_tbl[NUM_BUCKETS])
{
   int i;
   for(i=0;i<NUM_BUCKETS;i++)
     hash_tbl[i] = NULL;
}
void hash_tbl_init_tweet(tweet_t *hash_tbl_tweet[NUM_BUCKETS])
{
   int i;
   for(i=0;i<NUM_BUCKETS;i++)
     hash_tbl_tweet[i] = NULL;
}


/*function to insert data to the hash table*/
int insert_hash_tbl(user_t *hash_tbl[NUM_BUCKETS],char user_name[],char password[], char full_name[], char answer_1[], char answer_2[])
{
   int index;
   user_t *p_rec;

   index = hash_func(user_name);
  // printf("insert_hash_tbl : index = %d\n", index);

   /* Add at begining of linked list : O(1) */
   p_rec = (user_t *) malloc(sizeof(user_t));

   strcpy(p_rec->user_name, user_name);
   strcpy(p_rec->password,password);
   strcpy(p_rec->full_name, full_name);
   strcpy(p_rec->answer_1, answer_1);
   strcpy(p_rec->answer_2, answer_2);

   /* Set pointers, so as to en-chain the new node at the beginning */
   p_rec->next = hash_tbl[index];
   hash_tbl[index] = p_rec;

   return 1;
}

int search_hash_tbl(user_t *hash_tbl[NUM_BUCKETS], char user_name[],char full_name[], char password[], char answer_1[], char answer_2[])
{
   int index;
   user_t *p_rec;

   /* Generate Index */
   index = hash_func(user_name);
   //printf("search_hash_tbl : index = %d\n", index);

   p_rec = hash_tbl[index];

   while (p_rec!= NULL)
   {

      if (strcmp(p_rec->user_name, user_name) == 0)
      {
				//printf("Too Debug : \n Password in p_rec:  %s \n Password passed %s  \n ",p_rec->password,password);
				strcpy(full_name, p_rec->full_name);
				strcpy(answer_1, p_rec->answer_1);
				strcpy(answer_2, p_rec->answer_2);
					if(strcmp(p_rec->password,password)==0)
					{
            return 1;
			    }
      }
			p_rec = p_rec->next;
   }
	return 0;
	}

int search_hash_tbl_fulln(user_t *hash_tbl_fulln[NUM_BUCKETS], char user_name[],char full_name[])
{
   int index;
   user_t *p_rec;

   /* Generate Index */
   index = hash_func(full_name);
   //printf("search_hash_tbl_n : index = %d\n", index);

   p_rec = hash_tbl_fulln[index];

   while (p_rec != NULL)
   {
      if (strcmp(p_rec->full_name,full_name)==0)
      {
        /* Copy Data */
        strcpy(user_name, p_rec->user_name);

        return 1; // SUCCESS. Found it
      }
      // Move to next node
      p_rec = p_rec->next;
   }

   return 0; // FAILURE. Did not find it
}

void add_hash_tbl_fulln(user_t *hash_tbl_fulln[NUM_BUCKETS],char user_name[], char full_name[])
{
	hash_tbl_init(hash_tbl_fulln);

	FILE *fp;
	char password[10];
	char answer_1[15];
	char answer_2[15];

	fp=fopen("login.txt","r");
	if(fp == NULL)
	{
		printf("Error opening file! \n");
		exit(1);
	}

	while (!feof(fp))
   	{
    		fscanf(fp,"%s \t %s \t %s \t %s \t %s \t \n ",user_name,password,full_name,answer_1,answer_2);
    		insert_hash_tbl_fulln(hash_tbl_fulln,user_name,full_name);
   	}
   	fclose(fp);

}

int insert_hash_tbl_fulln(user_t *hash_tbl_fulln[NUM_BUCKETS],char user_name[],char full_name[])
{
   int index;
   user_t *p_rec;

   /* Generate Index */
   index = hash_func(full_name);
   //printf("insert_hash_tbl : index = %d\n", index);

   /* Add at begining of linked list : O(1) */
   p_rec = (user_t *) malloc(sizeof(user_t));

      /* Copy Data */
   strcpy(p_rec->user_name, user_name);
   strcpy(p_rec->full_name, full_name);

   /* Set pointers, so as to en-chain the new node at the beginning */
   p_rec->next = hash_tbl_fulln[index];
   hash_tbl_fulln[index] = p_rec;

   return 1;
}

void read_tweet_file(tweet_t *hash_tbl_tweet[NUM_BUCKETS],char hash_tag[],char *tweet_array[20],char tweet[20])
{
	hash_tbl_init_tweet(hash_tbl_tweet);

	FILE *fp;
	int i=0,j=0;

	fp=fopen("main","r");
	if(fp == NULL)
	{
		printf("Error opening file! \n");
		exit(1);
	}

	while (!feof(fp))
   	{
      		fscanf(fp,"%s",tweet);

        	tweet_array[i]=malloc(sizeof(char)  * (strlen(tweet)+1));
        	strcpy(tweet_array[i],tweet);
        	i++;
        	if(strcmp(tweet,".")==0)
		{
			for(j=0;j<i;j++)
			{
				if(tweet_array[j][0]=='#')
				{
					insert_hash_tbl_tweet(hash_tbl_tweet,tweet_array[j],tweet_array,tweet,i);
				}
			}
			i=0;
		}
	}
   	fclose(fp);
}

int insert_hash_tbl_tweet(tweet_t *hash_tbl_tweet[NUM_BUCKETS],char hash_tag[],char *tweet_array[],char tweet[],int counter)
{
   int index,i,j;
   tweet_t *p_rec;


   /* Generate Index */
   index = hash_func(hash_tag);
   //printf("insert_hash_tbl : index = %d\n", index);

   /* Add at begining of linked list : O(1) */
   p_rec = (tweet_t *) malloc(sizeof(tweet_t));
   j=0;

   for(i=0;i<counter;i++)
   {

	p_rec->tweet_array[j]=malloc(sizeof(char)  * (strlen(tweet_array[i])+1));
	strcpy(p_rec->tweet_array[j],tweet_array[i]);
	j++;
   }
   /* Set pointers, so as to en-chain the new node at the beginning */
   p_rec->next = hash_tbl_tweet[index];
   hash_tbl_tweet[index] = p_rec;

   return 1;
}


int search_hash_tbl_tweet(tweet_t *hash_tbl_tweet[NUM_BUCKETS],char hash_tag[])
{
   int index;
   int i=1;
   tweet_t *p_rec;

   /* Generate Index */
   index = hash_func(hash_tag);
   //printf("search_hash_tbl : index = %d\n", index);

   p_rec = hash_tbl_tweet[index];

   while (p_rec != NULL)
   {

        	while(strcmp(p_rec->tweet_array[i],".")!=0)
        	{
        		printf("%s \t",p_rec->tweet_array[i]);
        		i++;
        	}
       printf(" \n ");
       // Move to next node
       p_rec = p_rec->next;
       i=1;
   }

   return 0; // FAILURE. Did not find it
}

/* Function graph_init: Initialize graph to N vertices and 0 edges*/
void graph_init(fwrk *G, int num_vertices, char *vertex_names[])
{
  int vertex, i;

  /* Vertices */
  G->num_users = num_vertices;

  for(i=0;i<num_vertices;i++)
   strcpy((G->list_of_users[i]).user_name,vertex_names[i]);

  // Adjacency lists: Set to NULL
  for(vertex=0;vertex<num_vertices;vertex++)
      G->adj_list[vertex]  = NULL;
}

int createNetwork(fwrk *g1)
{

  char *strs[100];
  char temp_str[100];
  char temp_str1[100];
  char temp_str2[100];
  char temp_str3[100];
  char temp_str4[100];

    FILE *fp;
    fp = fopen ("login.txt", "r");
    int loopVar1=0;

   do
   {
   	fscanf(fp,"%s %s %s %s %s",temp_str,temp_str1,temp_str2,temp_str3,temp_str4);
     	strs[loopVar1]=malloc(sizeof(char) * strlen(temp_str+1));
			strcpy(strs[loopVar1],temp_str);
      loopVar1++;
      noOfWords++;

   }while(!feof(fp));
   fclose(fp);
   graph_init(g1,noOfWords-1, strs); /*FUNCTION TO INIALISE THE GRAPH*/
	return 0;
}

int connectNetwork(fwrk *G)
{
	FILE *fptr;
	char user_name[12];
	char follows[12];
	char x[12];

	fptr = fopen("main","r");
	int cmp;
	while(!feof(fptr))
	{
		fscanf(fptr,"%s %s %s",user_name,follows,x);
			cmp = strcmp(user_name,"TWEET");
			if(cmp !=  0)
			{
				if(DEBUG == 1)
				{
					printf("Username --%s-- \n",user_name);
					printf("Follows --%s-- \n",follows);
					printf("Username Index --%d-- \n",vertexReturn(G,user_name));
					printf("Follows Index --%d-- \n",vertexReturn(G,follows));
				}
				graph_add_edge(G,vertexReturn(G,user_name),vertexReturn(G,follows),user_name,follows);
			}
	}
	return 0;
}

user_t *linked_list_insert(user_t *head, int vertex ,char *str)
{
  user_t *newnode;
  newnode = malloc(sizeof(user_t));

  newnode->next = head;
  newnode->vertex = vertex;
	strcpy(newnode->user_name,str);

  return newnode;
}

int graph_add_edge(fwrk *G, int vertex1, int vertex2 ,char *user , char *follow)
{
  int index;
  connect *p_edge;

  // add edge
  G->adj_list[vertex1] = linked_list_insert(G->adj_list[vertex1], vertex2 ,follow);
  G->adj_list[vertex2] = linked_list_insert(G->adj_list[vertex2], vertex1,user);

  // store edge related information
  index = G->num_of_connections;
  p_edge = &G->connections[index];
  p_edge->user1 = vertex1;
  p_edge->user2 = vertex2;

  G->num_of_connections++;

  return 1;
}

int vertexReturn(fwrk *G,char *user_name)
{
  int i;
  for(i=0;i<MAX_NUM_USERS;i++)
  {
      if(strcmp(G->list_of_users[i].user_name,user_name)==0)
      return i ;
  }
	return -1;
}

int tweet(char *user_name)
{
	char choice;
  char stamp[6];
  time_t timer;
  strftime(stamp,6,"%H:%M",localtime(&timer));
  time(&timer);
  //stamp[6]=':';
  char tweet[140];
  do
	{
		printf("Xpress yourself\n Hey there, please end your tweet with a fullstop \n");
	  scanf(" %[^\n]",tweet);
  	printf("This is the tweet you entered : %s\n",tweet);
		printf("To continue press Y ,To Re-enter press N \n");
		scanf(" %c",&choice);
	}
	while(choice == 'N' || choice == 'n');

  FILE *fptr;
  fptr = fopen("main","a");
  fprintf(fptr,"%s\t%s\t%s\t%s\t \n","TWEET",user_name,stamp,tweet);

  return 0;
}

int timeline(fwrk *G,char *user_name)
{
  FILE *fp;
  fp=fopen("main","r");
  char wordStore[40];
  int chckForTweet;
  char *sepLine[40];
  int index;
  int inLoopVar;
  int inLoopVar2;
  int noOfFriends=0;
  int i=0;
	int j=0;
  index = vertexReturn(G,user_name);
  char *friends[11];
  user_t *user;
  user = G->adj_list[index];
  //int fIndex;
    while(user!=NULL)
    {
      //fIndex = user->vertex ;
      friends[i]=malloc(sizeof(char)*11);
      strcpy(friends[i],user->user_name);
      i++;
      noOfFriends++;
			user = user->next;
    }
		if(DEBUG==1)
		{
			printf("%s \n",friends[0]);
			printf("%s \n",friends[1]);

		}
		j=0;
 	while(!feof(fp))
  {

    fscanf(fp,"%s",wordStore);
    sepLine[j] = malloc(sizeof(char)  * (strlen(wordStore)+1)) ;
    strcpy(sepLine[j],wordStore);
    j++;

		if(DEBUG == 31 )
		{
			printf(" %s \n",sepLine[j-1]);
		}
		if(DEBUG == 2)
		{
			printf("From File : %s \n",sepLine[0]);
		}
		//printf("word current %s \n",wordStore);
    chckForTweet = strcmp(wordStore,".");
    if(chckForTweet == 0)
		{
				//printf("word 1 %s \n",sepLine[1]);
				if(strcmp(sepLine[0],"TWEET")==0)
        {
					for(inLoopVar=0;inLoopVar<noOfFriends;inLoopVar++)
          {
						if(strcmp(sepLine[1],friends[inLoopVar])==0 || strcmp(sepLine[1],user_name)==0)
            {
              for(inLoopVar2=1;inLoopVar2<j;inLoopVar2++)
              {
                printf("%s ",sepLine[inLoopVar2]);
              }
              printf("\n");
							break;
            }
          }
        }
				j=0;
		}

  }
return 0;
}

int follow_user(fwrk *G,char *usr1,char *usr2)
{
	FILE *f;
	f = fopen("main","a");
	fprintf(f,"%s %s .\n",usr1,usr2);
	fclose(f);
	return 0;
}

int main()
{
 	int choice;
	int choice2;
	char choice3;
 	int chck;
	char user_name[10];
	char user_namex[10];

	char full_name[35];
	char password[11];
	char answer_1[15];
	char answer_2[15];
	char hash_tag[30];
	char *tweet_array[20];
	char tweetx[20];
	int main_menu;
	int sub_menu;
	fwrk G;
	createNetwork(&G);
	connectNetwork(&G);
	add_hash_tbl(hash_tbl,user_name,password,full_name,answer_1,answer_2);
	add_hash_tbl_fulln(hash_tbl_fulln,user_name,full_name);
	read_tweet_file(hash_tbl_tweet,hash_tag,tweet_array,tweetx);
	if(DEBUG==1)
	{
		printf("--> %s \n",(G.list_of_users[0]).user_name);
		printf("--> %s \n",(G.list_of_users[1]).user_name);
		printf("--> %s \n",(G.list_of_users[2]).user_name);

		user_t *temp;
		temp = G.adj_list[0];
		while(temp != NULL)
		{
			printf("XX- %s -XX\n",temp->user_name);
			temp = temp->next;
		}
	}


	do
	{
		printf("Wat'ch you wanna do? \n");
		printf("(1)Create new account \n");
		printf("(2)Login \n");

		printf("(0)Exit \n");
		scanf("%d",&choice);

		switch(choice)
		{
			case 1:if(sign_up(user_name,password,full_name,answer_1,answer_2)==1)
						{
							printf("You have succesfully signed up\n");
							printf("You need to login to move ahead \n");
							main_menu = 1 ;
							break;
						}

			case 2:if(log_in(user_name,password,full_name,answer_1,answer_2)==1)
				{
					printf(" Login succesful \n Redirecting.....\n");
					main_menu = 99 ;
					sleep(1);
					system("clear");
					do
					{
						printf("Welcome   \n");
						printf("What do You want do :\n ");
						printf("(1) Tweet :\n ");
						printf("(2) Go to Timeline :\n");
						printf("(3) Follow people :\n");
						printf("(4) Trending Topics (#tags) :\n");
						printf("(5) LogOut :\n");
						printf(" Please Enter your choice \n");

						scanf("%d",&choice2);
						switch(choice2)
						{
							case 1 :system("clear");
											tweet(user_name);
											sub_menu=1;
											break;

							case 2 :system("clear");
											timeline(&G,user_name);
											printf("\n\n");
											sub_menu=1;
											break;

							case 3 :system("clear");
											printf("Enter the name you want to search for \n");
											scanf("%s",full_name);

											chck = search_hash_tbl_fulln(hash_tbl_fulln,user_namex,full_name);
											if(chck==1)
											{
												printf("Do you want to follow %s y/n ? \n",full_name);
												scanf(" %c",&choice3);
												if(choice3=='Y' || choice3=='y')
												{
													follow_user(&G,user_name,user_namex);
												}
											}
											else
											{
												printf("Couldn't find in records\n");
											}
											sub_menu = 1;

											break;

							case 4 :system("clear");
											printf("Search the topic \n pss.. Use search it like #XYZ \n");
											scanf("%s",hash_tag);
											search_hash_tbl_tweet(hash_tbl_tweet, hash_tag);
											sub_menu=1;
											break;

							case 5 :	system("clear");
							 					main_menu = 1;
												sub_menu=99;
												break;
						}
					}while(sub_menu==1);
				}
				else
				{
					main_menu = 1 ;
				}
				break;

			case 3:

			case 0: exit(0);/* Exit */

		}

	}while(main_menu==1);

	exit(0);
}
