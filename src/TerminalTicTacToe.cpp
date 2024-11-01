//============================================================================
// Name        : TerminalTicTacToe.cpp
// Author      : lambcoder
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================
#define AI_FIGHT(index1, index2) \
				ai1_index = index1;\
				ai2_index = index2;\
				if(ai2_index == ai1_index){\
					++ai1_index;\
					ai1_index %= 10;\
				}\
				AI::MatchBoxAI& ai1 = mbais[ai1_index]; \
				AI::MatchBoxAI& ai2 = mbais[ai2_index];\
				\
				mai_wins = mbi_wins = ties = 0u;\
				\
				if(coin(gen)){ \
					ai1.set_turn(Turn::X); \
					ai2.set_turn(Turn::O);\
				}\
				else{\
					ai1.set_turn(Turn::O);\
					ai2.set_turn(Turn::X);\
				}\
				\
				turn = Turn::X;\
				\
				for(unsigned _ = 0; _ < 100; ++_){\
					do{\
						std::cout << std::flush;\
						\
						if(turn == ai1.get_turn()){ \
							decision = ai1.get_decision(b); \
							ai2.register_opponent_action(decision.second+1, 3-decision.first,b);\
							b.mark_at(ai1.get_turn(), decision.second+1, 3-decision.first);\
						}\
						else{\
							decision = ai2.get_decision(b);\
							ai1.register_opponent_action(decision.second+1, 3-decision.first,b);\
							b.mark_at(ai2.get_turn(), decision.second+1, 3-decision.first);\
						}\
						turn = (turn == Turn::X ? Turn::O : Turn::X);\
					} while(!b.game_over()); \
					\
					if(Board::Winner winner = b.get_winner();\
					   (winner == Board::Winner::X && ai1.get_turn() == Turn::X) ||\
					   (winner == Board::Winner::O && ai1.get_turn() == Turn::O))\
					{\
						ai1.communicate_win();\
						++mai_wins;\
						ai2.communicate_loss();\
					}\
					else if(winner == Board::Winner::CAT)\
					{\
						ai1.communicate_cat();\
						ai2.communicate_cat();\
						++ties;\
					}\
					else\
					{\
						ai1.communicate_loss();\
						ai2.communicate_win();\
						++mbi_wins;\
					}\
					\
					b.reset();\
					ai1.reset();\
					ai2.reset();\
				}\
/*


TODO: adding testing and training functionality, along with proper lambdas.


*/
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include "MatchBoxAI.hpp"
#include "/home/lambcoder/include/make_ais"



using namespace std::chrono_literals;

template<typename T>
class TypeDeducer;

void Clear()
{
   std::cout << "\033[2J\033[1;1H";
}

template<unsigned N>
void test(auto&& gen, AI::MatchBoxAI (&students)[N],
		  unsigned bite_size) noexcept
{


}

template<unsigned NStudents, unsigned NTrainers>
void train(auto&& gen,
		   AI::MatchBoxAI (&students)[NStudents],
		   AI::MatchBoxAI (&trainers)[NTrainers],
		   unsigned bite_size) noexcept
{

}

using MainAI = AI::MatchBoxAI;
using namespace std::literals;

