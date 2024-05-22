// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_H
#define NEBULA_H

#include "../src/utility/CompilerHash.h"

#define GENERATE_ID(y) COMPILE_TIME_CRC32_STR(y)

#include "../src/core/Constants.h"
#include "../src/core/World.h"
#include "../src/core/Entity.h"
#include "../src/core/Component.h"
#include "../src/core/System.h"
#include "../src/core/Parser.h"

#endif // NEBULA_H