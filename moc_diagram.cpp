/****************************************************************************
** Meta object code from reading C++ file 'diagram.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "diagram.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'diagram.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Diagram_t {
    QByteArrayData data[11];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Diagram_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Diagram_t qt_meta_stringdata_Diagram = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Diagram"
QT_MOC_LITERAL(1, 8, 18), // "checkPrerequisites"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 14), // "setActive_slot"
QT_MOC_LITERAL(4, 43, 9), // "setActive"
QT_MOC_LITERAL(5, 53, 3), // "set"
QT_MOC_LITERAL(6, 57, 10), // "buildLines"
QT_MOC_LITERAL(7, 68, 17), // "paintDiagramColor"
QT_MOC_LITERAL(8, 86, 5), // "color"
QT_MOC_LITERAL(9, 92, 17), // "paintDiagramLines"
QT_MOC_LITERAL(10, 110, 15) // "Qt::GlobalColor"

    },
    "Diagram\0checkPrerequisites\0\0setActive_slot\0"
    "setActive\0set\0buildLines\0paintDiagramColor\0"
    "color\0paintDiagramLines\0Qt::GlobalColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Diagram[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   45,    2, 0x0a /* Public */,
       4,    1,   46,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    1,   50,    2, 0x0a /* Public */,
       9,    1,   53,    2, 0x0a /* Public */,

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

void Diagram::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Diagram *_t = static_cast<Diagram *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->checkPrerequisites(); break;
        case 1: _t->setActive_slot(); break;
        case 2: _t->setActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->buildLines(); break;
        case 4: _t->paintDiagramColor((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->paintDiagramLines((*reinterpret_cast< Qt::GlobalColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Diagram::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Diagram::checkPrerequisites)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Diagram::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_Diagram.data,
      qt_meta_data_Diagram,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Diagram::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Diagram::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Diagram.stringdata0))
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
            *reinterpret_cast<int*>(_a[0]) = -1;
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
QT_END_MOC_NAMESPACE
