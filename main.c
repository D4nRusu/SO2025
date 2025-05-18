#include <stdio.h>
#include <string.h>

#include "treasure-manager.h"
#include "treasure-hub.h"

int main(int argc, char** argv)
{
    if(argv[0][3] == 'h') // same main for both ./tm and ./th
        return th();
    return tm(argc, argv);
}