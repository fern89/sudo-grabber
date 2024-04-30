# sudo-grabber
Grab passwords entered into su/sudo using LD_PRELOAD hooks. 


![image](https://github.com/fern89/sudo-grabber/assets/139056562/cd01a6a1-1a6c-49c1-a126-4aaee8206de4)

## Methodology
We use a LD_PRELOAD hook in the read() call in the parent shell process (tested to work with bash and sh), to detect when a `su` or a `sudo` is run. Then we deploy a thread to open a blocking read() in the terminal, then allow the original read() call to complete and spawn a su password prompt. But when su tries to send a read() to terminal, the malicious thread's blocking read() is present first. Results in the user's password being sent to your thread. After reading the user's password, the malicious thread then sends the entered password to the read() summoned by su, allowing the user to not notice anything suspicious (if password entered is correct, su auth will work). While you get a copy of the password saved/transferred to your location of your choice.

## Compilation/Use
Compile with `gcc -shared -fPIC -ldl -o inject.so inject.c`. Run with `LD_PRELOAD=./inject.so bash`. Note this tool makes no attempt to conceal itself (ie a simple `echo $LD_PRELOAD` will reveal presence).

## Credits
Credits to https://lcamtuf.substack.com/p/that-time-i-built-an-ld_preload-worm for the idea to abuse su/sudo in this way
