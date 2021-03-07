# Doukutsu Flag Dumper

This is a standalone tool that I cobbled together in a few hours,
which is designed to generate a list of all flag numbers that are
used in your Cave Story (freeware) mod and where/how they are used.
Unlike Booster's Lab's "Generate Flag List" feature, which only
looks in the Stage/\*.tsc files, this tool also analyzes entities
that have a flag ID set, and considers the effect of the \<DNP and
\<DNA commands which set the flag IDs of any entities they delete.
In addition, Head, ArmsItem, and StageSelect (and really any .tsc
files in the top-level data directory, except for Credit.tsc) are
also scanned.

## Building

Compile all of the files with a C++17 compiler (it must support
the `<filesystem>` library) and link them together.

## Usage

Drag-and-drop the `data` folder of your CS mod into the .exe,
and...uh, well, the program writes to standard output, so you
actually probably shouldn't do that. Pipe the output to a file
instead, like

    DoukutsuFlagDumper <data folder path> > flag_list.txt

The program also supports one command-line option:

    Usage: DoukutsuFlagDumper.exe <CS /data directory> [options]
    Options:
        --show-flag-0   Shows usage of flag 0000 (but why?)

## Limitations

* Does not analyze Credit.tsc
* Only tested to work with CS freeware (though other versions that
  have a similar data folder layout and consistent capitalization
  should also work in theory)
* Will not detect flag usage in certain weird edge cases, like
  at the top of the script before the first event, or \<DNP/\<DNA
  in Head.tsc/ArmsItem.tsc/StageSelect.tsc, etc.
* Does not make lunch
