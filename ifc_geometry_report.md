# Подробный отчет по построению геометрии IFC в IfcOpenShell (C++)

## Введение

IfcOpenShell предоставляет мощный C++ API для работы с геометрией IFC моделей. Основным инструментом для построения геометрии является **геометрический итератор** (Geometry Iterator), который обеспечивает эффективную обработку геометрии с поддержкой многопоточности, кэширования и повторного использования.

## 1. Геометрический итератор (Geometry Iterator)

### 1.1 Основные принципы

Геометрический итератор - это основной механизм для обработки геометрии в IFC моделях. Он предоставляет:

- **Эффективную обработку**: Поддержка многопоточности и кэширования
- **Гибкую настройку**: Множество параметров для контроля процесса
- **Различные форматы вывода**: Триангулированная геометрия, BRep, кривые и поверхности
- **Фильтрацию элементов**: Возможность выбора конкретных элементов для обработки

### 1.2 Базовое использование

```cpp
#include <ifcopenshell/ifcopenshell.h>
#include <ifcopenshell/IfcGeom.h>

// Создание настроек
IfcGeom::IteratorSettings settings;
settings.set(IfcGeom::IteratorSettings::APPLY_DEFAULT_MATERIALS, true);

// Создание итератора
IfcGeom::Iterator geom_iterator(settings, ifc_file, filter_funcs, num_threads);

// Итерация по геометрии
for (auto& item : geom_iterator) {
    // Обработка каждого элемента геометрии
    auto shape = item.processing_result();
    // ...
}
```

## 2. Настройки итератора (Iterator Settings)

### 2.1 Настройки экземпляра итератора

#### exclude
- **Тип**: `std::vector<IfcParse::IfcEntityInstanceData*>`
- **Опция IfcConvert**: `--exclude` и `--exclude+`
- **По умолчанию**: NULL
- **Описание**: Исключает указанные геометрии из обработки. Взаимоисключающий с include.

#### include
- **Тип**: `std::vector<IfcParse::IfcEntityInstanceData*>`
- **Опция IfcConvert**: `--include` и `--include+`
- **По умолчанию**: NULL
- **Описание**: Обрабатывает только геометрию из включенных элементов.

Примеры использования в IfcConvert:
```bash
# Включение по типу сущности
IfcConvert model.ifc out.glb --include=entities IfcWall

# Включение по слою
IfcConvert model.ifc out.glb --include=layers A-WALL

# Включение по атрибуту
IfcConvert model.ifc out.glb --include=attribute GlobalId 1VQ5n5$RrEbPk8le4ZCI81
IfcConvert model.ifc out.glb --include=attribute Name Foo
IfcConvert model.ifc out.glb --include=attribute Description Bar
IfcConvert model.ifc out.glb --include=attribute Tag 123456

# Включение с дочерними элементами
IfcConvert model.ifc out.glb --include+=attribute Name "Level 1"
```

#### num_threads
- **Тип**: `int`
- **Опция IfcConvert**: `--threads` или `-j`
- **По умолчанию**: 1
- **Описание**: Количество параллельных потоков для обработки геометрии.

### 2.2 Настройки итератора

#### angle_unit
- **Тип**: `double`
- **Опция IfcConvert**: `--angle-unit`
- **По умолчанию**: 1.0
- **Описание**: Переопределяет единицу измерения угла, определенную в IFC.

```cpp
settings.set(IfcGeom::IteratorSettings::ANGLE_UNIT, 1.0);
```

#### apply-default-materials
- **Тип**: `bool`
- **Опция IfcConvert**: `--apply-default-materials`
- **По умолчанию**: true
- **Описание**: Применяет материалы по умолчанию к элементам без назначенных материалов.

```cpp
settings.set(IfcGeom::IteratorSettings::APPLY_DEFAULT_MATERIALS, true);
```

#### boolean-attempt-2d
- **Тип**: `bool`
- **Опция IfcConvert**: `--boolean-attempt-2d`
- **По умолчанию**: true
- **Описание**: Пытается выполнить булевы вычитания в 2D. Может ускорить обработку в 2-3 раза.

