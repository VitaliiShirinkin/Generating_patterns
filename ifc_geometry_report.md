# Подробный отчет по построению геометрии IFC в IfcOpenShell

## Введение

IfcOpenShell предоставляет мощный API для работы с геометрией IFC моделей. Основным инструментом для построения геометрии является **геометрический итератор** (Geometry Iterator), который обеспечивает эффективную обработку геометрии с поддержкой многопоточности, кэширования и повторного использования.

## 1. Геометрический итератор (Geometry Iterator)

### 1.1 Основные принципы

Геометрический итератор - это основной механизм для обработки геометрии в IFC моделях. Он предоставляет:

- **Эффективную обработку**: Поддержка многопоточности и кэширования
- **Гибкую настройку**: Множество параметров для контроля процесса
- **Различные форматы вывода**: Триангулированная геометрия, BRep, кривые и поверхности
- **Фильтрацию элементов**: Возможность выбора конкретных элементов для обработки

### 1.2 Базовое использование

#### В C++:
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

#### В Python:
```python
import ifcopenshell
import ifcopenshell.geom

# Создание настроек
settings = ifcopenshell.geom.settings()
settings.set("apply-default-materials", True)

# Создание итератора
iterator = ifcopenshell.geom.iterator(settings, ifc_file, num_threads=4)

# Итерация по геометрии
for item in iterator:
    # Обработка каждого элемента геометрии
    shape = item.processing_result()
    # ...
```

## 2. Настройки итератора (Iterator Settings)

### 2.1 Настройки экземпляра итератора

#### exclude
- **Тип**: LIST OF OBJ
- **Опция IfcConvert**: `--exclude` и `--exclude+`
- **По умолчанию**: NULL
- **Описание**: Исключает указанные геометрии из обработки. Взаимоисключающий с include.

#### include
- **Тип**: LIST OF OBJ
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
- **Тип**: INT
- **Опция IfcConvert**: `--threads` или `-j`
- **По умолчанию**: 1
- **Описание**: Количество параллельных потоков для обработки геометрии.

### 2.2 Настройки итератора

#### angle_unit
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--angle-unit`
- **По умолчанию**: 1
- **Описание**: Переопределяет единицу измерения угла, определенную в IFC.

#### apply-default-materials
- **Тип**: BOOL
- **Опция IfcConvert**: `--apply-default-materials`
- **По умолчанию**: True
- **Описание**: Применяет материалы по умолчанию к элементам без назначенных материалов.

#### boolean-attempt-2d
- **Тип**: BOOL
- **Опция IfcConvert**: `--boolean-attempt-2d`
- **По умолчанию**: True
- **Описание**: Пытается выполнить булевы вычитания в 2D. Может ускорить обработку в 2-3 раза.

#### building-local-placement
- **Тип**: BOOL
- **Опция IfcConvert**: `--building-local-placement`
- **По умолчанию**: False
- **Описание**: Не включает ObjectPlacement здания и выше в размещение элементов.

#### circle-segments
- **Тип**: INT
- **Опция IfcConvert**: `--circle-segments`
- **По умолчанию**: 16
- **Описание**: Количество сегментов для аппроксимации полных окружностей в ядре CGAL.

#### context-identifiers
- **Тип**: LIST OF STRING
- **Опция IfcConvert**: N/A
- **По умолчанию**: NULL
- **Описание**: Указывает конкретные контексты представления для обработки.

```python
settings = ifcopenshell.geom.settings()
settings.set("context-identifiers", ["Body", "Axis"])
```

#### context-ids
- **Тип**: LIST OF INT
- **Опция IfcConvert**: N/A
- **По умолчанию**: NULL
- **Описание**: Указывает ID контекстов представления для обработки.

#### context-types
- **Тип**: LIST OF STRING
- **Опция IfcConvert**: N/A
- **По умолчанию**: NULL
- **Описание**: Указывает типы контекстов представления для обработки.

```python
settings = ifcopenshell.geom.settings()
settings.set("context-types", ["Plan"])
```

#### convert-back-units
- **Тип**: BOOL
- **Опция IfcConvert**: `--convert-back-units`
- **По умолчанию**: False
- **Описание**: Восстанавливает координаты после конвертации, умножая на коэффициент единицы измерения.

#### debug
- **Тип**: BOOL
- **Опция IfcConvert**: `--debug`
- **По умолчанию**: False
- **Описание**: Записывает булевы операнды в файл для отладки.

#### dimensionality
- **Тип**: BOOL
- **Опция IfcConvert**: `--dimensionality`
- **По умолчанию**: 1
- **Описание**: Контролирует типы геометрии для обработки.

```python
settings = ifcopenshell.geom.settings()
settings.set("dimensionality", ifcopenshell.ifcopenshell_wrapper.CURVES)  # 0
settings.set("dimensionality", ifcopenshell.ifcopenshell_wrapper.SURFACES_AND_SOLIDS)  # 1, default
settings.set("dimensionality", ifcopenshell.ifcopenshell_wrapper.CURVES_SURFACES_AND_SOLIDS)  # 2
```

#### disable-boolean-result
- **Тип**: BOOL
- **Опция IfcConvert**: `--disable-boolean-result`
- **По умолчанию**: False
- **Описание**: Отключает вычисление IfcBooleanResult и возвращает только FirstOperand.

#### disable-opening-subtractions
- **Тип**: BOOL
- **Опция IfcConvert**: `--disable-opening-subtractions`
- **По умолчанию**: False
- **Описание**: Отключает вычитание геометрии проемов из хост-элементов.

#### edge-arrows
- **Тип**: BOOL
- **Опция IfcConvert**: `--edge-arrows`
- **По умолчанию**: False
- **Описание**: Добавляет стрелки к ребрам для указания направления кривых.

#### element-hierarchy
- **Тип**: BOOL
- **Опция IfcConvert**: `--element-hierarchy`
- **По умолчанию**: False
- **Описание**: Выводит относительные размещения вместо абсолютных (только для Collada .DAE).

#### enable-layerset-slicing
- **Тип**: BOOL
- **Опция IfcConvert**: `--enable-layerset-slicing`
- **По умолчанию**: False
- **Описание**: Создает поверхности для сегментации геометрии на основе IfcMaterialLayerSet.

#### force-space-transparency
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--force-space-transparency`
- **По умолчанию**: 0
- **Описание**: Переопределяет прозрачность пространств в геометрическом выводе.

