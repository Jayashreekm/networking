#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

#define PORT 5000

struct n
{
	int to;
	int from;
	int cin;
	char flag;
	char str[1024];
};

#define NEWNODE (struct n *)malloc(sizeof(struct n))

struct cliNofd
{
	int cid;
	int ofd;
	struct cliNofd *next;
};

#define NEWNODE1 (struct cliNofd *)malloc(sizeof(struct cliNofd))

struct cliNofd *start1=NULL,*last1=NULL,*temp1=NULL,*prev1=NULL;

struct n *node;

int main(int argc,char **argv)
{
	int sockfd,cin,ofd,len,n,myId,ifd,cout;
	char temp[1024];
	struct sockaddr_in serveraddr;
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0)
	{
		perror("While socker creating");
		exit(0);
	}
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=INADDR_ANY;
	serveraddr.sin_port=htons(PORT);

	len=sizeof(serveraddr);

	node=NEWNODE;
	node->to=-1;
	node->from=-1;
	sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);
	n=recvfrom(sockfd,node,1040,0,(struct sockaddr *)&serveraddr,&len); //here flag is 0
	myId=node->to;
	printf("my id is =%d",myId);
	printf("Available Clients are :");puts(node->str);

	if(argc>1)
	{
		ifd=open(argv[1],O_RDONLY,0644);
		if(ifd==-1)
		{
			perror("Error while opening the file at client side..");
			exit(0);
		}

		printf("To which client...");
		scanf("%d",&node->to);
		node->from=myId;
		node->flag='N';//N flag for New file at recv side 
		strcpy(node->str,argv[1]);
		sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);//flagis-1openthefile at recv end
		while(1)
		{
			n=recvfrom(sockfd,node,1040,0,(struct sockaddr *)&serveraddr,&len);
			if(node->from==-2)
			{
				node->to=-2;
				node->from=myId;
				strcpy(node->str,"");
				//node->flag uninitialized
				sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);
			}
			else
				break;
		}
		if(node->flag!='F')
		{
			perror("Waiting ...something went wrong..."); exit(0);
		}
		cin=read(ifd,&node->str,1024);
		while(1)
		{
			if(cin>0)
			{
				node->flag='C';
				node->to=node->from;
				node->from=myId;
				node->cin=cin;
				//node->str already copied
				sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);//sendingexitnode
			}
			else
			{
				n=node->from;
				node->flag='F';
				node->to=n;
				node->from=myId;
				strcpy(node->str,"EXIT");
				close(ifd);
				sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);//sending exitnode
				goto here2;
			}
			while(1)
			{
				n=recvfrom(sockfd,node,1040,0,(struct sockaddr *)&serveraddr,&len);
				if(node->from==-2)
				{
					node->to=-2;
					node->from=myId;
					//node->flag is uninitialized
					strcpy(node->str,"");
					sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);
				}
				else
					break;
			}//while 1
			if(node->flag!='F')
			{
				perror("Waiting ...something went wrong didnt recv ack..."); exit(0);
			}
			cin=read(ifd,&node->str,1024);
		}//while1

	}//argc >1

here1:  node->to=-2;
	node->from=myId;
	//node->flag is uninitialized
	strcpy(node->str,"");
	sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);

	while(1)
	{
here2:        n=recvfrom(sockfd,node,1040,0,(struct sockaddr *)&serveraddr,&len);
here:   if(node->from==-2)
	{
		node->to=-2;
		node->from=myId;
		//node->flag is uninitialized
		strcpy(node->str,"");
		sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);
		continue;
	}

	if(node->flag== 'N')
	{
		puts("new client is sending...\n");
		strcat(node->str,".tmp");
		ofd=open(node->str,O_CREAT|O_WRONLY,0644);
		if(ofd==-1)
		{
			perror("Error while opening the file at client side recv end ...");
			exit(0);
		}
		temp1=NEWNODE1;
		temp1->cid=node->from;
		temp1->ofd=ofd;
		temp1->next=NULL;
		if(start1==NULL && last1==NULL)
		{
			start1=temp1;
			last1=temp1;
		}
		else
		{
			last1->next=temp1;
			last1=temp1;
		}

		node->to=node->from;
		node->from=myId;
		node->flag='F';//flag to send data from now to sender
		strcpy(node->str,"");
		sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);
		continue;
	}
	if(node->flag=='C')
	{
		temp1=start1;
		while(temp1!=NULL)
		{
			if(temp1->cid==node->from)
			{
				ofd=temp1->ofd;
				break;
			}//if
			temp1=temp1->next;
		}//while
		cout=write(ofd,node->str,node->cin);
		node->flag=0;
		node->to=node->from;
		node->from=myId;
		node->flag='F';
		strcpy(node->str,"");
		sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);
		continue;
	}
	if(node->flag=='F')
	{
		prev1=NULL;
		temp1=start1;
		while(temp1!=NULL)
		{
			if(temp1->cid==node->from)
			{
				ofd=temp1->ofd;
				if(prev1==NULL)
				{	
					if(last1==start1)
					{       start1=NULL;
						last1=NULL; }
					else
						start1=start1->next;
				}
				else
				{
					prev1->next=temp1->next;
					if(temp1==last1)
						last1=prev1;
				}
				break;
			}//if
		}//while
		puts("Done...");
		close(ofd);
		node->to=-2;
		node->from=myId;
		//node->flag is uninitialized
		strcpy(node->str,"");
		sendto(sockfd,node,1040,0,(const struct sockaddr *)&serveraddr,len);
		continue;
	}

	}//while*/
}//main