```cpp
settings.set(IfcGeom::IteratorSettings::BOOLEAN_ATTEMPT_2D, true);
```

#### building-local-placement
- **Тип**: `bool`
- **Опция IfcConvert**: `--building-local-placement`
- **По умолчанию**: false
- **Описание**: Не включает ObjectPlacement здания и выше в размещение элементов.

```cpp
settings.set(IfcGeom::IteratorSettings::BUILDING_LOCAL_PLACEMENT, false);
```

#### circle-segments
- **Тип**: `int`
- **Опция IfcConvert**: `--circle-segments`
- **По умолчанию**: 16
- **Описание**: Количество сегментов для аппроксимации полных окружностей в ядре CGAL.

```cpp
settings.set(IfcGeom::IteratorSettings::CIRCLE_SEGMENTS, 16);
```

#### context-identifiers
- **Тип**: `std::vector<std::string>`
- **Опция IfcConvert**: N/A
- **По умолчанию**: NULL
- **Описание**: Указывает конкретные контексты представления для обработки.

```cpp
std::vector<std::string> context_ids = {"Body", "Axis"};
settings.set_context_identifiers(context_ids);
```

#### context-ids
- **Тип**: `std::vector<int>`
- **Опция IfcConvert**: N/A
- **По умолчанию**: NULL
- **Описание**: Указывает ID контекстов представления для обработки.

```cpp
std::vector<int> context_ids = {1, 2, 3};
settings.set_context_ids(context_ids);
```

#### context-types
- **Тип**: `std::vector<std::string>`
- **Опция IfcConvert**: N/A
- **По умолчанию**: NULL
- **Описание**: Указывает типы контекстов представления для обработки.

```cpp
std::vector<std::string> context_types = {"Plan"};
settings.set_context_types(context_types);
```

#### convert-back-units
- **Тип**: `bool`
- **Опция IfcConvert**: `--convert-back-units`
- **По умолчанию**: false
- **Описание**: Восстанавливает координаты после конвертации, умножая на коэффициент единицы измерения.

```cpp
settings.set(IfcGeom::IteratorSettings::CONVERT_BACK_UNITS, false);
```

#### debug
- **Тип**: `bool`
- **Опция IfcConvert**: `--debug`
- **По умолчанию**: false
- **Описание**: Записывает булевы операнды в файл для отладки.

```cpp
settings.set(IfcGeom::IteratorSettings::DEBUG, false);
```

#### dimensionality
- **Тип**: `int`
- **Опция IfcConvert**: `--dimensionality`
- **По умолчанию**: 1
- **Описание**: Контролирует типы геометрии для обработки.

```cpp
settings.set(IfcGeom::IteratorSettings::DIMENSIONALITY, IfcGeom::IteratorSettings::SURFACES_AND_SOLIDS);
// Доступные значения:
// IfcGeom::IteratorSettings::CURVES = 0
// IfcGeom::IteratorSettings::SURFACES_AND_SOLIDS = 1 (default)
// IfcGeom::IteratorSettings::CURVES_SURFACES_AND_SOLIDS = 2
```

#### disable-boolean-result
- **Тип**: `bool`
- **Опция IfcConvert**: `--disable-boolean-result`
- **По умолчанию**: false
- **Описание**: Отключает вычисление IfcBooleanResult и возвращает только FirstOperand.

```cpp
settings.set(IfcGeom::IteratorSettings::DISABLE_BOOLEAN_RESULT, false);
```

#### disable-opening-subtractions
- **Тип**: `bool`
- **Опция IfcConvert**: `--disable-opening-subtractions`
- **По умолчанию**: false
- **Описание**: Отключает вычитание геометрии проемов из хост-элементов.

```cpp
settings.set(IfcGeom::IteratorSettings::DISABLE_OPENING_SUBTRACTIONS, false);
```

#### edge-arrows
- **Тип**: `bool`
- **Опция IfcConvert**: `--edge-arrows`
- **По умолчанию**: false
- **Описание**: Добавляет стрелки к ребрам для указания направления кривых.

```cpp
settings.set(IfcGeom::IteratorSettings::EDGE_ARROWS, false);
```

