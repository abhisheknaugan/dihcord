/****************************************************************************
** Meta object code from reading C++ file 'gatewayclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/gatewayclient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gatewayclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13GatewayClientE_t {};
} // unnamed namespace

template <> constexpr inline auto GatewayClient::qt_create_metaobjectdata<qt_meta_tag_ZN13GatewayClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GatewayClient",
        "connected",
        "",
        "ready",
        "QJsonObject",
        "readyPayload",
        "guildAvailable",
        "guild",
        "messageReceived",
        "message",
        "gatewayError",
        "reason",
        "gatewayClosed",
        "voiceStateUpdate",
        "data",
        "voiceServerUpdate",
        "onSocketConnected",
        "onSocketDisconnected",
        "onTextMessageReceived",
        "sendHeartbeat"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'ready'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'guildAvailable'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 7 },
        }}),
        // Signal 'messageReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 9 },
        }}),
        // Signal 'gatewayError'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Signal 'gatewayClosed'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'voiceStateUpdate'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 14 },
        }}),
        // Signal 'voiceServerUpdate'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 14 },
        }}),
        // Slot 'onSocketConnected'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketDisconnected'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTextMessageReceived'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'sendHeartbeat'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GatewayClient, qt_meta_tag_ZN13GatewayClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GatewayClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13GatewayClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13GatewayClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13GatewayClientE_t>.metaTypes,
    nullptr
} };

void GatewayClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GatewayClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->ready((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 2: _t->guildAvailable((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 3: _t->messageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 4: _t->gatewayError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->gatewayClosed(); break;
        case 6: _t->voiceStateUpdate((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 7: _t->voiceServerUpdate((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 8: _t->onSocketConnected(); break;
        case 9: _t->onSocketDisconnected(); break;
        case 10: _t->onTextMessageReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->sendHeartbeat(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)()>(_a, &GatewayClient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)(const QJsonObject & )>(_a, &GatewayClient::ready, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)(const QJsonObject & )>(_a, &GatewayClient::guildAvailable, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)(const QJsonObject & )>(_a, &GatewayClient::messageReceived, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)(const QString & )>(_a, &GatewayClient::gatewayError, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)()>(_a, &GatewayClient::gatewayClosed, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)(const QJsonObject & )>(_a, &GatewayClient::voiceStateUpdate, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (GatewayClient::*)(const QJsonObject & )>(_a, &GatewayClient::voiceServerUpdate, 7))
            return;
    }
}

const QMetaObject *GatewayClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GatewayClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13GatewayClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GatewayClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void GatewayClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void GatewayClient::ready(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void GatewayClient::guildAvailable(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void GatewayClient::messageReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void GatewayClient::gatewayError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void GatewayClient::gatewayClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void GatewayClient::voiceStateUpdate(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void GatewayClient::voiceServerUpdate(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}
QT_WARNING_POP
