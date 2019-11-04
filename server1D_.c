#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#include<fcntl.h>
#include<unistd.h>

#define PORT 5000

struct storage
{
        int to,from,cin;
	char flag;
        char str[1024];
        struct storage *next;
};

#define NEWNODE1 (struct storage *)malloc(sizeof(struct storage))


struct clients
{
        int cid;
        struct clients *next;
};
#define NEWNODE (struct clients *)malloc(sizeof(struct clients))

struct n
{
        int to;
        int from;
	int cin;
	char flag;
        char str[1024];
};
#define NEWNODE2 (struct n *)malloc(sizeof(struct n))

struct n *node;
struct storage *temp1,*start1,*last1,*prev1;
struct clients *temp,*start,*last;


int main()
{
        int sockfd,len,n,maxClientCnt=0,cin,ifd;
        struct sockaddr_in serveraddr,clientaddr;
        char str[10];
        sockfd=socket(AF_INET,SOCK_DGRAM,0);
        if(sockfd<0)
        {
                perror("Error at server side while creating the socket...");
                exit(0);
        }
        serveraddr.sin_family=AF_INET;
        serveraddr.sin_addr.s_addr=INADDR_ANY;
        serveraddr.sin_port=htons(PORT);

        if((bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))<0)
        {
                perror("Error while binding...");
                exit(0);
        }

        len=sizeof(clientaddr);

        node=NEWNODE2;
here:   while(1)
        {
		//free(node);
		//node=NEWNODE2;
		//strcpy(node->str,"jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj   sdfkjsdhfk \n fsgkjfgsdkjfsdk\n jdfgsdf\n\n\ngdhghdhgd\ngjhg");
		
                n=recvfrom(sockfd,node,1040,0,(struct sockaddr *)&clientaddr,&len);
                if(node->to == -1 && node->from == -1)
                {
		/*ifd=open("a.txt.tmp",O_CREAT|O_WRONLY,0644);
                if(ifd==-1)
                {
                        perror("Error while opening the file at client side..");
                        exit(0);
                }
		n=write(ifd,node->str,strlen(node->str));
		close(ifd);*/
                        temp=NEWNODE;
                        temp->cid=maxClientCnt+1;
                        temp->next=NULL;
                        if(start==NULL&&last==NULL)
                        {
                                start=last=temp;
                        }
                        else
                        {
                                last->next=temp;
                                last=temp;
                        }
			//node->flag=0; uninitialized
                        node->to=maxClientCnt+1;
                        node->from=-2;
                        //adding new client in list 
                        maxClientCnt++;
                        //copying list of clients
                        temp=start;
                        strcpy(node->str,"");
                        while(temp!=NULL)
                        {
                                sprintf(str,"%d",temp->cid);
                                strcat(str,"\t");
                                strcat(node->str,str);
                                temp=temp->next;
                        }
			cin=strlen(node->str)+1;
                        sendto(sockfd,node,1040,0,(struct sockaddr *)&clientaddr,len);
                        continue;
                }//if
                if(node->to== -2)
                {
                        temp1=start1;
                        prev1=start1;
                        if(start1==NULL)
                                goto there;
                        while(temp1!=NULL)
                        {
                                if(temp1->to==node->from)
                                {
					node->flag=temp1->flag;
                                        node->to=node->from;
                                        node->from=temp1->from;
					node->cin=temp1->cin;
                                        strcpy(node->str,temp1->str);
                                        sendto(sockfd,node,1040,0,(struct sockaddr *)&clientaddr,len);
                                        if(prev1==start1 && temp1==start1)
                                        {       start1=start1->next; 
						if(start1==NULL)
							last1=NULL; 
					}
                                        else
                                                prev1->next=temp1->next;
                                        goto here;
                                }
                                prev1=temp1;
                                temp1=temp1->next;
                        }
                there:  node->to=node->from;
			//node->flag; uninitialized
                        node->from=-2;
                        strcpy(node->str,"");
                        sendto(sockfd,node,1040,0,(struct sockaddr *)&clientaddr,len);
                        continue;
                }//if

                if(node->to>0 && node->from>0)
                {
                        temp1=NEWNODE1;
			temp1->flag=node->flag;
                        temp1->to=node->to;
                        temp1->from=node->from;
			temp1->cin=node->cin;
                        strcpy(temp1->str,node->str);
                        temp1->next=NULL;
                        if(start1==NULL&&last1==NULL)
                        {       start1=temp1;
                                last1=temp1;
                        }
                        else
                        {
                                last1->next=temp1;
                                last1=temp1;
                        }
                        temp1=start1;
                        prev1=start1;
                        while(temp1!=NULL)
                        {
                                if(temp1->to==node->from)
                                {
					node->flag=temp1->flag;
                                        node->to=node->from;
                                        node->from=temp1->from;
					node->cin=temp1->cin;
                                        strcpy(node->str,temp1->str);
					cin=strlen(node->str)+1;
                                        sendto(sockfd,node,1040,0,(struct sockaddr *)&clientaddr,len);
                                        if(prev1==start1 && temp1==start1)
                                        {       start1=start1->next; 
						if(start1==NULL)
							last1=NULL; 
					}
                                        else
                                                prev1->next=temp1->next;
                                        goto here;
                                }
                                prev1=temp1;
                                temp1=temp1->next;
                        }//while
			//node->flag; uninitialized
                        node->to=node->from;
                        node->from=-2;
                        strcpy(node->str,"");
                        sendto(sockfd,node,1040,0,(struct sockaddr *)&clientaddr,len);
                        continue;
                }//if
        }//while
}