#### element-hierarchy
- **Тип**: `bool`
- **Опция IfcConvert**: `--element-hierarchy`
- **По умолчанию**: false
- **Описание**: Выводит относительные размещения вместо абсолютных (только для Collada .DAE).

```cpp
settings.set(IfcGeom::IteratorSettings::ELEMENT_HIERARCHY, false);
```

#### enable-layerset-slicing
- **Тип**: `bool`
- **Опция IfcConvert**: `--enable-layerset-slicing`
- **По умолчанию**: false
- **Описание**: Создает поверхности для сегментации геометрии на основе IfcMaterialLayerSet.

```cpp
settings.set(IfcGeom::IteratorSettings::ENABLE_LAYERSET_SLICING, false);
```

#### force-space-transparency
- **Тип**: `double`
- **Опция IfcConvert**: `--force-space-transparency`
- **По умолчанию**: 0.0
- **Описание**: Переопределяет прозрачность пространств в геометрическом выводе.

```cpp
settings.set(IfcGeom::IteratorSettings::FORCE_SPACE_TRANSPARENCY, 0.0);
```

#### function-step-param
- **Тип**: `double`
- **Опция IfcConvert**: `--function-step-param`
- **По умолчанию**: 0.5
- **Описание**: Параметр для определения размера шага при вычислении кривых на основе функций.

```cpp
settings.set(IfcGeom::IteratorSettings::FUNCTION_STEP_PARAM, 0.5);
```

#### function-step-type
- **Тип**: `int`
- **Опция IfcConvert**: `--function-step-type`
- **По умолчанию**: 0
- **Описание**: Метод определения размера шага для кривых на основе функций.

```cpp
settings.set(IfcGeom::IteratorSettings::FUNCTION_STEP_TYPE, IfcGeom::IteratorSettings::MAXSTEPSIZE);
// Доступные значения:
// IfcGeom::IteratorSettings::MAXSTEPSIZE = 0
// IfcGeom::IteratorSettings::MINSTEPS = 1
```

#### generate-uvs
- **Тип**: `bool`
- **Опция IfcConvert**: `--generate-uvs`
- **По умолчанию**: false
- **Описание**: Применяет проекцию коробки для получения UV координат.

```cpp
settings.set(IfcGeom::IteratorSettings::GENERATE_UVS, false);
```

#### iterator-output
- **Описание**: Контролирует тип вывода итератора.

```cpp
settings.set(IfcGeom::IteratorSettings::ITERATOR_OUTPUT, IfcGeom::IteratorSettings::NATIVE);
// Доступные значения:
// IfcGeom::IteratorSettings::NATIVE - нативная OCC репрезентация
// IfcGeom::IteratorSettings::SERIALIZED - сериализованная OCC репрезентация
```

#### keep-bounding-boxes
- **Тип**: `bool`
- **Опция IfcConvert**: `--keep-bounding-boxes`
- **По умолчанию**: false
- **Описание**: Сохраняет IfcBoundingBox в модели перед конвертацией геометрии.

```cpp
settings.set(IfcGeom::IteratorSettings::KEEP_BOUNDING_BOXES, false);
```

#### layerset-first
- **Тип**: `bool`
- **Опция IfcConvert**: `--layerset-first`
- **По умолчанию**: false
- **Описание**: Использует первый слой материала из набора как материал для всего элемента.

```cpp
settings.set(IfcGeom::IteratorSettings::LAYERSET_FIRST, false);
```

#### length-unit
- **Тип**: `double`
- **Опция IfcConvert**: `--length-unit`
- **По умолчанию**: 1.0
- **Описание**: Переопределяет единицу длины, определенную в IFC, как множитель метров.

```cpp
settings.set(IfcGeom::IteratorSettings::LENGTH_UNIT, 1.0);
```

#### mesher-angular-deflection
- **Тип**: `double`
- **Опция IfcConvert**: `--mesher-angular-deflection`
- **По умолчанию**: 0.5
- **Описание**: Устанавливает угловую толерантность мешера в радианах.

```cpp
settings.set_angular_tolerance(0.5);
```

#### mesher-linear-deflection
- **Тип**: `double`
- **Опция IfcConvert**: `--mesher-linear-deflection`
- **По умолчанию**: 1e-3
- **Описание**: Устанавливает толерантность отклонения мешера.

