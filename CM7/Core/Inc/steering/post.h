/*!
 * \file post.h
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief This file defines Power-On Self-Test (POST) structures for system diagnostics.
 */

#ifndef POST_H
#define POST_H

#include "fontutils.h"
#include "raster.h"

enum PostReturnCode {
    POST_RC_OK,    /*!< POST completed successfully. */
    POST_RC_ERROR, /*!< POST encountered an error. */
};

struct PostInitData {
    font_draw_line_callback draw_line;             /*!< Callback function required by fontutils module. */
    raster_draw_rectangle_callback draw_rectangle; /*!< Callback function required by screen module. */
};

#endif // POST_H
