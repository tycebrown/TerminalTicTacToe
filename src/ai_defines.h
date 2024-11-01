#ifndef _AI_DEFINES_H
#define _AI_DEFINES_H

#include <cstdint>

namespace AI{
namespace MatchBox{

inline constexpr static std::uint8_t DEFAULT_BEADS    = 5;
inline constexpr static std::uint8_t BEADS_ARR_LENGTH = 9;

//inline constexpr static auto DEFAULT_BEADS_ARRAY =
//	std::array<std::uint8_t, AI::MatchBox::BEADS_ARR_LENGTH>{ AI::MatchBox::DEFAULT_BEADS,
//	AI::MatchBox::DEFAULT_BEADS, AI::MatchBox::DEFAULT_BEADS, AI::MatchBox::DEFAULT_BEADS,
//	AI::MatchBox::DEFAULT_BEADS, AI::MatchBox::DEFAULT_BEADS, AI::MatchBox::DEFAULT_BEADS,
//	AI::MatchBox::DEFAULT_BEADS, AI::MatchBox::DEFAULT_BEADS};
}
}

#endif
