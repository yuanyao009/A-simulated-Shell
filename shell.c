
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <string.h>
	#include <fcntl.h>
	#include <unistd.h>


    /*global variable*/
  	const int BUFFERSIZE=80;
    char buffer[80];//buff the input of user

	int is_back=0;//determine whether it is a backgound process
	int status=0;//process status variable
	pid_t pid;//process ID


  	
	/**************decalartion of function***************/
	char *get_command(int *input_len);//get the input command
	void dispose_command(char *deal_in,int len);//dispose the command
	int redirect(char * in, int len);//realize the redirction function of shell
	int piple(char * in, int li_inlen);//relize the piple
	int is_fileexist(char * comm);//check whether excutable file is exist
	void multi_command(char * m_in, int m_len);//dipose the semicolon
	

	int main()// call get_command(), dispose_command()	{
		char * path;//current path
		char *user;//
		char *input=NULL;//user input
		int command_len=0;//number of input charateristic
		while(1) 
		{
			command_len=0;
			path = get_current_dir_name();//get current path
			user=getlogin();//get the user
			printf("%s@%s~desktop:~%s$",user,user, path);//simulated the shell interface
			if(input)
				free(input);//free the last command
			input=get_command(&command_len);//get command
			if(input)
			{
				dispose_command(input,command_len);//dipose the command
			}
		}
	}



	char *get_command(int * input_len)//get the command
	{
		char lc_char;
		char *get_in;
		(*input_len)=0;	

		lc_char = getchar();
		while(lc_char != '\n' && (*input_len) < BUFFERSIZE)
		{
	    		buffer[(*input_len) ++] = lc_char;
	   	 	lc_char = getchar();
		}

		/* if input length is too large*/
		if((*input_len) >= BUFFERSIZE) {
	  	   printf("Your command too long ! Please reenter your command !\n");
	  	   (*input_len) = 0;     /* Reset */
		   gets(buffer);
		   strcpy(buffer,"");
		   get_in=NULL;
	    	   return NULL;
		}
		else
	   		buffer[(*input_len)] = '\0'; /*add the \0 to form the string*/

		if((*input_len)==0)return NULL;//deal with when directly type enter key

		
		get_in = (char *)malloc(sizeof(char) * ((*input_len) + 1));
		strcpy(get_in, buffer);
		strcpy(buffer,"");
		return get_in;
	}


	void dispose_command(char *deal_in,int len)//multi_command()、piple（）、redirect()
	{
		char * arg[30];//store the command, corresponding parameters
		int i=0, j=0, k=0;
		pid_t pid;//process
		 for( i=0;i<=len;i++)
		 {
			if(deal_in[i]==';')
			{
				multi_command(deal_in,len);
				return;
			}
		 }
		 for( i = 0, j=0,k=0; i<=len; i ++) {
			/*realize the piple and redirection*/
			if(deal_in[i]=='<' || deal_in[i] == '>' || deal_in[i] == '|' || deal_in[i]==';') {
			  if(deal_in[i] == '|')
				{
			      	     piple(deal_in, len);
				     return;
				}
			   else if(deal_in[i]=='>' ||deal_in[i]=='<')
			       {
			       	     redirect(deal_in, len);
				     return;
				}
			}
			/*deal the space, tab */
			if(deal_in[i]== ' ' || deal_in[i] == '\t' || deal_in[i] == '\0') {
			   	if(j == 0)
			     	 continue;
			   	else {

				       buffer[j ++] = '\0';    //construct a string
				       arg[k] = (char *)malloc(sizeof(char) * j);//copy the first command into arg

				       
				       strcpy(arg[k], buffer);
			      	       j = 0;  /* for the next command*/
			      	       k ++;
			   	     }
			}
			else {       /*  '&' repesent the background process */
			  	  if(deal_in[i]== '&' && deal_in[i + 1] == '\0') {
					is_back = 1;//

					return;//continue for the next command
			     }

				buffer[j ++] = deal_in[i];
			}
		}
		if(!strcmp(arg[0],"exit")) {   //if the command is "exit", exit the program
			printf("bye-bye\n");
			exit(0);
		}

		/* using exec */
		arg[k] = (char *)malloc(sizeof(char));
		arg[k ++] = (char *)0;
		/* decide whether arg[0] is exist*/
		if(is_fileexist(arg[0]) == -1) {
		   	 printf("This is command is not founded ?!\n");
		   	 for(i = 0; i < k; i ++)
			 free(arg[i]);
		   	 return;

		}
		if((pid = fork()) == 0)      /*child*/
		     execv(buffer, arg);
		else                     /*father*/
		   if(is_back == 0)
		       waitpid(pid, &status, 0);

		   /* free the space*/
		   for(i = 0; i < k; i ++)   free(arg[i]);
	}

	int redirect(char * in, int len)//realize the redirection
	{

		char * argv[30], *filename[2];

		int fd_in, fd_out, is_in = -1, is_out = -1, num = 0;
		int is_back=0;
		int I,i, j, k=0;
		int status=0;


		/* implement the redirection, and we can change filename method, such as using the stdin,stdout */
		for(I = 0, j = 0, k = 0; I <= len; I ++) {
  			if(in[I] == ' ' || in[I] == '\t' || in [I] == '\0' || in[I] == '<' || in[I] == '>') {
        			if(in[I] == '>' || in[I] == '<') {
        			/* the maximum number of  '<', '> can only be one*/
       			 		if(num < 3) {
           					num ++;
          					if(in[I] == '<') 
               						is_in = num - 1;
           					else  is_out = num - 1;
        					/* ls > a*/
        					if(j > 0 &&num == 1) {
          						buffer[j ++] = '\0';
              						argv[k] = (char *)malloc(sizeof(char) * j);
              						strcpy(argv[k], buffer);
             						k ++;
             						j = 0;
        					}
   					}
     					else {
          					printf("The format is error !\n");
           					return - 1;
                			}
              			}
		        	if(j == 0) 
		             		continue;
		        	else {
					buffer[j ++] = '\0';
    
					if(num == 0){
	    					argv[k] = (char *)malloc(sizeof(char) * j);
	   					strcpy(argv[k], buffer);
	   					k ++;
					}
					/* if the destincation is filename*/
					else {
	  					filename[status] = (char *)malloc(sizeof(char) * j);
	  					strcpy(filename[status], buffer);
					}
					j = 0;   /* Initiate */ 
					continue;
		           	}
               		}
                	else{
                   		if(in[I] == '&' && in[I + 1] == '\0') {
				is_back = 1;
				return;
				}
				buffer[j ++] = in[I];
               		}
		}
		argv[k] = (char *)malloc(sizeof(char));
		argv[k ++] = (char *)0;

		if(is_fileexist(argv[0]) == -1) {
      			printf("This command is not founded !\n");
       			for(I = 0; I < k; I ++) free(argv[I]);
      			return 0;
		}
		
		if((pid = fork()) == 0) {
     			/* redirection*/
      			if(is_out != -1) {
          			if((fd_out = open(filename[is_out], O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR )) == -1) 				{
           				printf("Open out %s error \n", filename[is_out]);
           				return -1;
      				}
  			}
			/* redirection*/
			if(is_in != -1) {
     				if((fd_in = open(filename[is_in], O_RDONLY, S_IRUSR|S_IWUSR)) == -1) {
  					printf("Open in %s error \n", filename[is_in]);
  					return -1;
				}
			}
			if(is_out != -1) {
				if(dup2(fd_out, STDOUT_FILENO) == -1) {
	     				printf("Redirect Standard Out Error !\n");
	     				exit(1);
	  			}
			}
			if(is_in != -1) {
	    			if(dup2(fd_in, STDIN_FILENO) == -1) {
	       				printf("Redirect Standard In error !\n");
	       				exit(1);
	    			}
			}
			execv(buffer, argv);
          	}
		else 
                 	if(is_back == 0)        /* Run on the TOP */
				waitpid(pid, &status, 0);
		for(I = 0; I < k; I ++) 
			free(argv[I]);
          	if(is_in != -1) {
                	free(filename[is_in]);
                	close(fd_in);
           	}
          	if(is_out != -1){
	            	free(filename[is_out]);
                	close(fd_out);
         	 }
          	return 0;
	}


	void multi_command(char * m_in, int m_len)//deal with the multi-command
	{
		int pos;  //position of the semicolon
		int i=0,j=0;
		char *command=NULL;
		for(i=0;i<=m_len;i++)
		{
			if(m_in[i]==';')     //deal with the first command before semicolon
			{
				pos=i;
				command=(char *)malloc(sizeof(char) * (i+1));
				for(j=0;j<i;j++)
					command[j]=m_in[j];
					command[j]='\0';
					dispose_command(command,i+1);
					//printf("%s  %d  %d\n",command,m_len,pos);
					free(command);
					break;
			}
		}
		if(m_in[pos+1]=='\0')   //if the command is end with semicolon, directly return
			return;
		if(pos>=m_len)return;
		command=(char *)malloc(sizeof(char) * (m_len-pos));  //deal with the second command
		for(i=pos+1,j=0;i<=m_len;i++)
		{
			command[j++]=m_in[i];
		}
		dispose_command(command,m_len-pos);    //use the recursive method
		free(command);
	}

	int piple(char * in, int li_inlen)//piple function
	{
	   char * argv[2][30];
	   int  count;
	   is_back = 0;
	   int li_comm = 0, fd[2], fpip[2];
	   char lc_char=' ';
	   char lc_end[1];
	   pid_t child1, child2;
	   int I,i, j, k=0;
	   /* implement of piple*/
	   for(I = 0, j = 0, k = 0; I<=li_inlen; I ++) {
		      if(in[I]== ' ' || in[I] == '\t' ||in[I] == '\0' || in[I] == '|') {

				  if(in[I] == '|') {          /**/

					      if(j > 0) {
						   buffer[j ++] = '\0';

											   argv[li_comm][k] = (char *) malloc(sizeof(char) * j);
						   strcpy(argv[li_comm][k++], buffer);
					      }
					  //    argv[li_comm][k] = (char *)malloc(sizeof(char));
				      	      argv[li_comm][k++] = (char *)0;

				   	     li_comm++;
				    	     count = k;
					     k = 0; j = 0;
				   }

				   if(j == 0)  continue;

				   else {
					       buffer[j ++] = '\0';
					       argv[li_comm][k] = (char *)malloc(sizeof(char) * j);
					       strcpy(argv[li_comm][k], buffer);
					       k ++;
				    }

				    j = 0; /* Initiate */
				   continue;
			 }

			 else {
			     	if(in[I] == '&' && in[I + 1] == '\0') {
					 is_back = 1;

					 return 0;    			        }
		     	 }
		         buffer[j ++] = in[I];
	   }
		//argv [li_comm][k] = (char *)malloc(sizeof(char));
   		argv [li_comm][k++] = (char *)0;

	    if(is_fileexist(argv[0][0]) == -1) {
		 printf("This first command is not founed !\n");
		 for(I = 0; I < count; I ++) free(argv[0][I]);
		 return 0;
	    }

	    if(pipe(fd) == -1) {
		printf("Open pipe error !\n");
		return -1;
		 }

		  if((child1 = fork()) == 0) {
			close(fd[0]);
			if(fd[1] != STDOUT_FILENO) {
				 if(dup2(fd[1], STDOUT_FILENO) == -1) {
				      printf("Redirect Standard Out error !\n");
				      return -1;
				 }

				close(fd[1]);
			}
			execv(buffer, argv[0]);
		  }
		  else { 					/*father*/
			/* waith the end of wirting process*/
			waitpid(child1, &li_comm,0);
			lc_end[0] = 0x1a;
			write(fd[1], lc_end, 1);
			close(fd[1]);
			if(is_fileexist(argv[1][0]) == -1) {
			      printf("This command is not founded !\n");
			      for(I = 0; I < k; I ++) free(argv[1][I]);
			      return 0;
			 }

			/* child for the second command*/
			if((child2 = fork()) == 0) {
			     if(fd[0] != STDIN_FILENO) {

				if(dup2(fd[0], STDIN_FILENO) == -1) {
				    printf("Redirect Standard In Error !\n");
				    return -1;
				}
				close(fd[0]);
			     }
			     execv(buffer, argv[1]);
			  }
			  else      /*father*/
			      if(is_back == 0)  waitpid(child2, NULL, 0);
			}
			for(I = 0; I < count; I ++)  free(argv[0][I]);
			for(I = 0; I < k ; I ++) free(argv[1][I]);
			return 0;	
	}


	int is_fileexist(char * comm)//check the excutable file
	{
		char * env_path, * p;
		int i=0;
		env_path = getenv("PATH");
		p = env_path;
		while(*p != '\0') {
		   if(*p != ':')  buffer[i ++] = *p;
		   else {
		       buffer[i ++] = '/';
		       buffer[i] = '\0';
		       /* form the  pathname*/
		       strcat(buffer, comm);
		       if(access(buffer, F_OK) == 0)     /* file has been found*/
			     return 0;
		       else                          /* check for other paht*/
			     i = 0;
		    }
		    p ++;
		 }
		 /* return –1 if it doesn't find any things*/
		 return -1;
	}





