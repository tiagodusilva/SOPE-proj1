# SOPE-proj1

## Brief project description

This was the code produced for SOPE's first project, whose objective was to accurately replicate linux's `du` command, taking into account only the flags `-a`, `-b`, `-B`, `-L`, `-S` and `--max-depth=`, with the flag `-l` always active.

## Features implemented

All requested features were fully implemented.

## Known errors/bugs

When running `simpledu` on a really large folder (such asthe user's home for example), it sometimes does not output correctly. We assume this issue is related to running out of pid's, due to too many forks at once. Since synchronization hasn't been taught in class yet, we have no way to test our theory.

### Behaviour on different computers

As fas as we have explored, this program's behaviour doesn't differ on the various computers and linux distributions we tested (Ubuntu and Manjaro). These tests are elaborated below.

### Testing

To extensively test our `simpledu`, we developed two small bash scripts which run various combinations of flags on a list of directories and tests the output (or error code) against that of `du`, using the same criteria specified in the project's guidelines.

To complement this batch testing script, we have another one dedicated to run a single test on a single directory, calling `diff -y --suppress-common-lines` to show only the differences. We used it during development whenever a test case failed upon running the other script.

We have tested almost every combination of flags on the following directories:
- This git's root directory
- The `Documents/` directory
- The `.config/` directory
- The user's home directory

## Implementation details

### Fork's and exec's

As requested, each subdirectory is its own process, fork'ed and exec'ed from the respective parent directory's process. Each subdiretory has its own pipe to the parent, through which they send the required information to their father (or stdout).

### Signals

With the goal of easily handling signals, and being able to send a signal to every child using only `killpg()`, we had to separate every process in two groups:
- The original process alone in a group
- The sub-directory processes and a dummy process are together in another group

In order to guarantee that every call to `setpgid()` is successful, we must ensure that group is both created and still exists. Otherwise the function would return -1 and errno would have the value `EPERM`, meaning we lacked the permissions to complete the operation.

To solve this problem, the original process creates a dummy child as soon as possible, whose sole purpuose is to outlive all the other processes. This ensures the process group leader is always active while other processes try to enter it, meaning we will never have a permissions related error.

Besides from creating the process group, this dummy also sends a char via a pipe to the original process, to garantee that the group was created before other childs attempt to join it. Afterwards it enters a while loop, with a `sleep(99)` to have it in background until the original process sends a `SIGTERM` to kill it. The original process only continues its execution after it has received the confirmation char from the dummy child.

### Pipes

Instead of recovering an open pipe by reading the `/proc` folder or playing with the fact that file descriptors remain open after an `exec`, we have chosen a more resource intensive method that also avoids problems with desynchronization upon writing to `stdout`.

Between the moment a process fork's and exec's, the child uses `dup2()` to  redirect its `stdout` to the writing end of their respective pipe. This way, when a child writes to `stdout`, that information will be sent to its father.

For this purpuose we devised a small communications protocol between all our processes using a struct. This struct contains the file (or directory)'s name and size, alonside two bools indicating wether it is a directory or a file, and if it's the "main" subdirectory. This is, if we need to add it's size to the current directory.

The needed information, wether due to subdirectory size or prints, is passed from the subdirectory process' pipe (masked as `stdout`) to their parent, and so on, until it reaches the original process, which finally prints the information, formatted, to the "real" `stdout`.