```cpp
settings.set_deflection_tolerance(1e-3);
```

#### model-offset
- **Тип**: `std::array<double, 3>`
- **Опция IfcConvert**: `--model-offset`
- **По умолчанию**: {0.0, 0.0, 0.0}
- **Описание**: Устанавливает смещение для всех матриц геометрии.

```cpp
std::array<double, 3> offset = {1.0, 2.0, 3.0};
settings.set(IfcGeom::IteratorSettings::MODEL_OFFSET, offset);
```

#### model-rotation
- **Тип**: `std::array<double, 4>`
- **Опция IfcConvert**: `--model-rotation`
- **По умолчанию**: {0.0, 0.0, 0.0, 0.0}
- **Описание**: Применяет произвольное кватернионное вращение формы 'x,y,z,w' ко всем размещениям.

```cpp
std::array<double, 4> rotation = {0.0, 0.0, 0.0, 1.0};
settings.set(IfcGeom::IteratorSettings::MODEL_ROTATION, rotation);
```

#### no-normals
- **Тип**: `bool`
- **Опция IfcConvert**: `--no-normals`
- **По умолчанию**: false
- **Описание**: Не выводит нормали в геометрическом выводе. Экономит время и размер файла.

```cpp
settings.set(IfcGeom::IteratorSettings::NO_NORMALS, false);
```

#### no-parallel-mapping
- **Тип**: `bool`
- **Опция IfcConvert**: `--no-parallel-mapping`
- **По умолчанию**: false
- **Описание**: Выполняет маппинг заранее (однопоточный) вместо параллельного.

```cpp
settings.set(IfcGeom::IteratorSettings::NO_PARALLEL_MAPPING, false);
```

#### no-wire-intersection-check
- **Тип**: `bool`
- **Опция IfcConvert**: `--no-wire-intersection-check`
- **По умолчанию**: false
- **Описание**: Отключает проверки пересечения проволок.

```cpp
settings.set(IfcGeom::IteratorSettings::NO_WIRE_INTERSECTION_CHECK, false);
```

#### no-wire-intersection-tolerance
- **Тип**: `bool`
- **Опция IfcConvert**: `--no-wire-intersection-tolerance`
- **По умолчанию**: false
- **Описание**: Устанавливает толерантность пересечения проволок в 0.

```cpp
settings.set(IfcGeom::IteratorSettings::NO_WIRE_INTERSECTION_TOLERANCE, false);
```

#### precision
- **Тип**: `double`
- **Опция IfcConvert**: `--precision`
- **По умолчанию**: 0.0
- **Описание**: Устанавливает пользовательскую точность вместо точности IFC модели.

```cpp
settings.set(IfcGeom::IteratorSettings::PRECISION, 1e-6);
```

#### precision-factor
- **Тип**: `double`
- **Опция IfcConvert**: `--precision-factor`
- **По умолчанию**: 0.0
- **Описание**: Увеличивает линейную толерантность для более разрешительных кривых.

```cpp
settings.set(IfcGeom::IteratorSettings::PRECISION_FACTOR, 10.0);
```

#### reorient-shells
- **Тип**: `bool`
- **Опция IfcConvert**: `--reorient-shells`
- **По умолчанию**: false
- **Описание**: Переориентирует или сшивает связанные наборы граней для согласованной внешней ориентации.

```cpp
settings.set(IfcGeom::IteratorSettings::REORIENT_SHELLS, false);
```

#### site-local-placement
- **Тип**: `bool`
- **Опция IfcConvert**: `--site-local-placement`
- **По умолчанию**: false
- **Описание**: Исключает ObjectPlacement участка из размещения элементов.

```cpp
settings.set(IfcGeom::IteratorSettings::SITE_LOCAL_PLACEMENT, false);
```

#### surface-colour
- **Тип**: `bool`
- **Опция IfcConvert**: `--surface-colour`
- **По умолчанию**: false
- **Описание**: Приоритизирует цвет поверхности вместо диффузного.

```cpp
settings.set(IfcGeom::IteratorSettings::SURFACE_COLOUR, false);
```

