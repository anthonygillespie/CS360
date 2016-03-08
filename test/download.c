#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SOCKET_ERROR        -1
#define HOST_NAME_SIZE      255
#define URL_NAME_SIZE       255
#define MAX_CONTENT_LENGTH  1000000


/*************************************** START sample.cpp *************************************/

#include <vector>             // stl vector
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
using namespace std;

#define MAX_MSG_SZ      1024

// Determine if the character is whitespace
bool isWhitespace(char c)
{ switch (c)
    {
        case '\r':
        case '\n':
        case ' ':
        case '\0':
            return true;
        default:
            return false;
    }
}

// Strip off whitespace characters from the end of the line
void chomp(char *line)
{
    int len = strlen(line);
    while (isWhitespace(line[len]))
    {
        line[len--] = '\0';
    }
}

// Read the line one character at a time, looking for the CR
// You dont want to read too far, or you will mess up the content
char * GetLine(int fds)
{
    char tline[MAX_MSG_SZ];
    char *line;
    
    int messagesize = 0;
    int amtread = 0;
    while((amtread = read(fds, tline + messagesize, 1)) < MAX_MSG_SZ)
    {
        if (amtread >= 0)
            messagesize += amtread;
        else
        {
            perror("Socket Error is:");
            fprintf(stderr, "Read Failed on file descriptor %d messagesize = %d\n", fds, messagesize);
            exit(2);
        }
        //fprintf(stderr,"%d[%c]", messagesize,message[messagesize-1]);
        if (tline[messagesize - 1] == '\n')
            break;
    }
    tline[messagesize] = '\0';
    chomp(tline);
    line = (char *)malloc((strlen(tline) + 1) * sizeof(char));
    strcpy(line, tline);
    return line;
}
    
// Change to upper case and replace with underlines for CGI scripts
void UpcaseAndReplaceDashWithUnderline(char *str)
{
    int i;
    char *s;
    
    s = str;
    for (i = 0; s[i] != ':'; i++)
    {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = 'A' + (s[i] - 'a');
        
        if (s[i] == '-')
            s[i] = '_';
    }
    
}


// When calling CGI scripts, you will have to convert header strings
// before inserting them into the environment.  This routine does most
// of the conversion
char *FormatHeader(char *str, const char *prefix)
{
    char *result = (char *)malloc(strlen(str) + strlen(prefix));
    char* value = strchr(str,':') + 1;
    UpcaseAndReplaceDashWithUnderline(str);
    *(strchr(str,':')) = '\0';
    sprintf(result, "%s%s=%s", prefix, str, value);
    return result;
}

// Get the header lines from a socket
//   envformat = true when getting a request from a web client
//   envformat = false when getting lines from a CGI program

void GetHeaderLines(vector<char *> &headerLines, int skt, bool envformat)
{
    // Read the headers, look for specific ones that may change our responseCode
    char *line;
    char *tline;
    
    tline = GetLine(skt);
    while(strlen(tline) != 0)
    {
        if (strstr(tline, "Content-Length") || 
            strstr(tline, "Content-Type"))
        {
            if (envformat)
                line = FormatHeader(tline, "");
            else
                line = strdup(tline);
        }
        else
        {
            if (envformat)
                line = FormatHeader(tline, "HTTP_");
            else
            {
                line = (char *)malloc((strlen(tline) + 10) * sizeof(char));
                sprintf(line, "HTTP_%s", tline);                
            }
        }
        
        headerLines.push_back(line);
        free(tline);
        tline = GetLine(skt);
    }
    free(tline);
}

/************************************** END sample.cpp ****************************************/


int  main(int argc, char* argv[])
{
    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
    char strURL[URL_NAME_SIZE];
    int c;
    int cFlag;
    int cCount;
    int dFlag;

    /* Parse arguments */
    if(argc < 4)
      {
        printf("\nUsage: download host-name host-port path -c or -d\n");
        return 0;
      }
    else
      {
        cFlag = 0;
        cCount = 1;
        dFlag = 0;
        if (argc > 4) {
            while((c = getopt (argc, argv, "c:d")) != -1){
                switch (c) {
                    case 'c':
                        cFlag = 1;
                        cCount = atoi(optarg);
                        break;
                    case 'd':
                        dFlag = 1;
                        break;
                    default:
                        perror("\nERROR: CALLED DEFAULT CASE: Unknown argument found.\n");
                }

            }
        } 
        strcpy(strHostName,argv[argc-3]);
        nHostPort=atoi(argv[argc-2]);
        strcpy(strURL,argv[argc-1]); 
      }

    int i;
    for (i = 0; i < cCount; i++){

        /* Make a socket */
        if (dFlag) printf("\nMaking a socket\n");
        hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(hSocket == SOCKET_ERROR)
        {
            perror("\nERROR: Could not make a socket\n");
            return 0;
        }

        /* get IP address from name */
        pHostInfo=gethostbyname(strHostName);
        if (pHostInfo == 0){
            perror("ERROR: Invalid hostname");
            return 0;
        }
        /* copy address into long */
        memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

        /* fill address struct */
        Address.sin_addr.s_addr=nHostAddress;
        Address.sin_port=htons(nHostPort);
        Address.sin_family=AF_INET;

        /* connect to host */
        if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) 
           == SOCKET_ERROR)
        {
            perror("\nERROR: Could not connect to host (Verify port number is valid)");
            return 0;
        }

       
        /* Create HTTP Message */
        #define MAXGET 1000
        char *message =(char *)malloc(MAXGET);
        sprintf(message, "GET %s HTTP/1.0\r\nHost:%s:%d\r\n\r\n",strURL,strHostName,nHostPort);

        /* Send HTTP Message */
        if (dFlag) printf("\nRequest:\n%s\n", message);
        write(hSocket,message,strlen(message));

        /* Get the Response */
        if (dFlag) printf ("\nResponse:\n");

     /*************** sample.cpp stuff *****************/
        vector<char *> headerLines;
        char buffer[MAX_MSG_SZ];
        char contentType[MAX_MSG_SZ];

        /* First read the status line */
        char *startline = GetLine(hSocket);
        if (dFlag) printf("Status line %s\n\n",startline);

        /* Read the header lines */
        GetHeaderLines(headerLines, hSocket , false);

        int contentLength;
        /* Now print them out */
        if (dFlag) printf("Headers:\n");
        for (int i = 0; i < headerLines.size(); i++) {
            if (dFlag) printf("[%d] %s\n",i,headerLines[i]);
            if(strstr(headerLines[i], "Content-Length")) {
                    sscanf(headerLines[i], "Content-Length: %d", &contentLength);
            }
        }
        if (contentLength > MAX_CONTENT_LENGTH){
            perror("Content Length exceeds maximum size\n");
            return 0;
        }

        if (dFlag) printf("\n");
     /**************** end sample.cpp stuff ***********/

        /* Read response back from socket */
        char *pBuffer = (char *)malloc(contentLength); 
        nReadAmount=read(hSocket,pBuffer,contentLength);
        if (!cFlag) printf("%s", pBuffer);
        free(pBuffer);
        
        /* close socket */   
        if (dFlag) printf("\nClosing socket\n");                    
        if(close(hSocket) == SOCKET_ERROR)
        {
            perror("\nERROR: Could not close socket\n");
            return 0;
        } 
        free(message);
    }
    if (cFlag) printf("Successfully downloaded the page %d times\n", cCount);
}
