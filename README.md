# SO2025

## Project done by Dan-Mihai Rusu
Link to project description:
https://docs.google.com/document/d/1rTsc4YG_-UEfPQDMbXr0S1nA7a58HibXUkv_zVt34hc/edit?tab=t.0
## Progress report

    Phase 1 - complete (08/04/2025)
    Phase 2 - complete (03/05/2025)
    Phase 3 - work in progress

## How to build and run the project

Project can be built by running `make` in the root directory of the source code. This will compile the treasure manager, hub, and score calculator
It will output 3 executables, `tm` (Phase 1), `th` (Phase 2), and `tc` (Phase 3 calculator)

The configured compiler is `gcc`.

## Treasure manager available commands `./tm`:

`./tm --add <huntId>`

`./tm --l <huntId>`                     (lists treasures in a hunt)

`./tm --la`                             (lists all hunts)

`./tm --view <huntId> <treasureId>`

`./tm --rm_h <huntId>`                  (removes an entire hunt)

`./tm --rm_t <huntId> <treasureId>`     (removes a treasure)

## Treasure hub available commands `./th`:

`./th` to run the program and then

`start_monitor`         to create the child process (monitor)

`stop_monitor`          to end it

`list_hunts`            to list all treasures

`list_treasures`        to list treasures in a specified hunt

`view_treasure`         to view a treasure from a specified hunt

`exit`                  to exit the main program, provided that there is no monitor (child process) active

## Calculate score

`./tc <huntId>` to output the added value of each user in that hunt
