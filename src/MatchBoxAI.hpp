#ifndef _MATCH_BOX_AI_HPP
#define _MATCH_BOX_AI_HPP
#include "Board.hpp"
#include <map>
#include <fstream>
#include <random>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <iostream>
#include <type_traits>

//template<typename T>
//constexpr bool is_char_v =
//                  std::is_same_v<std::decay_t<T>, char> ||
//                  std::is_same_v<std::decay_t<T>, unsigned char> ||
//                  std::is_same_v<std::decay_t<T>, char16_t> ||
//                  std::is_same_v<std::decay_t<T>, char32_t> ||
//                  std::is_same_v<std::decay_t<T>, wchar_t>;

template<typename, template<typename...> typename>
struct is_instance : std::false_type {};
template<template<typename...> typename U, typename... Ms>
struct is_instance<U<Ms...>, U> : std::true_type {};

template<typename T, template<typename...> typename U>
constexpr bool is_instance_v = is_instance<T,U>::value;

template<bool, typename, typename>
struct mux;
template<typename first, typename second>
struct mux<false, first, second> { using type = first; };
template<typename first, typename second>
struct mux<true, first, second> { using type = second; };

template<bool B, typename F, typename S>
using mux_t = typename mux<B,F,S>::type;

//template<typename S>
//struct is_string_like:
//    mux_t<
//        is_instance_v<std::decay_t<S>, std::basic_string> ||
//        is_instance_v<std::decay_t<S>, std::basic_string_view> ||
//        (std::is_pointer_v<std::decay_t<S>> &&
//        is_char_v<
//            std::remove_pointer_t<std::decay_t<S>>
//        >),
//
//        std::false_type, std::true_type
//> {};

//template<typename S>
//constexpr bool is_string_like_v =
//				is_instance_v<std::decay_t<S>, std::basic_string> ||
//				is_instance_v<std::decay_t<S>, std::basic_string_view> ||
//				(std::is_pointer_v<std::decay_t<S>> &&
//				is_char_v<
//					std::remove_pointer_t<std::decay_t<S>>
//				>);
template<typename S>
constexpr bool is_string_like_v = std::is_convertible_v<S, std::string_view>;

template<typename...>
constexpr bool true_v = true;

template<typename Cont>
auto operator<<(std::ostream& out, const Cont& cont) ->
		std::enable_if_t<
			true_v<decltype(std::cbegin(cont)), decltype(std::cend(cont))> &&
			!is_string_like_v<Cont>,
			std::ostream&
		>
{
	out << "[ ";
	for(const auto& e: cont){
		out << e << ' ';
	}
	out << ']';
	return out;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& out, const std::pair<K,V>& it){
	out << '(' << it.first << ", " << it.second << ')';
	return out;
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& out, const std::map<K,V>& it){
	out << "{\n";
	for(const auto &p: it){
		out << "\t(" << p.first << ", " << p.second << ")\n";
	}
	return out << '}';
}

namespace AI{
class MatchBoxAI{
public:
	using BeadsType       = std::pair<std::array<unsigned,AI::MatchBox::BEADS_ARR_LENGTH>, std::size_t>;
	using BoardsType      = std::map<unsigned, BeadsType>;
	using ActionType      = std::pair<BeadsType*, unsigned short>;
public:
	std::fstream ai_file;
	const char* fname;

	BoardsType boards_map;
	unsigned short boards_known{0};

	Turn ai_turn = Turn::X;
	static std::mt19937 random_num_gen;
	std::uniform_int_distribution<unsigned> dist;

	ActionType actions[5]          = {};
	ActionType opponent_actions[5] = {};
	std::uint8_t nactions = 0, n_opponent_actions = 0;

	bool record = true;
	unsigned tie_mod = 0, win_mod;
public:

	MatchBoxAI(const char* file_name, bool dont_count_ties = false, bool encourage_win = false)
		: ai_file{file_name, std::ios::in}, fname{file_name}, win_mod{encourage_win}
	{
//		ai_file.seekg(0, std::ios::beg);
//		if(ai_file) std::cout << "ai file good!" << std::endl;
//		else{
////			auto flags = ai_file.flags();
//			std::cerr << "ai file bad! flags: ";
//			switch(ai_file.rdstate()){
//				case std::ios::badbit:  std::cout << "badbit" << std::flush;   break;
//				case std::ios::failbit: std::cout << "failbit" << std::flush;  break;
//				case std::ios::eofbit:  std::cout << "eofbit" << std::flush;   break;
//				case std::ios::goodbit: std::cout << "goodbit" << std::flush;  break;
//			}
//			exit(1);
//		}
		if(dont_count_ties){
			tie_mod = 1;
		}
		ai_file >> boards_known;

		BeadsType tmp_val;
		auto tmp_key = 0u;

		for(unsigned short i = 0; i < boards_known; ++i){
			ai_file >> tmp_key;

			for(auto& val : tmp_val.first){
				ai_file >> val;
				tmp_val.second += val;
			}

			boards_map.emplace(tmp_key, tmp_val);
			tmp_val.second = 0;
		}
		ai_file.close();
//		std::cout << boards_map << std::endl;
	}

	constexpr void set_turn(Turn ai_turn) noexcept{
		this->ai_turn = ai_turn;
	}
	constexpr Turn get_turn() const noexcept{
		return ai_turn;
	}

