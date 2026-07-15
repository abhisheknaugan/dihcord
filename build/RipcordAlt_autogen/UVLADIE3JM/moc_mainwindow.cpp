/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onGuildsFetched",
        "",
        "jsonPayload",
        "onGuildsFetchFailed",
        "reason",
        "onGatewayReady",
        "QJsonObject",
        "readyPayload",
        "onGuildAvailable",
        "guild",
        "onGatewayError",
        "onGuildSelected",
        "QListWidgetItem*",
        "item",
        "onChannelSelected",
        "onMessageReceived",
        "message",
        "onMessagesFetched",
        "channelId",
        "QJsonArray",
        "messages",
        "onMessagesFetchFailed",
        "onSendClicked",
        "onCurrentUserFetched",
        "user",
        "onEditProfileClicked",
        "onProfileSaveRequested",
        "patchFields",
        "onProfileUpdated",
        "updatedUser",
        "onProfileUpdateFailed",
        "onStatusChanged",
        "index",
        "onDetectableGamesFetched",
        "games",
        "onGameDetected",
        "displayName",
        "applicationId",
        "onGameEnded",
        "onVoiceStateUpdate",
        "data",
        "onVoiceServerUpdate",
        "onVoiceHandshakeComplete",
        "onVoiceError",
        "onVoiceLog",
        "line",
        "onMuteClicked",
        "onDeafenClicked",
        "onDisconnectVoiceClicked",
        "onVoiceSettingsClicked",
        "onMemberClicked",
        "onServerInfoClicked",
        "onRelationshipsFetched",
        "friends",
        "onDMChannelsFetched",
        "channels",
        "onDMOpened",
        "channel",
        "onUserProfileFetched",
        "onUserProfileFetchFailed",
        "onMessageProfileRequested",
        "userId",
        "onMessageContextMenu",
        "QPoint",
        "pos",
        "onTypingStarted",
        "onTypingTextChanged",
        "onClearTypingLabel",
        "onPinnedMessagesClicked",
        "onPinnedMessagesFetched"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onGuildsFetched'
        QtMocHelpers::SlotData<void(const QByteArray &)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 3 },
        }}),
        // Slot 'onGuildsFetchFailed'
        QtMocHelpers::SlotData<void(const QString &)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'onGatewayReady'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'onGuildAvailable'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 10 },
        }}),
        // Slot 'onGatewayError'
        QtMocHelpers::SlotData<void(const QString &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'onGuildSelected'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Slot 'onChannelSelected'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Slot 'onMessageReceived'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 17 },
        }}),
        // Slot 'onMessagesFetched'
        QtMocHelpers::SlotData<void(const QString &, const QJsonArray &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 19 }, { 0x80000000 | 20, 21 },
        }}),
        // Slot 'onMessagesFetchFailed'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 19 }, { QMetaType::QString, 5 },
        }}),
        // Slot 'onSendClicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCurrentUserFetched'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 25 },
        }}),
        // Slot 'onEditProfileClicked'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onProfileSaveRequested'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 28 },
        }}),
        // Slot 'onProfileUpdated'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(29, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 30 },
        }}),
        // Slot 'onProfileUpdateFailed'
        QtMocHelpers::SlotData<void(const QString &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'onStatusChanged'
        QtMocHelpers::SlotData<void(int)>(32, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 33 },
        }}),
        // Slot 'onDetectableGamesFetched'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 35 },
        }}),
        // Slot 'onGameDetected'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 37 }, { QMetaType::QString, 38 },
        }}),
        // Slot 'onGameEnded'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVoiceStateUpdate'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 41 },
        }}),
        // Slot 'onVoiceServerUpdate'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(42, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 41 },
        }}),
        // Slot 'onVoiceHandshakeComplete'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVoiceError'
        QtMocHelpers::SlotData<void(const QString &)>(44, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'onVoiceLog'
        QtMocHelpers::SlotData<void(const QString &)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 46 },
        }}),
        // Slot 'onMuteClicked'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeafenClicked'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnectVoiceClicked'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVoiceSettingsClicked'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMemberClicked'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(51, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Slot 'onServerInfoClicked'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRelationshipsFetched'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(53, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 54 },
        }}),
        // Slot 'onDMChannelsFetched'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(55, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 56 },
        }}),
        // Slot 'onDMOpened'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(57, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 58 },
        }}),
        // Slot 'onUserProfileFetched'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(59, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 25 },
        }}),
        // Slot 'onUserProfileFetchFailed'
        QtMocHelpers::SlotData<void(const QString &)>(60, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'onMessageProfileRequested'
        QtMocHelpers::SlotData<void(const QString &)>(61, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 62 },
        }}),
        // Slot 'onMessageContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(63, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 64, 65 },
        }}),
        // Slot 'onTypingStarted'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(66, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 41 },
        }}),
        // Slot 'onTypingTextChanged'
        QtMocHelpers::SlotData<void()>(67, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClearTypingLabel'
        QtMocHelpers::SlotData<void()>(68, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPinnedMessagesClicked'
        QtMocHelpers::SlotData<void()>(69, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPinnedMessagesFetched'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(70, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onGuildsFetched((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 1: _t->onGuildsFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->onGatewayReady((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 3: _t->onGuildAvailable((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 4: _t->onGatewayError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onGuildSelected((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 6: _t->onChannelSelected((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 7: _t->onMessageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 8: _t->onMessagesFetched((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[2]))); break;
        case 9: _t->onMessagesFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->onSendClicked(); break;
        case 11: _t->onCurrentUserFetched((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 12: _t->onEditProfileClicked(); break;
        case 13: _t->onProfileSaveRequested((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 14: _t->onProfileUpdated((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 15: _t->onProfileUpdateFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->onStatusChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->onDetectableGamesFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 18: _t->onGameDetected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 19: _t->onGameEnded(); break;
        case 20: _t->onVoiceStateUpdate((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 21: _t->onVoiceServerUpdate((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 22: _t->onVoiceHandshakeComplete(); break;
        case 23: _t->onVoiceError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 24: _t->onVoiceLog((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 25: _t->onMuteClicked(); break;
        case 26: _t->onDeafenClicked(); break;
        case 27: _t->onDisconnectVoiceClicked(); break;
        case 28: _t->onVoiceSettingsClicked(); break;
        case 29: _t->onMemberClicked((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 30: _t->onServerInfoClicked(); break;
        case 31: _t->onRelationshipsFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 32: _t->onDMChannelsFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 33: _t->onDMOpened((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 34: _t->onUserProfileFetched((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 35: _t->onUserProfileFetchFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 36: _t->onMessageProfileRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 37: _t->onMessageContextMenu((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 38: _t->onTypingStarted((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 39: _t->onTypingTextChanged(); break;
        case 40: _t->onClearTypingLabel(); break;
        case 41: _t->onPinnedMessagesClicked(); break;
        case 42: _t->onPinnedMessagesFetched((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 43)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 43;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 43)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 43;
    }
    return _id;
}
QT_WARNING_POP
