#pragma once

/* -*-c++-*- */
/*
 *  $Id: CgiDefs.h.in,v 1.10 2014/04/23 20:55:03 sebdiaz Exp $
 *
 *  Copyright (C) 1996 - 2004 Stephen F. Booth <sbooth@gnu.org>
 *                       2007 Sebastien DIAZ <sebastien.diaz@gmail.com>
 *  Part of the GNU cgicc library, http://www.gnu.org/software/cgicc
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 */


/*! \file CgiDefs.h
 * \brief Platform and operating system specific macro definitions.
 *
 * Platform and operating system specific macro definitions that deal
 * with namespace support and exporting of library functions.
 */
/*
// Include information from configure
#if HAVE_CONFIG_H
#  include "config.h"
#endif
*/
/*! \namespace cgicc
 * \brief The namespace containing the cgicc library
 *
 * The namespace containing all classes and functions of the 
 * GNU cgicc library.
 */

// Win32-specific setup
#ifdef WIN32
#define CGICC_API
#define HOST "Win32"
#define VERSION "@VERSION@"
#else
#define CGICC_API
#define HOST "Linux"
#define VERSION "@VERSION@"
#endif /* WIN32 */

