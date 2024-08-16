# 📂 Distributed File System

## 📝 Overview
This project showcases a **Distributed File System** built using **C** and **socket programming**. The system is designed with three specialized servers: **Smain**, **Spdf**, and **Stext**, each handling a specific file type (.c, .pdf, .txt). Clients interact exclusively with the **Smain** server, which seamlessly manages file storage and retrieval, distributing files to the appropriate servers behind the scenes.

## ✨ Features
- **Multi-server Architecture**: Efficiently manages different file types across multiple servers for optimized storage.
- **Socket Programming**: Ensures robust and seamless communication between clients and servers.
- **Client Transparency**: Clients operate under the assumption that all files are stored on Smain, unaware of the underlying file distribution.
- **Command-based Operations**: Supports a variety of operations, including file uploads, downloads, deletions, tar file creation, and directory content display.

## 📂 Project Structure
- **`Smain.c`**: Central server managing client requests and coordinating with `Spdf` and `Stext` servers.
- **`Spdf.c`**: Dedicated server for managing and storing `.pdf` files.
- **`Stext.c`**: Dedicated server for managing and storing `.txt` files.
- **`client24s.c`**: Client program that interacts with the `Smain` server to execute commands.

## 🛠️ Commands
- **`ufile`**: Upload a file to the server.
- **`dfile`**: Download a file from the server.
- **`rmfile`**: Remove a file from the server.
- **`dtar`**: Create and download a tar file of a specific file type.
- **`display`**: Display the contents of a directory.

## 🚀 Setup and Usage
1. **Compile the server and client programs using GCC**:
   ```bash
   gcc -o Smain Smain.c
   gcc -o Spdf Spdf.c
   gcc -o Stext Stext.c
   gcc -o client24s client24s.c
