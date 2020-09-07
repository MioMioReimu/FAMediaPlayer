#include "FAPktList.h"
FAPktList::FAPktList() {
}
FAPktList::FAPktList(unsigned int nb_pkt) {
	for (unsigned int i = 0; i < nb_pkt; i++) {
		AVPacket *p = new AVPacket();
		av_init_packet(p);
		push_back(p);
	}
}
FAPktList::~FAPktList() {
	while (size() > 0) {
		AVPacket *p = pop_back();
		av_free_packet(p);
		delete p;
	}
}
void FAPktList::push_back(AVPacket *p) {
	pktMutex.lock();
	pktlist.push_back(p);
	pktMutex.unlock();
}
void FAPktList::push_front(AVPacket *p) {
	pktMutex.lock();
	pktlist.push_front(p);
	pktMutex.unlock();
}
AVPacket * FAPktList::pop_back() {
	AVPacket *p = NULL;
	pktMutex.lock();

	if (pktlist.size() > 0) {
		p = pktlist.back();
		pktlist.pop_back();
	}

	pktMutex.unlock();
	return p;
	return p;
}
AVPacket * FAPktList::pop_front() {
	AVPacket *p = NULL;
	pktMutex.lock();

	if (pktlist.size() > 0) {
		p = pktlist.front();
		pktlist.pop_front();
	}

	pktMutex.unlock();
	return p;
}
unsigned int FAPktList::size() {
	pktMutex.lock();
	unsigned int s = pktlist.size();
	pktMutex.unlock();
	return s;
}
