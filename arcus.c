/**
 * @name Arcus
 * @author https://github.com/SigmaEG/Arcus
 * @note Last Updated: 14th January, 2024
 * @note Script Manager
 */

#include "arcus.h"

#pragma region LOCAL FUNCTION DECLARATIONS

  /**
   * @author https://github.com/SigmaEG/Arcus/Arcus
   * @brief Strips trailing characters from a string
   * 
   * @param string > Reference to a string to be stripped
   * @param to_strip > Char to strip from `string`
   * @param from_end > Whether to strip backwards or from the front
   * 
   * @return `void`
  */
  static
  void
  strip_trailing_char(
    char** string,
    char to_strip,
    bool from_end
  ) {
    if (string == NULL)
      return;

    if (*string == NULL)
      return;

    int32_t string_len = strlen(*string);
    int32_t new_pos = -1;
    
    if (!from_end) {
      bool char_found = false;

      for (int32_t idx = 0; idx < string_len; ++idx) {
        new_pos = idx;

        if (!char_found) {
          if ((*string)[idx] == to_strip)
            char_found = true;
        }

        if ((*string)[idx] != to_strip)
          break;
      }

      if (new_pos != -1 && char_found) {
        char* new_alloc = (char*)calloc((string_len - new_pos) + 1, sizeof(char));

        if (new_alloc != NULL) {
          char* read_from = *string + new_pos;

          snprintf(new_alloc, (string_len - new_pos) + 1, "%s", read_from);

          free(*string);
          *string = new_alloc;

          return;
        }
      }
    } else {
      bool char_found = false;

      for (int32_t idx = string_len - 1; idx > 0; --idx) {
        new_pos = idx;

        if (!char_found) {
          if ((*string)[idx] == to_strip)
            char_found = true;
        }

        if ((*string)[idx] != to_strip)
          break;
      }

      if (char_found) {
        char* new_alloc = (char*)calloc(new_pos + 2, sizeof(char));

        if (new_alloc != NULL) {
          snprintf(new_alloc, new_pos + 2, "%s", *string);

          free(*string);
          *string = new_alloc;

          return;
        }
      }
    }
  }

  /**
   * @author https://github.com/SigmaEG/Arcus/Arcus
   * @brief Frees a 2D C-Style String Array
   * 
   * @param char_2d > 2D C-Style String Array to `free(...)`
   * @param n_elements > Number of Elements inside `char*** char_2d`
   * 
   * @return `void`
  */
  static
  void
  free_char_2d(
    char*** char_2d,
    int32_t* n_elements
  ) {
    if (char_2d == NULL || n_elements == NULL)
      return;

    for (int32_t array_idx = 0; array_idx < *n_elements; ++array_idx) {
      char** array = char_2d[array_idx];

      if (array != NULL) {
        for (int32_t data_idx = 0; data_idx < 2; ++data_idx) {
          char* data = array[data_idx];

          if (data != NULL) {
            free(data);
            data = NULL;
          }
        }

        free(array);
        array = NULL;
      }
    }

    free(char_2d);
    char_2d = NULL;
    *n_elements = 0;
  }

  /**
   * @author https://github.com/SigmaEG/Arcus/Arcus
   * @brief Parses Arcus files efficiently
   * 
   * @param file > Arcus file reference
   * @param size_out > Reference to an `int32_t` that will store the amount of elements in `return`
   * 
   * @return `char***` - > A 2D C-Style String Array containing Arcus data
  */
  static
  char***
  parse(
    FILE* file,
    int32_t* size_out
  ) {
    if (file == NULL)
      return NULL;

    char*** parsed_data = NULL;
    int32_t data_size = 0;

    char* line = NULL;
    int32_t line_size;

    bool parsing_package = false;

    char* header = NULL;
    char* body = NULL;

    while ((line = arcus_getline(
      &line_size,
      true,
      '\n',
      file
    )) != NULL) {
      if (line_size <= 0) {
        if (line != NULL) {
          free(line);
          line = NULL;
        }

        continue;
      }

      if (strcmp(line, "{") == 0 || strcmp(line, "},") == 0) {
        parsing_package = true;
        
        if (line != NULL) {
          free(line);
          line = NULL;
        }

        continue;
      }

      if (strcmp(line, "}") == 0) {
        parsing_package = false;

        if (line != NULL) {
          free(line);
          line = NULL;
        }

        break;
      }

      if (parsing_package) {
        if (header == NULL && body == NULL) {
          strip_trailing_char(
            &line,
            ' ',
            false
          );
          strip_trailing_char(
            &line,
            '"',
            false
          );
          strip_trailing_char(
            &line,
            ' ',
            true
          );
          strip_trailing_char(
            &line,
            ',',
            true
          );
          strip_trailing_char(
            &line,
            '"',
            true
          );

          header = line;

          continue;
        }

        if (body == NULL && header != NULL) {
          strip_trailing_char(
            &line,
            ' ',
            false
          );
          strip_trailing_char(
            &line,
            '"',
            false
          );
          strip_trailing_char(
            &line,
            ' ',
            true
          );
          strip_trailing_char(
            &line,
            '"',
            true
          );

          body = line;
        }

        if (header != NULL && body != NULL) {
          if (parsed_data == NULL) {
            parsed_data = (char***)calloc(1, sizeof(char**));

            if (parsed_data != NULL) {
              parsed_data[0] = (char**)calloc(2, sizeof(char*));
              
              if (parsed_data[0] != NULL) {
                parsed_data[0][0] = header;
                parsed_data[0][1] = body;

                data_size = 1;
              } else {
                free(parsed_data);
                parsed_data = NULL;
                data_size = 0;

                free(header);
                free(body);

                continue;
              }
            } else {
              free(header);
              free(body);

              continue;
            }
          } else {
            char*** re_alloc = (char***)realloc(parsed_data, (data_size + 1) * sizeof(char**));
          
            if (re_alloc != NULL) {
              parsed_data = re_alloc;
              parsed_data[data_size] = (char**)calloc(2, sizeof(char*));
              
              if (parsed_data[data_size] != NULL) {
                parsed_data[data_size][0] = header;
                parsed_data[data_size][1] = body;

                ++data_size;
              } else {
                parsed_data = (char***)realloc(parsed_data, data_size * sizeof(char**));

                free(header);
                free(body);

                continue;
              }
            } else {
              free(header);
              free(body);

              continue;
            }
          }
        }

        header = NULL;
        body = NULL;

        continue;
      }
    }

    if (size_out != NULL)
      *size_out = data_size;

    fclose(file);

    return parsed_data;
  }

