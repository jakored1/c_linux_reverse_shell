# A simple reverse shell in C
To help me learn the C language, and better understand how to work with sockets in the language, I decided to make a reverse shell.

**DO NOT USE IN PRODUCTION OR ANYTHING SERIOUS!**  
The shell is not perfect and I'm sure it has bugs.  
It was made purely as a learning project.  
I might continue to add features to it in the future, and the day I feel it is production ready I will change this descrption :)

### Future plans (If I get to them)
- Currently the size of commands and responses that can be sent from the server to the client (or client to server) is limited. I would like to be able to support transferring any size of data.
- Add upload/download files feature to the special menu
- Support Ctrl+C (have it be sent to the client session instead of it closing the server - also support other stuff like Ctrl+U, Ctrl+Z, etc.)
- Support left/right arrow keys to navigate between characters in the command before it is sent, and up/down arrow keys to navigate between previously sent commands