#### function-step-param
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--function-step-param`
- **По умолчанию**: 0.5
- **Описание**: Параметр для определения размера шага при вычислении кривых на основе функций.

#### function-step-type
- **Тип**: INT
- **Опция IfcConvert**: `--function-step-type`
- **По умолчанию**: 0
- **Описание**: Метод определения размера шага для кривых на основе функций.

```python
settings = ifcopenshell.geom.settings()
settings.set("function-step-type", ifcopenshell.ifcopenshell_wrapper.MAXSTEPSIZE)  # 0
settings.set("function-step-type", ifcopenshell.ifcopenshell_wrapper.MINSTEPS)  # 1
```

#### generate-uvs
- **Тип**: BOOL
- **Опция IfcConvert**: `--generate-uvs`
- **По умолчанию**: False
- **Описание**: Применяет проекцию коробки для получения UV координат.

#### iterator-output
- **Описание**: Контролирует тип вывода итератора.

```python
settings = ifcopenshell.geom.settings()
# Нативная OCC репрезентация
settings.set("iterator-output", ifcopenshell.ifcopenshell_wrapper.NATIVE)
# Сериализованная OCC репрезентация
settings.set("iterator-output", ifcopenshell.ifcopenshell_wrapper.SERIALIZED)
```

#### keep-bounding-boxes
- **Тип**: BOOL
- **Опция IfcConvert**: `--keep-bounding-boxes`
- **По умолчанию**: False
- **Описание**: Сохраняет IfcBoundingBox в модели перед конвертацией геометрии.

#### layerset-first
- **Тип**: BOOL
- **Опция IfcConvert**: `--layerset-first`
- **По умолчанию**: False
- **Описание**: Использует первый слой материала из набора как материал для всего элемента.

#### length-unit
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--length-unit`
- **По умолчанию**: 1
- **Описание**: Переопределяет единицу длины, определенную в IFC, как множитель метров.

#### mesher-angular-deflection
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--mesher-angular-deflection`
- **По умолчанию**: 0.5
- **Описание**: Устанавливает угловую толерантность мешера в радианах.

#### mesher-linear-deflection
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--mesher-linear-deflection`
- **По умолчанию**: 1e-3
- **Описание**: Устанавливает толерантность отклонения мешера.

