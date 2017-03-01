/****************************************************************************
** Meta object code from reading C++ file 'moveit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../moveit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'moveit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Moveit_t {
    QByteArrayData data[14];
    char stringdata0[366];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Moveit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Moveit_t qt_meta_stringdata_Moveit = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Moveit"
QT_MOC_LITERAL(1, 7, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 30), // "on_actionAnnotations_triggered"
QT_MOC_LITERAL(4, 63, 26), // "on_actionNormals_triggered"
QT_MOC_LITERAL(5, 90, 33), // "on_actionControl_points_trigg..."
QT_MOC_LITERAL(6, 124, 38), // "on_actionInterpolated_points_..."
QT_MOC_LITERAL(7, 163, 23), // "on_actionAxes_triggered"
QT_MOC_LITERAL(8, 187, 40), // "on_actionDrag_trajectory_poin..."
QT_MOC_LITERAL(9, 228, 26), // "on_actionDry_run_triggered"
QT_MOC_LITERAL(10, 255, 34), // "on_actionReset_all_views_trig..."
QT_MOC_LITERAL(11, 290, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(12, 314, 26), // "on_actionSave_As_triggered"
QT_MOC_LITERAL(13, 341, 24) // "on_actionClose_triggered"

    },
    "Moveit\0on_actionOpen_triggered\0\0"
    "on_actionAnnotations_triggered\0"
    "on_actionNormals_triggered\0"
    "on_actionControl_points_triggered\0"
    "on_actionInterpolated_points_triggered\0"
    "on_actionAxes_triggered\0"
    "on_actionDrag_trajectory_point_triggered\0"
    "on_actionDry_run_triggered\0"
    "on_actionReset_all_views_triggered\0"
    "on_actionExit_triggered\0"
    "on_actionSave_As_triggered\0"
    "on_actionClose_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Moveit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x0a /* Public */,
       3,    0,   75,    2, 0x0a /* Public */,
       4,    0,   76,    2, 0x0a /* Public */,
       5,    0,   77,    2, 0x0a /* Public */,
       6,    0,   78,    2, 0x0a /* Public */,
       7,    0,   79,    2, 0x0a /* Public */,
       8,    0,   80,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x0a /* Public */,
      13,    0,   85,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Moveit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Moveit *_t = static_cast<Moveit *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_actionOpen_triggered(); break;
        case 1: _t->on_actionAnnotations_triggered(); break;
        case 2: _t->on_actionNormals_triggered(); break;
        case 3: _t->on_actionControl_points_triggered(); break;
        case 4: _t->on_actionInterpolated_points_triggered(); break;
        case 5: _t->on_actionAxes_triggered(); break;
        case 6: _t->on_actionDrag_trajectory_point_triggered(); break;
        case 7: _t->on_actionDry_run_triggered(); break;
        case 8: _t->on_actionReset_all_views_triggered(); break;
        case 9: _t->on_actionExit_triggered(); break;
        case 10: _t->on_actionSave_As_triggered(); break;
        case 11: _t->on_actionClose_triggered(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Moveit::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Moveit.data,
      qt_meta_data_Moveit,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Moveit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Moveit::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Moveit.stringdata0))
        return static_cast<void*>(const_cast< Moveit*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Moveit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
