/**
 * @file logging.h
 * @copyright 2003-2025 projectM Team
 * @brief Functions for registering log callbacks and setting log levels.
 * @since 4.2.0
 *
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2024 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#pragma once

#include "projectM-4/types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
   * @brief Callback function that is executed if projectM wants to log a message.
   *
   * The message pointer is only valid inside the callback. Make a copy of the string inside the
   * function to use it after the callback returns. Applications must not free the message pointer.
   *
   * @param message The message to be logged.
   * @param log_level The log level this message was created for.
   * @param user_data A user-defined data pointer that was provided when registering the callback,
   *                  e.g. context information.
   * @since 4.2.0
   */
typedef void (*projectm_log_callback)(const char* message, projectm_log_level log_level, void* user_data);


/**
 * @brief Sets a callback function that will be called for each message to be logged by projectM.
 *
 * The logging callback is independent of any projectM instance. Applications can set the callback
 * globally, or only for a single thread, or both. If both a global and a thread-specific callback
 * are set, the thread-specific callback takes precedence and the global callback will not be called.
 *
 * Since log messages will only be emitted by projectM when within an API call and the callback runs
 * in the same thread as the projectM instance, applications can keep track of the instance making
 * the call.
 *
 * If the application runs multiple projectM instances in separate threads, a global log callback
 * (or the same thread callback registered in multiple threads) may be called in parallel from each
 * thread, so the application has to take care about any possible race conditions in its own
 * logging code and make sure it's thread-safe.
 *
 * To remove a callback, pass NULL as the callback argument. Thread-specific callbacks are removed
 * automatically when the thread is terminated, though it is good practice to reset the callback at
 * the end of a thread.
 *
 * @param callback A pointer to the callback function.
 * @param current_thread_only If true, the callback is only set for the thread calling the function.
 * @param user_data A pointer to any data that will be sent back in the callback, e.g. context
 *                  information.
 * @since 4.2.0
 * @note Log messages can contain line breaks.
 */
PROJECTM_EXPORT void projectm_set_log_callback(projectm_log_callback callback,
                                               bool current_thread_only,
                                               void* user_data);

/**
 * Sets the minimum log level for which the callback should be called.
 * @param instance The projectM instance handle.
 * @param log_level The new log level to set. If set to PROJECTM_LOG_LEVEL_NOTSET, the global or default setting will be used.
 * @param current_thread_only If true, the log level is only set for the thread calling the function.
 * @since 4.2.0
 */
PROJECTM_EXPORT void projectm_set_log_level(projectm_log_level log_level,
                                            bool current_thread_only);

#ifdef __cplusplus
} // extern "C"
#endif
