/*
 * main.cpp
 *
 *  Created on: May 13, 2014
 *      Author: Mladen Dobrichev
 */

#include "fsss2.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
//#include <set>
#include <vector>

struct ch81 {
	char chars[81];
	bool operator < (const ch81 &rhs) const {
		return (memcmp(this, &rhs, sizeof(ch81)) < 0);
	}
	static int toString(const void* c, char *p) {
		int nClues = 0;
		for(int i = 0; i < 81; i++) {
			if(((const char*)c)[i]) {
				p[i] = ((const char*)c)[i] + '0';
				nClues++;
			}
			else
				p[i] = '.';
		}
		return nClues;
	}
	int toString(char *p) const {
		int nClues = 0;
		for(int i = 0; i < 81; i++) {
			if(chars[i]) {
				p[i] = chars[i] + '0';
				nClues++;
			}
			else
				p[i] = '.';
		}
		//p[81] = 0;
		return nClues;
	}
	int fromString(const char *p) {
		int nClues = 81;
		for(int i = 0; i < 81; i++) {
			chars[i] = p[i] - '0';
			if(chars[i] <= 0 || chars[i] > 9) {
				chars[i] = 0;
				nClues--;
			}
		}
		return nClues;
	}
	void clear() {
		memset(chars, 0, 81);
	}
};

//typedef std::set<ch81> puzzleSet;
typedef std::vector<ch81> puzzles;

int main(int argc, char* argv[])
{
	clock_t start, finish;
	int ret = 0;
	start = clock();

	char p[2000];
	fsss2 s;
	puzzles pSource;
	while(fgets(p, sizeof(p), stdin)) {
		ch81 c;
		memcpy(c.chars, p, 81);
		pSource.push_back(c);
	}
	size_t batchSize = pSource.size();
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1) private(s)
#endif //_OPENMP
	for(size_t x = 0; x < batchSize; x++) {
		int e[88]; //empty positions
		char res[88]; //result to print
		char invalid[88][10]; //invalid values
		int n = 0;
		ch81 ps = pSource[x];
		ch81 c;
		for(int i = 0; i < 81; i++) c.chars[i] = (ps.chars[i] <= '9' && ps.chars[i] >='0' ? ps.chars[i] - '0' : (e[n++] = i, 0));
		//find and store the impossible pencilmarks
		for(int i1 = 0; i1 < n; i1++) {
			int p1 = e[i1];
			for(char v1 = 1; v1 <= 9; v1++) {
				c.chars[p1] = v1;
				invalid[i1][(int)v1] = (0 == s.solve(c.chars, 1, NULL) ? 1 : 0);
			}
			c.chars[p1] = 0;
		}
		for(int i1 = 0; i1 < n - 2; i1++) {
			int p1 = e[i1];
			for(char v1 = 1; v1 <= 9; v1++) {
				if(invalid[i1][(int)v1]) continue;
				c.chars[p1] = v1;
				//if(!s.isIrreducible(c.chars)) continue;
				//if(0 == s.solve(c.chars, 1, NULL)) continue;
				for(int i2 = i1 + 1; i2 < n - 1; i2++) {
					int p2 = e[i2];
					for(char v2 = 1; v2 <= 9; v2++) {
						if(invalid[i2][(int)v2]) continue;
						c.chars[p2] = v2;
						if(0 == s.solve(c.chars, 1, NULL)) continue;
						//if(!s.isIrreducible(c.chars)) continue;
						for(int i3 = i2 + 1; i3 < n; i3++) {
							int p3 = e[i3];
							for(char v3 = 1; v3 <= 9; v3++) {
								if(invalid[i3][(int)v3]) continue;
								//num[0]++;
								c.chars[p3] = v3;
								//ps.insert(c);
								if(1 != s.solve(c.chars, 2, NULL)) continue;
								for(int i = 0; i < 81; i++) {
									res[i] = c.chars[i] ? c.chars[i] + '0' : '.';
								}
#ifdef _OPENMP
#pragma omp critical (out)
#endif //_OPENMP
								{
									printf("%81.81s\n", res);
									fflush(stdout);
								}
							}
							c.chars[p3] = 0;
						}
					}
					c.chars[p2] = 0;
				}
			}
			c.chars[p1] = 0;
		}
//		fprintf(stderr, ".");
	}

	finish = clock();
	//fprintf(stderr, "\n%d+%d+%d puzzles in %2.3f seconds.\n", num[0], num[1], num[2], (double)(finish - start) / CLOCKS_PER_SEC);
	fprintf(stderr, "\n%2.3f seconds.\n", (double)(finish - start) / CLOCKS_PER_SEC);
	return ret;
}
