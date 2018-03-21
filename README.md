Selective Overhead Mirroring System
Implementation of distributed transferring of files and directories from many Worker-Servers to one Manager-Server, using Shared Memory
, Multi- threading , Mutexes , Condition Variables and sockets for communication
between the threads running on different machines.

Compilation : make

Run: First run the ContentServer  like this -> ./ContentServer -p 3020 -d my_folder,
then run the Miror-Server like this-> ./MirrorServer -p 3000 -m new_foler -w 4,
then start the MirrorInitiator like this -> ./MirrorInitiator -n localhost -p 3000 -s localhost:3001:folder1/:file1 , localhost:3001:folder2/