	constexpr void communicate_win() noexcept
	{
		ActionType* action = actions;
		for(std::uint8_t i = 0; i < nactions; ++i, ++action)
		{
			// Increase bead count in winning action
			action->first->first[action->second] += 2-tie_mod+win_mod;
			action->first->second                += 2-tie_mod+win_mod;
		}

		action = opponent_actions;
		for(std::uint8_t i = 0; i < n_opponent_actions; ++i, ++action)
		{
			// Decrease bead count in losing action
			if(action->first->first[action->second] > 1){
				--(action->first->first[action->second]);
				--(action->first->second);
			}
		}
	}
	constexpr void communicate_loss() noexcept
	{
		ActionType* action = actions;
		for(std::uint8_t i = 0; i < nactions; ++i, ++action)
		{
			// Decrease bead count in losing action
			if(action->first->first[action->second] > 1){
				--(action->first->first[action->second]);
				--(action->first->second);
			}
		}

		action = opponent_actions;
		for(std::uint8_t i = 0; i < n_opponent_actions; ++i, ++action)
		{
			// Increase bead count in winning action
			action->first->first[action->second] += 2-tie_mod+win_mod;
			action->first->second                += 2-tie_mod+win_mod;
		}
	}
	constexpr void communicate_cat() noexcept
	{
		ActionType* action = actions;
		for(std::uint8_t i = 0; i < nactions; ++i, ++action)
		{
			// Increase bead count in cat-ing action
			action->first->first[action->second] += 1-tie_mod;
			action->first->second                += 1-tie_mod;
		}

		action = opponent_actions;
		for(std::uint8_t i = 0; i < n_opponent_actions; ++i, ++action)
		{
			// Increase bead count in cat-ing action
			action->first->first[action->second] += 1-tie_mod;
			action->first->second                += 1-tie_mod;
		}
	}
	constexpr void reset() noexcept{
		nactions = n_opponent_actions = 0;
	}
	static void seed() noexcept{
		random_num_gen.seed(std::time(nullptr));
	}

	static unsigned short get_array_index(unsigned short row, unsigned short col){
		return row * 3 + col;
	}
	static std::pair<unsigned short, unsigned short> get_coords_from_array_index(unsigned i){
		return std::make_pair(i/3, i%3);
	}


	std::pair<unsigned short, unsigned short> get_decision(Board& b){
		auto&& [int_str, rotations] = b.to_int_string(ai_turn);

		BeadsType* beads = nullptr;

		if(auto tmp = boards_map.find(int_str); tmp != boards_map.cend()){
			beads = &tmp->second;
		}
		else{
			beads = &boards_map.emplace(int_str,
				Board::available_slots(int_str)
			).first->second;
			++boards_known;
		}
		dist.param(std::uniform_int_distribution<unsigned>::param_type(0, (beads->second)-1));

		auto i = 0u;
		auto n = dist(random_num_gen);

		for(auto j=beads->first[0]; j <= n; j+=beads->first[++i]);

//		++nactions;
		actions[nactions++] = ActionType(beads, i);
//		std::cout << "Rotated board: " << std::setw(9) << int_str << std::endl;
//		std::cout << "Move with rotation " << rotations << ": "
//				<< get_coords_from_array_index(i).first << ", "
//				<< get_coords_from_array_index(i).second << '\n' << std::endl;
//		std::cout << "i: " << i << ", n: " << n << ", beads: [ " << std::flush;
//		for(const auto n: beads->first){
//			std::cout << n << " ";
//		}
//		std::cout << "], beads_count: " << beads->second << std::endl;
//		std::for_each_n(actions, nactions, [](const ActionType& action){
//			std::cout << std::flush;
//			std::cout << "\t{beads: \n\t\t";
//			std::copy(action.first->first.cbegin(), action.first->first.cend(),
//					std::ostream_iterator<unsigned short>(std::cout, ", "));
//			std::cout << "\n\t\tbead_count: " << action.first->second << ", \n";
//			std::cout << "\t\taction: " << action.second << "\n\t}\n";
//		});
//		std::cout << ']' << std::endl;
		return Board::undo_rotation(get_coords_from_array_index(i), rotations);
	}

	void register_opponent_action(unsigned short x, unsigned short y, const Board& b){
		auto [int_str, rotations] = b.to_int_string((ai_turn == Turn::X ? Turn::O : Turn::X));

		BeadsType* beads = nullptr;

		if(auto tmp = boards_map.find(int_str); tmp != boards_map.cend()){
			beads = &tmp->second;
		}
		else{
			beads = &boards_map.emplace(int_str,
				Board::available_slots(int_str)
			).first->second;
			++boards_known;
		}
		auto rotated_coords = Board::rotate(std::make_pair(x, y), rotations);
		opponent_actions[n_opponent_actions++] =
				ActionType(beads, get_array_index(3-rotated_coords.second, rotated_coords.first-1));
	}

	constexpr void set_is_recording(bool r){
		record = r;
	}
	~MatchBoxAI(){
		if(record){
			ai_file.open(fname, std::ios::out | std::ios::trunc);

			ai_file << boards_known << std::right << std::setfill('0');

			for(const auto &p : boards_map){
				ai_file << ' ' << std::setw(9) << p.first << ' ';

				for(auto i : p.second.first)
					ai_file << i << ' ';
			}
			ai_file.close();
		}
	}
};

std::mt19937 MatchBoxAI::random_num_gen;
}
#endif
