PROJECT ROADMAP

Phase 1: Stabilize Current Shell
- Finish `pwd`
- Finish `clear`
- Improve `help`
- Make `cd` more reliable
- Improve invalid input handling
- Make empty input safe
- Clean up command output formatting

Phase 2: Clean Up Project Structure
- Keep command handling organized
- Move built-in commands into helper functions
- Keep prompt logic separate
- Keep parsing logic separate
- Improve naming and file organization
- Make the shell easier to extend

Phase 3: Strengthen Tokenizing
- Improve splitting input into command and arguments
- Handle repeated spaces more cleanly
- Support quoted strings later
- Separate tokenizing from command execution
- Make parsing reliable enough for more advanced commands

Phase 4: Run External Commands
- Add `fork()`
- Add `execvp()`
- Add `wait()` / `waitpid()`
- Distinguish built-ins from external commands
- Run commands that are not part of the shell itself

Phase 5: Add Redirection And Pipes
- Add output redirection
- Add input redirection
- Add append redirection
- Add single pipes
- Add multiple pipes later
- Learn file descriptor basics through shell features

Phase 6: Add Data Structures Through Shell Features
- Add command history
- Use a dynamic array or linked list for history
- Add history search
- Add aliases
- Add variables
- Use a hash table for aliases or variables
- Add autocomplete later with a trie

Phase 7: Build A Real Parser
- Move from simple string splitting to structured tokens
- Represent commands with structs
- Represent pipes and redirections explicitly
- Build toward a parse tree / abstract syntax tree
- Make the shell capable of handling more complex syntax

Phase 8: Add Process And Job Features
- Add background jobs
- Track running processes
- Add foreground/background behavior
- Keep a process list or job list
- Improve overall shell control flow

Phase 9: Turn The Shell Into A Strong Systems Project
- Add configuration file support
- Add startup behavior
- Improve documentation
- Add tests for helpers and parser logic
- Improve internal architecture
- Make the shell stable enough to serve as a long-term systems project

Phase 10: Start Operating System Foundations
- Learn boot basics
- Learn linker scripts and low-level build flow
- Learn basic assembly needed for startup
- Build a tiny kernel that boots and prints text
- Run it in QEMU first

Phase 11: Add Core OS Pieces
- Add interrupt handling
- Add timer support
- Add keyboard input
- Add simple memory management
- Add a basic allocator
- Build toward paging and virtual memory later

Phase 12: Add Processes, System Calls, And User Space
- Add a task or process model
- Add simple scheduling
- Add context switching
- Add a system call interface
- Add user mode support
- Run simple programs outside the kernel itself

Phase 13: Add Filesystem And Shell Inside The OS
- Add a simple filesystem
- Add file read/write support
- Add directories
- Add a shell inside the operating system
- Reuse ideas from the current shell project where possible

Phase 14: Grow The OS Into A Larger No-GUI System
- Add better storage support
- Add stronger process handling
- Add more utilities
- Add better debugging support
- Add more device support over time
- Make the system usable as a text-based operating system

Phase 15: Long-Term Expansion
- Improve the shell and OS together
- Strengthen architecture
- Add more advanced data structures where they help
- Add more advanced parsers and process handling
- Keep the project modular enough to support larger future goals
