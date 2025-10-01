/****************************************************************************
** Meta object code from reading C++ file 'MeshRepair.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/repair/MeshRepair.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MeshRepair.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MeshRepairE_t {};
} // unnamed namespace

template <> constexpr inline auto MeshRepair::qt_create_metaobjectdata<qt_meta_tag_ZN10MeshRepairE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MeshRepair",
        "repairProgress",
        "",
        "meshName",
        "percentage",
        "stage",
        "repairStarted",
        "repairCompleted",
        "RepairResult",
        "result",
        "issuesFound",
        "QList<MeshIssue>",
        "issues",
        "criticalIssuesFound",
        "batchRepairProgress",
        "completed",
        "total",
        "batchRepairCompleted",
        "QList<RepairResult>",
        "results"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'repairProgress'
        QtMocHelpers::SignalData<void(const QString &, int, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'repairStarted'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'repairCompleted'
        QtMocHelpers::SignalData<void(const QString &, const RepairResult &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 8, 9 },
        }}),
        // Signal 'issuesFound'
        QtMocHelpers::SignalData<void(const QString &, const QList<MeshIssue> &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 11, 12 },
        }}),
        // Signal 'criticalIssuesFound'
        QtMocHelpers::SignalData<void(const QString &, const QList<MeshIssue> &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 11, 12 },
        }}),
        // Signal 'batchRepairProgress'
        QtMocHelpers::SignalData<void(int, int)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { QMetaType::Int, 16 },
        }}),
        // Signal 'batchRepairCompleted'
        QtMocHelpers::SignalData<void(const QList<RepairResult> &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MeshRepair, qt_meta_tag_ZN10MeshRepairE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MeshRepair::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MeshRepairE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MeshRepairE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MeshRepairE_t>.metaTypes,
    nullptr
} };

void MeshRepair::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MeshRepair *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->repairProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->repairStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->repairCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<RepairResult>>(_a[2]))); break;
        case 3: _t->issuesFound((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<MeshIssue>>>(_a[2]))); break;
        case 4: _t->criticalIssuesFound((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<MeshIssue>>>(_a[2]))); break;
        case 5: _t->batchRepairProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 6: _t->batchRepairCompleted((*reinterpret_cast< std::add_pointer_t<QList<RepairResult>>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MeshRepair::*)(const QString & , int , const QString & )>(_a, &MeshRepair::repairProgress, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshRepair::*)(const QString & )>(_a, &MeshRepair::repairStarted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshRepair::*)(const QString & , const RepairResult & )>(_a, &MeshRepair::repairCompleted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshRepair::*)(const QString & , const QList<MeshIssue> & )>(_a, &MeshRepair::issuesFound, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshRepair::*)(const QString & , const QList<MeshIssue> & )>(_a, &MeshRepair::criticalIssuesFound, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshRepair::*)(int , int )>(_a, &MeshRepair::batchRepairProgress, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MeshRepair::*)(const QList<RepairResult> & )>(_a, &MeshRepair::batchRepairCompleted, 6))
            return;
    }
}

const QMetaObject *MeshRepair::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MeshRepair::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MeshRepairE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MeshRepair::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void MeshRepair::repairProgress(const QString & _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void MeshRepair::repairStarted(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void MeshRepair::repairCompleted(const QString & _t1, const RepairResult & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void MeshRepair::issuesFound(const QString & _t1, const QList<MeshIssue> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void MeshRepair::criticalIssuesFound(const QString & _t1, const QList<MeshIssue> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void MeshRepair::batchRepairProgress(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void MeshRepair::batchRepairCompleted(const QList<RepairResult> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
