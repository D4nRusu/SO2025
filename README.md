# SO2025

## Project done by Dan-Mihai Rusu

## Progress report

    Phase 1 - complete (8/04/2025)
    Phase 2 - work in progress (02/05/2025)
    Phase 3 - x

## How to build and run the project

Project can be built by running `make` in the root directory of the source code. This will compile both treasure manager and hub, and
will output 2 executables, `tm` (Phase 1) and `th` (Phase 2).

The configured compiler is `gcc`.

## Treasure manager available commands:

`./tm --add <huntId>`
`./tm --l <huntId>`                     (lists treasures in a hunt)
`./tm --la`                             (lists all hunts)
`./tm --view <huntId> <treasureId>`
`./tm --rm_h <huntId>`                  (removes an entire hunt)
`./tm --rm_t <huntId> <treasureId>`     (removes a treasure)

## Treasure hub available commands:

`./th` to run the program and then

`start_monitor`         to create the child process (monitor)
`stop_monitor`          to end it
`list_hunts`            to list all treasures
`list_treasures`        to list treasures in a specified hunt
`view_treasure`         to view a treasure from a specified hunt
`exit`                  to exit the main program, provided that there is no monitor (child process) active
