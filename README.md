# File transfer: ASP

**Distributed File Sharing System - Client.c, Server.c & Mirror.c:**
This project introduces a distributed file-sharing system comprising three crucial components: **Client.c**, **Server.c**, and **Mirror.c**. The system facilitates seamless file retrieval and information queries through a client-server model. The client interacts with the server or mirror based on availability, providing a fault-tolerant experience.

**Client.c** serves as the user interface, enabling users to initiate commands like requesting file information, retrieving files based on extensions or creation dates, and quitting the client-server connection. The project employs a dynamic allocation approach, ensuring the first four connections are directed to the server, and subsequent connections alternate between the server and mirror instances. The utilization of the `fork` system call enhances parallelism, allowing the system to handle multiple client requests concurrently.

The server (**Server.c**) and mirror (**Mirror.c**) components share a common codebase but operate on different port numbers. They collaboratively manage client requests, with the mirror seamlessly taking over when the server is unavailable. This dual-server architecture, combined with the use of `fork` for parallel processing, optimizes resource utilization and ensures consistent file-sharing performance. Together, these components contribute to a fault-tolerant, distributed file-sharing system capable of adapting to dynamic network conditions.

I designed and implemented a Concurrent Client-Server File Transfer System in C, utilizing socket programming and fork for concurrency. The primary goal was to enable efficient communication between multiple clients and a server or mirror.

**Features**

**Concurrency with Fork:**
  - Employed the fork() system call to manage multiple clients concurrently.
  - Dynamic connection handling: Clients connected to the server or a mirror based on the client count, enhancing scalability.

**Client Commands:**
  - Clients could issue commands like getfn {filename}, getft {ext1} {ext2} {ext3}, etc.
  - Implemented a robust syntax-checking mechanism to ensure correct command input.

**File Handling:**

  - File transfer functionality using tar.gz files. Clients received tar.gz files in response to specific commands, and files were saved in the “f23project” folder in the home directory.

**Random Number Generation:**
  - Created a custom function for random number generation (gmrn) to generate unique tar filenames.

**Error Handling:**
  - Implemented error handling for socket creation and file operations, enhancing the system’s reliability.

**Client-Side Implementation:**

  a. **User-Friendly Interaction:**
    - Generated a friendly command prompt using a random string (cRs) to enhance the user experience.
    - Allowed users to input commands such as filenames, date specifications, and file extensions.

  b. **Command Execution:**
    - Sent commands to the server or mirror based on user input.
    - Executed commands asynchronously and received tar.gz files from the server or mirror.

**Server-Side Implementation:**

  a. **Concurrent Server:**
    - Managed multiple client connections concurrently, utilizing fork.
    - Executed commands from clients, performed necessary operations, and sent back tar.gz files.


**Get Started:**

### Step 1: Download Debian 12 ISO

Visit the official Debian website and download the Debian 12 ISO image. You can find it at [Debian Downloads](https://cdimage.debian.org/debian-cd/current/amd64/iso-dvd/).

### Step 2: Install VMware

If you don't have VMware installed, download and install VMware Workstation Player or VMware Workstation Pro from the [official VMware website](https://www.vmware.com/).

### Step 3: Create a New Virtual Machine

1. Open VMware and click on "Create a New Virtual Machine."

2. Choose "Installer disc image file (iso)" and browse to the location where you saved the Debian 12 ISO.

3. Complete the virtual machine creation wizard by specifying the guest operating system as "Linux" and version as "Debian 11.x or later."

### Step 4: Customize Virtual Machine Settings

1. Before starting the VM, you might want to customize its settings. Click on "Edit virtual machine settings."

2. Adjust parameters such as memory, CPU, and disk space according to your preferences and system resources.

### Step 5: Start the Virtual Machine

1. Start the virtual machine. It will boot from the Debian 12 ISO.

2. Follow the on-screen instructions to complete the Debian installation. This includes selecting language, region, keyboard layout, setting up users, and configuring the disk.

### Step 6: Complete the Installation

1. After Debian is installed, the system will prompt you to remove the installation media (ISO). Ensure you eject the ISO or unmount it from the virtual CD/DVD drive.

2. Finish the installation by following any remaining on-screen prompts.

### Step 7: Install VMware Tools

1. After the Debian VM is running, install VMware Tools for better integration between the guest and host systems.

   ```bash
   sudo apt update
   sudo apt install open-vm-tools
   ```

2. Follow the prompts to complete the installation.

### Step 8: Update and Upgrade

For security and stability, update and upgrade the Debian system:

```bash
sudo apt update
sudo apt upgrade
```

### Step 9: clone repository 
Clone a Git repository on your Debian 12 machine, you can follow these steps:

- Open a terminal on your Debian 12 machine.
  
```bash
sudo apt update
sudo apt install git
```

- Navigate to your home directory (you can use the `cd` command):

   ```bash
   cd ~
   ```

-  Clone the repository using the `git clone` command:

   ```bash
   git clone https://github.com/shashankkannan/Distributed-File-transfer--ASP.git
   ```

-  Enter the cloned repository directory:

   ```bash
   cd Distributed-File-transfer--ASP
   ```

Now you have successfully cloned the repository to your Debian 12 machine.


### Step 10: Run server.c, mirror.c
Certainly! I'll explain the steps you should take and the expected behavior for each command as part of the README page:

### Cloning the Repository:

1. **Clone the Repository:**
    - On the Debian 12 machine, navigate to the home directory using the terminal: `cd ~`
    - Clone the repository using the following command:
      ```bash
      git clone https://github.com/shashankkannan/Distributed-File-transfer--ASP.git
      ```
    - Enter the cloned repository directory: `cd Distributed-File-transfer--ASP`

### Running Server and Client:

1. **Compile and Run the Server (`server.c``mirror.c`):**
    - Compile and run your server application (`server.c``mirror.c`) that will handle file transfer operations.

2. **Compile and Run the Client (`client.c`):**
    - Open different terminals or machines.
    - In each terminal, compile and run the client application (`client.c`).

### Client Commands:

1. **`getfn filename`:**
    - If the file with the specified name exists in the directory tree rooted at ~, the server will return information such as filename, size (in bytes), date created, and file permissions.
    - The client will print the received information on its terminal.
    - If the file is not found, the client will print "File not found."

    Example:
    ```bash
    Client$ getfn sample.txt
    ```

2. **`getfz size1 size2`:**
    - The server will return a compressed file (`temp.tar.gz`) containing all files in the directory tree rooted at ~ with sizes between `size1` and `size2` (inclusive).
    - If no files match the specified size range, the server sends "No file found" to the client.

    Example:
    ```bash
    Client$ getfz 1240 12450
    ```

3. **`getft <extension list>`:**
    - The server will return a compressed file (`temp.tar.gz`) containing all files in the directory tree rooted at ~ with specified file types.
    - If no files match the specified file types, the server sends "No file found" to the client.

    Example:
    ```bash
    Client$ getft c txt
    Client$ getft jpg bmp pdf
    ```

4. **`getfdb date`:**
    - The server will return a compressed file (`temp.tar.gz`) containing all files in the directory tree rooted at ~ created on or before the specified date.

    Example:
    ```bash
    Client$ getfdb 2023-01-01
    ```

5. **`getfda date`:**
    - The server will return a compressed file (`temp.tar.gz`) containing all files in the directory tree rooted at ~ created on or after the specified date.

    Example:
    ```bash
    Client$ getfda 2023-03-31
    ```


