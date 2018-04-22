//---------------------------------------------------------------------------

#include <application/starter.h>

#include <unordered_map>
#include <hopscotch_map.h>

#include <benchmark>

//---------------------------------------------------------------------------

static const int KEYS_COUNT = 1000;

namespace asd
{
	template<class Key, class Val, class Hash = std::hash<Key>, class EqualTo = std::equal_to<Key>, class Alloc = std::allocator<std::pair<Key, Val>>>
	using hash_map = tsl::hopscotch_map<Key, Val, Hash, EqualTo, Alloc, 30, true>;
	using h_map_t = hash_map<size_t, size_t>;
	using std_map_t = std::unordered_map<size_t, size_t>;
	
	static entrance open([]() {
		h_map_t h_map;
		std_map_t std_map;
		
		std::hash<size_t> hasher;
		asd::array_list<pair<size_t, size_t>> keys;
		
		for(size_t i = 0; i < KEYS_COUNT; ++i) {
			keys.push_back({i, hasher(i)});
		}
		
		std::shuffle(keys.begin(), keys.end(), std::default_random_engine());
		
		benchmark("std::unordered_map sequential insert") << [&]() {
			for(size_t i = 0; i < KEYS_COUNT; ++i) {
				std_map.insert(std_map_t::value_type{i, i});
			}
			
			std_map.clear();
		};
		
		benchmark("hopscotch_map sequential insert") << [&]() {
			for(size_t i = 0; i < KEYS_COUNT; ++i) {
				h_map.insert(h_map_t::value_type{i, i});
			}
			
			h_map.clear();
		};
		
		std_map.clear();
		
		benchmark("std::unordered_map random insert") << [&]() {
			for(size_t i = 0; i < KEYS_COUNT; ++i) {
				std_map.insert(std_map_t::value_type{keys[i].first, i});
			}
			
			std_map.clear();
		};
		
		benchmark("hopscotch_map random insert") << [&]() {
			for(size_t i = 0; i < KEYS_COUNT; ++i) {
				h_map.insert(h_map_t::value_type{keys[i].first, i});
			}
			
			h_map.clear();
		};
		
		for(size_t i = 0; i < KEYS_COUNT; ++i) {
			std_map.insert(std_map_t::value_type{keys[i].first, i});
		}
		
		for(size_t i = 0; i < KEYS_COUNT; ++i) {
			h_map.insert(h_map_t::value_type{keys[i].first, i});
		}
		
		{
			size_t acc = 0;
			
			benchmark("std::unordered_map get") << [&]() {
				for(size_t i = 0; i < KEYS_COUNT; ++i) {
					acc += std_map.at(keys[i].first);
				}
			};
			
			std::cout << acc << std::endl;
		}
		
		{
			size_t acc = 0;
			
			benchmark("hopscotch_map get") << [&]() {
				for(int i = 0; i < KEYS_COUNT; ++i) {
					acc += h_map.at(keys[i].first, keys[i].second);
				}
			};
			
			std::cout << acc << std::endl;
		}
	});
}

//---------------------------------------------------------------------------
