#include <pile/parser/SLR/parser.hpp>
#include <pile/utils/common.hpp>

#include "spdlog/fmt/bundled/color.h"

namespace pile::Parser {
  using Table = SLR::Table;
  using TableEntry = SLR::TableEntry;

  Table* Table::add_shift_entries_to_id(uint16_t id, std::tuple<Grammar::Symbol, uint16_t> data) {
    this->entries[id].data.push_back(
        std::make_tuple(std::get<0>(data), TableEntry::Action::Shift, std::get<1>(data)));

    return this;
  }

  Table* Table::add_reduce_entries_to_id(uint16_t id, std::tuple<Grammar::Symbol, uint16_t> data) {
    this->entries[id].data.push_back(
        std::make_tuple(std::get<0>(data), TableEntry::Action::Reduce, std::get<1>(data)));

    return this;
  }

  Table* Table::add_goto_entries_to_id(uint16_t id, std::tuple<Grammar::Symbol, uint16_t> data) {
    this->entries[id].data.push_back(
        std::make_tuple(std::get<0>(data), TableEntry::Action::Goto, std::get<1>(data)));

    return this;
  }

  Table* Table::add_accept() {
    this->entries[1].data.push_back(
        std::make_tuple(Grammar::Terminal{"$"}, TableEntry::Action::Accept, 0));

    return this;
  }

  TableEntry& Table::operator[](uint16_t id) {
    return *std::ranges::find_if(entries, [id](const TableEntry& entry) { return entry.id == id; });
  }

  TableEntry& Table::operator[](const State& state) {
    return *std::ranges::find_if(
        entries, [&state](const TableEntry& entry) { return entry.id == state.id; });
  }

  void TableEntry::print_action(Action action, uint16_t data) {
    switch (action) {
      case TableEntry::Action::Shift:
        fmt::print(fmt::fg(fmt::color::crimson), "E{}\n", data);
        break;
      case TableEntry::Action::Reduce:
        fmt::print(fmt::fg(fmt::color::dark_cyan), "R{}\n", data);
        break;
      case TableEntry::Action::Accept:
        fmt::print(fmt::fg(fmt::color::green_yellow), "AC\n");
        break;
      case TableEntry::Action::Goto:
        fmt::print(fmt::fg(fmt::color::yellow), "{}\n", data);
        break;
      case TableEntry::Action::Error:
        fmt::print(fmt::fg(fmt::color::red), "ERR\n");
        break;
      default:
        fmt::print(fmt::fg(fmt::color::red), "Unknown action\n");
        break;
    }
  }

  Table* Table::print() {
    for (auto& entry : entries) {
      fmt::print("State {}\n", entry.id);
      for (auto& [symbol, action, data] : entry.data) {
        fmt::color color;

        switch (action) {
          case TableEntry::Action::Shift:
            fmt::print(fmt::fg(fmt::color::crimson), "E{} on [{}]\n", data,
                       Grammar::to_string(symbol));
            break;
          case TableEntry::Action::Reduce:
            fmt::print(fmt::fg(fmt::color::dark_cyan), "R{} on [{}]\n", data,
                       Grammar::to_string(symbol));
            break;
          case TableEntry::Action::Accept:
            fmt::print(fmt::fg(fmt::color::green_yellow), "AC on [$]\n");
            break;
          case TableEntry::Action::Goto:
            fmt::print(fmt::fg(fmt::color::yellow), "{} on [{}]\n", data,
                       Grammar::to_string(symbol));
            break;
          default:
            fmt::print(fmt::fg(fmt::color::red), "Unknown action\n");
            break;
        }
      }
      fmt::print("\n");
    }
    return this;
  }

  Table* Table::reserve(uint16_t size) {
    entries.reserve(size);
    std::ranges::generate_n(std::back_inserter(entries), size,
                            [i = 0]() mutable { return TableEntry(i++); });

    return this;
  }

  std::pair<TableEntry::Action, uint16_t> Table::get_action(uint16_t state_id, std::string symbol) {
    auto& entry = this->operator[](state_id);
    auto& data = entry.data;

    auto it = std::ranges::find_if(data, [&symbol](const auto& entry) {
      return Grammar::to_string(std::get<0>(entry)) == symbol;
    });

    if (it == data.end()) return std::make_pair(TableEntry::Action::Error, 0);

    return std::make_pair(std::get<1>(*it), std::get<2>(*it));
  }

  uint16_t Table::get_goto(uint16_t state_id, std::string symbol) {
    auto& entry = this->operator[](state_id);
    auto& data = entry.data;

    auto it = std::ranges::find_if(data, [&symbol](const auto& entry) {
      return Grammar::to_string(std::get<0>(entry)) == symbol;
    });

    if (it == data.end()) return 0;

    return std::get<2>(*it);
  }
}  // namespace pile::Parser
