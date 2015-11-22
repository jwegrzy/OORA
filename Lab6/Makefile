build:
	for number in 1 2 3 5 6 8 9 ; do \
		gcc chol_$$number.c -o output/chol_$$number -O0 -lpapi -lm -static ; \
	done

run:	
	for number in 1 2 3 5 6 8 9 ; do \
		for event in 0 1 2 3 4 5 ; do \
			for size in 500 750 1000 ; do \
				./output/chol_$$number matrices/$$size.txt $$size $$event >> result/$$event-$$size.txt ; \
			done \
		done \
	done

