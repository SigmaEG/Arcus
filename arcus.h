/**
 * @name Arcus
 * @author https://github.com/SigmaEG/Arcus
 * @note Last Updated: 13th January, 2024
 * @note Script Manager
 */

#pragma region INCLUDES

  #include <stdlib.h>
  #include <stdio.h>
  #include <stdbool.h>
  #include <stdint.h>
  #include <string.h>
  #include <ctype.h>
  #include <sys/stat.h>

  #ifdef __unix__
    #include <unistd.h>
  #endif

  #ifdef _WIN32

    // Source: https://solarianprogrammer.com/2019/04/08/c-programming-ansi-escape-codes-windows-macos-linux-terminals/
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
      #define ARCUS_INITED_WIN_TERM_PROC
      #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
    /* Source-End */

  #endif

#pragma endregion INCLUDES

#pragma region GLOBAL DEFINITIONS

  #define KNRM  "\x1B[0m"
  #define KRED  "\x1B[31m"
  #define KGRN  "\x1B[32m"
  #define KYEL  "\x1B[33m"
  #define KBLU  "\x1B[34m"
  #define KMAG  "\x1B[35m"
  #define KCYN  "\x1B[36m"
  #define KWHT  "\x1B[37m"

  #define RET_BASE_COMMAND 47395
  #define RET_SUB_COMMAND 47396

  #define ARCUS_VER "v1.1.2"
  #define LOLCAT_SUPPORT false

  // Source: https://manytools.org/hacker-tools/convert-images-to-ascii-art/go/
  #define SIGMA_SYMBOL_ASCII \
  "                                             \n" \
  "  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    \n" \
  "    @@@@@@@@@                       @@@@@@   \n" \
  "      @@@@@@@@@                        @@@   \n" \
  "        @@@@@@@@@                       @@   \n" \
  "          @@@@@@@@@                      @   \n" \
  "            @@@@@@@@@                        \n" \
  "              @@@@@@@@@                      \n" \
  "                @@@@@@@@@                    \n" \
  "                  @@@@@@@@@                  \n" \
  "                    @@@@@@@@@                \n" \
  "                      @@@@@                  \n" \
  "                     @@@@                    \n" \
  "                   @@@@                      \n" \
  "                 @@@@                        \n" \
  "               @@@@                          \n" \
  "             @@@@                          @@\n" \
  "           @@@@                           @@ \n" \
  "        @@@@@                           @@@@ \n" \
  "      @@@@@                         @@@@@@@@ \n" \
  "    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  \n" \
  "  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  \n" \
  "                                             \n"

static const char* commands[] = {
  "list",
  "install",
  "-h",
  "--help",
  "-V",
  "--version",
  "--ignore"
};
static const int32_t n_commands = sizeof(commands) / sizeof(commands[0]);

static char*** packages = NULL;
static int32_t n_packages = 0;
static char*** env_args = NULL;
static int32_t n_env_args = 0;

#pragma endregion GLOBAL DEFINITIONS

