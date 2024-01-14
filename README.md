# Arcus v1.1.2

Arcus is a simple lightweight Scriptable Package Manager compliant with `-std=c99` whilst implementing OS-Specific implementations where required.

It is written in C and is meant to be easily configurable using `arcus.pkgs` and `arcus.envs`. The syntax for such files is quite `strict` so please follow it accordingly.
- If you wish to add more packages to the script then you may do so in the `arcus_config/arcus.pkgs` 2D Array.
- If you wish to add more environment variables then you may do so in `arcus_config/arcus.envs`

> [!WARNING]
> Ensure you put a comma at the previous-most `}` to separate packages and/or environment variables in `arcus.pkgs` and `arcus.envs` otherwise they **WON'T** be parsed.

> [!IMPORTANT]
> Read [Usage](#usage) if you wish to learn how to use `Arcus`.
> 
> Read [Format](#format) if you wish to implement new environment variables and packages.
>
> Read [Credits](#credits) for a list of Credits.

## Usage
> [!NOTE]
> Running `arcus install` whilst there are `packages` present that use package managers that require `root` permissions will ask for them mid-installation.

> [!TIP]
> Ignoring the installation of one or more `packages` is fairly simple, simply append the `--ignore` flag to the end of `arcus install` following a whitespace-separated list of `packages` to ignore.
>
> For example, if you didn't want to install `lolcat` and `neofetch` and they are listed packages in `static const char* packages[][2]{...}` then you can run `arcus` as `arcus install --ignore lolcat neofetch`
```bash
usage: arcus <operation> [...]
operations:
				arcus {-h --help}
				arcus {-V --version}
				arcus list {--ignore ...} (arguments after --ignore are listed as ignored packages, separated by a whitespace)
				arcus install {--ignore ...} (may require root permissions, arguments after --ignore are listed as ignored packages, separated by a whitespace)
```

## Format

> static const char* env_args[][2] {...}
```
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
```
> [!WARNING]
> Implementing an environment variable in `arcus_config/arcus.envs` WILL call `int setenv(const char *name, const char *value, int overwrite)` internally with the `int overwrite` flag set to `1` `(true)`. Hence, be cautious about overwriting already-existing environment variables in your Shell Session. Though they will reset upon exiting the Shell Session.

For example if you wanted to add the environment variable for compiling something with `clang`, you can implement it as so in `arcus_config/arcus.envs`:
```
...
{
	"ARCUS_CLANG",
	"clang -Wall -Wextra -pedantic -o"
}
...
```
This environment variable can then be implemented in `arcus_config/arcus.pkgs` as so:
```
...
{
	"arcus",
	"${ARCUS_CLANG} ${ARCUS_PACKAGES} ${ARCUS_PACKAGES}.c"
}
...
```

Where the environment variables expand to the following:
- `${ARCUS_CLANG}` expands to `clang -Wall -Wextra -pedantic -o`
- `${ARCUS_PACKAGES}` expands to `arcus` (the header of your package)

> static const char* packages[][2] {...}
```
{
	"package-header",
	"commands-to-run-to-install-package-header"
},
{
	"package-header-2",
	"commands-to-run-to-install-package-header-2
}
```

> [!CAUTION]
> Do ***NOT*** implement a package header that contains a whitespace if you wish for it to be ignorable using the `--ignore ...` flag. Otherwise, if you wish to install multiple packages at once using `${ARCUS_PACKAGES}` then you may put multiple headers such you require it.

For example, if you wanted to install `neofetch` with `sudo pacman -S` you can implement a package as so:
```
...
{
	"neofetch",
	"${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}
}
...
```
Where the environment variables expand to the following:
- `${ARCUS_SU_PACMAN}` expands to `sudo pacman -S`
- `${ARCUS_DEFAULT_PACMAN_ARGS}` expands to `--needed --noconfirm`
- `${ARCUS_PACKAGES}` expands to `neofetch` (the header of your package)

Refer to [env_args](#format) in regards to environment variables as such as `${ARCUS_SU_PACMAN}`, `${ARCUS_DEFAULT_PACMAN_ARGS}` and `${ARCUS_PACKAGES}` 

> [!TIP]
> If you wish to run multiple commands at once for each package, then refer to your Operating Systems guide on how to separate commands in your `Default Terminal` as `Arcus` takes use of `int system(const char* command)`.
> For example in `bash`, commands can be separated by trailing semi-colons (`;`).
> 
> In Windows, if your Shell is PowerShell the same is apparent.
>
> An example of this, in `bash` would be the following:
```
...
{
	"lolcat",
	"${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}; echo \"Hello, World!\" | lolcat"`
}
...
```
> [!WARNING]
> Ensure all of your commands are within a single string and not on a newline as the parser won't pick this up and may potentially cause further issues for Arcus Script.

# Credits

> Authored and programmed by [SigmaEG](https://github.com/SigmaEG)
>
> Last Updated: 14th January, 2024
>
> Script Manager
