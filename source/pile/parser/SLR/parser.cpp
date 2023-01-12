#include <pile/parser/SLR/parser.hpp>
#include <pile/parser/grammar.hpp>
#include <pile/utils/utils.hpp>

namespace pile::Parser {
  SLR* SLR::populate_state_machine() {
    auto root = state_machine.get();

    root->expand(this->grammar)->calculate_goto(this->grammar)->print_all();

    return this;
  }
}  // namespace pile::Parser
