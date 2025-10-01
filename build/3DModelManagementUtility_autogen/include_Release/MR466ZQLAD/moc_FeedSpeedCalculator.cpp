/****************************************************************************
** Meta object code from reading C++ file 'FeedSpeedCalculator.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/tools/FeedSpeedCalculator.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FeedSpeedCalculator.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19FeedSpeedCalculatorE_t {};
} // unnamed namespace

template <> constexpr inline auto FeedSpeedCalculator::qt_create_metaobjectdata<qt_meta_tag_ZN19FeedSpeedCalculatorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FeedSpeedCalculator",
        "calculationCompleted",
        "",
        "CalculationResult",
        "result",
        "calculationFailed",
        "error",
        "safetyWarning",
        "warning",
        "performanceWarning",
        "parametersOptimized",
        "CuttingParameters",
        "optimized"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'calculationCompleted'
        QtMocHelpers::SignalData<void(const CalculationResult &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'calculationFailed'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'safetyWarning'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'performanceWarning'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'parametersOptimized'
        QtMocHelpers::SignalData<void(const CuttingParameters &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FeedSpeedCalculator, qt_meta_tag_ZN19FeedSpeedCalculatorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FeedSpeedCalculator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19FeedSpeedCalculatorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19FeedSpeedCalculatorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19FeedSpeedCalculatorE_t>.metaTypes,
    nullptr
} };

void FeedSpeedCalculator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FeedSpeedCalculator *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->calculationCompleted((*reinterpret_cast< std::add_pointer_t<CalculationResult>>(_a[1]))); break;
        case 1: _t->calculationFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->safetyWarning((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->performanceWarning((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->parametersOptimized((*reinterpret_cast< std::add_pointer_t<CuttingParameters>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FeedSpeedCalculator::*)(const CalculationResult & )>(_a, &FeedSpeedCalculator::calculationCompleted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FeedSpeedCalculator::*)(const QString & )>(_a, &FeedSpeedCalculator::calculationFailed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FeedSpeedCalculator::*)(const QString & )>(_a, &FeedSpeedCalculator::safetyWarning, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (FeedSpeedCalculator::*)(const QString & )>(_a, &FeedSpeedCalculator::performanceWarning, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (FeedSpeedCalculator::*)(const CuttingParameters & )>(_a, &FeedSpeedCalculator::parametersOptimized, 4))
            return;
    }
}

const QMetaObject *FeedSpeedCalculator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FeedSpeedCalculator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19FeedSpeedCalculatorE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FeedSpeedCalculator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void FeedSpeedCalculator::calculationCompleted(const CalculationResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void FeedSpeedCalculator::calculationFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void FeedSpeedCalculator::safetyWarning(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void FeedSpeedCalculator::performanceWarning(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void FeedSpeedCalculator::parametersOptimized(const CuttingParameters & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
