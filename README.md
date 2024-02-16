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
  a.**User-Friendly Interaction:**
    - Generated a friendly command prompt using a random string (cRs) to enhance the user experience.
    - Allowed users to input commands such as filenames, date specifications, and file extensions.

  b.**Command Execution:**
    - Sent commands to the server or mirror based on user input.
    - Executed commands asynchronously and received tar.gz files from the server or mirror.

**Server-Side Implementation:**
  a.**Concurrent Server:**
    - Managed multiple client connections concurrently, utilizing fork.
    - Executed commands from clients, performed necessary operations, and sent back tar.gz files.
