
#ifndef _BOARD_HPP
#define _BOARD_HPP

#include <cstdint>
#include <iostream>
#include <utility>
#include <array>
#include <algorithm>
#include "ai_defines.h"
#include <iomanip>
enum class Turn : std::uint8_t{X, O};

class Board{
public:
	enum class Winner: std::uint8_t{X,O,CAT, NOT_OVER};
private:
	enum class Cell : std::uint8_t{EMPTY='-', X='X', O='O'} board[3][3];
	mutable Winner winner{Winner::NOT_OVER};

	std::uint8_t empty_slots{9};

	friend std::ostream& operator<<(std::ostream& out, Cell c){
		return out << static_cast<unsigned char>(c);
	}
public:
	Board() noexcept/* :
		board{
			{Cell::EMPTY,Cell::EMPTY,Cell::EMPTY},
			{Cell::EMPTY,Cell::EMPTY,Cell::EMPTY},
			{Cell::EMPTY,Cell::EMPTY,Cell::EMPTY}}*/ = default;

	auto to_int_string(Turn ai_turn) const noexcept{
		std::uint32_t rotate0=0, rotate90=0, rotate180=0, rotate270=0;

		std::uint32_t mult = 100000000;
		std::uint32_t rotation1 = 0, rotation2 = 0;

		Cell ai_cell = (ai_turn == Turn::X ? Cell::X : Cell::O);
		// Calculate the values for each possible rotation for the current board
		for(std::size_t i{0}; i < 3; ++i){
			for(std::size_t j{0}; j < 3; ++j){
				rotate0   += mult * ((board[i][j] == Cell::EMPTY)     ? 0     : ((board[i][j]     == ai_cell) ? 2 : 1));
				rotate90  += mult * ((board[2-j][i] == Cell::EMPTY)   ? 0     : ((board[2-j][i]   == ai_cell) ? 2 : 1));
				rotate180 += mult * ((board[2-i][2-j] == Cell::EMPTY) ? 0     : ((board[2-i][2-j] == ai_cell) ? 2 : 1));
				rotate270 += mult * ((board[j][2-i] == Cell::EMPTY)   ? 0     : ((board[j][2-i]   == ai_cell) ? 2 : 1));
				mult /= 10;
			}
		}

		// Get the largest out of all of them
		std::uint32_t running1=0, running2=0;
		if(rotate0 >= rotate90){
			running1 = rotate0;
			rotation1  = 0;
		}
		else{
			running1 = rotate90;
			rotation1 = 1;
		}
		if(rotate180 >= rotate270){
			running2 = rotate180;
			rotation2  = 2;
		}
		else{
			running2 = rotate270;
			rotation2  = 3;
		}
//		std::cout << "Rotations of board: (0, " << std::setw(9) << rotate0 << "), (1, " << std::setw(9) << rotate90 << "), (2, "
//				<< std::setw(9) << rotate180 << "), (3, " << std::setw(9) << rotate270 << ")\n";
		if(running1 >= running2) return std::make_pair(running1, rotation1);
		else                    return std::make_pair(running2, rotation2);
	}
	static auto available_slots(std::uint32_t istr) noexcept{
		std::pair<std::array<unsigned, AI::MatchBox::BEADS_ARR_LENGTH>, std::size_t>
					available_slots;
		std::for_each(available_slots.first.rbegin(), available_slots.first.rend(),
				[istr, &available_slots](auto& i) mutable{
					if(istr % 10)
						i = 0;
					else{
						available_slots.second += (i = AI::MatchBox::DEFAULT_BEADS);
					}
					istr /= 10;
				}
		);

//		for(auto& i: available_slots.first){
//			// if istr % 3 is greater than 0, i = 0 else i = DEFAULT_BEADS;
//			if(istr % 10)
//				i = 0;
//			else{
//				available_slots.second += (i = AI::MatchBox::DEFAULT_BEADS);
//			}
//			istr /= 10;
//		}
		return available_slots;
	}

	static std::pair<unsigned short, unsigned short>
		undo_rotation(const std::pair<unsigned short, unsigned short>& coords, std::uint32_t rotations) noexcept
	{
		switch(rotations){
			case 0: return coords;
			case 1: return std::make_pair(2-coords.second, coords.first);
			case 2: return std::make_pair(2-coords.first, 2-coords.second);
			case 3: return std::make_pair(coords.second, 2-coords.first);
		}
	}
	static std::pair<unsigned short, unsigned short>
		rotate(const std::pair<unsigned short, unsigned short>& coords, std::uint32_t rotations) noexcept
	{
		switch(rotations){
			case 0: return coords;
			case 1: return std::make_pair(coords.second, 4-coords.first);
			case 2: return std::make_pair(4-coords.first, 4-coords.second);
			case 3: return std::make_pair(4-coords.second, coords.first);
		}
	}

