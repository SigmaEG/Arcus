/**
 * Programmed by Sigma 
 * Last Updated: 13th January, 2024
 * Arcus
 * > Scriptable Package Manager
 */

#include "arcus.h"

#ifdef _WIN32
  #ifdef ARCUS_INITED_WIN_TERM_PROC
    static HANDLE stdoutHandle;
    static DWORD outModeInit;

    void setupConsole(void) {
      DWORD outMode = 0;
      stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
      
      if(stdoutHandle == INVALID_HANDLE_VALUE)
        exit(GetLastError());
      
      if(!GetConsoleMode(stdoutHandle, &outMode))
        exit(GetLastError());
      
      outModeInit = outMode;
      
      // Enable ANSI escape codes
      outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      
      if(!SetConsoleMode(stdoutHandle, outMode))
        exit(GetLastError());
    }
    
    void restoreConsole(void) {
      // Reset colors
      printf("\x1b[0m");	
    
      // Reset console mode
      if(!SetConsoleMode(stdoutHandle, outModeInit))
        exit(GetLastError());
    }
  #else
    void setupConsole(void) {}
    
    void restoreConsole(void) {
        // Reset colors
        printf("\x1b[0m");
    }
  #endif
  /* Source-End */

  int32_t setenv(const char *name, const char *value, int32_t overwrite) {   
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

// Checks whether lolcat is found in /usr/bin for LOLCAT_SUPPORT
bool
has_lolcat(void) {
  #ifdef __unix__
    if (LOLCAT_SUPPORT)
      return access("/usr/bin/lolcat", F_OK) == 0 ? true : false;
  #endif

  return false;
}

/**
 * @author Sigma
 * @param size_out > Writes size of line (including null-terminator) if non-NULL;
 * @param remove_delim > Whether you want to include the delimiter in the line returned;
 * @param delimiter > Specify a delimiter (character to read up to);
 * @param fstream > Stream to read (e.g stdin, stdout);
*/
char*
arcus_getline(
  size_t* size_out,
  const bool remove_delim,
  const char delimiter,
  FILE* fstream
) {
  char* line = (char*)malloc(8 * sizeof(char));
  size_t line_sz = 8;

  if (line == NULL) {
    *size_out = 0;
    return NULL;
  }

  char character;
  size_t read_count = 0;

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

// Creates an environment variable for the current session
bool
set_env(
  const char* name,
  const char* value
) {
  if (has_lolcat()) {
    char* lolcat = (char*)calloc(strlen("echo \"< ALLOCATING TEMPORARY ENVIRONMENT VARIABLE :  =  >\" | lolcat") + strlen(name) + strlen(value) + 1, sizeof(char));
    sprintf(lolcat, "echo \"< ALLOCATING TEMPORARY ENVIRONMENT VARIABLE : %s=%s >\" | lolcat", name, value);

    system(lolcat);
    free(lolcat);
  }
  else
    printf(KBLU "< ALLOCATING TEMPORARY ENVIRONMENT VARIABLE : %s%s=%s%s >\n", KMAG, name, value, KBLU);

  if (setenv(name, value, 1) == 1) {
    fprintf(stderr, KRED "< FAILED TO ALLOCATE TEMPORARY ENVIRONMENT VARIABLE %s\"%s\"%s >\n", KMAG, name, KBLU);

    return false;
  }
  
  if (has_lolcat()) {
    char* lolcat = (char*)calloc(strlen("echo \"< SUCCESSFULLY ALLOCATED TEMPORARY ENVIRONMENT VARIABLE :  =  >\" | lolcat") + strlen(name) + strlen(value) + 1, sizeof(char));
    sprintf(lolcat, "echo \"< SUCCESSFULLY ALLOCATED TEMPORARY ENVIRONMENT VARIABLE : %s=%s >\" | lolcat", name, value);

    system(lolcat);
    free(lolcat);
  }
  else
    printf(KGRN "< SUCCESSFULLY ALLOCATED TEMPORARY ENVIRONMENT VARIABLE : %s%s=%s%s >\n", KMAG, name, value, KGRN);

  return true;
}

// Initializes environment variables for Package Installation
void
init_env_args(const bool remove) {
  for (size_t arg_idx = 0; arg_idx < n_env_args; ++arg_idx) {
    if (!remove) {
      if (!set_env(env_args[arg_idx][0], env_args[arg_idx][1])) {
        fprintf(stderr, KRED "< ERROR : QUITTING ARCUS >\n");
        exit(1);
      }
    } else
      unsetenv(env_args[arg_idx][0]);
  }
}

// Checks whether a package has been specified by the {--ignore ...} switch
bool
is_ignored(
  const char* package,
  const char** ignore,
  const size_t n_ignore
) {
  for (size_t ignore_idx = 0; ignore_idx < n_ignore; ++ignore_idx) {
    if (strcmp(package, ignore[ignore_idx]) == 0)
      return true;
  }

  return false;
}

// Lists packages to be installed, tagged with [IGNORED] if specified by {--ignore ...} switch
void
list_packages(
  const char** ignore,
  const size_t n_ignore
) {
  printf(KGRN "Packages to Install:\n");

  for (size_t pkg_idx = 0; pkg_idx < n_packages; ++pkg_idx) {
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

  printf(KYEL "\n%zu Package%s Ignored%s\n", n_ignore, n_ignore == 1 ? "" : "(s)", n_ignore == 0 ? ", See \"arcus {-h --help}\"" : "");
}

// Installs packages that aren't marked with by the {--ignore ...} switch
void
install_packages(
  const char** ignore,
  const size_t n_ignore
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

  for (size_t pkg_idx = 0; pkg_idx < n_packages; ++pkg_idx) {
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

// Compiles an ignore list from argv
char**
gen_ignore_list(
  char** argv,
  const size_t start_idx,
  const size_t end_idx
) {
  char** ignore_list = (char**)calloc((end_idx + 1) - start_idx, sizeof(char*));

  if (ignore_list != NULL) {
    for (size_t idx = start_idx; idx < end_idx + 1; ++idx)
      ignore_list[idx - start_idx] = argv[idx];
  }

  return ignore_list;
}

// Displays the Arcus Help Information
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

// Displays Arcus Version Information
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

int main(
  int argc,
  char** argv
) {
  #ifdef _WIN32
    setupConsole();
  #endif

  char* init_arg = NULL;

  if (argc >= 2)
    init_arg = argv[1];

  if (init_arg != NULL) {
    if (strcmp(init_arg, "-V") == 0 || strcmp(init_arg, "--version") == 0) {
      display_ver();

      #ifdef _WIN32
        restoreConsole();
      #endif

      exit(0);
    }

    if (strcmp(init_arg, "list") == 0) {
      char** ignore_list = NULL;
      size_t ignores = 0;

      if (argc >= 4) {
        char* is_ignore = strcmp(argv[2], "--ignore") == 0 ? argv[2] : NULL;

        if (is_ignore != NULL) {
          ignores = argc - 3;
          ignore_list = gen_ignore_list(argv, 3, argc - 1);
        }
      }

      list_packages(
        (const char**)ignore_list,
        ignores
      );
      free(ignore_list);
    
      printf(KBLU "\nRun \"%sarcus%s install {--ignore ...}%s\" to install packages\n", KMAG, KCYN, KBLU);
      #ifdef _WIN32
        restoreConsole();
      #endif
      exit(0);
    }

    if (strcmp(init_arg, "install") == 0) {
      char** ignore_list = NULL;
      size_t ignores = 0;

      if (argc >= 4) {
        char* is_ignore = strcmp(argv[2], "--ignore") == 0 ? argv[2] : NULL;

        if (is_ignore != NULL) {
          ignores = argc - 3;
          ignore_list = gen_ignore_list(argv, 3, argc - 1);
        }
      }

      install_packages(
        (const char**)ignore_list,
        ignores
      );
      free(ignore_list);

      #ifdef __unix__
        if (has_lolcat() && access("/usr/bin/neofetch", F_OK) == 0)
          system("neofetch | lolcat; echo -e \"< INSTALLATION SUCCESSFUL >\nEnjoy! :)\" | lolcat");
        else if (access("/usr/bin/neofetch", F_OK) == 0) {
          system("neofetch");
          printf(KGRN "< INSTALLATION SUCCESSFUL >\nEnjoy! :)\n");
        }
      #endif

      #ifdef _WIN32
        restoreConsole();
      #endif

      exit(0);
    }
  }

  display_help();

  #ifdef _WIN32
    restoreConsole();
  #endif

  exit(0);

  return 0;
}
