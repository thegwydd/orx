/* Orx - Portable Game Engine
 *
 * Orx is the legal property of its developers, whose names
 * are listed in the COPYRIGHT file distributed 
 * with this source distribution.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @file orxPlugin_Display.h
 * @date 23/04/2003
 * @author iarwain@orx-project.org
 *
 * @todo
 */

/**
 * @addtogroup orxPlugin
 * 
 * Header that defines all IDs of the display plugin
 *
 * @{
 */


#ifndef _orxPLUGIN_DISPLAY_H_
#define _orxPLUGIN_DISPLAY_H_

#include "plugin/define/orxPlugin_CoreID.h"


typedef enum __orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_t
{
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_INIT = 0,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_EXIT,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_SWAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_CREATE_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_DELETE_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_SAVE_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_LOAD_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_TRANSFORM_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_CLEAR_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_BLIT_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_SET_BITMAP_COLOR_KEY,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_SET_BITMAP_COLOR,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_SET_BITMAP_CLIPPING,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_BITMAP_COLOR,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_BITMAP_SIZE,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_SCREEN_BITMAP,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_SCREEN_SIZE,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_CREATE_TEXT,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_DELETE_TEXT,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_TRANSFORM_TEXT,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_SET_TEXT_STRING,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_SET_TEXT_FONT,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_TEXT_STRING,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_TEXT_FONT,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_TEXT_SIZE,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_PRINT_STRING,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_GET_APPLICATION_INPUT,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_ENABLE_VSYNC,
  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_IS_VSYNC_ENABLED,

  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_NUMBER,

  orxPLUGIN_FUNCTION_BASE_ID_DISPLAY_NONE = orxENUM_NONE

} orxPLUGIN_FUNCTION_BASE_ID_DISPLAY;

#endif /* _orxPLUGIN_DISPLAY_H_ */

/** @} */
