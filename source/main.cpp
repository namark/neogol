#include <cstdio>
#include <climits>
#include <chrono>
#include <thread>
#include <tuple>
#include <bitset>
#include <random>
#include <vector>
#include <map>
#include <iostream>

#include "simple/graphical.hpp"
#include "simple/interactive.hpp"
#include "simple/support.hpp"
#include "simple/geom/algorithm.hpp"

#include "recorder.h"
#include "factors.hpp"
#include "utils.hpp"


using namespace simple;
using namespace simple::graphical;
using namespace color_literals;
using namespace std::chrono;

using cell_type = surface::byte;
using cell_writer = pixel_writer<cell_type>;
using cell_bits = std::bitset<CHAR_BIT * sizeof(cell_type)>;

static_assert(std::is_same_v
	<cell_writer::raw_type, cell_writer::pixel_type> );

using palette = std::array<rgba_pixel, 256>;
constexpr palette make_palette(rgb_pixel base_color)
{
	using simple::support::add_overflow;
	using simple::support::count_trailing_zeros;
	palette colors{};
	for(cell_type i = 0; !add_overflow(i,i,cell_type(1));) // this skips 0 intentionally
	{
		auto rot = count_trailing_zeros(i); // because ctz is undefined for 0
		colors[i] = base_color / cell_bits().size() * (cell_bits().size() - rot);
	}
	// colors[0] will be 0_rgb by default construction anyways.
	return colors;
}

constexpr auto colors = make_palette(0x009dff_rgb);
constexpr auto frametime = duration<std::uintmax_t, std::ratio<1, 60>>{1};

void gol_advance(const cell_writer&, range<int2>);

using supported_concurrencies = std::integer_sequence<size_t, 1,2,4,6,8,10,12,14,16,32>;
constexpr auto split_range = prepare_splits(supported_concurrencies{});

