# Memory Management Simulation
<p align="center">
  <img width = 430 height = 216 src="img.png">
</p>

<br />Simulation of Memory Management System using LRU and WS Algorithms.
Reading from two files containing instruction details (memory traces and type) of processes, a simulation of visual memory based on hashed table pages is executed. Page replacements, disk writes/reads, page faults, number of traces and number of memory frames are counted and showed.

## Deployment

The code was developed and tested on **Linux Ubuntu 16.04 LTS** using **gcc v5.4.0**.

## Prerequisites

A Linux distribution is required with a gcc compiler order to compile and execute. _gcc v5.4.0_ or older is recommended.
<br />You can check your gcc compiler version using:
<br />``` $ gcc --version ```

You can install the latest gcc compiler on Ubuntu based systems using:
<br />``` $ sudo apt install build-essential ``` 

## Getting Started
Download Repository Locally using:
<br /> ```$ git clone https://github.com/AndCharalampous/memory-simulation.git ```

## Compilation
Compile code guiding in src directory and using the Makefile:
<br /> ```$ cd src```
<br /> ```$ make```
<br /> Executable file exe2 is created.

### Execution
Execute using:
<br /> ```$ ./exe2 ra fr q wss nR```
<br />ra: Replacement Algorithm {LRU, WS}
<br />fr: Memory size in frames
<br />q: Number of consecutive traces executed per step
<br />wss: Size of Working Set (valid only when ra = WS)
<br />nR: Number of maximum traces read per trace file

### Results
Results are printed to stdout after succesful execution.

## Author

**Andreas Charalampous** - https://github.com/AndCharalampous

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
