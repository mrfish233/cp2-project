#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "parse_file.h"
#include "script.h"

int32_t parseTomlScript(char *filename, Script *script);
