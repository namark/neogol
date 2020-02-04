#include "recorder.h"

#if defined ENABLE_THEORA_RECORDER
#include <random>

#include "simple/file.hpp"
#include "simple/geom/algorithm.hpp"


using simple::graphical::color_vector;
using simple::graphical::rgb_vector;
using simple::graphical::rgb_pixel;
using simple::geom::vector;
using simple::geom::gauss_jordan_elimination;

// color spec from https://theora.org/doc/Theora.pdf
constexpr auto offset = vector(16.f,128.f,128.f);
constexpr auto excursion = vector(219.f,224.f,224.f);
constexpr float Kr = 0.299f;
constexpr float Kb = 0.114f;
constexpr float iKr = 1.f - Kr;
constexpr float iKb = 1.f - Kb;
constexpr float iK = 1.f - Kb - Kr;

// cue 3blue1brown explaining this with projections of
// visualizations in 6 dimensions...
constexpr auto transform_rgb_yuv = gauss_jordan_elimination(
	vector(
		vector(1.f, 0.f,          2*iKr,         1.f, 0.f, 0.f),
		vector(1.f, -2*iKb*Kb/iK, -2*iKr*Kr/iK,  0.f, 1.f, 0.f),
		vector(1.f, 2*iKb,        0.f,           0.f, 0.f, 1.f)
	)
).mutant_clone(&vector<float,6>::last<3>);

unsigned ceil_16(unsigned n)
{
	return ((n + 0b1111) >> 4 ) << 4;
}

void write(std::ostream& os, const ogg_page& page)
{
	os.write(reinterpret_cast<const char*>(page.header), page.header_len);
	os.write(reinterpret_cast<const char*>(page.body), page.body_len);
}

recorder::recorder(std::string filename, int2 size) :
	file(simple::file::bwopex(filename)),
	ycbcr_buffer(size.x()*size.y()*3/2)
{
	ogg_stream_init(&video_stream, std::random_device{}());

	th_info_init(&video_info);
	video_info.frame_width = ceil_16(size.x());
	video_info.frame_height = ceil_16(size.y());
	video_info.pic_width = size.x();
	video_info.pic_height = size.y();
	video_info.pic_x = 0;
	video_info.pic_y = 0;
	video_info.fps_numerator = 60;
	video_info.fps_denominator = 1;
	video_info.colorspace = TH_CS_ITU_REC_470M;
	video_info.pixel_fmt = TH_PF_420;
	video_info.quality = 32;
	video_info.keyframe_granule_shift = 6;

	video_encoder = th_encode_alloc(&video_info);

	th_comment video_comments;
	th_comment_init(&video_comments);
	while(th_encode_flushheader(video_encoder, &video_comments, &packet) > 0)
	{
		ogg_stream_packetin(&video_stream, &packet);
		if(ogg_stream_pageout(&video_stream, &page)) // or while?
			write(file, page);
	}
	th_comment_clear(&video_comments);

	// apparently need to flush here
	if(ogg_stream_flush(&video_stream, &page))
		write(file, page);

	ycbcr_view[0].width = video_info.frame_width;
	ycbcr_view[0].height = video_info.frame_height;
	ycbcr_view[0].stride = video_info.frame_width;
	ycbcr_view[0].data = ycbcr_buffer.data();
	for(int i = 1; i < 3; ++i)
	{
		ycbcr_view[i].width = video_info.frame_width/2;
		ycbcr_view[i].height = video_info.frame_height/2;
		ycbcr_view[i].stride = video_info.frame_width/2;
		ycbcr_view[i].data = ycbcr_buffer.data()
			+ video_info.frame_width*video_info.frame_height
			+ video_info.frame_width*video_info.frame_height/4 * (i-1);
	}

	ogg_uint32_t freq = 64;
	th_encode_ctl(video_encoder,
		TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE, &freq, sizeof(freq));

}

void recorder::record(const simple::graphical::surface& frame, int cell_size, bool is_last)
{
	using namespace simple::graphical;
	auto palette = *frame.format().palette();
	auto pixels = std::get<pixel_writer<pixel_byte>>(frame.pixels());
	simple::geom::loop(int2::zero(), pixels.size()/2, int2::one(),
	[&](auto i)
	{
		float2 uv{};
		auto flat_i = i.x()+i.y()*video_info.frame_width/2;

		simple::geom::loop(int2::zero(), int2::one(2), int2::one(),
		[&](auto j)
		{
			auto sample = 2*i+j;
			rgb_vector pixel(palette[pixels[sample/cell_size]]);
			auto yuv = round(pixel(transform_rgb_yuv) * excursion + offset);
			auto flat_sample = sample.x()+sample.y()*video_info.frame_width;
			ycbcr_view[0].data[flat_sample] = yuv[0];
			uv += yuv.last<2>();
		});
		uv /= 4;
		uv.floor();
		ycbcr_view[1].data[flat_i] = uv[0];
		ycbcr_view[2].data[flat_i] = uv[1];
	});


	th_encode_ycbcr_in(video_encoder, ycbcr_view);
	while(th_encode_packetout(video_encoder, is_last, &packet))
	{
		ogg_stream_packetin(&video_stream, &packet);
		while(ogg_stream_pageout(&video_stream, &page))
			write(file, page);
	}

	if(is_last)
		if(ogg_stream_flush(&video_stream, &page))
			write(file, page);
}

recorder::~recorder()
{
	th_info_clear(&video_info);
	th_encode_free(video_encoder);
	ogg_stream_clear(&video_stream);
}

#else

recorder::recorder(std::string, int2)
{
}

void recorder::record(const simple::graphical::surface&, int, bool)
{
}

recorder::~recorder()
{
}


#endif
