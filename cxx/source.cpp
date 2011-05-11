/*
* Copyright (c) 2011 MLBA. All rights reserved.
*
* @MLBA_OPEN_LICENSE_HEADER_START@
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
* @MLBA_OPEN_LICENSE_HEADER_END@
*/

#include <map>

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

static dispatch_once_t init_data_tls = 0;
static pthread_key_t data_tls;

static void run_tls_initializer(void*){
	pthread_key_create(&data_tls, NULL);
}

void sourcetype::set_cb(source* s){
		assert(s);
		cb = s;
}

void sourcetype::ready(void* dt){
	if(cb)
		cb->notify(dt);
}

dispatch_source_t sourcetype::native(){
	return NULL;
}

class src_notify_operation : public operation {
public:
	src_notify_operation(void* dt, operation* op) : dt(dt), op(op){
		assert(op);
	}

	void operator()(){
		pthread_setspecific(data_tls, dt);
		(*op)();
		pthread_setspecific(data_tls, NULL);

		if(op->auto_delete())
			delete op;
	}

private:
	void* dt;
	operation* op;
};

class source::pdata {
public:
	pdata() : target(global_queue()) {
		dispatch_once_f(&init_data_tls, NULL, run_tls_initializer);
	}

	long suspend_ct;
	sourcetype* type;
	queue target;
	operation* handler;
};

source::source(sourcetype* src_t) : d(new pdata()){
	assert(d);
	d->suspend_ct = 0;
	d->type = src_t;
}

source::source(const source&) {}

source::~source(){
	delete d->type;
	delete d;
}

void source::suspend(){
	dispatch_atomic_dec(&d->suspend_ct);
}

void source::resume(){
	dispatch_atomic_inc(&d->suspend_ct);
}

void source::set_queue(const queue& q){
	d->target = q;
}

void source::handler(operation* op){
	assert(op);
	d->handler = op;
}

#ifdef XDISPATCH_HAS_BLOCKS
void source::handler(dispatch_block_t b){
	handler(new block_operation(b));
}
#endif

void source::notify(void* dt){
	if(d->suspend_ct < 0)
		return;

	d->target.async(new src_notify_operation(dt, d->handler));
}

dispatch_object_t source::native() const {
	return d->type->native();
}

void* source::data(){
	return pthread_getspecific(data_tls);
}

source& source::operator=(const source&){
	return *this;
}