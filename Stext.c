#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define PORT 8082    // Port number for Stext server
#define BUFSIZE 1024 // Buffer size for communication

// ANSI escape codes for colorizing output
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"

// Function to send a message to Smain
void send_message_to_smain(int smain_socket, char *message)
{
    write(smain_socket, message, strlen(message));
}

// Function to handle file upload from Smain
void handle_ufile(int smain_socket, char *filename, char *destination_path)
{
    char full_path[BUFSIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s/%s", getenv("HOME"), "stext", destination_path);

    char command[BUFSIZE];
    snprintf(command, sizeof(command), "mkdir -p %s", full_path);
    if (system(command) != 0)
    {
        send_message_to_smain(smain_socket, COLOR_RED "Error: Could not create directory\n" COLOR_RESET);
        return;
    }

    strncat(full_path, "/", BUFSIZE - strlen(full_path) - 1);
    strncat(full_path, filename, BUFSIZE - strlen(full_path) - 1);

    FILE *dest = fopen(full_path, "wb");
    if (!dest)
    {
        perror(COLOR_RED "File creation failed" COLOR_RESET);
        char error_message[BUFSIZE];
        snprintf(error_message, BUFSIZE, COLOR_RED "Error: Could not create file %s\n" COLOR_RESET, full_path);
        send_message_to_smain(smain_socket, error_message);
        return;
    }

    char buffer[BUFSIZE];
    ssize_t bytes;
    while ((bytes = read(smain_socket, buffer, BUFSIZE)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
        if (bytes < BUFSIZE) // assuming Smain will close connection after sending file
            break;
    }

    fclose(dest);
    send_message_to_smain(smain_socket, COLOR_GREEN "File uploaded successfully to Stext\n" COLOR_RESET);
}

// Function to handle file download request from Smain
void handle_dfile(int smain_socket, char *filename)
{
    char full_path[BUFSIZE];
    snprintf(full_path, sizeof(full_path), "%s/stext/%s", getenv("HOME"), filename);

    FILE *src_file = fopen(full_path, "rb");
    if (!src_file)
    {
        perror(COLOR_RED "File open failed" COLOR_RESET);
        char error_message[BUFSIZE];
        snprintf(error_message, BUFSIZE, COLOR_RED "Error: Could not open file %s\n" COLOR_RESET, full_path);
        send_message_to_smain(smain_socket, error_message);
        return;
    }

    char buffer[BUFSIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUFSIZE, src_file)) > 0)
    {
        if (write(smain_socket, buffer, bytes) < 0)
        {
            perror(COLOR_RED "Error sending file data" COLOR_RESET);
            break;
        }
    }
    fclose(src_file);

    send_message_to_smain(smain_socket, COLOR_GREEN "File transfer complete from Stext\n" COLOR_RESET);
}

// Function to handle file removal request from Smain
void handle_rmfile(int smain_socket, char *filename)
{
    char full_path[BUFSIZE];
    snprintf(full_path, sizeof(full_path), "%s/stext/%s", getenv("HOME"), filename);

    if (remove(full_path) == 0)
    {
        send_message_to_smain(smain_socket, COLOR_GREEN "File removed successfully from Stext\n" COLOR_RESET);
    }
    else
    {
        perror(COLOR_RED "File removal failed" COLOR_RESET);
        char error_message[BUFSIZE];
        snprintf(error_message, BUFSIZE, COLOR_RED "Error: Could not remove file %s\n" COLOR_RESET, full_path);
        send_message_to_smain(smain_socket, error_message);
    }
}

