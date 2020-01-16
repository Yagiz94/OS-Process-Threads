Participant:
	Yagiz Gani
	
Definition: 	
-> Using multiple processes and threads histogram is implemented in this project.
-> syn_phistogram uses multiprocess execution and syn_thistogram uses multi-thread execution.

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
