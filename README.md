# Distributed File System
Overview
This project implements a Distributed File System using socket programming in C. The system is designed with three servers: Smain, Spdf, and Stext, each responsible for handling specific file types (.c, .pdf, .txt). Clients interact with the Smain server, which transparently manages file storage and retrieval, distributing files to the appropriate servers in the background.

Features
- Multi-server Architecture: Efficiently manages different file types across multiple servers.
Socket Programming: Ensures seamless communication between clients and servers.
Client Transparency: Clients are unaware of file distribution across servers, interacting only with the Smain server.
Command-based Operations: Supports uploading, downloading, deleting files, creating tar files, and displaying directory contents.
Project Structure
Smain.c: Handles the main server operations, managing client requests and communicating with Spdf and Stext servers.
Spdf.c: Dedicated server for managing and storing .pdf files.
Stext.c: Dedicated server for managing and storing .txt files.
client24s.c: Client program that sends commands to the Smain server.
Commands
ufile: Upload a file to the server.
dfile: Download a file from the server.
rmfile: Remove a file from the server.
dtar: Create and download a tar file of a specific file type.
display: Display the contents of a directory.
Setup and Usage
Compile the server and client programs using GCC:
sh
Copy code
gcc -o Smain Smain.c
gcc -o Spdf Spdf.c
gcc -o Stext Stext.c
gcc -o client24s client24s.c
Run the servers (Smain, Spdf, Stext) on different terminals or machines.
Run the client program and enter the desired commands.
Tech Stack
C Programming
Socket Programming
Linux
Contributors
Rajvee Kalpeshbhai Rupani
