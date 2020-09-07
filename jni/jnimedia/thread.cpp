/*
 * Copyright (c) 2011 Petr Havlena  havlenapetr@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>

#include "thread.h"

#define THREAD_NAME_MAX 50

#define LOG_TAG "Thread"

static int sThreadCounter = 0;

Mutex::Mutex() {
	pthread_mutex_init(&mMutex, NULL);
}
Mutex::Mutex(const char* name) {
	pthread_mutex_init(&mMutex, NULL);
}
Mutex::Mutex(int type, const char* name) {
	if (type == SHARED) {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&mMutex, &attr);
		pthread_mutexattr_destroy(&attr);
	} else {
		pthread_mutex_init(&mMutex, NULL);
	}
}
Mutex::~Mutex() {
	pthread_mutex_destroy(&mMutex);
}
 int Mutex::lock() {
	return pthread_mutex_lock(&mMutex);
}
 void Mutex::unlock() {
	pthread_mutex_unlock(&mMutex);
}
 int Mutex::tryLock() {
	return pthread_mutex_trylock(&mMutex);
}

Sigmore::Sigmore() {
	pthread_cond_init(&mCond,NULL);
	pthread_mutex_init(&mLock,NULL);
}
Sigmore::~Sigmore() {
	pthread_mutex_destroy(&mLock);
	pthread_cond_destroy(&mCond);
}
void Sigmore::wait() {
	pthread_cond_wait(&mCond,&mLock);
}
void Sigmore::wake() {
	pthread_cond_signal(&mCond);
}
void Sigmore::wakeAll() {
	pthread_cond_broadcast(&mCond);
}

void Sigmore::lock() {
	pthread_mutex_lock(&mLock);
}
void Sigmore::unlock() {
	pthread_mutex_unlock(&mLock);
}

Thread::Thread() :
		mName(NULL) {
	init();
}

Thread::Thread(const char* name) :
		mName(name) {
	init();
}

Thread::~Thread() {
	pthread_mutex_destroy(&mLock);
	pthread_cond_destroy(&mCondition);
}

void Thread::init() {
	pthread_mutex_init(&mLock, NULL);
	pthread_cond_init(&mCondition, NULL);
	sThreadCounter++;
}

void Thread::start() {
	pthread_create(&mThread, NULL, handleThreadStart, this);
}

int Thread::join() {
	if (!mRunning) {
		return 0;
	}
	return pthread_join(mThread, NULL);
}

void* Thread::handleThreadStart(void* ptr) {
	Thread* thread = (Thread *) ptr;

	thread->mRunning = true;

	thread->run();

	thread->mRunning = false;

	return 0;
}

void Thread::run() {
}
