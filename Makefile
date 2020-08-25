all: mt_search

mt_search: mt_search.c
	gcc -pthread -o mt_search mt_search.c

run: mt_search
	./mt_search input1.txt 1 3 13

clean:
	rm mt_search
