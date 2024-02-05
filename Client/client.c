/*
Full Name:  Shashank Kannan	Ajay Shanmugam R
Student ID: 110122650		110122647
Section:    	2		   2
*/

#define _XOPEN_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#define CHUNK 16384

///////////////////////////////////////////////////////////////////////////////////---UTILITIES---////////////////////////////////////////////////////////////////////////////////////////
void sendCommand(int socket, const char *command) 
{
	send(socket, command, strlen(command), 0);
}

int gmrn() //random number generation by combining random number rn1 (1-10000) and rn2(1-20) to name distinct tar filenames
{
    int rn1 = rand() % 10000 + 1;
    int rn2 = rand() % 20 + 1;
    return rn1 * 100 + rn2;
}

void create_folder()
{
	char folder_path[256];
    	snprintf(folder_path, sizeof(folder_path), "%s/f23project", getenv("HOME"));
    	mkdir(folder_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void printerror()
{
	printf("\nThis seems to be an invalid input, please check the syntax and number of characters of the commands from the list:\n1.getfn {filename}\n2.getft {ext1} {ext2} {ext3}\n3.getfz {size1} {size2}\n4.getfdb {date}\n5.getfda {date}\n6.quitc: To exit\n");
}

int syntaxcheck(char cmd[1024], int argsc, char *cs)
{
 	const char *getfnStr = cs;
    	const char *getfnPos = strstr(cmd, getfnStr);
	if (getfnPos != NULL)
    	{
        	const char *afterGetfn = getfnPos + strlen(getfnStr);
		while (*afterGetfn != '\0' && isspace((unsigned char)*afterGetfn))
        	{
            		afterGetfn++;
        	}
		if (*afterGetfn != '\0') //checking for spaces
        	{
            		int spaceCount = 0;
			for (int i = 0; cmd[i] != '\0'; i++)
    			{
        			if (cmd[i] == ' ')
        			{
        				spaceCount++;
        			}
    			}
    			if(argsc==2)
    			{
    				if(spaceCount==2)
    				{
    					return 1;
    				}
    				else
    				{
	    				printerror();
    					return 0;
    				}
    			}
    			else
    			{
    				if (spaceCount < 1 || spaceCount > argsc)
    				{
        				printerror();
        				return 0;
    				}
    				else
    				{
    					return 1;
    				}
    			}
        	}
        	else
        	{
        		printerror();
        		return 0;
        	}
    	}

}

bool isDate(const char *dstr) //check if dstr is in date format - yyyy-mm-dd
{
    if (strlen(dstr) != 10) return false;
    if (sscanf(dstr, "%4d-%2d-%2d", &(int){0}, &(int){0}, &(int){0}) != 3) return false;
    return true;
}

const char* cRs() //random strings
{
    const char* strings[] = {
        "Your wish is my command",
        "Cast your spell",
        "Type to transform",
        "Input sorcery"
    };

    int numStrings = sizeof(strings) / sizeof(strings[0]);
    int randomIndex = rand() % numStrings;

    return strings[randomIndex];
}

///////////////////////////////////////////////////////////////////////////////////---EXTENSIONS/SIZE/DATE---/////////////////////////////////////////////////////////////////////////////
void receiveFileFromServer(int server_socket, const char *op_file) 
{
    	char hdir[256], cfile[512];
    	snprintf(hdir, sizeof(hdir), "%s/f23project", getenv("HOME")); //Save it in f23project in home directory of client
    	snprintf(cfile, sizeof(cfile), "%s/%s", hdir, op_file);
	FILE *opf = fopen(cfile, "wb");
    	if (opf == NULL) 
    	{
        	perror("Error opening output file");
        	exit(EXIT_FAILURE);
        }
	char buff[1024];
    	int data_RD;
	int i=1;
//Receive and write the file content
    	while ((data_RD = recv(server_socket, buff, sizeof(buff), 0)) > 0) 
    	{
        	fwrite(buff, 1, data_RD, opf);
        	if(data_RD<1024)
          		break;
    	}
    	fclose(opf);

    	if (data_RD == 0) 
    	{
        	printf("File received successfully!\n");
    	} 
    	else if (data_RD == -1) 
    	{
        	perror("Error receiving file");
    	}
}

/////////////////////////////////////////////////////////////////////////////////////////////---GETFN---//////////////////////////////////////////////////////////////////////////////////
void receiveResponse(int socket) 
{
   	char buff[10240];
    	ssize_t data_RD;
    	if ((data_RD = recv(socket, buff, sizeof(buff), 0)) > 0) //read data
    	{
        	buff[data_RD] = '\0';
        	printf("%s\n", buff); //print data
    	}
}

/////////////////////////////////////////////////////////////////////////////////////////---MAIN---///////////////////////////////////////////////////////////////////////////////////////
int main() 
{
	create_folder();
	int client_count =1;
        FILE *count_file = fopen("connection_count.txt", "r+");
        if (count_file == NULL) 
        {
            perror("Error opening connection count file");
            exit(EXIT_FAILURE);
        }
	fscanf(count_file, "%d", &client_count);
    	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    	if (client_socket == -1) 
    	{
        	perror("Socket creation failed");
        	exit(EXIT_FAILURE);
    	}
 // Determine whether the connection should be handled by the server or mirror
        if (client_count <= 4 || (client_count > 8 && client_count % 2 == 0)) 
        {
//We have given server address and port here to connect when clients count is either less than or equal to 4 or alternate times when its more than 8(when its even number).
    		struct sockaddr_in server_address;
    		server_address.sin_family = AF_INET;
    		server_address.sin_port = htons(12345);
    		inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));

// This connects to server
    		if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) 
    		{
        		perror("Connection failed");
        		close(client_socket);
        		exit(EXIT_FAILURE);
    		}
    	}
    	else if(client_count >4 && client_count<=8 || (client_count>8 && client_count % 2 != 0))
    	{	
   
//We have given mirror address and port here to connect when clients count is more than 4 and less than or equal to 8 or alternate times when its more than 8(when its odd number).
    		struct sockaddr_in server_address;
    		server_address.sin_family = AF_INET;
    		server_address.sin_port = htons(12346); // Use the mirror's port number
    		inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));

