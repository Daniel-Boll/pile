#include <pile/utils/common.hpp>

#include <pile/lexer/lexer.hpp>
#include <pile/parser/grammar.hpp>

int main(int argc, char **argv) {
  spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");

  cxxopts::Options options("Pile", "Pile is a stack-based programming language");

  // clang-format off
  options.add_options()
    ("c,compile", "Compile the given file", cxxopts::value<bool>()->default_value("false"))
    ("f,file", "The file to compile", cxxopts::value<std::string>())
    ("o,output", "The output file", cxxopts::value<std::string>())
    ("v,version", "Print version")
    ("d,debug", "Debug the compiler", cxxopts::value<bool>()->default_value("false"))
    ("r,REPL", "Run the REPL", cxxopts::value<bool>()->default_value("false"))
    ("h,help", "Print help");
  // clang-format on

  auto result = options.parse(argc, argv);

  if (argc == 1) {
    spdlog::error("The REPL is not yet implemented");
    exit(1);
  }

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (result.count("version")) {
    spdlog::info("Pile version {}", PILE_VERSION);
    exit(0);
  }

  if (result.count("REPL")) {
    spdlog::error("The REPL is not yet implemented");
  }

  if (result.count("debug")) {
    spdlog::set_level(spdlog::level::debug);
  }

  if (!result.count("file")) {
    spdlog::error("No file specified");
    exit(1);
  }

  if (result.count("compile")) {
    const auto output = result.count("output") ? result["output"].as<std::string>() : "a";
    const auto file = result["file"].as<std::string>();
    {  // Compilation process
      std::vector<std::string> include_dir{"."};
      try {
        pile::Lexer::analyze(file, &include_dir);
        const auto grammar = pile::Parser::Grammar::parse<"assets/grammar.spec.glc">();
        // grammar.print();
      } catch (const std::exception &e) {
        spdlog::error("Lexical analysis failed");
        spdlog::error("{}", e.what());
        exit(1);
      }
    }
  }

  return 0;
}