#pragma endregion LOCAL FUNCTION DECLARATIONS

#pragma region FUNCTION DECLARATIONS

  bool
  pathexists(const char* path) {
    struct stat file;

    if (stat(path, &file) == 0)
      return true;

    return false;
  }

  #pragma region OS-SPECIFIC DECLARATIONS

    #if defined(__unix__) || defined(__linux__)
      bool
      has_neofetch(void) {
        if (pathexists("/usr/local/bin/neofetch")
            || pathexists("/usr/bin/neofetch")
            || pathexists("/var/lib/flatpak/exports/bin/neofetch")
        ) {
          return true;
        }

        return false;
      }

      bool
      has_lolcat(void) {
        if (pathexists("/usr/local/bin/lolcat")
            || pathexists("/usr/bin/lolcat")
            || pathexists("/var/lib/flatpak/exports/bin/lolcat")
        ) {
          if (LOLCAT_SUPPORT)
            return true;
        }

        return false;
      }
    #endif

    #if defined(_WIN32)
      // Source: https://solarianprogrammer.com/2019/04/08/c-programming-ansi-escape-codes-windows-macos-linux-terminals/
      #ifdef ARCUS_INITED_WIN_TERM_PROC
        static HANDLE stdout_handle;
        static DWORD outmode_init;

        void
        enable_ansi(void) {
          DWORD out_mode = 0;
          stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
          
          if (stdout_handle == INVALID_HANDLE_VALUE)
            exit(GetLastError());
          
          if (!GetConsoleMode(stdout_handle, &out_mode))
            exit(GetLastError());
          
          outmode_init = out_mode;
          
          // Enable ANSI escape codes
          out_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
          
          if (!SetConsoleMode(stdout_handle, out_mode))
            exit(GetLastError());
        }
        
        void
        disable_ansi(void) {
          // ANSI \Esc[0m (Normal)
          printf("\x1b[0m");	
        
          if(!SetConsoleMode(stdout_handle, outmode_init))
            exit(GetLastError());
        }
      #else
        void
        enable_ansi(void) {}
        
        void
        disable_ansi(void) {
            // ANSI \Esc[0m (Normal)
            printf("\x1b[0m");
        }
      #endif
      /* Source-End */

      int32_t setenv(const char* name, const char* value, int32_t overwrite) {   
        int32_t errcode = 0;
        
        if (!overwrite) {
          size_t envsize = 0;
          errcode = getenv_s(&envsize, NULL, 0, name);
          
          if (errcode || envsize)
            return errcode;
        }

        return _putenv_s(name, value);
      }

      int32_t unsetenv(const char* name) {
        return setenv(name, "", 1);
      }
    #endif

  #pragma endregion OS-SPECIFIC DECLARATIONS

  char*
  arcus_getline(
    int32_t* size_out,
    const bool remove_delim,
    const char delimiter,
    FILE* fstream
  ) {
    char* line = (char*)malloc(8 * sizeof(char));
    int32_t line_sz = 8;

    if (line == NULL) {
      *size_out = 0;
      return NULL;
    }

    char character;
    int32_t read_count = 0;

    while (fread(&character, 1, sizeof(char), fstream) > 0) {
      if ((read_count + 1) == line_sz) {
        char* line_realloc = (char*)realloc(line, (line_sz * 2) * sizeof(char));

        if (line_realloc == NULL)
          break;

        line = line_realloc;
        line_sz *= 2;

        line[line_sz - 1] = '\0';

        if (size_out != NULL)
          *size_out = line_sz - 1;
      }
    
      ++read_count;
      line[read_count - 1] = character;

      if (character == delimiter)
        break;
    }

    if (read_count == 1) {
      free(line);
      line = NULL;

      if (size_out != NULL)
        *size_out = 0;

      return NULL;
    }

    if (!remove_delim)
      read_count += 1;

    if (read_count != line_sz) {
      char* line_realloc = (char*)realloc(line, line_sz * sizeof(char));

      if (line_realloc != NULL)
        line = line_realloc;
    }

    line_sz = read_count;
    line[line_sz - 1] = '\0';

    if (size_out != NULL)
      *size_out = line_sz;

    return line;
  }

  bool
  set_env(
    const char* name,
    const char* value
  ) {
    #if defined(__unix__) || defined(__linux__)
      if (has_lolcat()) {
        char* lolcat = (char*)calloc(strlen("echo \"< ALLOCATING TEMPORARY ENVIRONMENT VARIABLE :  =  >\" | lolcat") + strlen(name) + strlen(value) + 1, sizeof(char));
        sprintf(lolcat, "echo \"< ALLOCATING TEMPORARY ENVIRONMENT VARIABLE : %s=%s >\" | lolcat", name, value);

        system(lolcat);
        free(lolcat);
      } else
        printf(KBLU "< ALLOCATING TEMPORARY ENVIRONMENT VARIABLE : %s%s=%s%s >\n", KMAG, name, value, KBLU);
    #else
      printf(KBLU "< ALLOCATING TEMPORARY ENVIRONMENT VARIABLE : %s%s=%s%s >\n", KMAG, name, value, KBLU);
    #endif

    if (setenv(name, value, 1) == 1) {
      fprintf(stderr, KRED "< FAILED TO ALLOCATE TEMPORARY ENVIRONMENT VARIABLE %s\"%s\"%s >\n", KMAG, name, KBLU);

      return false;
    }
    
    #if defined(__unix__) || defined(__linux__)
      if (has_lolcat()) {
        char* lolcat = (char*)calloc(strlen("echo \"< SUCCESSFULLY ALLOCATED TEMPORARY ENVIRONMENT VARIABLE :  =  >\" | lolcat") + strlen(name) + strlen(value) + 1, sizeof(char));
        sprintf(lolcat, "echo \"< SUCCESSFULLY ALLOCATED TEMPORARY ENVIRONMENT VARIABLE : %s=%s >\" | lolcat", name, value);

        system(lolcat);
        free(lolcat);
      }
      else
        printf(KGRN "< SUCCESSFULLY ALLOCATED TEMPORARY ENVIRONMENT VARIABLE : %s%s=%s%s >\n", KMAG, name, value, KGRN);
    #else
        printf(KGRN "< SUCCESSFULLY ALLOCATED TEMPORARY ENVIRONMENT VARIABLE : %s%s=%s%s >\n", KMAG, name, value, KGRN);
    #endif


    return true;
  }

  void
  init_env_args(const bool remove) {
    for (int32_t arg_idx = 0; arg_idx < n_env_args; ++arg_idx) {
      if (!remove) {
        if (!set_env(env_args[arg_idx][0], env_args[arg_idx][1])) {
          fprintf(stderr, KRED "< ERROR : QUITTING ARCUS >\n");
          exit(1);
        }
      } else
        unsetenv(env_args[arg_idx][0]);
    }
  }

  bool
  is_ignored(
    const char* package,
    const char** ignore,
    const int32_t n_ignore
  ) {
    for (int32_t ignore_idx = 0; ignore_idx < n_ignore; ++ignore_idx) {
      if (strcmp(package, ignore[ignore_idx]) == 0)
        return true;
    }

    return false;
  }

  void
  list_packages(
    const char** ignore,
    const int32_t n_ignore
  ) {
    printf(KGRN "Packages to Install:\n");

    for (int32_t pkg_idx = 0; pkg_idx < n_packages; ++pkg_idx) {
      const char* pkg_name = packages[pkg_idx][0];
      char* pkg_how = NULL;
      bool ignored = is_ignored(pkg_name, ignore, n_ignore);

      if (strstr(packages[pkg_idx][1], "PACMAN") != NULL)
        pkg_how = "PACMAN";
      else if (strstr(packages[pkg_idx][1], "YAY") != NULL)
        pkg_how = "YAY-AUR";
      else if (strstr(packages[pkg_idx][1], "flatpak install") != NULL)
        pkg_how = "FLATPAK";
      else if (strstr(packages[pkg_idx][1], "git clone") != NULL || strstr(packages[pkg_idx][1], "github") != NULL)
        pkg_how = "GITHUB";
      else
        pkg_how = "SELF-DEFINED";

      printf(KCYN "[%s]:%s %s%s%s\n", pkg_how, KNRM, KMAG, pkg_name, ignored ? KYEL " [IGNORED]" : "");
    }

    printf(KYEL "\n%d Package%s Ignored%s\n", n_ignore, n_ignore == 1 ? "" : "(s)", n_ignore == 0 ? ", See \"arcus {-h --help}\"" : "");
  }

  void
  install_packages(
    const char** ignore,
    const int32_t n_ignore
  ) {
    list_packages(ignore, n_ignore);

    printf(KBLU "\nAre you sure you'd like to continue to installation? (%sY%s/%sn%s):%s ", KGRN, KBLU, KRED, KBLU, KGRN);

    char* confirmation = arcus_getline(NULL, true, '\n', stdin);

    if (confirmation != NULL) {
      if (tolower(confirmation[0]) != 'y') {
        printf(KRED "\n< INSTALLATION CANCELED >\n");
        free(ignore);

        exit(0);
      }

      free(confirmation);
    }

    printf(KGRN "Beginning installation...\n\n");

    init_env_args(false);

    printf("\n");

    for (int32_t pkg_idx = 0; pkg_idx < n_packages; ++pkg_idx) {
      const char* pkg_name = packages[pkg_idx][0];
      bool ignored = is_ignored(pkg_name, ignore, n_ignore);

      if (ignored) {
        printf(KYEL "< IGNORING : %s >\n", pkg_name);
        continue;
      }

      set_env("ARCUS_PACKAGES", pkg_name);

      int32_t ret = system(packages[pkg_idx][1]);
      if (ret == 130 || ret == 2 || ret == 33280) {
        printf(KRED "\n< INSTALLATION INTERRUPTED >\n\n");
        free(ignore);

        exit(2);
      }
    }

    init_env_args(true);
    unsetenv("ARCUS_PACKAGES");

    printf("\n");
  }

  void
  display_help(void) {
    printf(
      "%susage:%s arcus %s<operation> [...]%s\n"
      "%soperations:%s\n"
      "\tarcus %s{-h --help}%s\n"
      "\tarcus %s{-V --version}%s\n"
      "\tarcus list %s{--ignore ...}%s %s(arguments after --ignore are listed as ignored packages, separated by a whitespace)%s\n"
      "\tarcus install %s{--ignore ...}%s %s(may require root permissions, arguments after --ignore are listed as ignored packages, separated by a whitespace)\n\n",
      KYEL, KMAG, KCYN, KMAG, KYEL, KMAG, KCYN, KMAG, KCYN, KMAG, KCYN, KMAG, KYEL, KMAG, KCYN, KMAG, KYEL
    );
  }

  void
  display_ver(void) {
    printf(KMAG
      "  Arcus %s" ARCUS_VER "\n"
      "  Copyright (C) 2015 - 2024 SigmaTech\n\n"
      "  %sThis program may be freely redistributed under the terms of the GNU General Public License.\n%s"
      SIGMA_SYMBOL_ASCII,
      KYEL, KBLU, KMAG
    );
  }

  char***
  parse_pkgs(int32_t* size_out) {
    if (!pathexists("arcus_config")) {
      fprintf(stderr, KRED "< FAILED TO STAT DIRECTORY : arcus_config >");

      return NULL;
    }

    if (!pathexists("arcus_config/arcus.pkgs")) {
      fprintf(stderr, KRED "< FAILED TO STAT PACKAGES FILE : arcus_config/arcus.pkgs >");

      return NULL;
    }

    FILE* file = fopen("arcus_config/arcus.pkgs", "r");

    if (file == NULL)
      return NULL;

    return parse(file, size_out);
  }

  char***
  parse_envs(int32_t* size_out) {
    if (!pathexists("arcus_config")) {
      fprintf(stderr, KRED "< FAILED TO STAT DIRECTORY : arcus_config >");

      return NULL;
    }

    if (!pathexists("arcus_config/arcus.envs")) {
      fprintf(stderr, KRED "< FAILED TO STAT ENVIRONMENT VARIABLES FILE : arcus_config/arcus.envs >");

      return NULL;
    }

    FILE* file = fopen("arcus_config/arcus.envs", "r");

    if (file == NULL)
      return NULL;

    return parse(file, size_out);
  }

  int32_t
  parse_command(const char* cmd_name) {
    bool CHECK_SUB = false;

    if (strstr(cmd_name, "-") != NULL)
      CHECK_SUB = true;

    for (int32_t cmd_idx = 0; cmd_idx < n_commands; ++cmd_idx) {
      if (strcmp(commands[cmd_idx], cmd_name) == 0)
        return CHECK_SUB ? RET_SUB_COMMAND : RET_BASE_COMMAND;
    }

    return -1;
  }

  const char**
  parse_arguments(
    const char** argv,
    const int32_t n_max_argv,
    const char* arg_name,
    const int32_t n_expected_params,
    int32_t* n_params_out
  ) {
    if (parse_command(arg_name) == -1)
      return NULL; // Invalid Command to Parse

    printf("< BEGINNING ARGUMENT PARSER : %s >\n", arg_name);

    const char** arg_params = NULL;
    int32_t n_arg_params = 0;
    bool dynamic = false;

    if (n_expected_params > 0) {
      arg_params = (const char**)calloc(n_expected_params, sizeof(const char*));

      if (arg_params == NULL) {
        fprintf(stderr, KRED "< FAILED TO ALLOCATE %zu byte(s) FOR ARG_PARAMS >\n", n_expected_params * sizeof(const char*));

        if (n_params_out != NULL)
          *n_params_out = 0;

        return NULL;
      }
    } else if (n_expected_params == -1) {
      printf("dynamic\n");
      dynamic = true;
      arg_params = (const char**)calloc(1, sizeof(const char*));
      
      if (arg_params == NULL) {
        fprintf(stderr, KRED "< FAILED TO ALLOCATE %zu byte(s) FOR ARG_PARAMS >\n", sizeof(const char*));

        if (n_params_out != NULL)
          *n_params_out = 0;

        return NULL;
      }
    }

    bool arg_found = false;
    int32_t params_read = 0;

    for (int32_t param_idx = 0; param_idx < n_max_argv; ++param_idx) {
      if (!dynamic && params_read == n_arg_params)
        break;

      const char* param = argv[param_idx];

      if (!arg_found) {
        if (strcmp(param, arg_name) == 0) {
          arg_found = true;
          continue;
        }
      }

      if (parse_command(param) == -1) {
        // Parsing Valid Parameter (non-command)

        if (dynamic) {
          const char** re_alloc = (char const**)realloc(arg_params, (params_read + 1) * sizeof(const char*));

          if (re_alloc != NULL) {
            arg_params = re_alloc;

            if (n_params_out != NULL)
              *n_params_out = params_read + 1;
          }
          else {
            fprintf(stderr, KRED "< FAILED TO ALLOCATE %zu byte(s) FOR ARG_PARAMS >\n", sizeof(char*));
            
            break;
          }
        }

        ++params_read;
        arg_params[params_read - 1] = param;
      }
      else
        break;
    }

    if (n_params_out != NULL)
      *n_params_out = params_read;

    return arg_params;
  }

