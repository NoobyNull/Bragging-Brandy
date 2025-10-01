/****************************************************************************
** Meta object code from reading C++ file 'CutListCanvas.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ui/CutListCanvas.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CutListCanvas.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13CutListCanvasE_t {};
} // unnamed namespace

template <> constexpr inline auto CutListCanvas::qt_create_metaobjectdata<qt_meta_tag_ZN13CutListCanvasE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CutListCanvas",
        "onPartSelectionChanged",
        "",
        "onAddPartClicked",
        "onRemovePartClicked",
        "onClearPartsClicked",
        "onMaterialSelected",
        "material",
        "onSheetSizeChanged",
        "onThicknessChanged",
        "onOptimizationStarted",
        "onOptimizationProgress",
        "percentage",
        "onOptimizationCompleted",
        "onAlgorithmSelected",
        "algorithm",
        "onRunOptimizationClicked",
        "onStopOptimizationClicked",
        "onExportGCodeClicked",
        "onExportDXFClicked",
        "onExportReportClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onPartSelectionChanged'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddPartClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRemovePartClicked'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClearPartsClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMaterialSelected'
        QtMocHelpers::SlotData<void(const QString &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onSheetSizeChanged'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onThicknessChanged'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOptimizationStarted'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOptimizationProgress'
        QtMocHelpers::SlotData<void(int)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 12 },
        }}),
        // Slot 'onOptimizationCompleted'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAlgorithmSelected'
        QtMocHelpers::SlotData<void(const QString &)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Slot 'onRunOptimizationClicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStopOptimizationClicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportGCodeClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportDXFClicked'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportReportClicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CutListCanvas, qt_meta_tag_ZN13CutListCanvasE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CutListCanvas::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseCanvas::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CutListCanvasE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CutListCanvasE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13CutListCanvasE_t>.metaTypes,
    nullptr
} };

void CutListCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CutListCanvas *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onPartSelectionChanged(); break;
        case 1: _t->onAddPartClicked(); break;
        case 2: _t->onRemovePartClicked(); break;
        case 3: _t->onClearPartsClicked(); break;
        case 4: _t->onMaterialSelected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onSheetSizeChanged(); break;
        case 6: _t->onThicknessChanged(); break;
        case 7: _t->onOptimizationStarted(); break;
        case 8: _t->onOptimizationProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->onOptimizationCompleted(); break;
        case 10: _t->onAlgorithmSelected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onRunOptimizationClicked(); break;
        case 12: _t->onStopOptimizationClicked(); break;
        case 13: _t->onExportGCodeClicked(); break;
        case 14: _t->onExportDXFClicked(); break;
        case 15: _t->onExportReportClicked(); break;
        default: ;
        }
    }
}

const QMetaObject *CutListCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CutListCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CutListCanvasE_t>.strings))
        return static_cast<void*>(this);
    return BaseCanvas::qt_metacast(_clname);
}

int CutListCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
