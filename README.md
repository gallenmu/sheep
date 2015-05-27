# Simulation algoritHms for Empirical Evaluation of Processor performance (SHEEP)

This repository offers tools to simulate low level load on x86/x64 CPUs. 
The CPULoader consumes a specified number of CPU cycles, the CacheLoader performs CPU cache accesses in a hard-to-prefetch scenario. 
Also, low level benchmarking methods are provided, which are explained by [Intel](http://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf) [1].
The cache testing tool is based on ideas by [Ulrich Drepper](http://lwn.net/Articles/252125/).

The tools were used for performance evaluation of packet processing frameworks published in our [paper](http://www.net.in.tum.de/fileadmin/bibtex/publications/papers/gallenmueller_ancs2015.pdf) [2]. 

# References
[1] Gabriele Paoloni. How to Benchmark Code Execution Times on Intel IA-32 and IA-64 Instruction Set Architectures, September 2010.

[2] Sebastian Gallenmüller, Paul Emmerich, Florian Wohlfart, Daniel Raumer, and Georg Carle. Comparison of Frameworks for High-Performance Packet IO. In ACM/IEEE Symposium on Architectures for Networking and Communications Systems (ANCS 2015), May 2015.


