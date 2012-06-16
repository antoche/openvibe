/*
 * Copyright (C) 2008, Robert Oostenveld
 * F.C. Donders Centre for Cognitive Neuroimaging, Radboud University Nijmegen,
 * Kapittelweg 29, 6525 EN Nijmegen, The Netherlands
 *
 */

#include "buffer.h"
#include <pthread.h>

pthread_mutex_t mutexstatus = PTHREAD_MUTEX_INITIALIZER;
int tcpserverStatus = 0;

pthread_mutex_t mutexthreadcount = PTHREAD_MUTEX_INITIALIZER;
int threadcount = 0;

pthread_mutex_t mutexsocketcount = PTHREAD_MUTEX_INITIALIZER;
int socketcount = 0;

pthread_mutex_t mutexappendcount = PTHREAD_MUTEX_INITIALIZER;
int appendcount = 0;

