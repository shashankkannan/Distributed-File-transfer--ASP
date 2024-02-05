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
#define CHUNK 16384


//////////////////////////////////////////////////////////////////////////////---UTILITIES---/////////////////////////////////////////////////////////////////////////////////////////////

void sendFileToClient(int client_socket, const char *filename) 
{
//opens file {filename}
	FILE *file = fopen(filename, "rb");
    	if (file == NULL) 
    	{
        	perror("Error opening file for reading");
        	return;
    	}
    	char buf[CHUNK];
    	int bytesRead;
    	fseek(file, 0, SEEK_SET);
	int i=0;
//Reads data from the file into the buffer until the end of the file is reached
    	while ((bytesRead = fread(buf, 1, sizeof(buf), file)) > 0) 
    	{
        	send(client_socket, buf, bytesRead, 0);
	}
   
   	fclose(file);
}

int createSocket()
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    	if (server_socket == -1) 
    	{
        	perror("Socket creation failed");
        	exit(EXIT_FAILURE);
    	}
    
    	return server_socket;
}

char* incrementDate(const char *date) {
    struct tm t = {};
    sscanf(date, "%d-%d-%d", &t.tm_year, &t.tm_mon, &t.tm_mday);
    t.tm_year -= 1900;
    t.tm_mon--;
    t.tm_mday++;
    char *result = malloc(11);
    strftime(result, 11, "%Y-%m-%d", &t);
    return result;
}



/////////////////////////////////////////////////////////////////////////////////////---GET_FILE_INFO---//////////////////////////////////////////////////////////////////////////////////

void sendFileInformation(int client_socket, const char *filename) 
{
// We have written this function to send file information to the client
    	struct stat file_stat;
// This find command is to search for the file recursively and quit after the first match
	char find_command[256];
    	snprintf(find_command, sizeof(find_command), "find ~ -name '%s' -exec stat --printf '%%s\n%%Y\n%%A\n' {} + -quit", filename);

// The output is read here
    	FILE *find_output = popen(find_command, "r");
    	if (find_output == NULL) 
    	{
        	perror("Error executing find command");
        	send(client_socket, "Error finding file", strlen("Error finding file"), 0);
        	return;
    	}
// Here we read file information from find command output
    	long file_size;
    	time_t created_time;
    	char permissions[10];

    	if (fscanf(find_output, "%ld\n%ld\n%s", &file_size, &created_time, permissions) != 3) 
    	{
        	send(client_socket, "File not found", strlen("File not found"), 0);
        	pclose(find_output);
        	return;
    	}
	pclose(find_output);
// We convert the file creation time to a human-readable form
    	char created_time_str[20];
    	strftime(created_time_str, sizeof(created_time_str), "%Y-%m-%d %H:%M:%S", localtime(&created_time));
// This is the final output message to be sent
    	char file_info[1024];
    	snprintf(file_info, sizeof(file_info), "File: %s\nSize: %ld bytes\nCreated: %s\nPermissions: %s",
        filename, file_size, created_time_str, permissions);
// Send file information to the client
    	send(client_socket, file_info, strlen(file_info), 0);
}




////////////////////////////////////////////////////////////////////////////////////////////---EXTENSIONS---//////////////////////////////////////////////////////////////////////////////


void handleGetftCommand(int client_socket, char *extensions) {

//new temporary folder is created
	system("mkdir tempfolder");
	char tarCommand[256] = "tar czf temp.tar.gz -C tempfolder .";
	char findCommand[256] = "find . -maxdepth 3 -type f \\(";
	char *token = strtok(extensions, " ");
	while (token != NULL) 
	{
    		strcat(findCommand, " -name \"*.");
    		strcat(findCommand, token);
    		strcat(findCommand,"\"");
    		token = strtok(NULL, " ");
    		if (token != NULL) 
    		{
        		strcat(findCommand, " -o");
    		}
	}

	strcat(findCommand, " \\) -exec cp {} tempfolder \\;");
	printf("\nfindcommand:%s ",findCommand);
//find command is executed here which will copy all the files with given extensions to the temporary folder
	system(findCommand);
	printf("\nTarcommand: %s ",tarCommand);
//After that the temporary folder is zipped using tar (gz file)
	system(tarCommand);
//temporary folder is removed
	system("rm -rf tempfolder");
    
    	struct stat st;
//Checks if the file "temp.tar.gz exists
    	if (stat("temp.tar.gz", &st) == 0) 
    	{
//This sends the content of "temp.tar.gz" to the client using the sendFileToClient function and deletes temp.tar.gz
        	sendFileToClient(client_socket, "temp.tar.gz");
        	unlink("temp.tar.gz");
    	} 
    	else 
    	{
//If file does not exist, sends the message "No file found" to the client
        	send(client_socket, "No file found", strlen("No file found"), 0);
    	}
}


void sendFilesByExtension(int client_socket, char *extensions) 
{
   	handleGetftCommand(client_socket, extensions);   
}



//////////////////////////////////////////////////////////////////////////////////////////---SIZE/DATE---/////////////////////////////////////////////////////////////////////////////////