int main(int argc, char** argv) try
{
	const char* recording_file = argc > 1 ? argv[1] : nullptr;
	bool recording_started = false;
	size_t recorded_frame_count = 0;
	size_t current_recording_frame = 0;
	std::map<size_t,std::vector<interactive::event>> event_record;
	auto current_event_record = event_record.begin();
	std::optional<recorder> recorder;

	auto selected_concurrency = upper_bound(supported_concurrencies{}, std::thread::hardware_concurrency());
	std::cout << "Hardware concurrency: " << std::thread::hardware_concurrency() << '\n';
	std::cout << "Selected concurrency: " << selected_concurrency.value << '(' << selected_concurrency.index << ')'<< '\n';

	graphical::initializer graphics{};
	interactive::initializer input{};

	const auto screen_size = (*graphics.displays().begin()).current_mode().size;

	std::vector<int> cell_sizes;
	auto factorization = partial_prime_factorize(std::gcd(screen_size[0], screen_size[1]));
	for(auto&& p : factorization.powers) ++p;
	all_factors(
		factorization.primes,
		factorization.powers,
		std::vector<int>(factorization.primes.size()),
		std::back_inserter(cell_sizes)
	);

	sort(cell_sizes.begin(), cell_sizes.end());


	software_window win("neogol", screen_size, window::flags::borderless);

	surface world (win.size(), pixel_format(pixel_format::type::index8));
	if(auto palette = world.format().palette())
	{
		palette->set_colors(colors);
	}
	else
	{
		std::fputs("Wow! index8 surface has no palette???", stderr);
		return -1;
	}
	surface world_snapshot(world);

	auto pixels = world.pixels();
	if(!std::holds_alternative<cell_writer>(pixels))
	{
		std::fputs("Wow! Can't access index8 surface pixels as bytes???", stderr);
		return -2;
	}
	auto cells = std::get<cell_writer>(pixels);

	surface intermediate(win.size(), win.surface().format());

	std::seed_seq seeds{std::random_device{}(), std::random_device{}()};
	std::mt19937_64 twister{seeds};
	std::uniform_int_distribution<cell_type> dist{0,255};

	frametime_logger frametime_logger;
	bool done = false;
	auto cell_size = cell_sizes.begin();
	bool live = false;
	bool live_once = false;
	size_t history = 0;
	auto cell_size_snapshot = cell_size;
	bool live_snapshot = live;
	size_t history_snapshot = history;

	using namespace simple::interactive;
	auto quit_handler = [&done](quit_request)
		{ done = true; return false; };
	auto event_handler = support::overloaded
	{
		quit_handler,
		[&cells, &cell_size](mouse_down e)
		{
			if(e.data.button == mouse_button::left)
				cells[e.data.position / *cell_size] ^= 1;
			return true;
		},
		[&cells, &cell_size](mouse_motion e)
		{
			if(bool(e.data.button_state & mouse_button_mask::left))
			{
				auto start = e.data.position / *cell_size;
				auto end = start - e.data.motion / *cell_size;
				bresenham_line<int2>({start, end}, [&cells](int2 p)
				{
					cells[p] |= 1;
				});
				return true;
			}
			return false;
		},
		[&](key_pressed e)
		{
			switch(e.data.scancode)
			{

				case scancode::enter:
					live = !live;
				break;

				case scancode::right:
					live_once = true;
					live = false;
				break;

				case scancode::left:
					if(history > 1)
					{
						for(auto&& cell : cells.raw_range()) cell >>= 1;
						--history;
					}
					live = false;
				break;

				case scancode::equals:
				case scancode::kp_plus:
					if(cell_size < cell_sizes.end()-1)
						++cell_size;
				break;

				case scancode::minus:
				case scancode::kp_minus:
					if(cell_size > cell_sizes.begin())
						--cell_size;
				break;

				case scancode::grave:
					std::generate(cells.raw_range().begin(), cells.raw_range().end(), [&]() { return dist(twister); });
					history = 8;
				break;

				case scancode::backspace:
					std::fill(cells.raw_range().begin(), cells.raw_range().end(), 0);
					history = 0;
				break;

				case scancode::r:
					if(pressed(scancode::rshift) || pressed(scancode::lshift))
					{
						if(recording_file)
						{
							recording_started = !recording_started;
							if(recording_started)
							{
								std::cout << "Recording" << '\n';
								blit(world, world_snapshot);
								live_snapshot = live;
								history_snapshot = history;
								cell_size_snapshot = cell_size;
								recorded_frame_count = 0;
							}
							else
							{
								std::cout << "Rendering" << '\n';
								recorder.emplace(recording_file, win.size());
								current_recording_frame = 0;
								current_event_record = event_record.begin();
								blit(world_snapshot, world);
								live = live_snapshot;
								history = history_snapshot;
								cell_size = cell_size_snapshot;
							}

						}
						return false;
					}
				break;

				default: return false;
			}
			return true;
		},
		[](auto&&){ return false; }
	};
	auto recording_event_handler = support::overloaded
	{
		quit_handler,
		[&](key_pressed e)
		{
			switch(e.data.scancode)
			{
				case scancode::escape: if(recorder)
				{
					std::cout << "Rendering interrupted" << '\n';
					recorder->record(world, *cell_size, true);
					recorder.reset();
					event_record.clear();
					return true;
				}
				break;
				default: break;
			}
			return false;
		},
		[](auto&&){ return false; }
	};

	auto current_frame = steady_clock::now();
	while(!done)
	{
		current_frame = steady_clock::now();

		if(recorder)
		{
			bool escaped = false;
			while(auto event = next_event())
				escaped |= std::visit(recording_event_handler, *event);

			if(!escaped)
			{
				if(current_event_record != event_record.end())
					if(current_event_record->first < current_recording_frame)
						++current_event_record;

				// have to check this again?? come ooon...!
				if(current_event_record != event_record.end())
					if(current_event_record->first == current_recording_frame)
						for(auto&& event : current_event_record->second)
							std::visit(event_handler, event);
			}
		}
		else
		{
			while(auto event = next_event())
			{
				auto record = std::visit(event_handler, *event);

				if(recording_started && record)
					event_record[recorded_frame_count-1].push_back(*event);
			}
		}

		const int2 active_size = world.size() / *cell_size;
		if(live || live_once)
		{
			// so fast it does not benefit from multi-threading? o.o
			for(auto&& cell : cells.raw_range()) cell <<= 1;

			// technically UB? but should work fine for any sane arch
			// not too hard to un-UB with small performance hit
			// also TODO: if the world is small(er than nproc * cacheline) don't split
			support::apply_for(selected_concurrency.index,
				[&](auto splitter)
				{
					const auto whole = range<int2>{int2::one(),active_size-1};
					auto pieces = splitter(whole, bool2::j());
					std::array<std::thread, pieces.size()-1> threads;

					for(size_t i = 0; i < threads.size(); ++i)
					{
						threads[i] = std::thread(
							[&cells, piece=pieces[i]]()
							{ gol_advance(cells, piece); }
						);
					}

					gol_advance(cells, pieces.back());

					for(auto&& t : threads) t.join();
				},
				split_range);

			live_once = false;
			if(history < cell_bits().size())
				++history;
		}

		if(!blit(world, rect{active_size}, win.surface(), rect{win.size()}))
		{
			if(!blit(world, rect{active_size}, intermediate))
			{
				std::fputs("Wow! Can't blit from index8 to intermediate surface!", stderr);
				return -3;
			}
			if(!blit(intermediate, rect{active_size}, win.surface(), rect{active_size * *cell_size}))
			{
				std::fputs("Wow! Can't blit from intermediate surface to window surface?!", stderr);
				return -4;
			}
		}

		win.update();
		frametime_logger.log(steady_clock::now() - current_frame);

		if(recording_started)
		{
			++recorded_frame_count;
		}
		else if(recorder)
		{
			if(current_recording_frame < recorded_frame_count)
			{
				if(current_recording_frame == recorded_frame_count - 1 || done)
				{
					recorder->record(world, *cell_size, true);
					recorder.reset();
					event_record.clear();
				}
				else
					recorder->record(world, *cell_size, false);
			}
			++current_recording_frame;
			std::cout << "rendered: "
				<< current_recording_frame << '/'
				<< recorded_frame_count << '\n';
		}

		// this is not very precise, maybe busy wait for last millisecond(or less)
		std::this_thread::sleep_until(current_frame + frametime);
	}
	return 0;
}
catch(...)
{
	if(errno)
		std::perror("ERROR");

	const char* sdl_error = SDL_GetError();
	if(*sdl_error)
	{
		std::fputs(sdl_error, stderr);
		std::fputs("\n", stderr);
	}

	throw;
}

void gol_advance(const cell_writer& cells, range<int2> range)
{
	loop(range.lower(), range.upper(), int2::one(), [&](auto& i)
	{
		int living_neighbors = 0;

		loop(i-1, i+2, int2::one(), [&cells, &living_neighbors](auto& j)
		{
			cell_bits cell = cells[j];
			if(cell[1])
				living_neighbors += 1;
		});

		cell_bits cell = cells[i];
		living_neighbors -= cell[1]; // *2

		if(living_neighbors < 2 || living_neighbors > 3)
		{
			cell[0] = false;
		}
		else if(living_neighbors == 3 ||
			(living_neighbors == 2 && cell[1]))
		{
			cell[0] = true;
		}

		cells[i] = cell.to_ulong();

	});
}
