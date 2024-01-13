/**
 * Programmed by Sigma 
 * Last Updated: 13th January, 2024
 * Arcus
 * > Scriptable Package Manager
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

// Source: https://stackoverflow.com/a/27125283
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define ARCUS_VER "v0.2.7"
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
  },
};
static const size_t n_env_args = sizeof(env_args) / sizeof(env_args[0]);

static const char* packages[][2] = {
  {
    "upgrade-pacman",
    "${ARCUS_SU_PACMAN}yu ${ARCUS_DEFAULT_PACMAN_ARGS}"
  },
  {
    "gnome",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES} gnome-tweaks"
  },
  {
    "enable_gdm",
    "sudo systemctl enable --now gdm"
  },
  {
    "color-parallel-pacman",
    "sudo sed -i \"s/#Color/Color/g\" /etc/pacman.conf;"
    "sudo sed -i \"s/#ParallelDownloads/ParallelDownloads/g\" /etc/pacman.conf"
  },
  {
    "reflector", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES};"
    "sudo cp -i /etc/pacman.d/mirrorlist /etc/pacman.d/mirrorlist.bak;"
    "sudo reflector --verbose --latest 10 --protocol https --sort rate --save /etc/pacman.d/mirrorlist"
  },
  {
    "base-devel",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "git",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "yay", 
    "git clone https://aur.archlinux.org/yay.git;"
    "cd yay;"
    "makepkg -si --noconfirm;"
    "cd ..;"
    "rm -rf yay"
  },
  {
    "python",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES} python-pip python-pipx"
  },
  {
    "clang", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "go", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "gnome-browser-connector", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "openrazer-daemon", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES};"
    "sudo gpasswd -a $USER plugdev"
  },
  {
    "kate",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES};"
  },
  {
    "flatpak", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES};"
    "flatpak update -y"
  },
  {
    "linux",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "linux-lts",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "linux-headers",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "linux-firmware",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "perf", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "intel-ucode", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "otf-comicshanns-nerd",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "noto-fonts",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "p7zip", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "unrar", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "unzip", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "tar", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "rsync", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "neofetch", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "lolcat", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "cmatrix", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "htop", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "exfat-utils", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "exfat-utils", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "fuse-exfat", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "ntfs-3g", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "flac", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "jasper", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "aria2", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "curl", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "wget", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "primary-wallpaper",
    "sudo wget -c https://w.wallhaven.cc/full/p8/wallhaven-p8dqde.jpg -O /usr/share/backgrounds/primary_wallpaper.jpg"
  },
  {
    "dconf", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "firefox", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "libreoffice-fresh",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "vlc",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "gimp",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "krita",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "nautilus",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "vim",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "gparted",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "wine", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "winetricks", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "wine-mono", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "wine-gecko", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "jdk-openjdk", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "papirus-icon-theme", 
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "nvidia-open",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "catppuccin-gtk-theme-mocha",
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} ${ARCUS_PACKAGES};" 
    "ln -sf \"/usr/share/themes/Catppuccin-Mocha-Standard-Mauve-Dark/gtk-4.0/assets\" \"${HOME}/.config/gtk-4.0/assets\";" 
    "ln -sf \"/usr/share/themes/Catppuccin-Mocha-Standard-Mauve-Dark/gtk-4.0/gtk.css\" \"${HOME}/.config/gtk-4.0/gtk.css\";"
    "ln -sf \"/usr/share/themes/Catppuccin-Mocha-Standard-Mauve-Dark/gtk-4.0/gtk-dark.css\" \"${HOME}/.config/gtk-4.0/gtk-dark.css\""
  },
  {
    "catppuccin-cursors-mocha",
    "git clone https://github.com/catppuccin/papirus-folders.git catppuccin-papirus-folders;"
    "sudo cp -r catppuccin-papirus-folders/src/* /usr/share/icons/Papirus;"
    "catppuccin-papirus-folders/papirus-folders -C cat-mocha-mauve --theme Papirus-Dark;"
    "rm -rf catppuccin-papirus-folders"
  },
  {
    "catppuccin-grub-mocha",
    "git clone https://github.com/catppuccin/grub.git catppuccin-grub;"
    "sudo cp -r catppuccin-grub/src/* /boot/EFI/BOOT/;"
    "sudo sed -i 's;.*GRUB_THEME=.*;GRUB_THEME=\"/boot/EFI/BOOT/catppuccin-mocha-grub-theme/theme.txt\";g' /etc/default/grub;"
    "sudo grub-mkconfig -o /boot/grub/grub.cfg;"
    "rm -rf catppuccin-grub"
  },
  {
    "capslockfix",
    "sudo bash -c \"curl https://raw.githubusercontent.com/hexvalid/Linux-CapsLock-Delay-Fixer/master/bootstrap.sh > /usr/local/bin/capslockfix.sh\";"
    "sudo chmod +x /usr/local/bin/capslockfix.sh;"
    "mkdir -p ~/.config/autostart;"
    "echo -e \"[Desktop Entry]\nExec=/usr/local/bin/capslockfix.sh\nIcon=dialog-scripts\nName=capslockfix.sh\nType=Application\nX-KDE-AutostartScript=true\" > ~/.config/autostart/capslockfix.sh.desktop;"
    "sh /usr/local/bin/capslockfix.sh"
  },
  {
    "gnome-terminal-transparency",
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "gdm-settings",
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "gnome-extensions-cli",
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "install-useful-gnome-extensions",
    "gext install appindicatorsupport@rgcjonas.gmail.com;"
    "gext install arcmenu@arcmenu.com blur-my-shell@aunetx compiz-windows-effect@hermes83.github.com dash-to-dock@micxgx.gmail.com unblank@sun.wxg@gmail.com Vitals@CoreCoding.com;"
    "gext enable arcmenu@arcmenu.com blur-my-shell@aunetx compiz-windows-effect@hermes83.github.com dash-to-dock@micxgx.gmail.com unblank@sun.wxg@gmail.com Vitals@CoreCoding.com"
  },
  {
    "visual-studio-code-bin",
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "rclone",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} rclone;"
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} rclone-browser"
  },
  {
    "vencord-desktop",
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "flathub",
    "flatpak install flathub -y"
  },
  {
    "vinegar",
    "flatpak install org.vinegarhq.Vinegar -y"
  },
  {
    "kdenlive",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "steam",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "heroic-games-launcher",
    "${ARCUS_YAY} ${ARCUS_DEFAULT_YAY_ARGS} ${ARCUS_PACKAGES}"
  },
  {
    "zsh",
    "${ARCUS_SU_PACMAN} ${ARCUS_DEFAULT_PACMAN_ARGS} ${ARCUS_PACKAGES} powerline-fonts;"
    "sudo sed -i 's/ZSH_THEME=.*/ZSH_THEME=\"agnoster\"/g' ${HOME}/.zshrc;"
    "zsh -c \"source ${HOME}/.zshrc\";"
    "! test -d $ZSH && sh -c \"$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh); exit\"; exit 0"
  },
  {
    "update-all-packages",
    "${ARCUS_SU_PACMAN}yu ${ARCUS_DEFAULT_PACMAN_ARGS};"
    "${ARCUS_YAY}yu ${ARCUS_DEFAULT_YAY_ARGS};"
    "flatpak update -y"
  }
};
static const size_t n_packages = sizeof(packages) / sizeof(packages[0]);

// Checks whether lolcat is found in /usr/bin
bool
has_lolcat(void);

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
);

// Creates an environment variable for the current session
bool
set_env(
  const char* name,
  const char* value
);

// Initializes environment variables for Package Installation
void
init_env_args(const bool remove);

// Checks whether a package has been specified by the {--ignore ...} switch
bool
is_ignored(
  const char* package,
  const char** ignore,
  const size_t n_ignore
);

// Lists packages to be installed, tagged with [IGNORED] if specified by {--ignore ...} switch
void
list_packages(
  const char** ignore,
  const size_t n_ignore
);

// Installs packages that aren't marked with by the {--ignore ...} switch
void
install_packages(
  const char** ignore,
  const size_t n_ignore
);

// Compiles an ignore list from argv
char**
gen_ignore_list(
  char** argv,
  const size_t start_idx,
  const size_t end_idx
);

// Displays the Arcus Help Information
void
display_help(void);

// Displays Arcus Version Information
void
display_ver(void);
