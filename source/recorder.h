#ifndef RECORDER_H
#define RECORDER_H


#include "simple/graphical/surface.h"
#include "utils.hpp"

#if defined ENABLE_THEORA_RECORDER
#include <fstream>
#include <ogg/ogg.h>
#include <theora/theoraenc.h>
#endif

class recorder
{
	public:
	recorder(std::string, int2);
	void record(const simple::graphical::surface&, int, bool);
	~recorder();

	private:
#if defined ENABLE_THEORA_RECORDER
	std::fstream file;
	ogg_stream_state video_stream;
	th_info video_info;
	th_enc_ctx* video_encoder;
	ogg_packet packet;
	ogg_page page;
	th_ycbcr_buffer ycbcr_view;
	std::vector<unsigned char> ycbcr_buffer;
#endif
};




#endif /* end of include guard */