#### model-offset
- **Тип**: ARRAY<DOUBLE>
- **Опция IfcConvert**: `--model-offset`
- **По умолчанию**: 0,0,0
- **Описание**: Устанавливает смещение для всех матриц геометрии.

```python
settings = ifcopenshell.geom.settings()
settings.set("model-offset", (1.0, 2.0, 3.0))
```

#### model-rotation
- **Тип**: ARRAY<DOUBLE>
- **Опция IfcConvert**: `--model-rotation`
- **По умолчанию**: 0,0,0,0
- **Описание**: Применяет произвольное кватернионное вращение формы 'x,y,z,w' ко всем размещениям.

#### no-normals
- **Тип**: BOOL
- **Опция IfcConvert**: `--no-normals`
- **По умолчанию**: False
- **Описание**: Не выводит нормали в геометрическом выводе. Экономит время и размер файла.

#### no-parallel-mapping
- **Тип**: BOOL
- **Опция IfcConvert**: `--no-parallel-mapping`
- **По умолчанию**: False
- **Описание**: Выполняет маппинг заранее (однопоточный) вместо параллельного.

#### no-wire-intersection-check
- **Тип**: BOOL
- **Опция IfcConvert**: `--no-wire-intersection-check`
- **По умолчанию**: False
- **Описание**: Отключает проверки пересечения проволок.

#### no-wire-intersection-tolerance
- **Тип**: BOOL
- **Опция IfcConvert**: `--no-wire-intersection-tolerance`
- **По умолчанию**: False
- **Описание**: Устанавливает толерантность пересечения проволок в 0.

#### precision
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--precision`
- **По умолчанию**: 0
- **Описание**: Устанавливает пользовательскую точность вместо точности IFC модели.

#### precision-factor
- **Тип**: DOUBLE
- **Опция IfcConvert**: `--precision-factor`
- **По умолчанию**: 0
- **Описание**: Увеличивает линейную толерантность для более разрешительных кривых.

#### reorient-shells
- **Тип**: BOOL
- **Опция IfcConvert**: `--reorient-shells`
- **По умолчанию**: False
- **Описание**: Переориентирует или сшивает связанные наборы граней для согласованной внешней ориентации.

#### site-local-placement
- **Тип**: BOOL
- **Опция IfcConvert**: `--site-local-placement`
- **По умолчанию**: False
- **Описание**: Исключает ObjectPlacement участка из размещения элементов.

#### surface-colour
- **Тип**: BOOL
- **Опция IfcConvert**: `--surface-colour`
- **По умолчанию**: False
- **Описание**: Приоритизирует цвет поверхности вместо диффузного.

#### triangulation-type
- **Тип**: INT
- **Опция IfcConvert**: `--triangulation-type`
- **По умолчанию**: 0
- **Описание**: Тип плоской грани для вывода.

```python
settings = ifcopenshell.geom.settings()
settings.set("triangulation-type", ifcopenshell.ifcopenshell_wrapper.TRIANGLE_MESH)  # 0
settings.set("triangulation-type", ifcopenshell.ifcopenshell_wrapper.POLYHEDRON_WITHOUT_HOLES)  # 1
settings.set("triangulation-type", ifcopenshell.ifcopenshell_wrapper.POLYHEDRON_WITH_HOLES)  # 2
```

#### unify-shapes
- **Тип**: BOOL
- **Опция IfcConvert**: `--unify-shapes`
- **По умолчанию**: False
- **Описание**: Объединяет смежные копланарные и коллинеарные подформы перед триангуляцией.

#### use-material-names
- **Тип**: BOOL
- **Опция IfcConvert**: `--use-material-names`
- **По умолчанию**: False
- **Описание**: Использует имена материалов вместо уникальных ID для именования материалов.

#### use-python-opencascade
- **Тип**: BOOL
- **Опция IfcConvert**: N/A
- **По умолчанию**: False
- **Описание**: Использует Python OpenCASCADE для десериализации TopoDS_Shape.

#### use-world-coords
- **Тип**: BOOL
- **Опция IfcConvert**: `--use-world-coords`
- **По умолчанию**: False
- **Описание**: Применяет ObjectPlacement строительных элементов к геометрическому выводу.

#### validate
- **Тип**: BOOL
- **Опция IfcConvert**: `--validate`
- **По умолчанию**: False
- **Описание**: Устанавливает ненулевой код выхода при ошибках валидации.

#### weld-vertices
- **Тип**: BOOL
- **Опция IfcConvert**: `--weld-vertices`
- **По умолчанию**: False в IfcConvert, True в C++ и Python
- **Описание**: Объединяет вершины только на основе позиции, отбрасывая нормали.

## 3. Правильное использование итератора

### 3.1 Основные принципы

1. **Создание настроек**: Всегда настраивайте параметры в соответствии с требованиями
2. **Выбор элементов**: Используйте фильтры include/exclude для оптимизации
3. **Многопоточность**: Используйте num_threads для ускорения обработки
4. **Кэширование**: Итератор автоматически кэширует результаты для повторного использования

### 3.2 Примеры использования

#### Обработка только стен:
```python
import ifcopenshell
import ifcopenshell.geom

