/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <qt3draytraceextras_global_p.h>
#include <Qt3DRaytraceExtras/qfirstpersoncameracontroller.h>

#include <Qt3DRaytrace/qcamera.h>

#include <Qt3DCore/private/qentity_p.h>

namespace Qt3DLogic {
class QFrameAction;
} // Qt3DLogic

namespace Qt3DInput {
class QKeyboardDevice;
class QMouseDevice;
class QLogicalDevice;
class QAxis;
class QAnalogAxisInput;
class QButtonAxisInput;
} // Qt3DInput

namespace Qt3DRaytraceExtras {

class QFirstPersonCameraControllerPrivate : public Qt3DCore::QEntityPrivate
{
    Q_DECLARE_PUBLIC(QFirstPersonCameraController)
public:
    QFirstPersonCameraControllerPrivate();

    void applyInputAcceleration();

    Qt3DRaytrace::QCamera *m_camera;

    Qt3DInput::QKeyboardDevice *m_keyboardDevice;
    Qt3DInput::QMouseDevice *m_mouseDevice;

    Qt3DInput::QLogicalDevice *m_logicalDevice;
    Qt3DLogic::QFrameAction *m_frameAction;

    Qt3DInput::QAxis *m_rxAxis;
    Qt3DInput::QAxis *m_ryAxis;
    Qt3DInput::QAxis *m_txAxis;
    Qt3DInput::QAxis *m_tyAxis;
    Qt3DInput::QAxis *m_tzAxis;

    Qt3DInput::QAnalogAxisInput *m_mouseRxInput;
    Qt3DInput::QAnalogAxisInput *m_mouseRyInput;

    Qt3DInput::QButtonAxisInput *m_keyboardTxPosInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTyPosInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTzPosInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTxNegInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTyNegInput;
    Qt3DInput::QButtonAxisInput *m_keyboardTzNegInput;

    float m_moveSpeed;
    float m_lookSpeed;
    float m_acceleration;
    float m_deceleration;
};

} // Qt3DRaytraceExtras