#### triangulation-type
- **Тип**: `int`
- **Опция IfcConvert**: `--triangulation-type`
- **По умолчанию**: 0
- **Описание**: Тип плоской грани для вывода.

```cpp
settings.set(IfcGeom::IteratorSettings::TRIANGULATION_TYPE, IfcGeom::IteratorSettings::TRIANGLE_MESH);
// Доступные значения:
// IfcGeom::IteratorSettings::TRIANGLE_MESH = 0
// IfcGeom::IteratorSettings::POLYHEDRON_WITHOUT_HOLES = 1
// IfcGeom::IteratorSettings::POLYHEDRON_WITH_HOLES = 2
```

#### unify-shapes
- **Тип**: `bool`
- **Опция IfcConvert**: `--unify-shapes`
- **По умолчанию**: false
- **Описание**: Объединяет смежные копланарные и коллинеарные подформы перед триангуляцией.

```cpp
settings.set(IfcGeom::IteratorSettings::UNIFY_SHAPES, false);
```

#### use-material-names
- **Тип**: `bool`
- **Опция IfcConvert**: `--use-material-names`
- **По умолчанию**: false
- **Описание**: Использует имена материалов вместо уникальных ID для именования материалов.

```cpp
settings.set(IfcGeom::IteratorSettings::USE_MATERIAL_NAMES, false);
```

#### use-python-opencascade
- **Тип**: `bool`
- **Опция IfcConvert**: N/A
- **По умолчанию**: false
- **Описание**: Использует Python OpenCASCADE для десериализации TopoDS_Shape.

```cpp
settings.set(IfcGeom::IteratorSettings::USE_PYTHON_OPENCASCADE, false);
```

#### use-world-coords
- **Тип**: `bool`
- **Опция IfcConvert**: `--use-world-coords`
- **По умолчанию**: false
- **Описание**: Применяет ObjectPlacement строительных элементов к геометрическому выводу.

```cpp
settings.set(IfcGeom::IteratorSettings::USE_WORLD_COORDS, false);
```

#### validate
- **Тип**: `bool`
- **Опция IfcConvert**: `--validate`
- **По умолчанию**: false
- **Описание**: Устанавливает ненулевой код выхода при ошибках валидации.

```cpp
settings.set(IfcGeom::IteratorSettings::VALIDATE, false);
```

#### weld-vertices
- **Тип**: `bool`
- **Опция IfcConvert**: `--weld-vertices`
- **По умолчанию**: true в C++
- **Описание**: Объединяет вершины только на основе позиции, отбрасывая нормали.

```cpp
settings.set(IfcGeom::IteratorSettings::WELD_VERTICES, true);
```

## 3. Правильное использование итератора

### 3.1 Основные принципы

1. **Создание настроек**: Всегда настраивайте параметры в соответствии с требованиями
2. **Выбор элементов**: Используйте фильтры include/exclude для оптимизации
3. **Многопоточность**: Используйте num_threads для ускорения обработки
4. **Кэширование**: Итератор автоматически кэширует результаты для повторного использования

### 3.2 Примеры использования

#### Обработка только стен:
```cpp
#include <ifcopenshell/ifcopenshell.h>
#include <ifcopenshell/IfcGeom.h>
#include <vector>

int main() {
    // Открытие файла
    IfcParse::IfcFile ifc_file("model.ifc");
    
    // Создание настроек
    IfcGeom::IteratorSettings settings;
    settings.set(IfcGeom::IteratorSettings::APPLY_DEFAULT_MATERIALS, true);
    settings.set(IfcGeom::IteratorSettings::DIMENSIONALITY, 
                 IfcGeom::IteratorSettings::SURFACES_AND_SOLIDS);
    
    // Получение всех стен
    auto walls = ifc_file.instances_by_type("IfcWall");
    std::vector<IfcParse::IfcEntityInstanceData*> wall_entities;
    for (auto wall : walls) {
        wall_entities.push_back(wall);
    }
    
    // Создание итератора только для стен
    IfcGeom::Iterator iterator(settings, ifc_file, wall_entities, 4);
    
    // Обработка геометрии
    for (auto& item : iterator) {
        auto shape = item.processing_result();
        std::cout << "Обработан элемент: " << item.guid() << std::endl;
        std::cout << "Количество вершин: " << shape.geometry().verts().size() << std::endl;
        std::cout << "Количество граней: " << shape.geometry().faces().size() << std::endl;
    }
    
    return 0;
}
```

