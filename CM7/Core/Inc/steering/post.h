/*!
 * \file post.h
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines Power-On Self-Test (POST) structures for system diagnostics.
 */

#ifndef POST_H
#define POST_H

enum PostReturnCode {
    POST_RC_OK,    /*!< POST completed successfully. */
    POST_RC_ERROR, /*!< POST encountered an error. */
};

struct PostInitData;

#endif // POST_H
