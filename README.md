# Plugin-Based File Search Utility

This project implements a command line tool `lab12dviN3249` that performs recursive file searches using dynamically loaded plugins. Each plugin can supply its own command line options and file matching logic, allowing the core program to be easily extended without recompiling.

## Features

- **Dynamic plugins** – all `.so` files found in the `lib` directory are loaded at runtime. Each plugin provides its own options via `plugin_get_info()` and tests files via `plugin_process_file()`.
- **Boolean logic** – search criteria from multiple plugins can be combined using logical AND (`-A`, default) or OR (`-O`). The result can be inverted with `-N`.
- **Colored tree output** – matching files are printed as a directory tree. Directories are shown in blue and executable files in red.
- **Debug mode** – set the environment variable `LAB12DEBUG` to enable verbose debug output.

## Directory Layout

```
lab_12_dvi_N3249/
├── makefile                # build rules for the core and plugins
├── src/                    # program sources
│   ├── core/               # main application logic
│   ├── include/            # header files and plugin API
│   └── plugins/            # sample plugin implementations
├── lib/                    # built plugin shared libraries (created by `make`)
├── build/                  # temporary object files (created by `make`)
├── test plugin/            # sample files for testing the utility
├── valgrind-out-O2.txt     # valgrind log example
└── valgrind-out-O3.txt     # valgrind log example
```

### Source Overview

- **`src/core`** – contains the entry point (`main.c`) and helper modules:
  - `catch_all_options.c` – collects core and plugin options;
  - `functions.c` – plugin loading and file scanning logic;
  - `output_printing_block.c` – builds and prints the colored file tree;
  - `error_handlers_functions.c` – error/usage messages.
- **`src/plugins`** – example plugins:
  - `pic.c` – detects image files by header (PNG/JPG/BMP/GIF);
  - `test.c` – searches a file for a given text substring.
- **`src/include`** – `inc.h` with common declarations and `plugin_api.h` describing the plugin interface.

## Building

Run `make` in the project root. This creates the executable `lab12dviN3249` and builds each plugin as `lib/lib<name>.so`.

```
$ make
```

## Usage

```
lab12dviN3249 [OPTIONS] [DIRECTORY]
```

If `DIRECTORY` is omitted, the program lists available plugins. Otherwise it scans the directory recursively and prints all files that satisfy the provided plugin criteria.

### Core Options

- `-P, --plugins-dir DIR` – path to the directory containing plugin `.so` files (default `./lib`).
- `-A, --and` – combine criteria with logical AND (default).
- `-O, --or` – combine criteria with logical OR.
- `-N, --not` – invert the result after applying AND/OR.
- `-h, --help` – show help.
- `-v, --version` – show program version and plugin list.

Each plugin may provide additional long options. For instance, the sample `pic` plugin defines `--pic <fmt_list>` where `fmt_list` is a comma-separated list of `png`, `jpg`, `bmp`, `gif`. The `test` plugin defines `--test <string>` to search for a substring inside files.

### Examples

```
# Search for images OR files containing "PNG" in the ./test plugin directory
./lab12dviN3249 -O --pic jpg,png,gif --test PNG ./test\ plugin/

# Search only for image files
./lab12dviN3249 --pic jpg,png,gif ./test\ plugin/
```

## Writing Your Own Plugin

To add new search criteria, create a shared library implementing the functions declared in `src/include/plugin_api.h`:

1. `plugin_get_info()` should fill a `struct plugin_info` describing the plugin options.
2. `plugin_process_file()` receives a file path and the parsed options; it must return `0` if the file matches, `>0` if it does not, or `<0` on error (setting `errno`).

Place the compiled `libyourplugin.so` into the plugins directory (by default `./lib`) so it is discovered at startup.

## License

This project was created as a laboratory assignment and is provided as-is for educational purposes.

