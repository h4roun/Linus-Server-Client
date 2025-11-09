# Linux FIFO Client-Server System

A comprehensive inter-process communication (IPC) system demonstrating named pipes (FIFOs) in Linux, featuring a C-based server and client implementation with a modern web interface. The system showcases real-time IPC mechanisms, signal handling, and web integration.

## Project Overview

This project implements:
- A server process that handles multiple client requests
- Client processes that can request random numbers
- Web interface for real-time interaction
- Python Flask backend bridging web and native components

### Key Features

- **Native IPC Using FIFOs**: Implements bi-directional communication using named pipes
- **Signal-based Process Coordination**: Uses SIGUSR1 for inter-process synchronization
- **Multi-client Support**: Server can handle multiple client connections
- **Web Integration**: Modern web interface for easy interaction
- **Real-time Updates**: Asynchronous updates in the web UI
- **Robust Error Handling**: Comprehensive error management for IPC operations

## Technical Architecture

### Components

1. **Server (`serveur.c`):**
   - Creates and manages named pipes (`/tmp/fifo1`, `/tmp/fifo2`)
   - Handles client requests for random numbers
   - Uses signal handlers for graceful shutdown (SIGINT, SIGTERM)
   - Implements non-blocking I/O for efficient communication

2. **Client (`client.c`):**
   - Connects to server's named pipes
   - Sends requests for random numbers
   - Handles server responses asynchronously
   - Uses signal handlers for synchronization

3. **Web Client (`client_web.c`):**
   - Specialized client implementation for web interface
   - Adapted for integration with Flask backend

4. **Web Backend (`app.py`):**
   - Flask server serving the web interface
   - API endpoints for server control and client requests
   - Bridges web UI with native C components

### Communication Protocol

The system uses a custom protocol defined in `serv_cli_fifo.h`:

\`\`\`c
typedef struct {
    pid_t pid_client;  // Client process ID
    int n;            // Number of random values requested
} Question;

typedef struct {
    int nb_valeurs;   // Number of values in response
    int valeurs[NMAX]; // Array of random values
} Reponse;
\`\`\`

## Building and Running

### Prerequisites
- Linux environment (or WSL on Windows)
- GCC compiler
- Python 3.8+ with pip
- Make utility

### Installation

1. Clone the repository:
   \`\`\`bash
   git clone https://github.com/h4roun/Linus-Server-Client.git
   cd Linus-Server-Client
   \`\`\`

2. Build the C components:
   \`\`\`bash
   make
   \`\`\`

3. Install Python dependencies:
   \`\`\`bash
   python -m pip install -r requirements.txt
   \`\`\`

### Running the System

1. Start the native server:
   \`\`\`bash
   ./serveur
   \`\`\`
   Note the displayed PID.

2. Run a native client:
   \`\`\`bash
   ./client <server_pid>
   \`\`\`
   Replace `<server_pid>` with the server's PID.

3. Start the web interface:
   \`\`\`bash
   python app.py
   \`\`\`
   Access the web interface at http://localhost:5000

## Project Structure

- `serveur.c` - Main server implementation
- `client.c` - Native client implementation
- `client_web.c` - Web-adapted client
- `serv_cli_fifo.h` - Common definitions and structures
- `Handlers_Serv.h` - Server signal handlers
- `Handlers_Cli.h` - Client signal handlers
- `app.py` - Flask web backend
- `index.html` - Web interface
- `Makefile` - Build configuration
- `requirements.txt` - Python dependencies

## Implementation Details

### Server Features
- Non-blocking FIFO operations
- Signal-based client synchronization
- Random number generation
- Clean shutdown handling
- Multiple client support

### Client Features
- Command-line interface
- Asynchronous response handling
- Signal-based synchronization
- Error recovery

### Web Integration
- Real-time updates using Flask
- Clean process management
- Error handling and recovery
- User-friendly interface

## Error Handling

The system implements comprehensive error handling for:
- FIFO creation/opening failures
- Process communication errors
- Signal handling issues
- Resource cleanup
- Web backend errors

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to your branch
5. Create a Pull Request

## Troubleshooting

### Common Issues

1. **FIFO Creation Errors**
   - Check directory permissions in /tmp
   - Ensure no stale FIFOs exist

2. **Client Connection Issues**
   - Verify server is running
   - Check server PID is correct
   - Ensure FIFOs are accessible

3. **Web Interface Problems**
   - Check Python dependencies
   - Verify port 5000 is available
   - Check Flask server logs

### Debug Tips
- Use `strace` to monitor system calls
- Check /tmp for FIFO existence
- Monitor process signals with `ps`
- Review Flask logs for web issues

## License

MIT License - See LICENSE file for details.

## Acknowledgments

This project demonstrates various Linux IPC mechanisms and system programming concepts including:
- Named pipes (FIFOs)
- Signal handling
- Process management
- Web integration
- Real-time communication

For questions or issues, please open a GitHub issue.