#pragma endregion FUNCTION DECLARATIONS

int32_t main(
  int32_t argc,
  const char** argv
) {
  #if defined(_WIN32)
    enable_ansi();
  #endif

  const char* init_arg = NULL;

  if (argc >= 2)
    init_arg = argv[1];

  if (init_arg != NULL) {
    if (strcmp(init_arg, "-V") == 0 || strcmp(init_arg, "--version") == 0) {
      display_ver();

      #if defined(_WIN32)
        disable_ansi();
      #endif

      exit(0);
    }

    if (strcmp(init_arg, "list") == 0) {
      packages = parse_pkgs(&n_packages);
      env_args = parse_envs(&n_env_args);

      if (packages == NULL || env_args == NULL) {
        printf(
          KRED "< FAILED TO PARSE %s%s%s >",
          packages == NULL ? "PACKAGES" : "",
          packages == NULL && env_args == NULL ? " AND " : "",
          env_args == NULL ? "ENVIRONMENT VARIABLES" : ""
        );

        exit(1);
      }

      int32_t ignores = 0;
      const char** ignore_list = parse_arguments(argv + 2, argc - 2, "--ignore", -1, &ignores);

      list_packages(
        (const char**)ignore_list,
        ignores
      );

      free(ignore_list);
      free_char_2d(packages, &n_packages);
      free_char_2d(env_args, &n_env_args);

      printf(KBLU "\nRun \"%sarcus%s install {--ignore ...}%s\" to install packages\n", KMAG, KCYN, KBLU);
      #if defined(_WIN32)
        disable_ansi();
      #endif
      exit(0);
    }

    if (strcmp(init_arg, "install") == 0) {
      packages = parse_pkgs(&n_packages);
      env_args = parse_envs(&n_env_args);

      if (packages == NULL || env_args == NULL) {
        printf(
          KRED "< FAILED TO PARSE %s%s%s >",
          packages == NULL ? "PACKAGES" : "",
          packages == NULL && env_args == NULL ? " AND " : "",
          env_args == NULL ? "ENVIRONMENT VARIABLES" : ""
        );

        exit(1);
      }

      int32_t ignores = 0;
      const char** ignore_list = parse_arguments(argv + 2, argc - 2, "--ignore", -1, &ignores);

      install_packages(
        (const char**)ignore_list,
        ignores
      );
      free(ignore_list);
      free_char_2d(packages, &n_packages);
      free_char_2d(env_args, &n_env_args);

      bool PRINTED_INS_SUCC = false;

      #if defined(__unix__) || defined(__linux__)
        if (has_lolcat() && has_neofetch()) {
          system("neofetch | lolcat; echo -e \"< INSTALLATION SUCCESSFUL >\nEnjoy! :)\" | lolcat");
          PRINTED_INS_SUCC = true;
        }
        else if (has_neofetch())
          system("neofetch");
      #endif

      if (!PRINTED_INS_SUCC)
        printf(KGRN "< INSTALLATION SUCCESSFUL >\nEnjoy! :)\n");

      #if defined(_WIN32)
        disable_ansi();
      #endif

      exit(0);
    }
  }

  display_help();

  free_char_2d(packages, &n_packages);
  free_char_2d(env_args, &n_env_args);

  #if defined(_WIN32)
    disable_ansi();
  #endif

  exit(0);

  return 0;
}