int main() {
	std::ios::sync_with_stdio(false);

	Board b;
	bool done = false;
	int ai1_index = 0;
	int ai2_index = 0;
	auto mai_wins = 0u, mbi_wins = 0u, ties = 0u;

	std::pair<unsigned short, unsigned short> decision;

	unsigned short x, y;
	std::string clearer;
	Turn turn = Turn::X;

	std::mt19937 gen(std::time(nullptr));
	std::uniform_int_distribution coin(0,1), mbai_index(0,9), student_index(0,3);
	auto get_student = [&student_index, &gen]{

	};

	MAKE_AIS;

	AI::MatchBoxAI::seed();

	bool print_query = true, redo_switch = false;

	do{
		Clear();
		std::cout << std::setw(17) << std::setfill('-') << "\n" << std::setfill(' ');
		b.reset();

		char players;

		do{
			redo_switch = false;
			std::cout << "1 or 2 players (press 'q' to quit)? ";
			std::cin >> players;

			switch(std::tolower(players)){

				case '0':{
					AI_FIGHT(mbai_index(gen), mbai_index(gen))
					std::cout << "AI " << ai1_index << " won: " << mai_wins << "\nAI " << ai2_index << " won: " \
						<< mbi_wins << "\nTies: " << ties << std::endl;
					std::this_thread::sleep_for(5000ms);
					print_query = false;
				}; break;

				case 'f':{
					f_redo_first:;

					unsigned index;
					std::cout << "Enter an ai_index (0-9): ";
					std::cin >> index;

					if(index < 0 || index > 9){
						std::cout << "Error, invalid index. Try again\n";
						goto f_redo_first;
					}
					mai = &mbais[index];
				} [[fallthrough]];

				case '1':{
					mai->seed();

					if(coin(gen)){
						std::cout << "AI is X!" << std::endl;
						mai->set_turn(Turn::X);
					}
					else{
						std::cout << "AI is O!" << std::endl;
						mai->set_turn(Turn::O);
					}
					turn = Turn::X;

					do{
						b.print();
						if(turn == Turn::X) std::cout << "X's turn!" << std::endl;
						else                std::cout << "O's turn!" << std::endl;

						if(turn != mai->get_turn()){
							b.read_register_opponent_action_and_mark_at_coordinates(x, y, turn, *mai);
							Clear();
						}
						else{
							decision = mai->get_decision(b);
							b.mark_at(mai->get_turn(), decision.second+1, 3-decision.first);
						}

						turn = (turn == Turn::X ? Turn::O : Turn::X);

					} while(!b.game_over());
					b.print();

					if(Board::Winner winner = b.get_winner();
					   (winner == Board::Winner::X && mai->get_turn() == Turn::X) ||
					   (winner == Board::Winner::O && mai->get_turn() == Turn::O))
					{
						mai->communicate_win();
					}
					else if(winner == Board::Winner::CAT)
					{
						mai->communicate_cat();
					}
					else
					{
						mai->communicate_loss();
					}

					mai->reset();
					print_query = true;
				} break;

				case '2':{
					do{
						b.print();
						if(turn == Turn::X) std::cout << "X's turn!" << std::endl;
						else std::cout << "O's turn!" << std::endl;

						b.read_and_mark_at_coordinates(x, y, turn);

						Clear();
						turn = (turn == Turn::X ? Turn::O : Turn::X);

					} while(!b.game_over());
					b.print();

					print_query = true;
				} break;

				case 'm':{
					for(std::size_t i = 0; i < 10000; ++i){
						AI_FIGHT(mbai_index(gen), mbai_index(gen))
	//					std::cout << "AI " << ai1_index << " won: " << mbi_wins << "\nAI " << ai2_index << " won: "
	//					<< mai_wins << "\nTies: " << ties << std::endl;
					}
					print_query = false;
				} break;

				case 'p':{
					redo_first:;

					unsigned first, second;
					std::cout << "Enter an ai_index (0-9): ";
					std::cin >> first;

					if(first < 0 || first > 9){
						std::cout << "Error, invalid index. Try again\n";
						goto redo_first;
					}

					redo_second:;
					std::cout << "Enter an ai_index (0-9): ";
					std::cin >> second;

					if(second < 0 || second > 9){
						std::cout << "Error, invalid index. Try again\n";
						goto redo_second;
					}

					AI_FIGHT(first,second)


					std::cout << "AI " << ai1_index << " won: " << mbi_wins << "\nAI " << ai2_index << " won: " \
					<< mai_wins << "\nTies: " << ties << std::endl;

					std::this_thread::sleep_for(5000ms);
					print_query = false;

				} break;

				case 'q':{
	//				goto done;
					done = true;
					print_query = true;
				} break;
				case 't':{
					static constexpr auto small_training_set_size  = 50u;
					static constexpr auto big_training_set_size    = 500u;
					static constexpr auto human_training_set_size  = 10u;

//					static constexpr auto training_set_bites       = {10u, 20u};
					static constexpr auto small_training_set_bite  = 10u;
					static constexpr auto big_training_set_bite    = 20u;

					auto training_set_bite = 0u;
					auto training_set_size = 0u;

					char type{};
					bool redo = false;

					std::cin >> type;

					do{
						redo = false;

						switch(type){
							case 's':{
								training_set_bite = small_training_set_bite;
								training_set_size = small_training_set_size;
							} break;

							case 'b':{
								training_set_bite = big_training_set_bite;
								training_set_size = big_training_set_size;
							} break;

							default: std::cout << "Error, invalid input. Redo. \n\n"; redo = true;
						}
					} while(redo);

					for(; training_set_size > 0; training_set_size -= training_set_bite){
						train(gen, ai_students, mbais, training_set_bite);
						test(gen, ai_students, training_set_bite);
					}

				} break;
				default: std::cout << "Error, invalid input. Redo. \n\n"; redo_switch = true;
			}
		} while(redo_switch);
	} while(!done && (print_query ? b.print_winner_and_query() : true));
//	done:;

	return 0;
}