	void print() noexcept{
		for(unsigned short i = 3; i > 0; --i){
			std::cout << i << " [ " << board[3-i][0] << ' ';
			for(unsigned short j = 1; j < 3; ++j){
				std::cout << "| " << board[3-i][j] << ' ';
			}
			std::cout << "]\n";
		}
		std::cout << "    1   2   3\n\n";
	}
	static Winner all_same(Cell c1, Cell c2, Cell c3){
		if(c1 == c2 && c2 == c3){
			if(c1 == Cell::X) return Winner::X;
			else if(c1 == Cell::O) return Winner::O;
		}
		return Winner::NOT_OVER;
	}
	bool game_over() const noexcept{
		Winner tmp;
		for(unsigned short i{}; i < 3; ++i){
			if(tmp = all_same(board[i][0], board[i][1], board[i][2]); tmp != Winner::NOT_OVER){
				winner = tmp;
				return true;
			}
			else if(tmp = all_same(board[0][i], board[1][i], board[2][i]); tmp != Winner::NOT_OVER){
				winner = tmp;
				return true;
			}
		}
		if(tmp = all_same(board[0][0], board[1][1], board[2][2]); tmp != Winner::NOT_OVER){
			winner = tmp;
			return true;
		}
		else if(tmp = all_same(board[0][2], board[1][1], board[2][0]); tmp != Winner::NOT_OVER){
			winner = tmp;
			return true;
		}
		else if(empty_slots == 0){
			winner = Winner::CAT;
			return true;
		}
		return false;
	}
	bool mark_at(Turn t, unsigned short x, unsigned short y) noexcept{
		if(board[3-y][x-1] != Cell::EMPTY) return false;
		--empty_slots;

		board[3-y][x-1] = (t == Turn::X ? Cell::X : Cell::O);
		return true;
	}

	template<typename AIType>
	bool mark_at_and_register_opponent_action(Turn t, unsigned short x, unsigned short y, AIType& ai) noexcept{
		if(board[3-y][x-1] != Cell::EMPTY) return false;

		ai.register_opponent_action(x, y, *this);

		--empty_slots;
		board[3-y][x-1] = (t == Turn::X ? Cell::X : Cell::O);
		return true;
	}

	bool print_winner_and_query()const noexcept{
		std::cout << "WINNER: ";
		char tmp;
		switch(winner){
			case Winner::X: std::cout << 'X'; break;
			case Winner::O: std::cout << 'O'; break;
			case Winner::CAT: std::cout << "CAT!"; break;
			default: std::cout << "Game not over!";
		}
		std::cout << "\nWould you like to play again (Y/N)? ";

		redo:;
		std::cin >> tmp;

		switch(std::tolower(tmp)){
			case 'y':{

				return true;
			}
			case 'n': return false;
			default: std::cout << "Error, invalid input, please try again: "; goto redo;
		}
	}
	constexpr void reset() noexcept{
		winner = Winner::NOT_OVER;
		for(std::uint8_t i{}; i < 3; ++i){
			for(std::uint8_t j{}; j < 3; ++j){
				board[i][j] = Cell::EMPTY;
			}
		}
		empty_slots = 9;
	}

	void read_and_mark_at_coordinates(unsigned short& x, unsigned short& y, Turn turn){
		static std::string clearer;
		try_all_again:;

		std::cout << "\nEnter your x-coordinate: ";

		x_try_again:;
		std::cin >> x;

		if(x > 3 || x == 0){
			std::cin.clear();
			std::getline(std::cin, clearer);
			std::cout << "\nError, invalid coordinate, please try again: ";
			goto x_try_again;
		}

		std::cout << "\nEnter your y-coordinate: ";

		y_try_again:;
		std::cin >> y;

		if(y > 3 || y == 0){
			std::cin.clear();
			std::getline(std::cin, clearer);

			std::cout << "\nError, invalid coordinate, please try again: ";
			goto y_try_again;
		}

		if(!mark_at(turn, x, y)){
			std::cout << "\nError, space already filled. Try again.\n";
			goto try_all_again;
		}
	}

	template<typename AIType>
	void read_register_opponent_action_and_mark_at_coordinates(unsigned short& x, unsigned short& y, Turn turn, AIType& ai){
		static std::string clearer;
		try_all_again:;

		std::cout << "\nEnter your x-coordinate: ";

		x_try_again:;
		std::cin >> x;

		if(x > 3 || x == 0){
			std::cin.clear();
			std::getline(std::cin, clearer);
			std::cout << "\nError, invalid coordinate, please try again: ";
			goto x_try_again;
		}

		std::cout << "\nEnter your y-coordinate: ";

		y_try_again:;
		std::cin >> y;

		if(y > 3 || y == 0){
			std::cin.clear();
			std::getline(std::cin, clearer);

			std::cout << "\nError, invalid coordinate, please try again: ";
			goto y_try_again;
		}

		if(!mark_at_and_register_opponent_action(turn, x, y, ai)){
			std::cout << "\nError, space already filled. Try again.\n";
			goto try_all_again;
		}
	}

	Winner get_winner() const noexcept{
		return winner;
	}
};
#endif
