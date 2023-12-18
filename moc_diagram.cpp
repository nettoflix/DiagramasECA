/****************************************************************************
** Meta object code from reading C++ file 'diagram.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "diagram.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'diagram.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.1. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDiagramENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSDiagramENDCLASS = QtMocHelpers::stringData(
    "Diagram",
    "checkPrerequisites",
    "",
    "setActive_slot",
    "setActive",
    "set",
    "buildLines",
    "paintDiagramColor",
    "color",
    "paintDiagramLines",
    "Qt::GlobalColor"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDiagramENDCLASS_t {
    uint offsetsAndSizes[22];
    char stringdata0[8];
    char stringdata1[19];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[10];
    char stringdata5[4];
    char stringdata6[11];
    char stringdata7[18];
    char stringdata8[6];
    char stringdata9[18];
    char stringdata10[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDiagramENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDiagramENDCLASS_t qt_meta_stringdata_CLASSDiagramENDCLASS = {
    {
        QT_MOC_LITERAL(0, 7),  // "Diagram"
        QT_MOC_LITERAL(8, 18),  // "checkPrerequisites"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 14),  // "setActive_slot"
        QT_MOC_LITERAL(43, 9),  // "setActive"
        QT_MOC_LITERAL(53, 3),  // "set"
        QT_MOC_LITERAL(57, 10),  // "buildLines"
        QT_MOC_LITERAL(68, 17),  // "paintDiagramColor"
        QT_MOC_LITERAL(86, 5),  // "color"
        QT_MOC_LITERAL(92, 17),  // "paintDiagramLines"
        QT_MOC_LITERAL(110, 15)   // "Qt::GlobalColor"
    },
    "Diagram",
    "checkPrerequisites",
    "",
    "setActive_slot",
    "setActive",
    "set",
    "buildLines",
    "paintDiagramColor",
    "color",
    "paintDiagramLines",
    "Qt::GlobalColor"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDiagramENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   51,    2, 0x0a,    2 /* Public */,
       4,    1,   52,    2, 0x0a,    3 /* Public */,
       6,    0,   55,    2, 0x0a,    5 /* Public */,
       7,    1,   56,    2, 0x0a,    6 /* Public */,
       9,    1,   59,    2, 0x0a,    8 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, 0x80000000 | 10,    8,

       0        // eod
};

Q_CONSTINIT const QMetaObject Diagram::staticMetaObject = { {
    QMetaObject::SuperData::link<QPushButton::staticMetaObject>(),
    qt_meta_stringdata_CLASSDiagramENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDiagramENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDiagramENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Diagram, std::true_type>,
        // method 'checkPrerequisites'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setActive_slot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setActive'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'buildLines'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'paintDiagramColor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'paintDiagramLines'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::GlobalColor, std::false_type>
    >,
    nullptr
} };

void Diagram::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Diagram *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->checkPrerequisites(); break;
        case 1: _t->setActive_slot(); break;
        case 2: _t->setActive((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->buildLines(); break;
        case 4: _t->paintDiagramColor((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->paintDiagramLines((*reinterpret_cast< std::add_pointer_t<Qt::GlobalColor>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Diagram::*)();
            if (_t _q_method = &Diagram::checkPrerequisites; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *Diagram::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Diagram::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDiagramENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QPushButton::qt_metacast(_clname);
}

int Diagram::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Diagram::checkPrerequisites()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