#### Обработка с пользовательскими настройками:
```cpp
#include <ifcopenshell/ifcopenshell.h>
#include <ifcopenshell/IfcGeom.h>

int main() {
    IfcParse::IfcFile ifc_file("model.ifc");
    
    // Создание настроек
    IfcGeom::IteratorSettings settings;
    
    // Настройка точности
    settings.set(IfcGeom::IteratorSettings::PRECISION, 1e-6);
    settings.set(IfcGeom::IteratorSettings::PRECISION_FACTOR, 10.0);
    
    // Настройка мешера
    settings.set_deflection_tolerance(1e-3);
    settings.set_angular_tolerance(0.5);
    
    // Настройка материалов
    settings.set(IfcGeom::IteratorSettings::APPLY_DEFAULT_MATERIALS, true);
    settings.set(IfcGeom::IteratorSettings::USE_MATERIAL_NAMES, true);
    
    // Настройка геометрии
    settings.set(IfcGeom::IteratorSettings::CIRCLE_SEGMENTS, 32);
    settings.set(IfcGeom::IteratorSettings::BOOLEAN_ATTEMPT_2D, true);
    
    // Создание итератора
    IfcGeom::Iterator iterator(settings, ifc_file, {}, 8);
    
    // Обработка
    for (auto& item : iterator) {
        // Обработка каждого элемента
        auto shape = item.processing_result();
        // ...
    }
    
    return 0;
}
```

### 3.3 Оптимизация производительности

1. **Используйте фильтры**: Обрабатывайте только нужные элементы
2. **Настройте точность**: Увеличьте толерантность для ускорения
3. **Используйте многопоточность**: Установите num_threads в соответствии с CPU
4. **Отключите ненужные функции**: Например, no-normals для экономии времени
5. **Используйте 2D булевы операции**: boolean-attempt-2d для ускорения

### 3.4 Обработка ошибок

```cpp
#include <ifcopenshell/ifcopenshell.h>
#include <ifcopenshell/IfcGeom.h>
#include <iostream>
#include <exception>

int main() {
    try {
        IfcParse::IfcFile ifc_file("model.ifc");
        IfcGeom::IteratorSettings settings;
        IfcGeom::Iterator iterator(settings, ifc_file);
        
        for (auto& item : iterator) {
            try {
                auto shape = item.processing_result();
                // Обработка успешного результата
            } catch (const std::exception& e) {
                std::cerr << "Ошибка обработки элемента " << item.guid() 
                          << ": " << e.what() << std::endl;
                continue;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка создания итератора: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## 4. Структуры данных и классы

### 4.1 IfcGeom::IteratorSettings

Основной класс для настройки параметров итератора:

```cpp
class IfcGeom::IteratorSettings {
public:
    // Методы настройки
    void set(IteratorSettings::Setting setting, bool value);
    void set(IteratorSettings::Setting setting, int value);
    void set(IteratorSettings::Setting setting, double value);
    void set(IteratorSettings::Setting setting, const std::string& value);
    void set(IteratorSettings::Setting setting, const std::vector<std::string>& value);
    void set(IteratorSettings::Setting setting, const std::vector<int>& value);
    void set(IteratorSettings::Setting setting, const std::array<double, 3>& value);
    void set(IteratorSettings::Setting setting, const std::array<double, 4>& value);
    
    // Специальные методы
    void set_deflection_tolerance(double tolerance);
    void set_angular_tolerance(double tolerance);
    void set_context_identifiers(const std::vector<std::string>& identifiers);
    void set_context_ids(const std::vector<int>& ids);
    void set_context_types(const std::vector<std::string>& types);
    
