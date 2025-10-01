/****************************************************************************
** Meta object code from reading C++ file 'ProgressiveLoader.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/render/ProgressiveLoader.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProgressiveLoader.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17ProgressiveLoaderE_t {};
} // unnamed namespace

template <> constexpr inline auto ProgressiveLoader::qt_create_metaobjectdata<qt_meta_tag_ZN17ProgressiveLoaderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ProgressiveLoader",
        "loadingProgress",
        "",
        "percentage",
        "stage",
        "loadingStateChanged",
        "LoadingState",
        "state",
        "modelUpdated",
        "ModelData",
        "model",
        "memoryUsageChanged",
        "bytesUsed",
        "memoryWarning",
        "requiredBytes",
        "availableBytes",
        "qualityChanged",
        "newLevel",
        "detailLevelChanged",
        "level",
        "loadingFailed",
        "error",
        "loadingCancelled"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'loadingProgress'
        QtMocHelpers::SignalData<void(int, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::QString, 4 },
        }}),
        // Signal 'loadingStateChanged'
        QtMocHelpers::SignalData<void(enum LoadingState)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'modelUpdated'
        QtMocHelpers::SignalData<void(const ModelData &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'memoryUsageChanged'
        QtMocHelpers::SignalData<void(qint64)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 },
        }}),
        // Signal 'memoryWarning'
        QtMocHelpers::SignalData<void(qint64, qint64)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 14 }, { QMetaType::LongLong, 15 },
        }}),
        // Signal 'qualityChanged'
        QtMocHelpers::SignalData<void(float)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 17 },
        }}),
        // Signal 'detailLevelChanged'
        QtMocHelpers::SignalData<void(int)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 19 },
        }}),
        // Signal 'loadingFailed'
        QtMocHelpers::SignalData<void(const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 21 },
        }}),
        // Signal 'loadingCancelled'
        QtMocHelpers::SignalData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ProgressiveLoader, qt_meta_tag_ZN17ProgressiveLoaderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ProgressiveLoader::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ProgressiveLoaderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ProgressiveLoaderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17ProgressiveLoaderE_t>.metaTypes,
    nullptr
} };

void ProgressiveLoader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ProgressiveLoader *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loadingProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->loadingStateChanged((*reinterpret_cast< std::add_pointer_t<enum LoadingState>>(_a[1]))); break;
        case 2: _t->modelUpdated((*reinterpret_cast< std::add_pointer_t<ModelData>>(_a[1]))); break;
        case 3: _t->memoryUsageChanged((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 4: _t->memoryWarning((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 5: _t->qualityChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 6: _t->detailLevelChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->loadingFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->loadingCancelled(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(int , const QString & )>(_a, &ProgressiveLoader::loadingProgress, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(LoadingState )>(_a, &ProgressiveLoader::loadingStateChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(const ModelData & )>(_a, &ProgressiveLoader::modelUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(qint64 )>(_a, &ProgressiveLoader::memoryUsageChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(qint64 , qint64 )>(_a, &ProgressiveLoader::memoryWarning, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(float )>(_a, &ProgressiveLoader::qualityChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(int )>(_a, &ProgressiveLoader::detailLevelChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)(const QString & )>(_a, &ProgressiveLoader::loadingFailed, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ProgressiveLoader::*)()>(_a, &ProgressiveLoader::loadingCancelled, 8))
            return;
    }
}

const QMetaObject *ProgressiveLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgressiveLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ProgressiveLoaderE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ProgressiveLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ProgressiveLoader::loadingProgress(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void ProgressiveLoader::loadingStateChanged(LoadingState _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ProgressiveLoader::modelUpdated(const ModelData & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ProgressiveLoader::memoryUsageChanged(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ProgressiveLoader::memoryWarning(qint64 _t1, qint64 _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void ProgressiveLoader::qualityChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ProgressiveLoader::detailLevelChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void ProgressiveLoader::loadingFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void ProgressiveLoader::loadingCancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
