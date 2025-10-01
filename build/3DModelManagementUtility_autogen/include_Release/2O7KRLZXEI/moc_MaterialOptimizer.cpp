/****************************************************************************
** Meta object code from reading C++ file 'MaterialOptimizer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/nesting/MaterialOptimizer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MaterialOptimizer.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17MaterialOptimizerE_t {};
} // unnamed namespace

template <> constexpr inline auto MaterialOptimizer::qt_create_metaobjectdata<qt_meta_tag_ZN17MaterialOptimizerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MaterialOptimizer",
        "optimizationProgress",
        "",
        "percentage",
        "generation",
        "bestFitness",
        "optimizationStarted",
        "algorithm",
        "optimizationCompleted",
        "OptimizationResult",
        "result",
        "generationCompleted",
        "averageFitness",
        "populationImproved",
        "newBestFitness",
        "performanceMetricsUpdated",
        "PerformanceMetrics",
        "metrics"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'optimizationProgress'
        QtMocHelpers::SignalData<void(int, int, float)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 4 }, { QMetaType::Float, 5 },
        }}),
        // Signal 'optimizationStarted'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'optimizationCompleted'
        QtMocHelpers::SignalData<void(const OptimizationResult &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'generationCompleted'
        QtMocHelpers::SignalData<void(int, float, float)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 }, { QMetaType::Float, 5 }, { QMetaType::Float, 12 },
        }}),
        // Signal 'populationImproved'
        QtMocHelpers::SignalData<void(float)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 14 },
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
    return QtMocHelpers::metaObjectData<MaterialOptimizer, qt_meta_tag_ZN17MaterialOptimizerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MaterialOptimizer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17MaterialOptimizerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17MaterialOptimizerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17MaterialOptimizerE_t>.metaTypes,
    nullptr
} };

void MaterialOptimizer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MaterialOptimizer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->optimizationProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3]))); break;
        case 1: _t->optimizationStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->optimizationCompleted((*reinterpret_cast< std::add_pointer_t<OptimizationResult>>(_a[1]))); break;
        case 3: _t->generationCompleted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3]))); break;
        case 4: _t->populationImproved((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 5: _t->performanceMetricsUpdated((*reinterpret_cast< std::add_pointer_t<PerformanceMetrics>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MaterialOptimizer::*)(int , int , float )>(_a, &MaterialOptimizer::optimizationProgress, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaterialOptimizer::*)(const QString & )>(_a, &MaterialOptimizer::optimizationStarted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaterialOptimizer::*)(const OptimizationResult & )>(_a, &MaterialOptimizer::optimizationCompleted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaterialOptimizer::*)(int , float , float )>(_a, &MaterialOptimizer::generationCompleted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaterialOptimizer::*)(float )>(_a, &MaterialOptimizer::populationImproved, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaterialOptimizer::*)(const PerformanceMetrics & )>(_a, &MaterialOptimizer::performanceMetricsUpdated, 5))
            return;
    }
}

const QMetaObject *MaterialOptimizer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MaterialOptimizer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17MaterialOptimizerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MaterialOptimizer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void MaterialOptimizer::optimizationProgress(int _t1, int _t2, float _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void MaterialOptimizer::optimizationStarted(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void MaterialOptimizer::optimizationCompleted(const OptimizationResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void MaterialOptimizer::generationCompleted(int _t1, float _t2, float _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3);
}

// SIGNAL 4
void MaterialOptimizer::populationImproved(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void MaterialOptimizer::performanceMetricsUpdated(const PerformanceMetrics & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
