/****************************************************************************
** Meta object code from reading C++ file 'voiceclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/voiceclient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'voiceclient.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11VoiceClientE_t {};
} // unnamed namespace

template <> constexpr inline auto VoiceClient::qt_create_metaobjectdata<qt_meta_tag_ZN11VoiceClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "VoiceClient",
        "voiceHandshakeComplete",
        "",
        "voiceError",
        "reason",
        "voiceLog",
        "line",
        "voiceDisconnected",
        "onSocketConnected",
        "onSocketDisconnected",
        "onTextMessageReceived",
        "message",
        "onBinaryMessageReceived",
        "onUdpReadyRead",
        "sendHeartbeat"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'voiceHandshakeComplete'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'voiceError'
        QtMocHelpers::SignalData<void(const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'voiceLog'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'voiceDisconnected'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSocketConnected'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketDisconnected'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTextMessageReceived'
        QtMocHelpers::SlotData<void(const QString &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'onBinaryMessageReceived'
        QtMocHelpers::SlotData<void(const QByteArray &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 11 },
        }}),
        // Slot 'onUdpReadyRead'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'sendHeartbeat'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<VoiceClient, qt_meta_tag_ZN11VoiceClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject VoiceClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11VoiceClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11VoiceClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11VoiceClientE_t>.metaTypes,
    nullptr
} };

void VoiceClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<VoiceClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->voiceHandshakeComplete(); break;
        case 1: _t->voiceError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->voiceLog((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->voiceDisconnected(); break;
        case 4: _t->onSocketConnected(); break;
        case 5: _t->onSocketDisconnected(); break;
        case 6: _t->onTextMessageReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onBinaryMessageReceived((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 8: _t->onUdpReadyRead(); break;
        case 9: _t->sendHeartbeat(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (VoiceClient::*)()>(_a, &VoiceClient::voiceHandshakeComplete, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (VoiceClient::*)(const QString & )>(_a, &VoiceClient::voiceError, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (VoiceClient::*)(const QString & )>(_a, &VoiceClient::voiceLog, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (VoiceClient::*)()>(_a, &VoiceClient::voiceDisconnected, 3))
            return;
    }
}

const QMetaObject *VoiceClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VoiceClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11VoiceClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int VoiceClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void VoiceClient::voiceHandshakeComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void VoiceClient::voiceError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void VoiceClient::voiceLog(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void VoiceClient::voiceDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
