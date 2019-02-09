/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#include <qfirstpersoncameracontroller_p.h>

#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QButtonAxisInput>

#include <Qt3DLogic/QFrameAction>

namespace Qt3DRaytraceExtras {

QFirstPersonCameraControllerPrivate::QFirstPersonCameraControllerPrivate()
    : m_camera(nullptr)
    , m_keyboardDevice(new Qt3DInput::QKeyboardDevice)
    , m_mouseDevice(new Qt3DInput::QMouseDevice)
    , m_logicalDevice(new Qt3DInput::QLogicalDevice)
    , m_frameAction(new Qt3DLogic::QFrameAction)
    , m_rxAxis(new Qt3DInput::QAxis)
    , m_ryAxis(new Qt3DInput::QAxis)
    , m_txAxis(new Qt3DInput::QAxis)
    , m_tyAxis(new Qt3DInput::QAxis)
    , m_tzAxis(new Qt3DInput::QAxis)
    , m_mouseRxInput(new Qt3DInput::QAnalogAxisInput)
    , m_mouseRyInput(new Qt3DInput::QAnalogAxisInput)
    , m_keyboardTxPosInput(new Qt3DInput::QButtonAxisInput)
    , m_keyboardTyPosInput(new Qt3DInput::QButtonAxisInput)
    , m_keyboardTzPosInput(new Qt3DInput::QButtonAxisInput)
    , m_keyboardTxNegInput(new Qt3DInput::QButtonAxisInput)
    , m_keyboardTyNegInput(new Qt3DInput::QButtonAxisInput)
    , m_keyboardTzNegInput(new Qt3DInput::QButtonAxisInput)
    , m_moveSpeed(10.0f)
    , m_lookSpeed(360.0f)
    , m_acceleration(-1.0f)
    , m_deceleration(-1.0f)
{
    m_mouseRxInput->setAxis(Qt3DInput::QMouseDevice::X);
    m_mouseRxInput->setSourceDevice(m_mouseDevice);
    m_mouseRyInput->setAxis(Qt3DInput::QMouseDevice::Y);
    m_mouseRyInput->setSourceDevice(m_mouseDevice);

    m_keyboardTxPosInput->setButtons({Qt::Key_D});
    m_keyboardTxPosInput->setScale(1.0f);
    m_keyboardTxPosInput->setSourceDevice(m_keyboardDevice);
    m_keyboardTxNegInput->setButtons({Qt::Key_A});
    m_keyboardTxNegInput->setScale(-1.0f);
    m_keyboardTxNegInput->setSourceDevice(m_keyboardDevice);
    m_keyboardTyPosInput->setButtons({Qt::Key_Q});
    m_keyboardTyPosInput->setScale(1.0f);
    m_keyboardTyPosInput->setSourceDevice(m_keyboardDevice);
    m_keyboardTyNegInput->setButtons({Qt::Key_E});
    m_keyboardTyNegInput->setScale(-1.0f);
    m_keyboardTyNegInput->setSourceDevice(m_keyboardDevice);
    m_keyboardTzPosInput->setButtons({Qt::Key_S});
    m_keyboardTzPosInput->setScale(1.0f);
    m_keyboardTzPosInput->setSourceDevice(m_keyboardDevice);
    m_keyboardTzNegInput->setButtons({Qt::Key_W});
    m_keyboardTzNegInput->setScale(-1.0f);
    m_keyboardTzNegInput->setSourceDevice(m_keyboardDevice);

    m_rxAxis->addInput(m_mouseRxInput);
    m_ryAxis->addInput(m_mouseRyInput);

    m_txAxis->addInput(m_keyboardTxPosInput);
    m_txAxis->addInput(m_keyboardTxNegInput);
    m_tyAxis->addInput(m_keyboardTyPosInput);
    m_tyAxis->addInput(m_keyboardTyNegInput);
    m_tzAxis->addInput(m_keyboardTzPosInput);
    m_tzAxis->addInput(m_keyboardTzNegInput);

    m_logicalDevice->addAxis(m_rxAxis);
    m_logicalDevice->addAxis(m_ryAxis);
    m_logicalDevice->addAxis(m_txAxis);
    m_logicalDevice->addAxis(m_tyAxis);
    m_logicalDevice->addAxis(m_tzAxis);

    applyInputAcceleration();
}

void QFirstPersonCameraControllerPrivate::applyInputAcceleration()
{
    const auto inputs = {
        m_keyboardTxPosInput,
        m_keyboardTxNegInput,
        m_keyboardTyPosInput,
        m_keyboardTyNegInput,
        m_keyboardTzPosInput,
        m_keyboardTzNegInput,
    };
    for(auto *input : inputs) {
        input->setAcceleration(m_acceleration);
        input->setDeceleration(m_deceleration);
    }
}

QFirstPersonCameraController::QFirstPersonCameraController(Qt3DCore::QNode *parent)
    : QFirstPersonCameraController(*new QFirstPersonCameraControllerPrivate, parent)
{}

QFirstPersonCameraController::QFirstPersonCameraController(QFirstPersonCameraControllerPrivate &dd, Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(dd, parent)
{
    Q_D(QFirstPersonCameraController);

    QObject::connect(d->m_frameAction, &Qt3DLogic::QFrameAction::triggered, this, &QFirstPersonCameraController::frameAction);

    addComponent(d->m_frameAction);
    addComponent(d->m_logicalDevice);
}

void QFirstPersonCameraController::frameAction(float dt)
{
    Q_D(QFirstPersonCameraController);
    if(d->m_camera) {
        float tx = d->m_txAxis->value() * d->m_moveSpeed;
        float ty = d->m_tyAxis->value() * d->m_moveSpeed;
        float tz = d->m_tzAxis->value() * d->m_moveSpeed;
        float rx = d->m_rxAxis->value() * d->m_lookSpeed;
        float ry = d->m_ryAxis->value() * d->m_lookSpeed;
        d->m_camera->translate(QVector3D{tx, ty, tz} * dt);
        d->m_camera->panWorld(rx * dt);
        d->m_camera->tiltWorld(-ry * dt);
    }
}

Qt3DRaytrace::QCamera *QFirstPersonCameraController::camera() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_camera;
}

float QFirstPersonCameraController::moveSpeed() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_moveSpeed;
}