#pragma region FUNCTION DECLARATIONS

  /**
   * @author https://github.com/SigmaEG/Arcus/Arcus
   * @brief Checks whether a path exists (File/Directory)
   * 
   * @param path > Path to `stat`
   * 
   * @return `bool` - > Whether the path exists or not
  */
  bool
  pathexists(const char* path);

  #pragma region OS-SPECIFIC FUNCTION DECLARATIONS

    #ifdef __unix__

      /**
       * @author https://github.com/SigmaEG/Arcus/Arcus
       * @brief Checks whether a user has neofetch installed (Linux only?)
       * 
       * @return `bool` > Whether neofetch exists or not
      */
      bool
      has_neofetch(void);

      /**
       * @author https://github.com/SigmaEG/Arcus/Arcus
       * @brief Checks whether a user has lolcat installed (Linux only?)
       * 
       * @return `bool` > Whether the lolcat exists or not
      */
      bool
      has_lolcat(void);

    #endif

    #ifdef _WIN32

      // Source: https://solarianprogrammer.com/2019/04/08/c-programming-ansi-escape-codes-windows-macos-linux-terminals/
      void enable_ansi(void);
      void disable_ansi(void);

      int32_t setenv(const char* name, const char* value, int32_t overwrite);
      int32_t unsetenv(const char* name);
      /* Source-End */

    #endif

  #pragma endregion OS-SPECIFIC FUNCTION DECLARATIONS

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Reads a line from a `FILE*` stream (stdin, stdout, etc)
   * 
   * @param size_out > Writes size of line (including null-terminator) if non-NULL
   * @param remove_delim > Whether you want to include the delimiter in the line returned
   * @param delimiter > Specify a delimiter (character to read up to)
   * @param fstream > Stream to read (e.g stdin, stdout)
   * 
   * @return `char*` - > A String to the Line Parsed from `FILE* fstream`
  */
  char*
  arcus_getline(
    int32_t* size_out,
    const bool remove_delim,
    const char delimiter,
    FILE* fstream
  );

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Creates an Environment Variable for the Current Session Only
   * 
   * @param name > Name of the Environment Variable
   * @param value > Value of the Environment Variable
   * 
   * @return `bool` - > Whether the Environment Variable was successfully set
  */
  bool
  set_env(
    const char* name,
    const char* value
  );

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Initializes environment variables for Package Installation
   * 
   * @param remove > Whether the environment args should be removed or not
   * 
   * @return `void`
  */
  void
  init_env_args(const bool remove);

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Checks whether a package has been specified by the {--ignore ...} switch
   * 
   * @param package > Name of the Package to query
   * @param ignore > The ignore list to query with package
   * @param n_ignore > Number of ignored packages (size of `ignore`)
   * 
   * @return `bool` - > Whether the Package has been flagged as `[IGNORED]`
  */
  bool
  is_ignored(
    const char* package,
    const char** ignore,
    const int32_t n_ignore
  );

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Lists packages to be installed, tagged with [IGNORED] if specified by {--ignore ...} switch
   * 
   * @param ignore > The ignore list to query
   * @param n_ignore > Number of ignored packages (size of `ignore`)
   * 
   * @return `void`
  */
  void
  list_packages(
    const char** ignore,
    const int32_t n_ignore
  );

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Installs packages that aren't marked with by the {--ignore ...} switch.
   * 
   * @param ignore > The ignore list to query
   * @param n_ignore > Number of ignored packages (size of `ignore`)
   * 
   * @return `void`
  */
  void
  install_packages(
    const char** ignore,
    const int32_t n_ignore
  );

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Displays Arcus Help Information.
   * 
   * @return `void`
  */
  void
  display_help(void);

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Displays Arcus Version Information.
   * 
   * @return `void`
  */
  void
  display_ver(void);

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Parses the Package database from an external file into a usable 2D C-Array.
   * 
   * @param size_out > If not NULL, sets the pointer to the amount of elements in `return`
   * 
   * @return `char**` - > Returns a 2D C-Style String Array containing the parsed Packages.
  */
  char***
  parse_pkgs(int32_t* size_out);

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Parses the Environment Variable database from an external file into a usable 2D C-Array.
   * 
   * @param size_out > If not NULL, sets the pointer to the amount of elements in `return`
   * 
   * @return `char**` - > Returns a 2D C-Style String Array containing the parsed Environment Variables.
  */
  char***
  parse_envs(int32_t* size_out);

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Determines whether a command is valid and is of two categories, a BASE or SUB command.
   * 
   * @param cmd_name > Command to parse
   * 
   * @return `int32_t` - > Returns -1 such that the command passed is not a valid command in `static const char* commands[]{...}` else see `RET_BASE_COMMAND` and `RET_SUB_COMMAND`
  */
  int32_t
  parse_command(const char* cmd_name);

  /**
   * @author https://github.com/SigmaEG/Arcus
   * @brief Parses an argument (command) and its parameters from the argv list.
   * 
   * @param argv > Passed from `int32_t main(..., const char** argv)`
   * @param n_max_argv > Passed from `int32_t main(int32_t argc, ...)`
   * @param arg_name > Name of the Command to Parse
   * @param n_expected_params > Amount of Parameters to Expect from Command (use -1 to read until end or next command)
   * @param n_params_out > Pointer to an external `int32_t` that stores the size of the list on success.
   * 
   * @return `const char**` - > Returns a list `const char**` of parameters on success or NULL
   * @note ON freeing the returned list, DO NOT free its children as they are constant pointers to argv variables.
  */
  const char**
  parse_arguments(
    const char** argv,
    const int32_t n_max_argv,
    const char* arg_name,
    const int32_t n_expected_params,
    int32_t* n_params_out
  );

#pragma endregion FUNCTION DECLARATIONS
