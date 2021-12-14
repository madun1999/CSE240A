CC=gcc
OPTS=-g -std=c99 -Werror
TRACES= fp_1 fp_2 int_1 int_2 mm_1 mm_2

all: exec

exec: main.o predictor.o perceptron_predictor.o common.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o perceptron_predictor.o common.o

main.o: main.c predictor.h 
	$(CC) $(OPTS) -c main.c

predictor.o: predictor.h predictor.c
	$(CC) $(OPTS) -c predictor.c

perceptron_predictor.o: perceptron_predictor.h perceptron_predictor.c
	$(CC) $(OPTS) -c perceptron_predictor.c

common.o: common.h common.c
	$(CC) $(OPTS) -c common.c

test_custom_all_traces: exec
	rm custom_results.txt
	for trace in $(TRACES); do \
	  echo "Testing" $$trace | tee custom_results.txt -a ; \
	  bunzip2 -kc ../traces/$$trace.bz2 | ./predictor --custom >> custom_results.txt; \
	  echo "" >> custom_results.txt ; \
	done

test_gshare_all_traces: exec
	rm gshare_results.txt
	for trace in $(TRACES); do \
	  echo "Testing" $$trace | tee gshare_results.txt -a ; \
	  bunzip2 -kc ../traces/$$trace.bz2 | ./predictor --gshare:13 >> gshare_results.txt; \
	  echo "" >> gshare_results.txt ; \
	done

test_tournament_all_traces: exec
	rm tournament_results.txt
	for trace in $(TRACES); do \
	  echo "Testing" $$trace | tee tournament_results.txt -a ; \
	  bunzip2 -kc ../traces/$$trace.bz2 | ./predictor --tournament:9:10:10 >> tournament_results.txt; \
	  echo "" >> tournament_results.txt ; \
	done

clean:
	rm -f *.o predictor;
