/****************************************************************************
** Meta object code from reading C++ file 'DatabaseManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/core/DatabaseManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DatabaseManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
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
struct qt_meta_tag_ZN15DatabaseManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto DatabaseManager::qt_create_metaobjectdata<qt_meta_tag_ZN15DatabaseManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DatabaseManager",
        "databaseInitialized",
        "",
        "databaseClosed",
        "databaseError",
        "error",
        "details",
        "modelInserted",
        "ModelMetadata",
        "model",
        "modelUpdated",
        "modelDeleted",
        "id",
        "projectInserted",
        "ProjectData",
        "project",
        "projectUpdated",
        "projectDeleted"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'databaseInitialized'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'databaseClosed'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'databaseError'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'modelInserted'
        QtMocHelpers::SignalData<void(const ModelMetadata &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'modelUpdated'
        QtMocHelpers::SignalData<void(const ModelMetadata &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'modelDeleted'
        QtMocHelpers::SignalData<void(const QUuid &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QUuid, 12 },
        }}),
        // Signal 'projectInserted'
        QtMocHelpers::SignalData<void(const ProjectData &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Signal 'projectUpdated'
        QtMocHelpers::SignalData<void(const ProjectData &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Signal 'projectDeleted'
        QtMocHelpers::SignalData<void(const QUuid &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QUuid, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DatabaseManager, qt_meta_tag_ZN15DatabaseManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DatabaseManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DatabaseManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DatabaseManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15DatabaseManagerE_t>.metaTypes,
    nullptr
} };

void DatabaseManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DatabaseManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->databaseInitialized(); break;
        case 1: _t->databaseClosed(); break;
        case 2: _t->databaseError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->modelInserted((*reinterpret_cast< std::add_pointer_t<ModelMetadata>>(_a[1]))); break;
        case 4: _t->modelUpdated((*reinterpret_cast< std::add_pointer_t<ModelMetadata>>(_a[1]))); break;
        case 5: _t->modelDeleted((*reinterpret_cast< std::add_pointer_t<QUuid>>(_a[1]))); break;
        case 6: _t->projectInserted((*reinterpret_cast< std::add_pointer_t<ProjectData>>(_a[1]))); break;
        case 7: _t->projectUpdated((*reinterpret_cast< std::add_pointer_t<ProjectData>>(_a[1]))); break;
        case 8: _t->projectDeleted((*reinterpret_cast< std::add_pointer_t<QUuid>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)()>(_a, &DatabaseManager::databaseInitialized, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)()>(_a, &DatabaseManager::databaseClosed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)(const QString & , const QString & )>(_a, &DatabaseManager::databaseError, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)(const ModelMetadata & )>(_a, &DatabaseManager::modelInserted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)(const ModelMetadata & )>(_a, &DatabaseManager::modelUpdated, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)(const QUuid & )>(_a, &DatabaseManager::modelDeleted, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)(const ProjectData & )>(_a, &DatabaseManager::projectInserted, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)(const ProjectData & )>(_a, &DatabaseManager::projectUpdated, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (DatabaseManager::*)(const QUuid & )>(_a, &DatabaseManager::projectDeleted, 8))
            return;
    }
}

const QMetaObject *DatabaseManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DatabaseManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DatabaseManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DatabaseManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void DatabaseManager::databaseInitialized()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DatabaseManager::databaseClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DatabaseManager::databaseError(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void DatabaseManager::modelInserted(const ModelMetadata & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void DatabaseManager::modelUpdated(const ModelMetadata & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void DatabaseManager::modelDeleted(const QUuid & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void DatabaseManager::projectInserted(const ProjectData & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void DatabaseManager::projectUpdated(const ProjectData & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void DatabaseManager::projectDeleted(const QUuid & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
