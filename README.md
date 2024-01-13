> Arcus

Arcus is a simple lightweight Scriptable Package Manager.

It is written in C and is meant to be easily configurable, internally of course.
- If you wish to add more environment variables then you may do so in the `env_args` 2D Array.
- If you wish to add more packages to the script then you may do so in the `packages` 2D Array.

Read [Format](#format) on how to implement new environment variables and packages.
> Usage
```bash
usage: arcus <operation> [...]
operations:
	arcus {-h --help}
	arcus {-V --version}
	arcus list {--ignore ...} (arguments after --ignore are listed as ignored packages, separated by a whitespace)
	arcus install {--ignore ...} (requires root, arguments after --ignore are listed as ignored packages, separated by a whitespace)
```

## Format

> env_args
```c
// DEFAULT env_args IMPLEMENTATION
static const char* env_args[][2] = {
  {
    "ARCUS_SU_PACMAN",
    "sudo pacman -S"
  },
  {
    "ARCUS_DEFAULT_PACMAN_ARGS",
    "--needed --noconfirm"
  },
  {
    "ARCUS_YAY",
    "yay -S"
  },
  {
    "ARCUS_DEFAULT_YAY_ARGS",
    "--needed --noconfirm"
  }
};
```
For example if you wanted to add the environment variable for compiling something with clang, you can implement it as so:
```c
static const char* env_args[][2] = {
  ...
  {
    "ARCUS_CLANG",
    "clang -Wall -Wextra -pedantic -o"
  }
  ...
}
```
This environment variable can then be implemented in a `package` as so:
```c
static const char* packages[][2] = {
  ...
  {
    "arcus",
    "${ARCUS_CLANG} ${ARCUS_PACKAGES} ${ARCUS_PACKAGES}.c"
  }
  ...
};
```

Where the environment variables expand to the following:
- `${ARCUS_CLANG}` expands to `clang -Wall -Wextra -pedantic -o`
- `${ARCUS_PACKAGES}` expands to `arcus` (the header of your package)

> packages

```c
static const char* packages[][2] = {
  {
    "package-header",
    "commands-to-run-to-install-package-header"
  },
  {
    "package-header-2",
    "commands-to-run-to-install-package-header-2
  }
};
```

For example, if you wanted to install `neofetch` with `sudo pacman -S` you can implement a package as so:
```c
static const char* packages[][2] {
  ...
  {
    "neofetch",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}
  }
  ...
};
```
Where the environment variables expand to the following:
- `${ARCUS_SU_PACMAN}` expands to `sudo pacman -S`
- `${ARCUS_DEFAULT_PACMAN_ARGS}` expands to `--needed --noconfirm`
- `${ARCUS_PACKAGES}` expands to `neofetch` (the header of your package)