// This connects to mirror
    		if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) 
    		{
        		perror("Connection failed");
        		close(client_socket);
        		exit(EXIT_FAILURE);
    		}
    
    	}

    	char command[1024];

    	while (1) 
    	{
    		srand(time(NULL));
    		const char* cSg = cRs(); // Just a randomly generated string to be displayed while getting commands
        	printf("\n%s, Enter a command: ",cSg); 
        	fgets(command, sizeof(command), stdin);
        	command[strcspn(command, "\n")] = '\0'; 
        	

// In the next portion we have sent the commands to server and receieved the server's response and called the appropriate function to handle.
       
  		if(strstr(command,"getfda "))
       		{
       			char *dstr = command + strlen("getfda ");
//checking if argument is in date - "yyyy-mm-dd" format 
       			if (isDate(dstr))
    			{
       				int mr = gmrn();
       				char fn[38];
//checking syntax of the command
       				int check = syntaxcheck(command,1,"getfda");
				if(check==1)
    				{
    					printf("\n Yaay! Executed command: %s !!\n",command);
        				sendCommand(client_socket, command);
//tarfilename sent to be used to create tar file 
    					snprintf(fn, sizeof(fn), "getfda_%05d.tar.gz", mr); 
    					printf("\nTar file: %s created\n",fn);
    					receiveFileFromServer(client_socket, fn);
       				}
       			}
       			else
       			{
       				printerror();
       			}
       		}
       		else if(strstr(command,"getfdb "))
       		{
       			char *dstr = command + strlen("getfdb ");
       			if (isDate(dstr))
    			{
       				int mr = gmrn();
       				char fn[38]; 
       				int check = syntaxcheck(command,1,"getfdb");
				if(check==1)
    				{
    					printf("\n Yaay! Executed command: %s !!\n",command);
        				sendCommand(client_socket, command);
    					snprintf(fn, sizeof(fn), "getfdb_%05d.tar.gz", mr);
    					printf("\nTar file: %s created\n",fn);
    					receiveFileFromServer(client_socket, fn);
       				}
       			}
       			else
       			{
       				printerror();
       			}
       		}
       		else if(strstr(command,"getft "))
       		{
       			int mr = gmrn();
       			char fn[38]; 
       			int check = syntaxcheck(command,3,"getft");
			if(check==1)
    			{
    				printf("\n Yaay! Executed command: %s !!\n",command);
        			sendCommand(client_socket, command);
    				snprintf(fn, sizeof(fn), "getft_%05d.tar.gz", mr);
    				printf("\nTar file: %s created\n",fn);
    				receiveFileFromServer(client_socket, fn);
    			}
    			
       		}
       		 
       
       		else if(strstr(command,"getfz "))
       		{	
       			int mr = gmrn();
       			char fn[38];
       			int check = syntaxcheck(command,2,"getfz");
			if(check==1)
    			{
    				printf("\n Yaay! Executed command: %s !!\n",command);
        			sendCommand(client_socket, command);
    				snprintf(fn, sizeof(fn), "getfz_%05d.tar.gz", mr);
    				printf("\nTar file: %s created\n",fn);
    				receiveFileFromServer(client_socket, fn);
       			}
       		}
       		else if(strstr(command,"getfn "))
       		{
       			
       			int check = syntaxcheck(command,1,"getfn");
			if(check==1)
    			{
       				printf("\n Yaay! Executed command: %s !!\n",command);
        			sendCommand(client_socket, command);
       				receiveResponse(client_socket);
       			}
       		}
       		else if (strcmp(command, "quitc") == 0) 
       		{
       			break;
       		}
       		else
       		{
       			printerror();
       		}
    	}

	close(client_socket);
	return 0;
}

