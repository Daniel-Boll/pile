# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.3] - 2022-10-29
### Added
- LL(1) follow set tests

## [0.1.2] - 2022-10-29
### Added
- LL(1) grammar for the language

### Removed
- previous grammar spec decoy

## [0.1.1] - 2022-10-27
### Added
- GLC Parser
  - `pile::Parser::Grammar` parser<file> function
  - unit test for `pile::Parser::Grammar` parser<file> function

### Changed
- Assets now have a test subdirectory for unit tests files

## [0.1.0] - 2022-10-07
### Added
- Started the usage of the new Lexer with the notable system:
  - Macro Expander
  - Includer
  - Finite Automatum

### Removed
- Project clean up in order to use the new system
  - Deprecated and removed the old "parser"
  - Deprecated and removed the old "compiler"
  - Deprecated and removed the old "repl"
- Removed examples
