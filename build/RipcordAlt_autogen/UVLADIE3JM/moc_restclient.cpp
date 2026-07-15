/****************************************************************************
** Meta object code from reading C++ file 'restclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/restclient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'restclient.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10RestClientE_t {};
} // unnamed namespace

template <> constexpr inline auto RestClient::qt_create_metaobjectdata<qt_meta_tag_ZN10RestClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RestClient",
        "loginSucceeded",
        "",
        "token",
        "mfaRequired",
        "ticket",
        "captchaRequired",
        "loginFailed",
        "reason",
        "guildsFetched",
        "jsonPayload",
        "guildsFetchFailed",
        "messagesFetched",
        "channelId",
        "QJsonArray",
        "messages",
        "messagesFetchFailed",
        "messageSendFailed",
        "currentUserFetched",
        "QJsonObject",
        "user",
        "currentUserFetchFailed",
        "profileUpdated",
        "updatedUser",
        "profileUpdateFailed",
        "detectableGamesFetched",
        "games",
        "detectableGamesFetchFailed",
        "relationshipsFetched",
        "friends",
        "dmChannelsFetched",
        "channels",
        "dmOpened",
        "channel",
        "userProfileFetched",
        "userProfileFetchFailed",
        "pinnedMessagesFetched"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'loginSucceeded'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'mfaRequired'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'captchaRequired'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'loginFailed'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'guildsFetched'
        QtMocHelpers::SignalData<void(const QByteArray &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 10 },
        }}),
        // Signal 'guildsFetchFailed'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'messagesFetched'
        QtMocHelpers::SignalData<void(const QString &, const QJsonArray &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { 0x80000000 | 14, 15 },
        }}),
        // Signal 'messagesFetchFailed'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'messageSendFailed'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'currentUserFetched'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Signal 'currentUserFetchFailed'
        QtMocHelpers::SignalData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'profileUpdated'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 23 },
        }}),
        // Signal 'profileUpdateFailed'
        QtMocHelpers::SignalData<void(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'detectableGamesFetched'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 26 },
        }}),
        // Signal 'detectableGamesFetchFailed'
        QtMocHelpers::SignalData<void(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'relationshipsFetched'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 29 },
        }}),
        // Signal 'dmChannelsFetched'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 31 },
        }}),
        // Signal 'dmOpened'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 33 },
        }}),
        // Signal 'userProfileFetched'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Signal 'userProfileFetchFailed'
        QtMocHelpers::SignalData<void(const QString &)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'pinnedMessagesFetched'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(36, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RestClient, qt_meta_tag_ZN10RestClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RestClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10RestClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10RestClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10RestClientE_t>.metaTypes,
    nullptr
} };

void RestClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RestClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loginSucceeded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->mfaRequired((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->captchaRequired(); break;
        case 3: _t->loginFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->guildsFetched((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 5: _t->guildsFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->messagesFetched((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[2]))); break;
        case 7: _t->messagesFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->messageSendFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->currentUserFetched((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 10: _t->currentUserFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->profileUpdated((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 12: _t->profileUpdateFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->detectableGamesFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 14: _t->detectableGamesFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->relationshipsFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 16: _t->dmChannelsFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 17: _t->dmOpened((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 18: _t->userProfileFetched((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 19: _t->userProfileFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->pinnedMessagesFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::loginSucceeded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::mfaRequired, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)()>(_a, &RestClient::captchaRequired, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::loginFailed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QByteArray & )>(_a, &RestClient::guildsFetched, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::guildsFetchFailed, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & , const QJsonArray & )>(_a, &RestClient::messagesFetched, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & , const QString & )>(_a, &RestClient::messagesFetchFailed, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & , const QString & )>(_a, &RestClient::messageSendFailed, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonObject & )>(_a, &RestClient::currentUserFetched, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::currentUserFetchFailed, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonObject & )>(_a, &RestClient::profileUpdated, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::profileUpdateFailed, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonArray & )>(_a, &RestClient::detectableGamesFetched, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::detectableGamesFetchFailed, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonArray & )>(_a, &RestClient::relationshipsFetched, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonArray & )>(_a, &RestClient::dmChannelsFetched, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonObject & )>(_a, &RestClient::dmOpened, 17))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonObject & )>(_a, &RestClient::userProfileFetched, 18))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QString & )>(_a, &RestClient::userProfileFetchFailed, 19))
            return;
        if (QtMocHelpers::indexOfMethod<void (RestClient::*)(const QJsonArray & )>(_a, &RestClient::pinnedMessagesFetched, 20))
            return;
    }
}

const QMetaObject *RestClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RestClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10RestClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RestClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void RestClient::loginSucceeded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void RestClient::mfaRequired(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void RestClient::captchaRequired()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RestClient::loginFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void RestClient::guildsFetched(const QByteArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void RestClient::guildsFetchFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void RestClient::messagesFetched(const QString & _t1, const QJsonArray & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void RestClient::messagesFetchFailed(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void RestClient::messageSendFailed(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void RestClient::currentUserFetched(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void RestClient::currentUserFetchFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void RestClient::profileUpdated(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void RestClient::profileUpdateFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1);
}

// SIGNAL 13
void RestClient::detectableGamesFetched(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void RestClient::detectableGamesFetchFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}

// SIGNAL 15
void RestClient::relationshipsFetched(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1);
}

// SIGNAL 16
void RestClient::dmChannelsFetched(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 16, nullptr, _t1);
}

// SIGNAL 17
void RestClient::dmOpened(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 17, nullptr, _t1);
}

// SIGNAL 18
void RestClient::userProfileFetched(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 18, nullptr, _t1);
}

// SIGNAL 19
void RestClient::userProfileFetchFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 19, nullptr, _t1);
}

// SIGNAL 20
void RestClient::pinnedMessagesFetched(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 20, nullptr, _t1);
}
QT_WARNING_POP
