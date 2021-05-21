#ch3 Project1 UNIX SHELL: SimpleShell
excute:
```
gcc -o SimpleShell SimpleShell.c
./SimpleShell
```
then the shell opens.

The shell supports
1. basic commands
2. && added to the end of the command to let the command execute in background
3. !! command to re-execute the last command. If no last command exists, an error message will occur.
4. > input redirection. For example, cd > in.txt, when the contents of in.txt is /proc, will cd to file /proc. However, && in input file is not supported.
5. $\<$ output redirection.the result of the command will be writen in the file followed by <
6. singel pipe, command like ls -l | less is supported.

# ch2 Prject2 Linux Kernel Module for Task Information
test method
1. make
```
compile the module
```
2. insmod
if the module pid has been inserted, this step can be skipped.

3. echo "1395" > /proc/pid
write to pid

4. cat /proc/pid
read /proc/pid
 
### To be done

The basic functions are finished. 
Need to be added: 1. error detection 2.&& in in.txt file(current loop and next !! loop) 3. segment safety(test wild pointer, ensure free)
