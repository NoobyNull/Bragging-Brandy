/****************************************************************************
** Meta object code from reading C++ file 'ModelService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/core/ModelService.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ModelService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12ModelServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto ModelService::qt_create_metaobjectdata<qt_meta_tag_ZN12ModelServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ModelService",
        "modelLoaded",
        "",
        "ModelMetadata",
        "model",
        "modelDeleted",
        "id",
        "modelUpdated",
        "modelsImported",
        "QList<ModelMetadata>",
        "models",
        "importProgress",
        "filename",
        "percentage",
        "exportProgress",
        "errorOccurred",
        "operation",
        "error",
        "details"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modelLoaded'
        QtMocHelpers::SignalData<void(const ModelMetadata &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'modelDeleted'
        QtMocHelpers::SignalData<void(const QUuid &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QUuid, 6 },
        }}),
        // Signal 'modelUpdated'
        QtMocHelpers::SignalData<void(const ModelMetadata &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'modelsImported'
        QtMocHelpers::SignalData<void(const QList<ModelMetadata> &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'importProgress'
        QtMocHelpers::SignalData<void(const QString &, int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::Int, 13 },
        }}),
        // Signal 'exportProgress'
        QtMocHelpers::SignalData<void(const QString &, int)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::Int, 13 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 }, { QMetaType::QString, 17 }, { QMetaType::QString, 18 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ModelService, qt_meta_tag_ZN12ModelServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ModelService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ModelServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ModelServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12ModelServiceE_t>.metaTypes,
    nullptr
} };

void ModelService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ModelService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modelLoaded((*reinterpret_cast< std::add_pointer_t<ModelMetadata>>(_a[1]))); break;
        case 1: _t->modelDeleted((*reinterpret_cast< std::add_pointer_t<QUuid>>(_a[1]))); break;
        case 2: _t->modelUpdated((*reinterpret_cast< std::add_pointer_t<ModelMetadata>>(_a[1]))); break;
        case 3: _t->modelsImported((*reinterpret_cast< std::add_pointer_t<QList<ModelMetadata>>>(_a[1]))); break;
        case 4: _t->importProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->exportProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 6: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ModelService::*)(const ModelMetadata & )>(_a, &ModelService::modelLoaded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelService::*)(const QUuid & )>(_a, &ModelService::modelDeleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelService::*)(const ModelMetadata & )>(_a, &ModelService::modelUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelService::*)(const QList<ModelMetadata> & )>(_a, &ModelService::modelsImported, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelService::*)(const QString & , int )>(_a, &ModelService::importProgress, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelService::*)(const QString & , int )>(_a, &ModelService::exportProgress, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ModelService::*)(const QString & , const QString & , const QString & )>(_a, &ModelService::errorOccurred, 6))
            return;
    }
}

const QMetaObject *ModelService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModelService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ModelServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModelService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ModelService::modelLoaded(const ModelMetadata & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ModelService::modelDeleted(const QUuid & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ModelService::modelUpdated(const ModelMetadata & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ModelService::modelsImported(const QList<ModelMetadata> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ModelService::importProgress(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void ModelService::exportProgress(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void ModelService::errorOccurred(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
