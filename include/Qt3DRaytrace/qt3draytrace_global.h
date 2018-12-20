/*
 * Copyright (C) 2018 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <Qt3DCore/qt3dcore_global.h>

#if defined(QUARTZ_SHARED)
#  if defined(QT3DRAYTRACE_LIBRARY)
#    define QT3DRAYTRACESHARED_EXPORT Q_DECL_EXPORT
#  else
#    define QT3DRAYTRACESHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QT3DRAYTRACESHARED_EXPORT
#endif
