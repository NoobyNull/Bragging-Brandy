/****************************************************************************
** Meta object code from reading C++ file 'ThumbnailGenerator.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/render/ThumbnailGenerator.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ThumbnailGenerator.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18ThumbnailGeneratorE_t {};
} // unnamed namespace

template <> constexpr inline auto ThumbnailGenerator::qt_create_metaobjectdata<qt_meta_tag_ZN18ThumbnailGeneratorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ThumbnailGenerator",
        "thumbnailGenerated",
        "",
        "modelId",
        "thumbnailPath",
        "thumbnailGenerationFailed",
        "error",
        "batchGenerationProgress",
        "completed",
        "total",
        "thumbnailCacheCleared",
        "thumbnailCacheCleaned",
        "freedBytes",
        "generationQueueChanged",
        "queuedCount",
        "performanceMetricsUpdated",
        "PerformanceMetrics",
        "metrics"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'thumbnailGenerated'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 },
        }}),
        // Signal 'thumbnailGenerationFailed'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'batchGenerationProgress'
        QtMocHelpers::SignalData<void(int, int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Int, 9 },
        }}),
        // Signal 'thumbnailCacheCleared'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'thumbnailCacheCleaned'
        QtMocHelpers::SignalData<void(qint64)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 12 },
        }}),
        // Signal 'generationQueueChanged'
        QtMocHelpers::SignalData<void(int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Signal 'performanceMetricsUpdated'
        QtMocHelpers::SignalData<void(const PerformanceMetrics &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 16, 17 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ThumbnailGenerator, qt_meta_tag_ZN18ThumbnailGeneratorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ThumbnailGenerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ThumbnailGeneratorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ThumbnailGeneratorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18ThumbnailGeneratorE_t>.metaTypes,
    nullptr
} };

void ThumbnailGenerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ThumbnailGenerator *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->thumbnailGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->thumbnailGenerationFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->batchGenerationProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->thumbnailCacheCleared(); break;
        case 4: _t->thumbnailCacheCleaned((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 5: _t->generationQueueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->performanceMetricsUpdated((*reinterpret_cast< std::add_pointer_t<PerformanceMetrics>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ThumbnailGenerator::*)(const QString & , const QString & )>(_a, &ThumbnailGenerator::thumbnailGenerated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThumbnailGenerator::*)(const QString & , const QString & )>(_a, &ThumbnailGenerator::thumbnailGenerationFailed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThumbnailGenerator::*)(int , int )>(_a, &ThumbnailGenerator::batchGenerationProgress, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThumbnailGenerator::*)()>(_a, &ThumbnailGenerator::thumbnailCacheCleared, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThumbnailGenerator::*)(qint64 )>(_a, &ThumbnailGenerator::thumbnailCacheCleaned, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThumbnailGenerator::*)(int )>(_a, &ThumbnailGenerator::generationQueueChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThumbnailGenerator::*)(const PerformanceMetrics & )>(_a, &ThumbnailGenerator::performanceMetricsUpdated, 6))
            return;
    }
}

const QMetaObject *ThumbnailGenerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ThumbnailGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18ThumbnailGeneratorE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ThumbnailGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ThumbnailGenerator::thumbnailGenerated(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void ThumbnailGenerator::thumbnailGenerationFailed(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void ThumbnailGenerator::batchGenerationProgress(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void ThumbnailGenerator::thumbnailCacheCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ThumbnailGenerator::thumbnailCacheCleaned(qint64 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ThumbnailGenerator::generationQueueChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ThumbnailGenerator::performanceMetricsUpdated(const PerformanceMetrics & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
