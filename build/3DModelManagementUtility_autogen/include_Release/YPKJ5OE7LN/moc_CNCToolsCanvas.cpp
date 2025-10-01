/****************************************************************************
** Meta object code from reading C++ file 'CNCToolsCanvas.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ui/CNCToolsCanvas.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CNCToolsCanvas.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14CNCToolsCanvasE_t {};
} // unnamed namespace

template <> constexpr inline auto CNCToolsCanvas::qt_create_metaobjectdata<qt_meta_tag_ZN14CNCToolsCanvasE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CNCToolsCanvas",
        "onToolSelectionChanged",
        "",
        "onAddToolClicked",
        "onEditToolClicked",
        "onDeleteToolClicked",
        "onImportToolsClicked",
        "onExportToolsClicked",
        "onMaterialSelectionChanged",
        "onAddMaterialClicked",
        "onEditMaterialClicked",
        "onCalculateFeedSpeedClicked",
        "onOptimizeParametersClicked",
        "onMaterialChanged",
        "material",
        "onToolChanged",
        "tool",
        "onUpdateWearClicked",
        "onResetWearClicked",
        "onToolUsageRecorded",
        "usageTime"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onToolSelectionChanged'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddToolClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEditToolClicked'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteToolClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onImportToolsClicked'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportToolsClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMaterialSelectionChanged'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddMaterialClicked'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEditMaterialClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCalculateFeedSpeedClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOptimizeParametersClicked'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMaterialChanged'
        QtMocHelpers::SlotData<void(const QString &)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Slot 'onToolChanged'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Slot 'onUpdateWearClicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onResetWearClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToolUsageRecorded'
        QtMocHelpers::SlotData<void(float)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Float, 20 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CNCToolsCanvas, qt_meta_tag_ZN14CNCToolsCanvasE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CNCToolsCanvas::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseCanvas::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CNCToolsCanvasE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CNCToolsCanvasE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14CNCToolsCanvasE_t>.metaTypes,
    nullptr
} };

void CNCToolsCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CNCToolsCanvas *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onToolSelectionChanged(); break;
        case 1: _t->onAddToolClicked(); break;
        case 2: _t->onEditToolClicked(); break;
        case 3: _t->onDeleteToolClicked(); break;
        case 4: _t->onImportToolsClicked(); break;
        case 5: _t->onExportToolsClicked(); break;
        case 6: _t->onMaterialSelectionChanged(); break;
        case 7: _t->onAddMaterialClicked(); break;
        case 8: _t->onEditMaterialClicked(); break;
        case 9: _t->onCalculateFeedSpeedClicked(); break;
        case 10: _t->onOptimizeParametersClicked(); break;
        case 11: _t->onMaterialChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->onToolChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->onUpdateWearClicked(); break;
        case 14: _t->onResetWearClicked(); break;
        case 15: _t->onToolUsageRecorded((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *CNCToolsCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CNCToolsCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14CNCToolsCanvasE_t>.strings))
        return static_cast<void*>(this);
    return BaseCanvas::qt_metacast(_clname);
}

int CNCToolsCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseCanvas::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