float QFirstPersonCameraController::lookSpeed() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_lookSpeed;
}

float QFirstPersonCameraController::acceleration() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_acceleration;
}

float QFirstPersonCameraController::deceleration() const
{
    Q_D(const QFirstPersonCameraController);
    return d->m_deceleration;
}

void QFirstPersonCameraController::setCamera(Qt3DRaytrace::QCamera *camera)
{
    Q_D(QFirstPersonCameraController);
    if(d->m_camera != camera) {
        if(d->m_camera) {
            d->unregisterDestructionHelper(d->m_camera);
        }

        if(camera && camera->parent() == nullptr) {
            camera->setParent(this);
        }
        d->m_camera = camera;

        if(d->m_camera) {
            d->registerDestructionHelper(d->m_camera, &QFirstPersonCameraController::setCamera, d->m_camera);
        }

        emit cameraChanged(camera);
    }
}

void QFirstPersonCameraController::setMoveSpeed(float moveSpeed)
{
    Q_D(QFirstPersonCameraController);
    if(!qFuzzyCompare(d->m_moveSpeed, moveSpeed)) {
        d->m_moveSpeed = moveSpeed;
        emit moveSpeedChanged(moveSpeed);
    }
}

void QFirstPersonCameraController::setLookSpeed(float lookSpeed)
{
    Q_D(QFirstPersonCameraController);
    if(!qFuzzyCompare(d->m_lookSpeed, lookSpeed)) {
        d->m_lookSpeed = lookSpeed;
        emit lookSpeedChanged(lookSpeed);
    }
}

void QFirstPersonCameraController::setAcceleration(float acceleration)
{
    Q_D(QFirstPersonCameraController);
    if(!qFuzzyCompare(d->m_acceleration, acceleration)) {
        d->m_acceleration = acceleration;
        d->applyInputAcceleration();
        emit accelerationChanged(acceleration);
    }
}

void QFirstPersonCameraController::setDeceleration(float deceleration)
{
    Q_D(QFirstPersonCameraController);
    if(!qFuzzyCompare(d->m_deceleration, deceleration)) {
        d->m_deceleration = deceleration;
        d->applyInputAcceleration();
        emit decelerationChanged(deceleration);
    }
}

} // Qt3DRaytraceExtras