void handleGetfdaCommand(int client_socket, const char *date, int isBefore) 
{
//Temporary folder is created
	system("mkdir folderfortar");
	char findCommand[256];
//if isBefore is 0, then the getfda is chosen and the files which are created after the given date is found.
	if(isBefore==0)
	{ 	
		snprintf(findCommand, sizeof(findCommand), "find ~ -maxdepth 2 -type f -newermt \"%s\" -exec cp -t folderfortar {} +", date);
	}
//if isBefore is 1, then the getfdb is chosen and the files which are created before the given date is found.
	else if(isBefore==1)
	{
		char *date1="2020-01-01";
		char *nD = incrementDate(date);
		snprintf(findCommand, sizeof(findCommand), "find ~ -maxdepth 2 -type f -newermt \"%s\" ! -newermt \"%s\" -exec cp -t folderfortar {} +", date1, nD);
	}
//else, the getfz is chosen and the files which have size between size1 and size2
	else
	{
		int size1 = atoi(date);
		int size2 = isBefore;
		snprintf(findCommand, sizeof(findCommand), "find ~ -type f -size +%dc -a -size -%dc -exec cp -t folderfortar {} +", size1, size2);
	}
    	printf("\nfindcommand: %s\n",findCommand);
//The respective find command is executed and the files are copied to temporary folder
    	system(findCommand);
//The temporary folder is zipped using tar (gz file)
    	system("tar czf temp.tar.gz -C folderfortar .");
//The temporary folder is removed
    	system("rm -r folderfortar");
	struct stat st;
    	if (stat("temp.tar.gz", &st) == 0) 
    	{
        	sendFileToClient(client_socket, "temp.tar.gz");
        	unlink("temp.tar.gz");
    	} 
    	else 
    	{
        	send(client_socket, "No file found", strlen("No file found"), 0);
    	}
}

void sendFilesByDate(int client_socket, const char *date, int isBefore) 
{
	handleGetfdaCommand(client_socket, date, isBefore);
}


///////////////////////////////////////////////////////////////////////////////////////////---PCLIENTREQUEST---///////////////////////////////////////////////////////////////////////////

void pclientrequest(int client_socket) 
{
	char buf[1024];
    	int data_REC;

    	while (1) 
    	{
// Receive command from the client
        	data_REC = recv(client_socket, buf, sizeof(buf), 0);
        	if (data_REC <= 0) 
        	{
            		break;
            	}
        	buf[data_REC] = '\0'; 
//For "quitc" command
		if (strcmp(buf, "quitc") == 0) 
		{
            		break; 
        	} 
//For "getfn {filename}" command
        	else if (strncmp(buf, "getfn", 5) == 0) 
        	{
            		char *filename = buf + 6; 
            		sendFileInformation(client_socket, filename);
        	}
//For "getfz {size1} {size2}" command
        	else if (strncmp(buf, "getfz", 5) == 0) 
        	{
            		int size2;
            		char size1[50];
            		sscanf(buf + 6, "%s %d", size1, &size2);
            		sendFilesByDate(client_socket, size1, size2);
        	}
//For "getft {ext1} {ext2} {ext3}" command
        	else if (strncmp(buf, "getft", 5) == 0) 
        	{
            		char *extensions = buf + 6; 
            		sendFilesByExtension(client_socket, extensions);
        	}
//For "getfdb {date}" command 
        	else if (strncmp(buf, "getfdb", 6) == 0) 
        	{
            		char *date = buf + 7;
            		sendFilesByDate(client_socket, date, 1);
        	}
//For "getfda {date}" command
        	else if (strncmp(buf, "getfda", 6) == 0) 
        	{
            		char *date = buf + 7; 
            		sendFilesByDate(client_socket, date, 0);
        	}
        	else 
        	{
            		send(client_socket, "Unknown command", strlen("Unknown command"), 0);
      		}
    	}
    	close(client_socket);
    	exit(0);
}


//////////////////////////////////////////////////////////////////////////////////////---MAIN---//////////////////////////////////////////////////////////////////////////////////////////
int main() 
{
	int client_count = 0;
    	int server_socket = createSocket();
    
    	// Here we bind the socket to an address and port
    	struct sockaddr_in server_address;
    	server_address.sin_family = AF_INET;
    	server_address.sin_addr.s_addr = INADDR_ANY;
    	server_address.sin_port = htons(12346); // port number

    	if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) 
    	{
        	perror("Binding failed");
        	close(server_socket);
        	exit(EXIT_FAILURE);
    	}

    	if (listen(server_socket, 10) == -1) 
    	{
        	perror("Listening failed");
        	close(server_socket);
        	exit(EXIT_FAILURE);
    	}

    	while (1) 
    	{
        	int client_socket = accept(server_socket, NULL, NULL);
        	if (client_socket == -1) 
        	{
            		perror("Acceptance failed");
            		continue;
        	}
        
        	FILE *count_file = fopen("/home/aspdeb/clientrun/connection_count.txt", "r+");
        	if (count_file == NULL) 
        	{
            		perror("Error opening connection count file");
            		exit(EXIT_FAILURE);
        	}
		fscanf(count_file, "%d", &client_count);
        
        	printf("execution from Mirror.c\n");
        	if (fork() == 0) 
        	{	//CHILD PROCESS
            		close(server_socket);
            		pclientrequest(client_socket);
        	} 
        	else 
        	{
            		// PARENT PROCESS
            		close(client_socket);
        	}
    
        	client_count++;
        	fseek(count_file, 0, SEEK_SET);
        	fprintf(count_file, "%d", client_count);
        	fclose(count_file);
    	}

    	close(server_socket);
    	return 0;
}

