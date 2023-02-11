
Some quick notes on updating for running with M1 macs

- required updates to mex maker
- requires specifying target architecture of x86\_64

MATLAB uses Rosetta 2 to run Intel code on M1 (true for 2020b and I think is still valid in 2022/2023). This means that the code needs to be compiled for a target architecture of x86\_64 (targeting relevant libraries). 

However, SIMD instructions can't just translate naturally.

rough notes ...

- tried compile time detection of M1 mac - couldn't get to work
- now MATLAB passes defined variable to let code know it is working with M1 MAC (?? name)
- tried importing neon code, but that is not relevant as targeting Rosetta 2 and not ARM
- tried not importing intel intrinsics header, but SIMD not recognized - weird error about inlining failing to call always inline (lots of these on SO) 
- it turned out, I need the Intel intrinsics but was missing the arch flag
- adding the arch flag let me know that GCC 11 didn't support the arch flag
- some more searching and I found out I needed the Rosetta 2 GCC, not the ARM GCC
- this meant basically installing a second version of homebrew. I took notes on how to do this in the mex\_turtle\_json file
- once this was done and the arch flag was in place things mostly worked BUT ...
- performance sucked!!!! Still don't know why. 
- added NO\_SIMD flag and wrote "roughly" equivalent non-SIMD code and performance increased by maybe a factor of 10x
- note this new non-SIMD code is not as good about checking for valid JSON so some invalid files may slide past as being OK. This code is only enabled for M1 macs

