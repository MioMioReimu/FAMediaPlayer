#ifndef __FAPKTLIST_H__
#define __FAPKTLIST_H__
#ifdef __cplusplus
extern "C"
{
#endif
	#include <stdint.h>
	#include <libavcodec/avcodec.h>
#ifdef __cplusplus
}
#include "thread.h"
#include <list>
#endif
class FAPktList {
public:
	FAPktList();
	FAPktList(unsigned int nb_pkt);
	virtual ~FAPktList();
	void push_back(AVPacket *p);
	void push_front(AVPacket *p);
	unsigned int size();
	AVPacket* pop_back();
	AVPacket* pop_front();
private:
	Mutex pktMutex;
	std::list<AVPacket *> pktlist;
};

#endif
