# Directory lines counter

## WARNING: This project need refactor and bug fixes. As of today, the code will segfault.

A small C utility to count the number of lines of code in a project/directory. Dirlcnt stands for "Directory lines counter".

## Build

`make`

## Usage

`./dirlcnt <path-to-scan> <file-ext-list> <gitignore-file-path>`

Example:
`./dirlcnt ./ go,js .gitignore`

This command will scan all Go and JS files, ignoring nodes modules if they are in gitignore.