# Открытие файла
ifc_file = ifcopenshell.open("model.ifc")

# Создание настроек
settings = ifcopenshell.geom.settings()
settings.set("apply-default-materials", True)
settings.set("dimensionality", ifcopenshell.ifcopenshell_wrapper.SURFACES_AND_SOLIDS)

# Получение всех стен
walls = ifc_file.by_type("IfcWall")

# Создание итератора только для стен
iterator = ifcopenshell.geom.iterator(
    settings, 
    ifc_file, 
    include=walls,
    num_threads=4
)

# Обработка геометрии
for item in iterator:
    shape = item.processing_result()
    print(f"Обработан элемент: {item.guid}")
    print(f"Количество вершин: {len(shape.geometry.verts)}")
    print(f"Количество граней: {len(shape.geometry.faces)}")
```

#### Обработка с пользовательскими настройками:
```python
import ifcopenshell
import ifcopenshell.geom

# Создание настроек
settings = ifcopenshell.geom.settings()

# Настройка точности
settings.set("precision", 1e-6)
settings.set("precision-factor", 10.0)

# Настройка мешера
settings.set("mesher-linear-deflection", 1e-3)
settings.set("mesher-angular-deflection", 0.5)

# Настройка материалов
settings.set("apply-default-materials", True)
settings.set("use-material-names", True)

# Настройка геометрии
settings.set("circle-segments", 32)
settings.set("boolean-attempt-2d", True)

# Создание итератора
iterator = ifcopenshell.geom.iterator(settings, ifc_file, num_threads=8)

# Обработка
for item in iterator:
    # Обработка каждого элемента
    pass
```

### 3.3 Оптимизация производительности

1. **Используйте фильтры**: Обрабатывайте только нужные элементы
2. **Настройте точность**: Увеличьте толерантность для ускорения
3. **Используйте многопоточность**: Установите num_threads в соответствии с CPU
4. **Отключите ненужные функции**: Например, no-normals для экономии времени
5. **Используйте 2D булевы операции**: boolean-attempt-2d для ускорения

### 3.4 Обработка ошибок

```python
import ifcopenshell
import ifcopenshell.geom

try:
    settings = ifcopenshell.geom.settings()
    iterator = ifcopenshell.geom.iterator(settings, ifc_file)
    
    for item in iterator:
        try:
            shape = item.processing_result()
            # Обработка успешного результата
        except Exception as e:
            print(f"Ошибка обработки элемента {item.guid}: {e}")
            continue
            
except Exception as e:
    print(f"Ошибка создания итератора: {e}")
```

## 4. Заключение

Геометрический итератор IfcOpenShell предоставляет мощный и гибкий инструмент для работы с геометрией IFC моделей. Правильная настройка параметров и использование фильтров позволяет оптимизировать производительность и получить желаемый результат. Основные принципы:

1. **Всегда настраивайте параметры** в соответствии с требованиями
2. **Используйте фильтры** для оптимизации обработки
3. **Применяйте многопоточность** для ускорения
4. **Обрабатывайте ошибки** для надежности
5. **Тестируйте настройки** на небольших моделях перед обработкой больших файлов

Данный отчет охватывает основные аспекты работы с геометрией IFC в IfcOpenShell и может служить руководством для разработчиков, работающих с данной библиотекой.