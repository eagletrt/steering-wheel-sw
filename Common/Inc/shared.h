/*!
 * \file shared.h
 * \date 2025-12-21
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Header file defining shared data structures
 *
 * \details This file contains the definition of data structures
 *     that are shared between the 2 cores.
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

/*!
 * \brief Mock structure representing shared data
 */
struct MockSharedData {
    uint32_t value;
    float ratio;
};

#endif // SHARED_H
