Participant:
	Yagiz Gani
	
Definition: 

-> In this project, a Histogram is implemented by using multiple processes and multiple threads.

-> Semaphore is implemented to manage process synchronization & mutex lock is used to establish thread synchronization.

-> syn_phistogram.c uses multi-process execution and syn_thistogram.c uses multi-thread execution.

In syn_phistogram.c: 

		argv[1] = minvalue
		argv[2] = maxvalue
		argv[3] = bincount
		argv[4] = N (child process no)
		argv[i+5] = filename1
		.
		.	
		.
		.
		argv[last] = outputfile

In syn_thistogram.c:

		argv[1] = minvalue
		argv[2] = maxvalue
		argv[3] = bincount
		argv[4] = N ( running threads )
		argv[i+5] = filename1
		.
		.	
		.
		.
		argv[last-1] -> outputfile
		argv[last] = batchsize // size for thread read