// Function to create a tarball of all .txt files
void handle_dtar(int smain_socket)
{
    char tar_filename[BUFSIZE];
    snprintf(tar_filename, sizeof(tar_filename), "%s/txtfiles.tar", getenv("HOME"));

    char command[BUFSIZE];
    snprintf(command, sizeof(command), "tar -cvf %s -C %s/stext .", tar_filename, getenv("HOME"));
    if (system(command) != 0)
    {
        send_message_to_smain(smain_socket, COLOR_RED "Error: Tarball creation failed in Stext\n" COLOR_RESET);
        return;
    }

    FILE *tar_file = fopen(tar_filename, "rb");
    if (!tar_file)
    {
        perror(COLOR_RED "Failed to open tar file" COLOR_RESET);
        send_message_to_smain(smain_socket, COLOR_RED "Error: Failed to open tar file in Stext\n" COLOR_RESET);
        return;
    }

    char buffer[BUFSIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BUFSIZE, tar_file)) > 0)
    {
        if (write(smain_socket, buffer, bytes) != bytes)
        {
            perror(COLOR_RED "Failed to send tar file contents" COLOR_RESET);
            break;
        }
    }

    fclose(tar_file);
    send_message_to_smain(smain_socket, COLOR_GREEN "Tar file transfer complete from Stext\n" COLOR_RESET);

    if (remove(tar_filename) != 0)
    {
        perror(COLOR_RED "Failed to delete tar file" COLOR_RESET);
    }
}

// Function to handle requests from Smain
void prcclient(int smain_socket)
{
    char buffer[BUFSIZE];
    int n;

    while ((n = read(smain_socket, buffer, BUFSIZE)) > 0)
    {
        buffer[n] = '\0';

        char *command = strtok(buffer, " ");
        if (strcmp(command, "ufile") == 0)
        {
            char *filename = strtok(NULL, " ");
            char *destination_path = strtok(NULL, " ");
            if (filename && destination_path)
            {
                handle_ufile(smain_socket, filename, destination_path);
            }
            else
            {
                send_message_to_smain(smain_socket, COLOR_RED "Error: Invalid command format for ufile\n" COLOR_RESET);
            }
        }
        else if (strcmp(command, "dfile") == 0)
        {
            char *filename = strtok(NULL, " ");
            if (filename)
            {
                handle_dfile(smain_socket, filename);
            }
            else
            {
                send_message_to_smain(smain_socket, COLOR_RED "Error: Invalid command format for dfile\n" COLOR_RESET);
            }
        }
        else if (strcmp(command, "rmfile") == 0)
        {
            char *filename = strtok(NULL, " ");
            if (filename)
            {
                handle_rmfile(smain_socket, filename);
            }
            else
            {
                send_message_to_smain(smain_socket, COLOR_RED "Error: Invalid command format for rmfile\n" COLOR_RESET);
            }
        }
        else if (strcmp(command, "dtar") == 0)
        {
            handle_dtar(smain_socket);
        }
        else
        {
            send_message_to_smain(smain_socket, COLOR_RED "Error: Unknown command\n" COLOR_RESET);
        }
    }

    if (n < 0)
    {
        perror(COLOR_RED "Read failed" COLOR_RESET);
    }
    close(smain_socket);
}

int main()
{
    int server_socket, smain_socket;
    struct sockaddr_in server_addr, smain_addr;
    socklen_t smain_len = sizeof(smain_addr);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror(COLOR_RED "Socket failed" COLOR_RESET);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror(COLOR_RED "Bind failed" COLOR_RESET);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0)
    {
        perror(COLOR_RED "Listen failed" COLOR_RESET);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf(COLOR_GREEN "Stext server listening on port %d\n" COLOR_RESET, PORT);

    while (1)
    {
        if ((smain_socket = accept(server_socket, (struct sockaddr *)&smain_addr, &smain_len)) < 0)
        {
            perror(COLOR_RED "Accept failed" COLOR_RESET);
            continue;
        }

        if (fork() == 0)
        {
            close(server_socket);
            prcclient(smain_socket);
            close(smain_socket);
            exit(0);
        }
        else
        {
            close(smain_socket);
        }

        waitpid(-1, NULL, WNOHANG);
    }

    return 0;
}