/*
 * barrier4.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads4w - POSIX Threads for Windows
 *      Copyright 1998 John E. Bossom
 *      Copyright 1999-2018, Pthreads4w contributors
 *
 *      Homepage: https://sourceforge.net/projects/pthreads4w/
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *
 *      https://sourceforge.net/p/pthreads4w/wiki/Contributors/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * --------------------------------------------------------------------------
 *
 * Declare a single barrier object, multiple wait on it,
 * and then destroy it.
 *
 */

#include "test.h"

enum {
	NUMTHREADS = 16
};

pthread_barrier_t barrier = NULL;
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
static int serialThreadCount = 0;
static int otherThreadCount = 0;

void * func(void * arg)
{
	int result = pthread_barrier_wait(&barrier);
	assert(pthread_mutex_lock(&mx) == 0);
	if(result == PTHREAD_BARRIER_SERIAL_THREAD) {
		serialThreadCount++;
	}
	else if(0 == result) {
		otherThreadCount++;
	}
	else{
		printf("Barrier wait failed: error = %s\n", error_string[result]);
		fflush(stdout);
		return NULL;
	}
	assert(pthread_mutex_unlock(&mx) == 0);
	return NULL;
}

int main()
{
	int i, j;
	pthread_t t[NUMTHREADS + 1];
	for(j = 1; j <= NUMTHREADS; j++) {
		printf("Barrier height = %d\n", j);
		serialThreadCount = 0;
		assert(pthread_barrier_init(&barrier, NULL, j) == 0);
		for(i = 1; i <= j; i++) {
			assert(pthread_create(&t[i], NULL, func, NULL) == 0);
		}
		for(i = 1; i <= j; i++) {
			assert(pthread_join(t[i], NULL) == 0);
		}
		assert(serialThreadCount == 1);
		assert(pthread_barrier_destroy(&barrier) == 0);
	}
	assert(pthread_mutex_destroy(&mx) == 0);
	return 0;
}