    // Константы настроек
    enum Setting {
        APPLY_DEFAULT_MATERIALS,
        BOOLEAN_ATTEMPT_2D,
        BUILDING_LOCAL_PLACEMENT,
        CIRCLE_SEGMENTS,
        CONVERT_BACK_UNITS,
        DEBUG,
        DIMENSIONALITY,
        DISABLE_BOOLEAN_RESULT,
        DISABLE_OPENING_SUBTRACTIONS,
        EDGE_ARROWS,
        ELEMENT_HIERARCHY,
        ENABLE_LAYERSET_SLICING,
        FORCE_SPACE_TRANSPARENCY,
        FUNCTION_STEP_PARAM,
        FUNCTION_STEP_TYPE,
        GENERATE_UVS,
        ITERATOR_OUTPUT,
        KEEP_BOUNDING_BOXES,
        LAYERSET_FIRST,
        LENGTH_UNIT,
        MODEL_OFFSET,
        MODEL_ROTATION,
        NO_NORMALS,
        NO_PARALLEL_MAPPING,
        NO_WIRE_INTERSECTION_CHECK,
        NO_WIRE_INTERSECTION_TOLERANCE,
        PRECISION,
        PRECISION_FACTOR,
        REORIENT_SHELLS,
        SITE_LOCAL_PLACEMENT,
        SURFACE_COLOUR,
        TRIANGULATION_TYPE,
        UNIFY_SHAPES,
        USE_MATERIAL_NAMES,
        USE_PYTHON_OPENCASCADE,
        USE_WORLD_COORDS,
        VALIDATE,
        WELD_VERTICES
    };
    
    // Константы для значений
    enum Dimensionality {
        CURVES = 0,
        SURFACES_AND_SOLIDS = 1,
        CURVES_SURFACES_AND_SOLIDS = 2
    };
    
    enum TriangulationType {
        TRIANGLE_MESH = 0,
        POLYHEDRON_WITHOUT_HOLES = 1,
        POLYHEDRON_WITH_HOLES = 2
    };
    
    enum IteratorOutput {
        NATIVE = 0,
        SERIALIZED = 1
    };
    
    enum FunctionStepType {
        MAXSTEPSIZE = 0,
        MINSTEPS = 1
    };
};
```

### 4.2 IfcGeom::Iterator

Основной класс итератора:

```cpp
class IfcGeom::Iterator {
public:
    // Конструкторы
    Iterator(const IteratorSettings& settings, 
             const IfcParse::IfcFile& file,
             const std::vector<IfcParse::IfcEntityInstanceData*>& include = {},
             int num_threads = 1);
    
    // Итерация
    iterator begin();
    iterator end();
    
    // Информация
    size_t size() const;
    bool empty() const;
};
```

### 4.3 IfcGeom::Element

Класс элемента геометрии:

```cpp
class IfcGeom::Element {
public:
    // Основная информация
    std::string guid() const;
    std::string name() const;
    std::string type() const;
    
    // Геометрия
    IfcGeom::ProcessingResult processing_result() const;
    
    // Материалы
    std::vector<IfcGeom::Material> materials() const;
    
    // Трансформация
    gp_Trsf transformation() const;
};
```

### 4.4 IfcGeom::ProcessingResult

Результат обработки геометрии:

```cpp
class IfcGeom::ProcessingResult {
public:
    // Геометрия
    const IfcGeom::Geometry& geometry() const;
    
    // Материалы
    const std::vector<IfcGeom::Material>& materials() const;
    
    // Трансформация
    const gp_Trsf& transformation() const;
    
    // Статус
    bool success() const;
    std::string error_message() const;
};
```

## 5. Заключение

Геометрический итератор IfcOpenShell предоставляет мощный и гибкий инструмент для работы с геометрией IFC моделей в C++. Правильная настройка параметров и использование фильтров позволяет оптимизировать производительность и получить желаемый результат. Основные принципы:

1. **Всегда настраивайте параметры** в соответствии с требованиями
2. **Используйте фильтры** для оптимизации обработки
3. **Применяйте многопоточность** для ускорения
4. **Обрабатывайте ошибки** для надежности
5. **Тестируйте настройки** на небольших моделях перед обработкой больших файлов

Данный отчет охватывает основные аспекты работы с геометрией IFC в IfcOpenShell C++ API и может служить руководством для разработчиков, работающих с данной библиотекой.