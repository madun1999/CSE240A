CC=gcc
OPTS=-g -std=c99 -Werror
TRACES= fp_1 fp_2 int_1 int_2 mm_1 mm_2

all: main.o predictor.o perceptron_predictor.o common.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o perceptron_predictor.o common.o

main.o: main.c predictor.h 
	$(CC) $(OPTS) -c main.c

predictor.o: predictor.h predictor.c
	$(CC) $(OPTS) -c predictor.c

perceptron_predictor.o: perceptron_predictor.h perceptron_predictor.c
	$(CC) $(OPTS) -c perceptron_predictor.c

common.o: common.h common.c
	$(CC) $(OPTS) -c common.c

test_custom_all_traces: 
	rm custom_results.txt
	for trace in $(TRACES); do \
	  echo "Testing" $$trace | tee custom_results.txt -a ; \
	  bunzip2 -kc ../traces/$$trace.bz2 | ./predictor --custom >> custom_results.txt; \
	  echo "" >> custom_results.txt ; \
	done

clean:
	rm -f *.o predictor;
